#pragma once

#include <mediastreamer2/mediastream.h>
#include <ortp/ortp.h>
#include <mediastreamer2/msrtp.h>

/// �� mcu �� sink ����ý������
class SinkBase
{
	void (*cb_data_)(void *opaque, double stamp, void *data, int len, bool key);
	void *opaque_;
	std::string mcu_ip_;
	int mcu_rtp_port_, mcu_rtcp_port_, payload_;
	MSTicker *ticker_;
	MSFilter *f_recv_, *f_void_;	// rtp recv filter �� zonekey void filter
	RtpSession *rtp_;
	bool first_frame_;
	uint32_t last_stamp_;	// ���ڴ���ʱ�����������
	double next_stamp_;	// ʹ����ĵ�ǰʱ�����
	ost::Mutex cs_;

public:
	/// data Ϊ�ص����������յ����ݣ�����֡���󣬽������ã�ע�⣬Ϊ�ڲ��߳� ...
	SinkBase(int payload, const char *mcu_ip, int mcu_rtp_port, int mcu_rtcp_port, void (*data)(void *opaque, double stamp, void *data, int len, bool key), void *opaque);
	virtual ~SinkBase(void);

	// ���� ...
	int Start();
	int Stop();

	// �ⲿ�����������������ִ�еĶ������紦�� rtcp ��
	void RunOnce();

protected:
	virtual double payload_freq() const = 0;	// ���ض�Ӧ���͵�ʱ���Ƶ�ʣ�һ����˵�� h264 ʹ�� 90000.0���� ilbc ʹ�� 8000.0
	virtual MSQueue *post_handle(mblk_t *im) = 0;	// ���յ����ݺ�Ĵ���

	virtual MSFilter *trans_in() { return 0; }
	virtual MSFilter *trans_out() { return 0; }

	virtual int size_for(int index, mblk_t *im) { return im->b_wptr - im->b_rptr; }
	virtual void save_for(int index, mblk_t *im, unsigned char *buf) { memcpy(buf, im->b_rptr, im->b_wptr - im->b_rptr); }
	virtual bool is_key(int index, mblk_t *im) { return true; }	// ��Ƶ���� key

	MSQueue queue_;

private:
	static void cb_data(void *opaque, mblk_t *data)
	{
		((SinkBase*)opaque)->cb_data(data);
	}

	void cb_data(mblk_t *data);
};
