#pragma once

#include "WorkerThread.h"
#include "MemberStream.h"
#include "MixerStream.h"

// ����ģʽ����ϯ������...
enum ConferenceMode
{
	CM_DIRCETOR,	// ����ģʽ������ģʽ�У����г�Ա����Ƶ�ɵ���������ϣ�ѹ��������ÿ����Ա
	CM_FREE,		// ����ģʽ��ÿ����Ա����ѡ��ϣ����������
};

/** ��Ӧһ�����飬
 */
class Conference
{
	ConferenceMode mode_;
	ZonekeyVideoMixer *mixer_;	// ������ϯģʽ

public:
	Conference(ConferenceMode mode);
	virtual ~Conference(void);

	ConferenceMode mode() const { return mode_; }

	// ���һ�������Ա
	MemberStream *add_member_stream(const char *client_ip, int client_port);

	// ���� MemberStream::id() �ҵ���Ӧ�� MemberStream
	MemberStream *get_member_stream(int id);

	// �ͷŻ����Ա
	void del_member_stream(int id);

private:

};
