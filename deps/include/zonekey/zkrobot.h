#ifndef _zkrobot__hh
#define _zkrobot__hh

/** ���� XMPP��
	��Ҫʵ�����¹��ܣ�
		1. ע�᣺
		2. �������¼���
		3. �������
		4. �����������ȡ��Ӧ

	Ӧ�ó�����Ҫ��Ϊ���࣬�����ͱ���������sip�е� uac�� uas �ĸ���

	������
		zkrbt_create(...);
		zkrbt_request(...);
		...

	������
		zkrbt_create(...);
		
		zkrbt_callback.command() {
			...
			str = zkrbt_alloc(size);
			return str;
		}
 */

typedef struct zkrobot_t zkrobot_t;

#ifdef __cplusplus
extern "C" {
#endif 

/** ȫ��֪ͨ��Ϣ */
typedef struct zkrbt_callback
{
	/** ���յ������¼�ʱ�Ĵ��� 

			@param opaque: create �ṩ�Ĳ���
			@param jid: �����ߵ� jid
			@param op: 1 ���ģ�2 ȡ������
			@return 1: ����2���ܾ�
	 */
	int (*subscribed)(void *opaque, const char *jid, int op);

	/** �յ��Է�����
		
			@param jid: ����������
			@param cmd_str: �����ַ�������0�������ַ���
			@return: ������Ҫ���ص��ַ���������ʹ�� zkrbt_alloc() �����ڴ棬����ʹ�� 0 �����������ϣ���ظ���ֱ�ӷ��� null ����
	 */
	char *(*command)(void *opaque, const char *jid, const char *cmd_str);

	/** ����״̬֪ͨ
		
			@param code: ���������¼�
							0: ���ӳɹ���
							-1�����类�ж�
	 */
	void (*connect_notify)(void *opaque, int code);
} zkrbt_callback;

/** һ����ϵ����Ϣ
 */
typedef struct zkrbt_roster_contact
{
	const char *jid;
	const char *name;
	const char *subscription;	// both, from, to, none �������Ĺ�ϵ
	const char **group;			// �������顱������ͬʱ���ڶ����
	int group_cnt;

} zkrbt_roster_contact;


/** ��ʼ���⣬�������� zkrbt_xxx ֮ǰ���������ȵ���
 */
void zkrbt_init ();

/** ����һ��ʵ��

		@param ins: ���Σ�����ɹ����򷵻ض���ʵ��
		@param jid: jabberid
		@param passwd: ע�ᵽ jabber ����������Ŀ���
		@param cbs: �ص�֪ͨ�ӿ�
		@param opaque:
		@return  0 �ɹ���
				-1 
 */
int zkrbt_create (zkrobot_t **ins, const char *jid, const char *passwd, zkrbt_callback *cbs, void *opaque);

/** �ͷ�ʵ�� */
void zkrbt_close (zkrobot_t *ins);

/** ʵ����ע�ᣬ����ʵ�֣�����0�ɹ����������ֵ
 */
int zkrbt_register_ibr(const char *jid, const char *passwd, const char *email);

/** ����һ�� request�����ڴ��յ� response��һ������ client ����Ϊ

		@param response: �����0�������յ� res ��ص�
 */
int zkrbt_request (zkrobot_t *ins, const char *jid, const char *str, void (*response)(void *opaque, int code, const char *str), void *opaque);

/** �����ڴ�
 */
char *zkrbt_alloc (int size);

/** ����ĳ�� jid������ callback.subscribe ��֪ͨ�����Ƿ�ɹ�
 */
int zkrbt_subscribe (zkrobot_t *ins, const char *jid);

/** ȡ������ĳ�� jid������ callback.subscribe ��֪ͨ�Ƿ�ɹ�
 */
int zkrbt_unsubscribe (zkrobot_t *ins, const char *jid);

/** ���ػ�������Ϣ

		@param ins:
		@param contacts: ���Σ��������л������е���ϵ����Ϣ�б�����ʹ�� zkrbt_free_contact() �ͷ�ÿһ��
		@param cnt: ���Σ�contacts ��Ч��Ŀ
		@return < 0 ʧ��
 */
int zkrbt_get_roster (zkrobot_t *ins, zkrbt_roster_contact ***contacts, int *cnt);

/** �ͷ� contact �ṹ
 */
void zkrbt_free_contacts (zkrobot_t *ins, zkrbt_roster_contact **contacts, int cnt);

#ifdef __cplusplus
}
#endif

#endif /** zkrobot.h */
