
#ifndef _xmpp_uac__hh
#define _xmpp_uac__hh

typedef struct uac_token_t uac_token_t;

#define MAX_GROUP_CNT 8

/** һ����ϵ����Ϣ;
 */
typedef struct uac_roster_item{
	uac_roster_item *next;
	char *jid;
	char *name;
	char *subscription;	// both, from, to, none �������Ĺ�ϵ;  
	char *group[MAX_GROUP_CNT];		// �������顱������ͬʱ���ڶ����, ���8��;
	int group_cnt;	
} uac_roster_item;

#ifdef __cplusplus
extern "C" {
#endif

void zk_xmpp_uac_init(void);
void zk_xmpp_uac_shutdown(void);
	
typedef struct cb_xmpp_uac{
	
	void (*cb_receive_presence_available)(uac_token_t *token, char *remote_jid, int is_available, void *userdata);
	//����ֵΪ1��ʾͬ��, Ϊ0��ʾ��ͬ��;
	int (*cb_receive_subscription_req)(uac_token_t *token, char *remote_jid, int is_suscription, void *userdata);
	//����ֵΪ1��ʾȷ��, Ϊ0��ʾ����;
	int (*cb_receive_subscription_rsp)(uac_token_t *token, char *remote_jid, int is_promised, void *userdata);
	//roster_listΪ����,Ҫ��free()�������ͷ�;
	void (*cb_receive_roster)(uac_token_t *token, uac_roster_item *roster_list, int is_update, void *userdata);

	/** 
		@param is_connected: 1,���ӽ���; 0, ���ӶϿ�;
	 */
	void (*cb_connect_notify)(uac_token_t *const token, int is_connected, void *userdata);
} cb_xmpp_uac;

//ret��NULL Ϊ�ɹ����͵�¼��Ϣ;�Ƿ��¼�ɹ��鿴cb_connect_notify;
uac_token_t *zk_xmpp_uac_log_in(const char *jid, const char *passwd, cb_xmpp_uac *cbs, void *opaque);

void zk_xmpp_uac_log_out(uac_token_t *token);

typedef struct zk_xmpp_uac_msg_token{
	char *remote_jid;
	char *cmd;
	char *option;
	void *userdata;
}zk_xmpp_uac_msg_token;

typedef void (*uac_receive_rsp)(zk_xmpp_uac_msg_token *rsp_token, const char *result, const char *option);
bool zk_xmpp_uac_send_cmd(uac_token_t *token, const char *remote_jid, const char *cmd, const char *option, void *opaque,
		            uac_receive_rsp cb);
	
enum uac_subscribe_type{
	UAC_OPT_SUBSCRIBE,
	UAC_OPT_UNSUBSCRIBE
};

int zk_xmpp_uac_subscribe(uac_token_t *token, const char *remote_jid, uac_subscribe_type opt);

typedef void (*cb_query_presence)(uac_token_t* token, const char *remote_jid, int is_present);
void zk_xmpp_uac_ping(uac_token_t *token, const char *remote_jid, cb_query_presence callback);

#ifdef __cplusplus
}
#endif

#endif

