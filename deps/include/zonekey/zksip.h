#ifndef _zksip__hh
#define _zksip__hh

#include "../common/zkconfig/zkconfig.h"

#ifdef __cplusplus
extern "C" {
#endif 

typedef struct zksip_callback
{
	/* ���յ��Է�ring��Ϣ��֪ͨӦ�ý���
	 * @param call_id zksip_make_call()���õ�call id
	 * @param user_data ��sipʵ���󶨵��û�����
	 */
	void (*on_remote_ringning)(int call_id, void *user_data);
	
	/* ���յ��Է�������Ϣ��֪ͨӦ�ý���
	 * @param call_id zksip_make_call()���õ�call id
	 * @param user_data ��sipʵ���󶨵��û�����
	 */
	void (*on_accepted)(int call_id, void *user_data);
    
	/* �����з�����ʧ��ʱ֪ͨӦ�ý���
	 * �޸�ʱ�ɸ�on_disconneted()���Ӳ���code
	 * ��on_call_failed()����on_disconnected()
	 * @param call_id zksip_make_call()���õ�call id
	 * @param code ����ʧ�ܵĴ����룬���类���з��ܾ�ʱ��code==603
	 * @param user_data ��sipʵ���󶨵��û�����
	 */
	void (*on_caller_failed)(int call_id, int code, void *user_data);
	
	/* ���յ����к�֪ͨӦ�ý���,�ú���������������
	 * @param reomte_uri ���з���uri
	 * @param call_id Ϊ���κ��з����call id
	 */
	void (*on_incoming_call)(int call_id, void *user_data);
	
	/* �����ӶϿ���֪ͨӦ�ý���
	 * @param call_id ����ͨ����call id
	 * @param user_data ��sipʵ���󶨵��û�����
	 */
	void (*on_disconneted)(int call_id, void *user_data);	
}zksip_callback;

typedef struct zksip_cred_info
{
	char    *realm;	
    char	*username;	
    char	*pwd;		
}zksip_cred_info;

typedef struct zksip_register_cfg
{
	/* ��ѡ��(����ΪNULL����������ʾ�����û����û���
	 */
	char *display_name;

	/** 
     * The full SIP URL for the account. The value can take name address or 
     * URL format, and will look something like "sip:account@serviceprovider".
     *
     * This field is mandatory.
	 * ����ֶ��Ǳ����е�
     */
	char *id;
	
	/** 
     * This is the URL to be put in the request URI for the registration,
     * and will look something like "sip:serviceprovider".
     *
     * This field should be specified if registration is desired. If the
     * value is empty, no account registration will be performed.
	 * �����Ҫע�ᣬ����ֶξ��Ǳ����
     */
	char *reg_uri;

	zksip_cred_info cred_info;

	/* ע��/ע���ɹ���ʧ��ʱ����
	 * @param status  ע��/ע���ɹ�ʱstatusΪ0������Ϊ�����Ĵ�����
	 * @param regc_data �û�ע��ʱ�󶨵�����
	 */
	void (*zksip_regc_cb)(int status, void *regc_data);
}zksip_register_cfg;

typedef struct zksip_cfg
{
	zksip_callback cb;
	int max_calls; //��֧�ֵ�call�����������Ӧ����0С�ڵ���32��
	void *userdata; //�û��豣֤���������ڼ�userdataһֱ��Ч

	zkconfig_t *conf;
}zksip_cfg;



/* ��ʼ�� 
 * @return �ɹ�����0��ʧ�ܷ��غ��ʵĴ�����
 */
int zklib_init(void);


/* ����sipʵ��
 * ע�⣺һ������ֻ�ܴ���һ��ʵ������zksip_createֻ�ܵ���һ��
 * @param cfg ���û���д��������Ϣ
 * @return �ɹ�����0, ʧ�ܷ��غ��ʵĴ�����
 */
int zksip_create(zksip_cfg *cfg);


/* run����������zksip�е��¼�
 * @param msec_timeout ʱ�������Ժ���ơ���Ӧ��̫���Ƽ�Ϊ10
 */
void zksip_run(int msec_timeout);

/* ע�ᣬ�첽����ģʽ
 * @param regc_cfg ע�������������Ϣ
 * @return ���ɹ�����ע����Ϣʱ������0�����򷵻غ��ʵĴ�����
 * ע�⼴ʹ��������0��������ʾע��ɹ���Ӧ�ں�������0��
 * ���zksip_regc_cb�����е�status����
 * ע��ɹ���ÿ��һ��ʱ���Զ�����ע�ᣬ
 * ��ʱ��ÿ�ζ������regc_cfg�е�zksip_regc_cb
 */
int zksip_register(zksip_register_cfg *regc_cfg, void *regc_data);

/* ȡ��ע��
 * @return �ɹ�����ȡ��ע�����Ϣ�󷵻�0
 */
int zksip_unregister(void);

/* ����ָ����uri
 * @param dst_uri Ҫ���е��û���uri,���� "sip:example@remote"
 * @param p_call_id ��������call_id��ָ��
 * @param sdp ����sdp�ַ���
 * @return ����ɹ�����0�����򷵻�����ʧ�ܵĴ�����
 */
int zksip_make_call(const char *dst_uri, int *p_call_id, const char *sdp);


/* ���з�ѡ����ܺ���,һ����on_incoming_call()�е���
 * @param call_id Ϊ����ͨ��������call id
 * @param sdp ����sdp�ַ���
 */ 
void zksip_accept(int call_id, const char *sdp);

/* ���з�ѡ��ܾ�����,һ����on_incoming_call()�е���
 * @param call_id Ϊ����ͨ��������call id
 */
void zksip_reject(int call_id, int code=603);


/* �Ҷ�
 * �ں��з�����calling��ringing��confirmed��ͨ��
 * �ڱ����з�����confirmedͨ��
 * @param ins ��zksip_creat()���ص�ָ��
 * @param call_id Ҫ�Ҷϵ�call��id
 * @return �ɹ�����0�����򷵻�����ʧ�ܵĴ�����,ʧ��ԭ�����call_id����ȷ��
 */
int zksip_hang_up(int call_id);


/* ���ٴ�����sipʵ��
 * �ɹ�����0��ʧ�ܷ��غ��ʵĴ�����
 */
int zksip_destroy(void);

/* �õ�Զ�˵���call_id��ص�sdp
 * @param call_id 
 * @return �����Զ�˵�sdp������Զ��sdp�ַ��������򣬷���NULL
 */
const char* zksip_get_remote_sdp(int call_id);

/* �õ�Զ�˵���call_id��ص�uri
 * @param call_id 
 * @return �����Զ�˵�uri������Զ��uri�ַ��������򣬷���NULL
 */
const char* zksip_get_remote_uri(int call_id);



#ifdef __cplusplus
}
#endif 



#endif //_zksip__hh



