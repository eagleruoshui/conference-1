/** ÿ�� conference ���һ�� video mixer */

#pragma once

#include <mediastreamer2/mediastream.h>
#include <mediastreamer2/zk.video.mixer.h>

class Conference
{
public:
	Conference(void);
	~Conference(void);

	// ��ӳ�Ա
	int addMember(const char *ip, int port);
	int delMember(const char *ip, int port);
};
