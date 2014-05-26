/** ���� xmpp ʵ��ԭ����mseģ���з���ע��

		// ��ʼ������
		ctx = zkrbt_mse_open("app.zonekey.com.cn");	// ָ��xmpp server

		// ע�����callback_cmd Ϊ����ص�
		zkr_mse_reg_service(ctx, service_name, service_type, service_url, callback_cmd, opaque);

		// ע���豸
		zkr_mse_reg_device(ctx, device_name, device_type, device_desc,callback_cmd, opaque);

		// �ͷ�
		zkrbt_mse_close(ctx);

	�ڲ�ʵ�֣�
		ע�����
			zkrbt_mse_reg_service():
				if (zkrbt_create(...) failure)
					zkrbt_register_irb(....)
				if (zkrbt_create(...) failure)
					error

				zkrbt_get_roster()
				if (root@domain is not in contacts)
					zkrbt_subscribe(root@...)

				�� command Ϊ internal.describe ʱ�����ط�����Ϣ

 */

#ifndef _zkrobot_mse__hh
#define _zkrobot_mse__hh

#ifdef __cplusplus
extern "C" {
#endif /* c++ */

#define ROBOT_MSE_VERSION "0.0.1"

typedef struct zkrbt_mse_t zkrbt_mse_t;

/** �豸���ԣ������Ч��ӦΪ null
 */
struct zkrbt_mse_device_type
{
	const char *vendor;
	const char *model;
	const char *version;
	const char *info;
};

/** ���յ� ReqCmd ʱ���ص������� serviceһ����������񣬶����豸��һ��������
		@param opaque
		@param jid: ������Ϣ�� jid
		@param cmd_str: �����ַ���
		@param cmd_options: ��������ַ���
		@param result_str: ���Σ�ִ�н����������� zkrbt_mse_malloc() �����ڴ棬���ɿ⸺���ͷ�! ����Ϊ null
		@return 0 �ɹ���-1 ʧ��
 */
typedef int (*PFN_zkrbt_mse_command)(void *opaque, const char *jid, const char *cmd_str, const char *cmd_options, char **result_str);

zkrbt_mse_t *zkrbt_mse_open(const char *xmpp_domain);
void zkrbt_mse_close(zkrbt_mse_t *ctx);

int zkrbt_mse_reg_service(zkrbt_mse_t *ctx, const char *service_name, const char *service_type, const char *service_url,
						  PFN_zkrbt_mse_command callback_cmd, void *opaque);
int zkrbt_mse_reg_device(zkrbt_mse_t *ctx, const char *device_name, const char *device_type, const struct zkrbt_mse_device_type *desc,
						 PFN_zkrbt_mse_command callback_cmd, void *opaque);

// �����ڴ�
void *zkrbt_mse_malloc(int size);


#ifdef __cplusplus
}
#endif /* c++ */

#endif /* zkrobot_mse.h */
