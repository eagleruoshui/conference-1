/** ����һ��������˵����Ϊ���������
        ��ϯģʽ����ʱ��ҿ������Ǿ����������Ƶ������
		����ģʽ���൱�ڵ㲥

	���ڲ����߷�Ϊ��
		�����ߣ��ṩ����  ----> Source
		���ڣ�ʹ�����ݣ�������δ���ǹ���   ----> Sink
 */

#pragma once

#include "Source.h"
#include "Sink.h"
#include "util.h"
#include "Stream.h"
#include <cc++/thread.h>
#include <deque>
#include <map>
#include <string>

enum ConferenceType
{
	CT_FREE,
	CT_DIRECTOR,
};

struct SourceDescription
{
	int sid;
	std::string desc;
	SourceStat stats;
	int payload;
};

struct SinkDescription
{
	int sinkid;
	const char *desc;
	const char *who;
	SinkStat stats;
	int payload;
};

struct StreamDescription
{
	int streamid;
	std::string desc;
	StreamStat stats;
	int payload;
};

struct XmppResponse
{
	void *token;
	std::string cmd, cmd_options;
	std::string result, result_options;
	std::string from;
};

/** ʵ��XmppNotify������Ҫ֪ͨʱ���㲥�����н�����
 */
struct XmppNotifyData
{
	std::deque<XmppResponse> tokens;
};
typedef std::map<std::string, XmppNotifyData> XMPP_NOTIFY_DATAS;

class Server;

/** addSource/delSource: ���ɾ�����ݹ�����
	addSink/delSink: ���ɾ������

	������ִ�н����ͨ�� KVS ����.
 */
class Conference
{
	friend class Server;

public:
	Conference(int id);
	virtual ~Conference(void);

	/** ���û���������Ϣ
	 */
	void set_desc(const char *desc)
	{
		if (desc) desc_ = desc;
	}

	// �� Server ���ã�����������������
	// ��ֻ���ṩһ������ :)
	void run_once();

	// ���ظ� Conference �Ƿ��Ѿ����У�����˵��û�� sources, sinks(?), streams
	bool idle();

	const char *desc() const { return desc_.c_str(); }

	/** �� xmpp �����߳��е���
		����������һ������Դ��
		params �а�����
			payload: ���룺100 ���� 110��100 ��ʾh264��110 ��ʾ speex wb��
			desc: ��ѡ��Դ����
			
		����ɹ���results �а�����
			sid: source id������Ψһ��ʾ��· source
			server_ip: server ip
			server_rtp_port��server rtp ���ն˿�
			server_rtcp_port: server rtcp ���ն˿�

	 */
	int addSource(KVS &params, KVS &results);

	/** �� xmpp �����߳��е���
		ɾ�������е�����Դ
		params �а���:
			sid: source id
			
	 */
	int delSource(KVS &params, KVS &results);

	/** �� xmpp �����߳��е���
		����һ���������
		params �а�����
			sid: source id��ϣ���ۿ���һ·source�������ϯģʽ����ֵ��Ч��

		����ɹ���results ����:
			sinkid: sink id
			server_ip: server ip
			server_rtp_port��server rtp ���ն˿�
			server_rtcp_port: server rtcp ���ն˿�
	 */
	int addSink(KVS &params, KVS &results);
	int delSink(KVS &params, KVS &results);

	// �� xmpp �����߳��е���
	int addStream(KVS &params, KVS &results);
	int addStreamWithPublisher(KVS &params, KVS &results);
	int delStream(KVS &params, KVS &results);

	/** �� xmpp �����߳��е��ã����������� addSink/addStream/addSource/delSink/delStream/delSource/setParams �����
	 */
	int chkCastNotify(const char *who, const char *cmd, const char *options, const char *result, const char *result_options);

	// �� xmpp �߳��е��ã���˼�ǣ�client ����һ���첽���󣬵� mcu �����������ʱ��ͨ�� zkrbt_mse_respond()����client���յ� response
	int waitEvent(const char *from, const char *cmd, const char *options, KVS &params, KVS &results, void *uac_token);
	int reg_notify(const char *from_jid, void *uac_token);

	virtual int setParams(KVS &params, KVS &results)
	{
		return 0;
	}
	virtual int getParams(KVS &params, KVS &results)
	{
		return 0;
	}

	int cid() const
	{
		return cid_;
	}

	/// ���ػ���������ʱ�䣬��
	double uptime() const
	{
		return util_time_now() - time_begin_;
	}

	virtual ConferenceType type() const = 0;

	// to get all source's description
	std::vector<SourceDescription> get_source_descriptions(int payload = -1);
	std::vector<SinkDescription> get_sink_descriptions(int payload = -1);
	std::vector<StreamDescription> get_stream_descriptions(int payload = -1);

protected:

	/** ����һ������Դ��������Դ���� graph ��
		���� < 0 ʧ��
	 */
	virtual int add_source(Source *s, KVS &params) = 0;

	/** ɾ��һ�� source
	 */
	virtual int del_source(Source *s) = 0;

	/** ���һ�������ߣ�

	  	sid: the Source id or Stream id
	 */
	virtual int add_sink(int sid, Sink *sink, KVS &params) = 0;

	/** ɾ��һ��������
	 */
	virtual int del_sink(Sink *s) = 0;

	virtual int add_stream(Stream *s, KVS &params) = 0;
	virtual int del_stream(Stream *s) = 0;

	/** �� id1, id2 ����Ƶλ�ã���������ģʽʵ�֣�ȱʡ��Ϊ�ɹ�
	 */
	virtual int vs_exchange_position(int id1, int id2) { return 0; }

	// ���� add_sink ʱ�ж� sid �� Source ���� Stream
	bool is_source_id(int sid)
	{
		return (sid % 2) == 1;
	}

protected:
	typedef std::vector<Source*> SOURCES;
	SOURCES sources_;
	ost::Mutex cs_sources_;
	
	typedef std::vector<Sink*> SINKS;
	SINKS sinks_;
	ost::Mutex cs_sinks_;

	typedef std::vector<Stream*> STREAMS;
	STREAMS streams_;
	ost::Mutex cs_streams_;
	std::string desc_;

private:
	double time_begin_;	// ��������ʱ��
	int sid_, sink_id_, stream_id_;		// ��������Ψһ�� id��sid_ ���������� stream_id_ ����ż��
	int cid_;	// ���� id

	XMPP_NOTIFY_DATAS xmpp_notify_datas_;
	ost::Mutex cs_xmpp_notify_data_;

	typedef std::map<std::string, std::deque<void*> > REG_NOTIFY_TOKENS;
	REG_NOTIFY_TOKENS reg_notify_tokens_;
	ost::Mutex cs_reg_notify_tokens_;

	int addStream(int payload, const char *desc, bool sink, bool publisher, KVS &params, KVS &result);

	bool idle_;	// �Ƿ����
	double stamp_idle_;	// ��� idle ��ʱ�䣬����ϵ idle ���� 2 ���ӣ�����Ϊ�� conference ������

protected:
	Source *find_source(int sid);
	Stream *find_stream(int sid);
};
