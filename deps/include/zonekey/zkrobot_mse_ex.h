#ifndef _zkrobot_mse_ex__hh
#define _zkrobot_mse_ex__hh

#ifdef __cplusplus
extern "C" {
#endif

void zkrbt_mse_init(const char *domain);
void zkrbt_mse_shutdown(void);

enum zkrobot_mse_catalog
{
	ZKROBOT_SERVICE,
	ZKROBOT_DEVICE,
	ZKROBOT_LOGIC,
	ZKROBOT_OFFLINE
};

typedef struct zkrobot_mse{
	char id[256];
	//char resource[256];
	zkrobot_mse_catalog catalog;
	union {
		struct {
			char url[256];
		} service;

		struct  {
			char vendor[256];
			char model[256];
			char version[64];
			char info[256];
		} device;
	}mse_ins;
	
	char type[32];
} zkrobot_mse;

typedef struct zkrobot_mse_t zkrobot_mse_t;

typedef struct zk_mse_cbs{
/** 
	���յ�message����������(reqcmd)ʱ����;
	@param remote_jid: ������ߵ�jid;
	@param str_cmd: �����ַ���;
	@param str_option: �������;
	@param is_delay: �������Ƿ�����������. 0, ����; 1, ��;
	@param userdata: zkrbt_login()�����opaqueָ��;
	@param token: ����zkrbt_respond, ����ΪNULLʱ, ����Ҫ�û��ظ�;
	*/
void (*PFN_zkrbt_mse_command)(zkrobot_mse_t *mse_ins, const char *remote_jid, const char *str_cmd, const char *str_option, 
	                int is_delay, void *userdata, void *token);

/**
	�Ƿ��¼�ɹ�;
*/
void (*PFN_zkrbt_mse_log_in)(zkrobot_mse_t * mse_ins, int is_successful, void *userdata);
}zk_mse_cbs;

/** 
	�ɹ�����mse����ʵ��ָ��, ʧ�ܷ���NULL;
	�Ƿ��¼�ɹ�Ҫ��PFN_zkrbt_mse_log_in�ص�;
*/
zkrobot_mse_t *zkrbt_mse_new(zkrobot_mse *mse, zk_mse_cbs *cbs, void *userdata);
void zkrbt_mse_destroy(zkrobot_mse_t *rbt_mse);

int zkrbt_mse_respond(void *token, const char *result, const char *param);

#ifdef __cplusplus
}
#endif

#endif


