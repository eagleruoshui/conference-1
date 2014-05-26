#pragma once

#include <opencv/highgui.h>


// ��Ӧһ· webcam stream...
class CameraStream
{
public:
	CameraStream(void);
	~CameraStream(void);

	int open();
	int close();

	void run_once();	// ���ⲿѭ������
	void set_server_info(const char *ip, int rtp_port, int rtcp_port)
	{
		server_ip_ = ip;
		server_rtp_port_ = rtp_port;
		server_rtcp_port_ = rtcp_port;

		switch_ = true;
	}

	double fps() const;

private:
	SwsContext *sws_;
	x264_t *encoder_;
	AVPicture pic_;
	CvCapture *cap_;

	int64_t pts_;
	unsigned char *frame_buf_;
	int buf_size_;
	int data_size_;

	RtpSession *rtp_;
	MSTicker *ticker_sender_, *ticker_recver_;
	MSFilter *filter_rtp_sender_, *filter_rtp_recver_, *filter_h264_sender_, *filter_decoder_, *filter_yuv_sink_;
	ZonekeyH264SourceWriterParam sender_params_;

	std::string server_ip_;
	int server_rtp_port_, server_rtcp_port_;
	bool switch_;

	void init();

	/** �� cam ȡ��һ֡���ݣ�����ת��Ϊ WIDTH x HEIGHT
	*/
	AVPicture *next_pic(CvCapture *cam)
	{
		IplImage *img = cvQueryFrame(cam);

		unsigned char *data[4];
		data[0] = (unsigned char*)img->imageData;
		data[1] = data[2] = data[3] = 0;
		int stride[4];
		stride[0] = img->widthStep;
		stride[1] = stride[2] = stride[3] = 0;

		sws_scale(sws_, data, stride, 0, img->height, pic_.data, pic_.linesize);

		return &pic_;
	}

	/** �� nals ���浽 frame_buf_ ��
	 */
	int save_nal(x264_nal_t nal)
	{
		if (buf_size_ - data_size_ < nal.i_payload) {
			buf_size_ += nal.i_payload;
			frame_buf_ = (unsigned char *)realloc(frame_buf_, buf_size_);
		}

		memcpy(frame_buf_ + data_size_, nal.p_payload, nal.i_payload);
		data_size_ += nal.i_payload;
		return nal.i_payload;
	}

	/** ѹ�� pic ���ݣ����� nals �ֽ��� */
	int encode_frame(AVPicture *pic)
	{
		x264_picture_t pic_in, pic_out;
		x264_picture_init(&pic_in);
		x264_picture_init(&pic_out);
		
		pic_in.img.i_csp = X264_CSP_I420;
		pic_in.img.i_plane = 3;
		for (int i = 0; i < 3; i++) {
			pic_in.img.i_stride[i] = pic->linesize[i];
		}
		pic_in.img.plane[0] = pic->data[0];
		pic_in.img.plane[1] = pic->data[2];
		pic_in.img.plane[2] = pic->data[1];

		x264_nal_t *nals;
		int nal_cnt;

		pic_in.i_pts = pts_++;
		pic_in.i_type = X264_TYPE_AUTO;

		data_size_ = 0;

		int rc = x264_encoder_encode(encoder_, &nals, &nal_cnt, &pic_in, &pic_out);
		if (rc >= 0 && nal_cnt > 0) {
			for (int i = 0; i < nal_cnt; i++) {
				save_nal(nals[i]);
			}
		}

		return data_size_;
	}
};
