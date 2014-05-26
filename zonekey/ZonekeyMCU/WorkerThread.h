#pragma once

#include <cc++/thread.h>

class WorkerThread : public ost::Thread
{
public:
	WorkerThread(void);
	virtual ~WorkerThread(void);

	// �������̷߳��Ϳ��������ֱ�������߳�ִ���� reply() �󣬲��ܷ��� res_code_
	int req(int code)
	{
		req_code_ = code;
		evt_req_.signal();
		evt_res_.wait();
		int res = res_code_;
		evt_res_.reset();
		return res;
	}

protected:
	// �����̵߳��ã���ȡ��һ����������
	bool has_req(int *code, int waitms)
	{
		if (evt_req_.wait(waitms)) {
			*code = req_code_;
			evt_req_.reset();
			return true;
		}
		else
			return false;
	}

	void reply(int res_code)
	{
		res_code_ = res_code;
		evt_res_.signal();
	}

private:
	int req_code_, res_code_;
	ost::Event evt_req_, evt_res_;
};
