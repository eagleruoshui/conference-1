#pragma once

#include <zonekey/zkrobot_mse_ex.h>
#include <vector>
#include <cc++/thread.h>
#include "Conference.h"

struct GlobalConfig
{
	unsigned int cap_max;	// ֪ͨ���еĻ������Ŀ

	GlobalConfig()
	{
		cap_max = -1;
	}
};

extern GlobalConfig *_gc;

// �ṩ mse ���ƽӿ�
class Server
{
public:
	Server(void);
	~Server(void);

	void run(int *quit);

private:
	// �����б�
	typedef std::vector<Conference*> CONFERENCES;
	CONFERENCES conferences_;
	ost::Mutex cs_conferences_;

	// ���ڲ��Ե�һ������ :)
	//Conference *conference_;

	zkrobot_mse_t *robot_;	// ע��Ϊ zonekey.mse �������ڽ��ܿ�������

	static void callback_login(zkrobot_mse_t * mse_ins, int is_successful, void *userdata);
	static void callback_command(zkrobot_mse_t *mse_ins, const char *remote_jid, const char *str_cmd, const char *str_option, 
	                int is_delay, void *userdata, void *token);

	int next_id_;

private:
	//Conference *test_conf_free_;	// ���������ɻ���
	//Conference *test_conf_director_;


	// ��������
	int create_conference(KVS &params, KVS &results);
	int destroy_conference(KVS &params, KVS &results);
	int list_conferences(KVS &params, KVS &results);
	int info_conference(KVS &params, KVS &results);

	// ע�����֪ͨ�������� stream/source �����仯ʱ�������� reg_notify �� response
	int reg_notify(const char *from_jid, void *uac_token, KVS &params, KVS &results);

	// ���û������
	int set_params(KVS &params, KVS &results);
	int get_params(KVS &params, KVS &results);

	// ���ɻ���
	int fc_add_source(KVS &params, KVS &results);
	int fc_del_source(KVS &params, KVS &results);
	int fc_list_sources(KVS &params, KVS &results);
	int fc_add_sink(KVS &params, KVS &results);
	int fc_del_sink(KVS &params, KVS &results);

	// ��������
	int dc_add_stream(KVS &params, KVS &results, bool publisher);
	int dc_del_stream(KVS &params, KVS &results);
	int dc_list_streams(KVS &params, KVS &results);
	int dc_add_source(KVS &params, KVS &results);
	int dc_del_source(KVS &params, KVS &results);
	int dc_add_sink(KVS &params, KVS &results);
	int dc_del_sink(KVS &params, KVS &results);
	int dc_get_all_videos(KVS &params, KVS &results);
	int dc_vs_exchange_position(KVS &params, KVS &results);	// �������� video stream ��λ��

	// ��ȡ cpu/mem/network ռ�����
	int get_sys_info(double *cpu, double *mem, double *net_recv, double *net_send);
};
