#pragma once

#include <zonekey/xmpp_uac.h>

// ����һ����Ҫ��������񣬵� NormalUser ����ʱ��login �ɹ��������͵�ָ���� jid�������յ� response ��...
struct XmppTask
{
	std::string remote_jid;
	std::string cmd;
	std::string option;
	void *opaque;
	uac_receive_rsp cb;
	bool sent;
};

/** һ�������࣬��Ӧ�ŵ�¼�� xmpp server �� normaluser �û������ڷ��ͽ��� xmpp message
	���ǽ�����棬�� is_ok() �Ƿ��� ....
 */
class NormalUser
{
	NormalUser(void);
	~NormalUser(void);
	static NormalUser *_instance;
	bool connected_;
	uac_token_t *uac_;
	bool quit_;
	ost::Mutex cs_tasks_;
	typedef std::vector<XmppTask *> TASKS;
	TASKS tasks_, tasks_waiting_res_;	// һ�����ڻ��淢�͵��б�һ�����ڵȴ��յ� respond
	ost::Event evt_task_added_;
	uintptr_t th_;

public:
	static NormalUser *Instance()
	{
		if (!_instance)
			_instance = new NormalUser;
		return _instance;
	}

	void addTask(const char *remote_jid, const char *cmd, const char *option, void *opaque, uac_receive_rsp cb)
	{
		XmppTask *task = new XmppTask;
		task->remote_jid = remote_jid;
		task->cmd = cmd;
		task->option = option;
		task->opaque = opaque;
		task->cb = cb;

		ost::MutexLock al(cs_tasks_);
		tasks_.push_back(task);
		evt_task_added_.signal();
	}

private:
	static void cb_connect_notify(uac_token_t *const token, int is_connected, void *userdata);
	static unsigned __stdcall _proc_run(void *p);	// �����߳�
	void proc_run();
	bool is_ok() const { return connected_; }	// �����Ƿ���ã�
	void send_all_pending_tasks();	// �������� tasks
};
