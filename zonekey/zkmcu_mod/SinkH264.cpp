#include "StdAfx.h"
#include "SinkH264.h"
#include <assert.h>

SinkH264::SinkH264(const char *mcu_ip, int mcu_rtp_port, int mcu_rtcp_port, void (*data)(void *opaque, double stamp, void *data, int len, bool key), void *opaque)
	: SinkBase(100, mcu_ip, mcu_rtp_port, mcu_rtcp_port, data, opaque)
{
	packer_ = rfc3984_new();
}


SinkH264::~SinkH264(void)
{
	rfc3984_destroy(packer_);
}

MSQueue* SinkH264::post_handle(mblk_t *im)
{
	// ʹ�� rfc3984 ��� ...
	rfc3984_unpack(packer_, im, &queue_);

	// ����󣬻���Ҫ��� annex b ��ʽ������ 00 00 00 01 xx ������
	return &queue_;
}

int SinkH264::size_for(int index, mblk_t *m)
{
	// ���� h264 ��nals��index == 0ʱ������ +4, ��� 00 00 00 01
	// ���������ķ��صģ�����  +3��������� 00 00 01
	if (index == 0)
		return m->b_wptr - m->b_rptr + 4;
	else
		return m->b_wptr - m->b_rptr + 3;
}

void SinkH264::save_for(int index, mblk_t *m, unsigned char *buf)
{
	if (index == 0) {
		buf[0] = buf[1] = buf[2] = 0;
		buf[3] = 1;
		buf += 4;
	}
	else {
		buf[0] = buf[1] = 0;
		buf[2] = 1;
		buf += 3;
	}
	memcpy(buf, m->b_rptr, m->b_wptr - m->b_rptr);
}

bool SinkH264::is_key(int index, mblk_t *m)
{
	// �����һ���� sps������Ϊ�� key
	assert(index == 0);
	return m->b_rptr[0] == 0x67;
}
