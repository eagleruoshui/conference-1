#pragma once

/** ���� AudioStream, VideoStream����� MixerStream �ǲ�ͬ�ġ���Ӧ����Ƶ�����е�"��ϯģʽ"
								  -----------------------
		rtp recv ---> decoder --->|						|          /--> rtp sender
		                          |						|         /
		rtp recv ---> decoder --->|  zonekey video mixer|---> tee --> rtp sender
		                          |						|        \
		rtp recv ---> decoder --->|						|         \--> rtp sender
		                          |----------------------

		���� AudioConference ������֧�ֶ�̬���ӣ�ɾ�� rtp recv / rtp sender ...

		һ�� VideoMixerStream ʹ��һ�������̣߳�ͬʱ֧�ֶ�· rtp recver�������
		���� zonekey.video,mixer ��ϣ�ͨ�� tee ������پ��� rtp sender ���͵� rtp recv ��Ӧ�� rtp session


		ʹ�ò��裺
			1. ZonekeyVideoMixer *mixer = new ZonekeyVideoMixer();

			2. mixer->set_encoding_setting(...); ���ñ������

			3, VideoMixerStream *stream = mixer->addStream(...); ����һ����Ƶ��Ա

			4. stream->get_canvas_size(&width, &height);	// ��ȡ����ʵ�ʴ�С

			5. stream->set_channel_desc(....);			// ���ø� stream �����ʾ�ڻ����У�

			6. ..... ʹ���� ....

			7. mixer->delStream(stream);	//  ɾ���� stream

			8. delete mixer;		//  �ͷ�����
 */ 

#include <mediastreamer2/mediastream.h>
#include <mediastreamer2/zk.video.mixer.h>
#include "MemberStream.h"
#include <string>
#include <vector>

// Ϊ�˼򵥣��������ͬʱ MAX_STREAMS ·�������ֵ���� tee outputs ����Ŀ�� 1��tee ʣ���һ·���������ڽ��ܵ㲥 :)
#define MAX_STREAMS 9

#define DEBUG_PREVIEW 1

// ��Ӧһ·����һ·����Ӧ��һ�� rtpsession + һ�� rtp recver + һ�� h264 ������ + һ�� rtp sender
class VideoMixerStream : public MemberStream
{
	int cid_;		// ��Ӧ�� zonekey.video.mixer �е�channel id
	MSFilter *filter_decoder_;
	MSFilter *filter_mixer_;	// ���ڷ���ֱ�ӷ��� mixer �ķ���

public:
	VideoMixerStream(const char *ip, int port, int cid, MSFilter *mixer_);
	~VideoMixerStream();

	// ��ȡ��������
	void get_canvas_size(int *width, int *height);

	/** ����ͨ�����ԣ��续��������λ�ã�͸����
	 */
	int set_channel_desc(int x, int y, int width, int height, int alpha);

	virtual MSFilter *get_recver_filter() { return filter_decoder_; }	// ����Ϊ h264 �����

	/** �������ԣ�����������Ч��Ŀ�Ĳ��� :)

			name                    params						              ˵��
		------------     ----------------------------------     ------------------------------------------------
		 ch_desc           x, y, width, height, alpha            ���ñ�ͨ�����ԣ�5�����������в������� int ����
		 ch_zorder		  ["up"|"down"|"top"|"bottom"|...]       ����zorder���ԣ�һ���������ַ�������

	 */
	virtual int set_params(const char *name, int nums, ...);
};

/** ���� zonekey.video.mixer �Ļ����
		

 */
class ZonekeyVideoMixer
{
	MSTicker *ticker_;	// �����̣߳��������� graphs
	typedef std::vector<VideoMixerStream*> STREAMS;
	STREAMS streams_;
	MSFilter *filter_mixer_;	// zonekey.video.mixer
	MSFilter *filter_tee_;		// ���� zonekey.video,mixer->outputs[1]

#if DEBUG_PREVIEW
	MSFilter *filter_sender_preview_;
	RtpSession *rtp_preview_;
#endif // debug preview

public:
	ZonekeyVideoMixer(void);
	virtual ~ZonekeyVideoMixer(void);

public:
	// ���û�����ԣ���Ҫ�� h264 ѹ������
	int set_encoding_setting(int width, int height, int kbps, double fps, int gop);

	// ����һ·��������ɹ������� VideoStream������޷�����µ� stream�ˣ����� 0
	// ע�Ⱑ����ʱ stream ��û���ڻ�������ʾ����Ҫ���� stream->set_channel_desc() ���е�
	VideoMixerStream *addStream(const char *ip, int port);

	// ɾ��һ·��
	void delStream(VideoMixerStream *stream);

	// ���� cid ��ȡ Stream
	VideoMixerStream *getStreamFromID(int cid);

private:
	// �� zonekey.video.mixer ����һ���µ� cid
	int allocate_channel();
	void release_channel(int cid);

	// ��ӵ����� grapha ��
	int add_stream_to_graph(VideoMixerStream *stream);
	int del_stream_from_graph(VideoMixerStream *stream);
};
