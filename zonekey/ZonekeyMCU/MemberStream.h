/** ����һ����Ա��һ·������һ·��Ƶ��������һ·��Ƶ��
	�����ϣ�һ·��������
			rtp session: �����շ� rtp ���ݣ��������ͣ����� SENDONLY, RECVONLY, SENDRECV ����
			rtp recver filter: ���ڽ�������client������
			rtp sender filter: ���ڷ��͵� client ������
 */

#pragma once

#include <mediastreamer2/mediastream.h>
#include "util.h"

enum MemberStreamType
{
	SENDONLY,
	RECVONLY,
	SENDRECV,
};

class MemberStream
{
public:
	MemberStream(int id, MemberStreamType type, int payload);
	virtual ~MemberStream(void);

	// ��ͬ���͵� MemberStream�����������ͬ�� sender ���� recver
	virtual MSFilter *get_sender_filter() { return filter_sender_; }
	virtual MSFilter *get_recver_filter() { return filter_recver_; }
	virtual int id() const { return id_; }

	// �������ԣ��������Լ�ʵ��֧�ְ�
	virtual int set_params(const char *name, int num, ...) 
	{
		return 0;
	}

	RtpSession *get_rtp_session() { return rtp_session_; }
	const char *get_server_ip() { return util_get_myip(); }
	int get_server_port() 
	{ 
		if (type_ == RECVONLY || type_ == SENDRECV)
			return rtp_session_get_local_port(rtp_session_); 
		else
			return 0;
	}

private:
	MSFilter *filter_sender_, *filter_recver_;
	RtpSession *rtp_session_;
	MemberStreamType type_;
	int id_;
};
