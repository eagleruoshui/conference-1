#pragma once

#include <ortp/ortp.h>

/** ��Ӧһ������Ľ����ߣ��� ZonekeyStream ���
 */
class ZonekeySink
{
public:
	ZonekeySink(int id, RtpSession *rs);
	virtual ~ZonekeySink(void);

	int id() const { return id_; }
	RtpSession *rtp_session() { return rtp_session_; }

private:
	int id_;
	RtpSession *rtp_session_;
};
