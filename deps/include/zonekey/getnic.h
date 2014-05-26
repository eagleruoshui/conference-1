/** ��ȡ������Ϣ */

#ifndef __get_nic__hh
#define __get_nic__hh

#ifdef __cplusplus
extern "C" {
#endif /* c++ */

/** ������Ϣ������������ȡ ethernet, ppp ���� */
struct zknet_nic_describe
{
	char *name;
	char *desc;
	int type;			// 1: ethernet, 2: ppp��3��wifi ������ win vista ֮�󣬷���wifi����Ϊ ethernet��
	char *hwaddr;	// 0 ������Ӳ����ַ��һ��Ϊ mac ��ַ
};

/** ��ȡ������Ϣ
		@param info Ϊ���Σ�������� zknet_free_nic_describe() �ͷ�
		@param cnt ���Σ����� zknet_nic_describe ����
		@return < 0 ʧ�ܣ��ɹ�ʱ = *cnt
 */
int zknet_get_nic_describe(struct zknet_nic_describe **info, int *cnt);
void zknet_free_nic_describe(struct zknet_nic_describe *info, int cnt);

/** ��ȡip��ַ��Ϣ

		char **ips = 0;
		int cnt = 0;
		zknet_get_ips(&ips, &cnt);
		for (int i = 0; i < cnt; i++) {
			ips[i] ...
		}
		zknet_free_ips(ips, cnt);
 */
int zknet_get_ips(char ***ips, int *cnt);
void zknet_free_ips(char **ips, int cnt);

#ifdef __cplusplus
}
#endif /* c++ */

#endif /** getnic.h */
