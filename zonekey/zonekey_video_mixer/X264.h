/** ��װ x264 encoder ��ʹ�ã�����Ϊ YUV�����Ϊ MSQueue��ʹ�� rfc3984 ���
 */

#pragma once

#include <mediastreamer2/mediastream.h>
#include <mediastreamer2/rfc3984.h>
extern "C" {
#	include <x264.h>
}

// ����
struct X264Cfg
{
	int width, height;		// ����ֱ���
	double fps;				// ֡��
	int kbitrate;			// kbps
	int gop;
};

// encode �����״̬
struct X264FrameState
{
	int frame_type;	// X264_TYPE_xxx
	int qp;			// last qp
	int64_t pts, dts;

	int nals;
	int bytes;
};

class X264
{
	X264Cfg cfg_;
	x264_t *x264_;
	bool force_key_;
	Rfc3984Context *packer_;
	int64_t next_pts_;

public:
	X264(X264Cfg *cfg);	// ��������ֱ���
	~X264(void);

	void force_key_frame();	// ǿ������ؼ�֡

	// ѹ�� yuv ���ݵ� queue
	int encode(unsigned char *data[4], int stride[4], MSQueue *queue, X264FrameState *state);

	// ʹ�� rfc3984 ���, nals Ϊ x264 ѹ����� nal��rtp ���ڱ������������
	void rfc3984_pack(MSQueue *nals, MSQueue *rtp, uint32_t stamp);

	const X264Cfg *cfg() const { return &cfg_; }
};
