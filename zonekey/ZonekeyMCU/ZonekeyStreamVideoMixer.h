#pragma once

#include <mediastreamer2/mediastream.h>
#include <ortp/ortp.h>
#include "zonekeystream.h"

/** ���ڻ�ϵ���Ƶ��

		���ڻ����Ƶ���������Ϊ YUV ��������Ϊ h264 ��
 */
class ZonekeyStreamVideoMixer : public ZonekeyStream
{
public:
	ZonekeyStreamVideoMixer(int id, RtpSession *rs);
	~ZonekeyStreamVideoMixer(void);

	// ���� rtp recv --> h264 decoder -->
	MSFilter *get_input() { return filter_decoder_; }

	// ���� --> rtp sender
	MSFilter *get_output() { return filter_sender_; }

	int chid() const { return chid_; }
	void set_chid(int ch) { chid_ = ch; }

private:
	MSFilter *filter_recver_, *filter_decoder_, *filter_sender_;
	int chid_;
};
