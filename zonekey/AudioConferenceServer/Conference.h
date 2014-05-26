#pragma once

#include <mediastreamer2/mediastream.h>
#include <mediastreamer2/msconference.h>
#include <cc++/thread.h>
#include <map>
#include <string>

/** ��װ mediaStream2 �е� audioconference �ӿ�. 
 */
class Conference
{
public:
	Conference(int audio_rate);	// ����ʹ�õĲ�����. 
	virtual ~Conference(void);

	/** ����һ����Ա���ɹ�������һ�˵� local port��ʧ�ܷ��� -1

		 @param pt: Ŀǰ��ʹ�� 111, speex wb

	 */
	int addMember(const char *ip, int port, int pt);

	/** ɾ��һ����Ա���ɹ����� 0
	 */
	int delMember(const char *ip, int port, int pt);

private:
	struct Who
	{
		std::string ip;
		int port;
		int pt;

		int local_port;	// ����. 

		AudioStream *as;
		MSAudioEndpoint *ae;

		// Ϊ��֧�� map �� sort.
		bool operator < (const Who &who) const
		{
			return ip < who.ip || ((ip == who.ip) && (port < who.port));
		}
	};

	MSAudioConference *ac_;
	MSAudioConferenceParams acp_;
	typedef std::map<struct Who, AudioStream*> STREAMS;
	STREAMS streams_;		// ��Ӧmember�� AudioStream ʵ��ָ��. 
	ost::Mutex cs_streams_;
};
