/** һ·��Ƶ stream ....
	
		�ṩ���������ҽ��ܻ��������������


 */

#pragma once

#include <ortp/ortp.h>
#include <mediastreamer2/mediastream.h>

#include "zonekeystream.h"
class ZonekeyStreamAudio : public ZonekeyStream
{
public:
	ZonekeyStreamAudio(int id, RtpSession *rs);
	virtual ~ZonekeyStreamAudio(void);

	MSFilter *get_input() { return filter_decoder_; }

private:
	MSFilter *filter_decoder_;
};
