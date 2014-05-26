#pragma once

#include <string>
#include <mediastreamer2/mediastream.h>
#include "util.h"
#include <cc++/thread.h>
#include <vector>

class Sink;

#define USER_TRANSPORT 0

/** Stream == Source + Sink
 */

struct StreamStat
{
	uint64_t sent, recv;	// the bytes send/recv
	uint64_t packet_sent, packet_recv;	// rtp stats
	uint64_t packet_lost_recv;	// ���� rtp stats
	uint32_t jitter;	// ���� RR/SR report block
	uint64_t packet_lost_sent;	// RR/SR report block
};

class Stream
{
public:
	/// Stream: һ������£�һ�� stream �� source �� sink���ر�ģ�֧�� publisher���� ����Ҫ sink
	Stream(int id, int payload_type, const char *desc, bool support_sink, bool support_publisher);
	virtual ~Stream();

	bool support_publisher() const { return filter_publisher_ != 0; }
	bool support_sink() const { return filter_sender_!= 0; }

	int stream_id() const { return id_; }
	int channel_id() const { return cid_; }
	void channel_id(int id) { cid_ = id; }
	int payload_type() const { return payload_; }
	const char *desc() const { return desc_.c_str(); }
	StreamStat stat() const { return stat_; }

	// �㲥
	int add_sink(Sink *sink);
	int del_sink(Sink *sink);
	
	// return filter_recver_ default
	virtual MSFilter *get_source_filter() { return filter_recver_; }
	virtual MSFilter *get_sink_filter() { return filter_sender_; }

	/// ���� publisher filter��ֻ�е���һ· Stream ��Ҫ֧�� publisher ʱ���ŷ�����Ч��ȱʡ��֧�֡�
	virtual MSFilter *get_publisher_filter() { return filter_publisher_; }

	virtual int get_source_pin() { return 0; }

	// to get server rtp port, bi-direction
	int server_rtp_port() const { return rtp_port_; }
	int server_rtcp_port() const { return rtcp_port_; }
	const char *server_ip() const { return util_get_myip(); }

	RtpSession *rtp_session() { return rtp_sess_; }
	void process_rtcp(mblk_t *m);

	// call from Conference thread
	void run();
	bool death() { return death_; }

protected:
	virtual void on_rtcp_sr(mblk_t *m);
	virtual void on_rtcp_rr(mblk_t *m);
	virtual void on_rtcp_sdes(mblk_t *m);
	virtual void on_rtcp_bye(mblk_t *m);
	virtual void on_rtcp_app(mblk_t *m);

	bool death_;

	/// �ж� stat �е� packet_recved �Ƿ�ʱ�䲻��
	double last_check_stamp_;
	uint64_t packet_recved_;	//	�յ��İ��������ڼ������û�и��µİ���
	bool is_timeout(double now, const rtp_stats_t *stat);

protected:
	int id_, payload_, cid_;
	std::string desc_;
	MSFilter *filter_recver_;
	MSFilter *filter_sender_;
	MSFilter *filter_publisher_, *filter_tee_;
	RtpSession *rtp_sess_;
	OrtpEvQueue *evq_;
	int rtp_port_, rtcp_port_;
	StreamStat stat_;
#if USER_TRANSPORT
	struct _RtpTransport *trans_rtp_, *trans_rtcp_;
#endif

private:
	std::vector<Sink*> sinks_;
	ost::Mutex cs_sinks_;
};
