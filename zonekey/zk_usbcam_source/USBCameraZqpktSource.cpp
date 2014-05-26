#include "USBCameraZqpktSource.h"
extern "C" {
#  include <libavcodec/avcodec.h>
#  include <libswscale/swscale.h>
#  include <x264.h>
}
#include <opencv/highgui.h>
#include <zonekey/zqsender.h>
#include <zonekey/zq_atom_pkt.h>
#include <zonekey/zq_atom_types.h>

USBCameraZqpktSource::USBCameraZqpktSource(int cam, int port)
	: cam_id_(cam)
	, tcp_port_(port)
{
	fps_ = 25.0;
	kbitrate_ = 500;
	width_ = 960;
	height_ = 540;
}

USBCameraZqpktSource::~USBCameraZqpktSource(void)
{
}

int USBCameraZqpktSource::Start()
{
	quit_ = false;
	last_err_ = -1;
	// ���������̣߳����ȴ��� usb camera ����
	evt_open_.reset();
	start();
	evt_open_.wait();
	return last_err_;
}

void USBCameraZqpktSource::Stop()
{
	quit_ = true;
	evt_close_.signal();
	join();	// �ȴ������߳̽���
}

static double now()
{
	struct timeval tv;
	ost::gettimeofday(&tv, 0);
	return tv.tv_sec + tv.tv_usec / 1000000.0;
}

typedef void (*PFN_scale)(SwsContext *sws, IplImage *img, AVPicture tarpic);

/// �� RGB24 ת���� YUV420P
static void scale_from_rgb24(SwsContext *sws, IplImage *img, AVPicture tar)
{
	AVPicture pic;
	pic.data[0] = (unsigned char*)img->imageData;
	pic.data[1] = pic.data[2] = pic.data[3] = 0;
	pic.linesize[0] = img->widthStep;
	pic.linesize[1] = pic.linesize[2] = pic.linesize[3];

	sws_scale(sws, pic.data, pic.linesize, 0, img->height, tar.data, tar.linesize);
}

// ���� img �����ͣ����� swscale
static SwsContext *getScaler(IplImage *img, int w, int h, AVPicture *pic, PFN_scale *proc)
{
	SwsContext *sws = 0;

	// TODO: Ӧ�ü�� IplImage ��ÿ�ָ�ʽ
	if (!strcmp(img->colorModel, "RGB")) {
		if (img->nChannels == 3 && img->depth == 8) {
			// RGB24
			sws = sws_getContext(img->width, img->height, PIX_FMT_BGR24, w, h, PIX_FMT_YUV420P, SWS_FAST_BILINEAR, 0, 0, 0);
			avpicture_free(pic);
			avpicture_alloc(pic, PIX_FMT_YUV420P, w, h);
			*proc = scale_from_rgb24;
		}
	}
	return sws;
}

// ѹ��Ϊ x264 nals
static void encode(x264_t *enc, AVPicture *pic, int64_t *pts, x264_nal_t **nals, int *nal_num)
{
	x264_picture_t ipic, opic;
	x264_picture_init(&ipic);
	x264_picture_init(&opic);

	ipic.i_type = X264_TYPE_AUTO;
	ipic.i_pts = *pts;
	ipic.img.i_csp = X264_CSP_I420;
	ipic.img.i_plane = 3;
	for (int i = 0; i < 3; i++) {
		ipic.img.plane[i] = pic->data[i];
		ipic.img.i_stride[i] = pic->linesize[i];
	}

	if (x264_encoder_encode(enc, nals, nal_num, &ipic, &opic) >= 0) {
		*pts++;
	}
}

// x264
static x264_t *getEncoder(int width, int height, double fps, int kbitrate)
{
	x264_param_t param;
	x264_param_default_preset(&param, "ultrafast", "zerolatency");

	param.i_threads = 0;

	param.i_width = width;
	param.i_height = height;

	param.i_keyint_max = (int)fps;	// 
	param.i_fps_den = param.i_timebase_den = 1;
	param.i_fps_num = param.i_timebase_num = (int)fps;

	param.b_repeat_headers = 1;	// ��Ҫ�ظ� sps/pps...
	param.b_annexb = 1;			// ����ֱ�ӷ���

	// ���ʿ���
	param.rc.i_rc_method = X264_RC_ABR;
	param.rc.i_bitrate = kbitrate;
	param.rc.i_vbv_max_bitrate = (int)(kbitrate * 1.1);
	param.rc.i_vbv_buffer_size = (int)(kbitrate / 2.0);	// ϣ���� 500 ms ֮�ڵĵ���
	param.rc.i_qp_max = 52;
	param.rc.i_qp_step = 6;

	x264_t *encoder = x264_encoder_open(&param);
	return encoder;
}

static int send(ZqSenderTcpCtx *sender, x264_nal_t *nals, int num, double stamp, int width, int height)
{
	// �Ƿ�Ϊ�ؼ�֡
	// FIXME: �򵥵��жϵ�һ���Ƿ�Ϊ sps
	bool key = false;
	if (nals[0].p_payload[4] == 0x67)
		key = true;

	if (key) // ���� sync
		zqsnd_tcp_send(sender, CONST_ATOM_SYNC, 16);

	// DATA ����
	size_t datalen = 0;
	for (int i = 0; i < num; i++) {
		datalen += nals[i].i_payload;
	}

	// ����ͷ��Ϣ
	zq_atom_header vid;
	vid.type.type_i = ZQ_ATOMS_TYPE_VIDEO;
	vid.size = sizeof(vid) + sizeof(zq_atom_header) + sizeof(zq_atom_video_header_data) + sizeof(zq_atom_header) + datalen;
	zqsnd_tcp_send(sender, &vid, sizeof(vid));

	zq_atom_header vh;
	vh.type.type_i = ZQ_ATOM_TYPE_VIDEO_HEADER;
	vh.size = sizeof(vh) + sizeof(zq_atom_video_header_data);
	zqsnd_tcp_send(sender, &vh, sizeof(vh));

	zq_atom_video_header_data vhd;
	vhd.stream_codec_type = 0x1b;
	vhd.id = 0;
	vhd.frame_type = key ? 'I' : 'P';
	vhd.width = width;
	vhd.height = height;
	vhd.pts = (unsigned int)stamp * 45000.0;
	vhd.dts = vhd.pts;
	zqsnd_tcp_send(sender, &vhd, sizeof(vhd));

	zq_atom_header dh;
	dh.type.type_i = ZQ_ATOM_TYPE_DATA;
	dh.size = sizeof(dh) + datalen;
	zqsnd_tcp_send(sender, &dh, sizeof(dh));

	for (int i = 0; i < num; i++) {
		// ���� body ��Ϣ
		zqsnd_tcp_send(sender, nals[i].p_payload, nals[i].i_payload);
	}

	return 0;
}

void USBCameraZqpktSource::run()
{
	// ���Դ� cam_id_ ָ���� usb cam������ɹ������� last_err_ = 0���������� evt_open_
	CvCapture *cap = cvCaptureFromCAM(cam_id_);
	if (!cap) {
		// һ���� cam_id_ ����
		last_err_ = -1;
		evt_open_.signal();
		return;
	}

	// ��ȡһ֡ͼ�񣬾������
	IplImage *img = cvQueryFrame(cap);
	while (!img)
		img = cvQueryFrame(cap);

	AVPicture pic;
	avpicture_alloc(&pic, PIX_FMT_YUV420P, 16, 16);

	PFN_scale scale = 0;
	SwsContext *sws = getScaler(img, width_, height_, &pic, &scale);
	x264_t *x264 = getEncoder(width_, height_, fps_, kbitrate_);

	if (!sws || !x264) {
		last_err_ = -2;
		evt_open_.signal();
		return;
	}

	// ���� zqsender
	ZqSenderTcpCtx  *sender = 0;
	if (zqsnd_open_tcp_server(&sender, tcp_port_, 0) < 0) {
		last_err_ = -3;
		evt_open_.signal();
		return;
	}

	// last_err_ = 0;
	last_err_ = 0;
	evt_open_.signal(); // ��ʽ��ʼ

	// ���ݲ�������ÿ֮֡��ĵȴ�ʱ��
	int frame_duration = (int)(1000.0 / fps_), delta = 0;	// delta��������cpu��ʱ���´εȴ�ʱ��Ϊ wait = frame_duration - delta;

	// ������ͷ�ɹ����������ò�����������Ӧ��encoder��scaler .....
	int64_t pts = 0;
	double tl = now();
	while (!quit_) {
		if (evt_close_.wait(frame_duration - delta))
			continue; // Stop() ��������

		// �ɼ�
		img = cvQueryFrame(cap);
		double stamp = now();

		// ׼������һ֡���� ...
		scale(sws, img, pic);	// pic ���Ѿ�Ϊ���������ݣ�ֱ��ʹ�� x264 ѹ������

		// ѹ�� ...
		x264_nal_t *nals = 0;
		int nal_num = 0;
		encode(x264, &pic, &pts, &nals, &nal_num);

		// ����Ƿ������ 
		if (nal_num > 0) {
			// ���͵� zqsender
			send(sender, nals, nal_num, stamp, img->width, img->height);
		}

		// ͨ������ delta����֡�ʾ����ܵ��ȶ�
		double tn = now();
		if (tn - tl > frame_duration / 1000.0) {
			// ˵���ȴ�ʱ�䳬����һ֡�ļ����cpu ̫���ˣ���Ҫ���� delta
			delta++;
		}
		else {
			// 
			delta --;
		}
		tl = tn;

		if (delta > frame_duration)
			delta = frame_duration;

		fprintf(stderr, "frame duration=%d, delta=%d\n", frame_duration, delta);
	}

	zqsnd_close_tcp_server(sender);
	avpicture_free(&pic);
	sws_freeContext(sws);
	x264_encoder_close(x264);
	cvReleaseCapture(&cap);
}
