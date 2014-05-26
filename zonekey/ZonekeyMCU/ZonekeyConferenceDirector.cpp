#include "ZonekeyConferenceDirector.h"
#include <assert.h>
#include "ZonekeyStreamVideoMixer.h"

ZonekeyConferenceDirector::ZonekeyConferenceDirector(void)
	: ZonekeyConference(CM_DIRCETOR)
{
	audio_ = 0;

	ticker_ = ms_ticker_new();

	video_mixer_filter_ = ms_filter_new_from_name("ZonekeyVideoMixer");
	video_tee_filter_ = ms_filter_new(MS_TEE_ID);
	video_publisher_filter_ = ms_filter_new_from_name("ZonekeyPublisher");
	audio_publisher_filter_ = ms_filter_new_from_name("ZonekeyPublisher");

	assert(video_mixer_filter_ != 0 && video_tee_filter_ != 0);
	ms_filter_link(video_mixer_filter_, 1, video_tee_filter_, 0);	// ���� mixer filter h264 ����� tee
	ms_filter_link(video_tee_filter_, 9, video_publisher_filter_, 0);	// ���� video publisher

	// TODO: ��ʼ�� audio ...
}

ZonekeyConferenceDirector::~ZonekeyConferenceDirector(void)
{
	ms_ticker_destroy(ticker_);

	ms_filter_destroy(video_mixer_filter_);
	ms_filter_destroy(video_tee_filter_);
	ms_filter_destroy(video_publisher_filter_);
	ms_filter_destroy(audio_publisher_filter_);
}

/** ������ϯģʽ�Ĳ������ã�������Ƶ����Ƶ�Ĳ������ã�
		��Ƶ��
			TODO��

		��Ƶ��
			v.encoder.setting:   "width", "height", "fps", "kbps", "gop": ������Ƶ����������
			v.stream.desc:		"id", "x", "y", "width", "height", "alpha": ����ĳһ· stream �����ԣ�ע�� id Ϊ ZonekeyStream::id() ������ ZonekeyStreamVideoMixer::chid()
			v.stream.zorder:    "id", "mode": ����ĳһ· stream �� z-order

 */
int ZonekeyConferenceDirector::set_params(const char *prop, KVS &params)
{
	char info[1024];

	if (!strcmp(prop, "v.encoder.setting")) {
		if (chk_params(params, info, "width", "height", "fps", "kbps", "gop", 0)) {
			return setting_vencoder(atoi(params["width"].c_str()), atoi(params["height"].c_str()), atoi(params["kbps"].c_str()), atof(params["fps"].c_str()), atoi(params["gop"].c_str()));
		}
		else {
			fprintf(stderr, "[Conference] %s: for 'v.encoder.setting': %s\n", __FUNCTION__, info);
			return -1;
		}
	}
	else if (!strcmp(prop, "v.stream.desc")) {
		if (chk_params(params, info, "id", "x", "y", "width", "height", "alpha", 0)) {
			return setting_vstream_desc(atoi(params["id"].c_str()), atoi(params["x"].c_str()), atoi(params["y"].c_str()), atoi(params["width"].c_str()), atoi(params["height"].c_str()), atoi(params["alpha"].c_str()));
		}
		else {
			fprintf(stderr, "[Conference] %s: for 'v.stream.desc': %s\n", __FUNCTION__, info);
			return -1;
		}
	}
	else if (!strcmp(prop, "v.stream.zorder")) {
		if (chk_params(params, info, "id", "mode", 0)) {
			return setting_vstream_zorder(atoi(params["id"].c_str()), params["mode"].c_str());
		}
		else {
			fprintf(stderr, "[Conference] %s: for 'v.stream.zorder': %s\n", __FUNCTION__, info);
			return -1;
		}
	}
	// TODO�����������

	return -1;
}

int ZonekeyConferenceDirector::get_params(const char *prop, KVS &results)
{
	return -1;
}

ZonekeyStream *ZonekeyConferenceDirector::createStream(RtpSession *rs, KVS &params, int id)
{
	char info[1024];
	assert(chk_params(params, info, "payload", 0));	// �ض����� payload
	int payload_type = atoi(params["payload"].c_str());

	if (payload_type == 100) {
		// h264
		
		ZonekeyStreamVideoMixer *s = new ZonekeyStreamVideoMixer(id, rs);
		
		// �� s ���� video graph ��
		if (insert_video_stream(s) < 0) {
			// ʧ��
			delete s;
			return 0;
		}

		// �� params �еĲ������ݵ� mixer_
		if (chk_params(params, info, "x", "y", "width", "height", "alpha", 0)) {
			ZonekeyVideoMixerChannelDesc desc;
			desc.x = atoi(params["x"].c_str());
			desc.y = atoi(params["y"].c_str());
			desc.width = atoi(params["width"].c_str());
			desc.height = atoi(params["height"].c_str());
			desc.alpha = atoi(params["alpha"].c_str());
			
			ms_filter_call_method(video_mixer_filter_, ZONEKEY_METHOD_VIDEO_MIXER_SET_CHANNEL_DESC, &desc);
		}

		return s;
	}
	else if (payload_type == 110) {
		// speex wb
		// TODO: ��Ƶ������
	}

	return 0;
}

ZonekeySink *ZonekeyConferenceDirector::createSink(RtpSession *rs, KVS &params, int id)
{
	char info[1024];
	assert(chk_params(params, info, "payload", 0));
	int payload_type = atoi(params["payload"].c_str());

	if (payload_type == 100) {
		// h264 �����ߣ�ֱ�ӽ� rs ��ӵ� video publisher ����
		ZonekeySink *sink = new ZonekeySink(id, rs);
		if (add_video_sink_to_publisher(sink) < 0) {
			// ʧ��
			delete sink;
			return 0;
		}

		return sink;
	}
	else if (payload_type == 110) {
		// speex �����ߣ�ֱ����ӵ� audio publisher ����
		// TODO: ...
	}
	return 0;
}

void ZonekeyConferenceDirector::freeStream(ZonekeyStream *s)
{
	/** ���� RtpSession payload �ж��� video ���� audio
	 */
	RtpSession *rtp_sess = s->rtp_session();
	assert(rtp_sess);

	int pt = rtp_session_get_send_payload_type(rtp_sess);
	if (pt == 100) {
		// h264
		ZonekeyStreamVideoMixer *stream = (ZonekeyStreamVideoMixer*)s;

		// �� video grpha ��ɾ��
		remove_video_stream(stream);

		delete stream;
	}
	else if (pt == 110) {
		// speex wb
		// TODO: ��Ƶ...
	}
}

void ZonekeyConferenceDirector::freeSink(ZonekeySink *sink)
{
	RtpSession *rtp_sess = sink->rtp_session();
	assert(rtp_sess);

	int pt = rtp_session_get_send_payload_type(rtp_sess);
	if (pt == 100) {
		// h264
		del_video_sink_from_publisher(sink);
		delete sink;
	}
	else if (pt == 110) {
		// speex wb
		// TODO: ...
	}
}

int ZonekeyConferenceDirector::insert_video_stream(ZonekeyStreamVideoMixer *s)
{
	/** �� s ���� video graph ��
	 */

	// step 0: �� mixer_filter ���� ch
	int ch = -1;
	int rc = ms_filter_call_method(video_mixer_filter_, ZONEKEY_METHOD_VIDEO_MIXER_GET_CHANNEL, &ch);
	if (rc < 0) {
		// ʧ���ˣ�һ���ǲ�֧�ָ���Ĳ�������
		fprintf(stderr, "[Conference] %s: no MORE free ch for the member!\n", __FUNCTION__);
		return -1;
	}
	
	fprintf(stdout, "[Conference] %s: using ch id=%d\n", __FUNCTION__, ch);

	s->set_chid(ch);	// �൱��Ҫ�����ڲ�� :)

	// step 1: stop ticker
	ms_ticker_detach(ticker_, video_mixer_filter_);

	// step 2: ���� s �� output �� mixer 
	ms_filter_link(s->get_input(), 0, video_mixer_filter_, ch);

	// step 3: ���� tee output �� s
	ms_filter_link(video_tee_filter_, ch, s->get_output(), 0);

	// step 4: �ָ� ticker
	ms_ticker_attach(ticker_, video_mixer_filter_);

	return 0;
}

int ZonekeyConferenceDirector::remove_video_stream(ZonekeyStreamVideoMixer *s)
{
	/** �� s �� video graph ��ɾ������Ӧ insert_video_stream()
	 */

	ms_ticker_detach(ticker_, video_mixer_filter_);
	ms_filter_unlink(s->get_input(), 0, video_mixer_filter_, s->chid());
	ms_filter_unlink(video_tee_filter_, s->chid(), s->get_output(), 0);
	ms_ticker_attach(ticker_, video_mixer_filter_);

	return 0;
}

int ZonekeyConferenceDirector::add_video_sink_to_publisher(ZonekeySink *s)
{
	/* �� sink ��ӵ� video_publisher_filter ��
	 */
	return ms_filter_call_method(video_publisher_filter_, ZONEKEY_METHOD_PUBLISHER_ADD_REMOTE, s->rtp_session());
}

int ZonekeyConferenceDirector::del_video_sink_from_publisher(ZonekeySink *s)
{
	return ms_filter_call_method(video_publisher_filter_, ZONEKEY_METHOD_PUBLISHER_DEL_REMOTE, s->rtp_session());
}

int ZonekeyConferenceDirector::setting_vencoder(int width, int height, int kbsp, double fps, int gop)
{
	// TODO: 
	return -1;
}


/** ������Ƶ stream ������!
 */
int ZonekeyConferenceDirector::setting_vstream_desc(int id, int x, int y, int width, int height, int alpha)
{
	ZonekeyStream *s = get_stream(id);
	if (!s) {
		fprintf(stderr, "[Conference] %s: can't FIND stream id=%d!!!\n", __FUNCTION__, id);
		return -1;
	}

	ZonekeyStreamVideoMixer *vs = (ZonekeyStreamVideoMixer*)s;

	ZonekeyVideoMixerChannelDesc desc;
	desc.x = x;
	desc.y = y;
	desc.width = width;
	desc.height = height;
	desc.alpha = alpha;
	desc.id = vs->chid();

	return ms_filter_call_method(video_mixer_filter_, ZONEKEY_METHOD_VIDEO_MIXER_SET_CHANNEL_DESC, &desc);
}

int ZonekeyConferenceDirector::setting_vstream_zorder(int id, const char *mode)
{
	ZonekeyStream *s = get_stream(id);
	if (!s) {
		fprintf(stderr, "[Conference] %s: can't FIND stream id=%d!!!\n", __FUNCTION__, id);
		return -1;
	}

	ZonekeyStreamVideoMixer *vs = (ZonekeyStreamVideoMixer*)s;
	
	ZonekeyVideoMixerZOrder order;
	order.id = vs->chid();
	if (!strcmp("top", mode))
		order.order_oper = ZONEKEY_VIDEO_MIXER_ZORDER_TOP;
	else if (!strcmp("up", mode))
		order.order_oper = ZONEKEY_VIDEO_MIXER_ZORDER_UP;
	else if (!strcmp("down", mode))
		order.order_oper = ZONEKEY_VIDEO_MIXER_ZORDER_DOWN;
	else if (!strcmp("bottom", mode))
		order.order_oper = ZONEKEY_VIDEO_MIXER_ZORDER_BOTTOM;
	else {
		fprintf(stderr, "[Conference] %s: unknown mode='%s'\n", __FUNCTION__, mode);
		return -1;
	}
	
	return ms_filter_call_method(video_mixer_filter_, ZONEKEY_METHOD_VIDEO_MIXER_SET_ZORDER, &order);
}
