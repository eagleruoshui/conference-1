
#pragma once

#include <mediastreamer2/mediastream.h>
#include <ortp/ortp.h>
#include <map>
#include "ZonekeySink.h"
#include "ZonekeyStream.h"
#include "util.h"

// ����ģʽ����ϯ������...
enum ZonekeyConferenceMode
{
	CM_DIRCETOR,	// ����ģʽ������ģʽ�У����г�Ա����Ƶ�ɵ���������ϣ�ѹ��������ÿ����Ա
	CM_FREE,		// ����ģʽ��ÿ����Ա����ѡ��ϣ����������
};

/** ����Ļ����ӿڣ�

	�ֳ������Ա��
		һ���������ṩ�ߣ������ߣ���ʹ�� add_stream()/del_stream() �ӿڣ�
		һ�����������ʹ���ţ����ڣ���ʹ�� add_sink()/del_sink() �ӿ�

 */
class ZonekeyConference
{
public:
	ZonekeyConference(ZonekeyConferenceMode mode);
	virtual ~ZonekeyConference(void);

	/** ���˫��ͨѶʵ����params һ��Ϊ���� sdp ��õ���key,value

			ĿǰҪ��
			    client_ip: 
				client_port:
				payload: xxx

			TODO:	Ҳ���ܰ�����ѡ�ߣ���ҪЭ��
	 */
	ZonekeyStream *add_stream(KVS &params);
	void del_stream(ZonekeyStream *stream);
	ZonekeyStream *get_stream(int id);

	// ���ɾ�������ߣ���Щֻ����ͨ����
	ZonekeySink *add_sink(KVS &params);
	void del_sink(ZonekeySink *sink);
	ZonekeySink *get_sink(int id);
	
	// ��������
	virtual int set_params(const char *prop, KVS &params)
	{
		return -1;
	}

	// ��ȡ����
	virtual int get_params(const char *prop, KVS &results)
	{
		return -1;
	}

	// ���ػ���ģʽ
	ZonekeyConferenceMode mode() const { return mode_; }

protected:
	// ���������ʵ�֣��� add_stream() �е���
	virtual ZonekeyStream *createStream(RtpSession *sess, KVS &params, int id) = 0;
	virtual void freeStream(ZonekeyStream *stream) = 0;

	// ��������������ʵ�֣��� add_sink()�е���
	virtual ZonekeySink *createSink(RtpSession *rtp_session, KVS &params, int id) = 0;
	virtual void freeSink(ZonekeySink *sink) = 0;

protected:
	ZonekeyConferenceMode mode_;

private:
	typedef std::map<int, ZonekeyStream*> STREAMS;
	STREAMS map_streams_;

	typedef std::map<int, ZonekeySink*> SINKS;
	SINKS map_sinks_;

	int stream_id_, sink_id_;		// 
};
