/** һ�������ߣ������Ͼ���һ�� rtp sender��������ͨ�� Sink ����

		���͵ģ�mcu Ӧ�������ڹ����ϣ���һ�� client ���������� NAT ���棬�����Ƿ���Բ������������̣�

				client ����һ���㲥ʱ�� mcu ����һ��������� ip:port����ʱ��mcu ����ֱ��ͨ���ýڵ㷢�ͣ����ǵȴ��յ� client �ĵ�һ������
				client �յ��˷������ ip:port���Լ�����һ�� rtp session�����ҳ��� ip:port ���ͼ�������������....
 */

#pragma once

#include <mediastreamer2/mediastream.h>
#include <ortp/ortp.h>
#include <string>

struct SinkStat
{
	uint64_t sent;	// total bytes sent
	uint64_t packets;	// total packets sent
	uint64_t packets_lost;	// lost packets, from RR
	uint32_t jitter;	// last jitter, from RR
};

class Sink
{
public:
	Sink(int id, int pt, const char *desc, const char *who);
	virtual ~Sink(void);

	RtpSession *get_rtp_session();
	int sink_id();
	int payload_type();
	const char *server_ip();
	int server_rtp_port();
	int server_rtcp_port();

	void process_rtcp(mblk_t *m);

	const char *desc();
	SinkStat stat();
	const char *who();	// ���͵�˭�����ֵ�� add_sink() ʱ��who=xxx ָ��

	void run();	// calling from Conference threading
	bool death() const; // һ��Ϊ Conference Threading ���ã������ж��Ƿ�

protected:
	RtpSession *rtp_session_;
	int id_, pt_;
	std::string desc_, who_;
	SinkStat stat_;
	OrtpEvQueue *evq_;	// to recv RTCP pkg.
	bool death_;

	double time_last_rtcp_;	// ����յ��� rtcp �������ڼ���Ƿ�ʱ
};
