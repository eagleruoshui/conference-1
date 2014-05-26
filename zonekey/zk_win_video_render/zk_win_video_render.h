#pragma once

/** �������ҪΪ�˷���� c# ʹ��

		c# �ṩһ�����ھ�����ṩ server_ip, server_rtp, server_rtcp �������յ����ݿڣ�����ָ���Ĵ�������ʾ��Ƶ
 */

#ifdef __cplusplus
extern "C" {
#endif // c++

typedef struct zk_vr_t zk_vr_t;

// �������ȵ��ã�
void zkvr_init();

// ��ȡһ��ʵ��
zk_vr_t *zkvr_open(void *hwnd, const char *server_ip, int server_rtp_port, int server_rtcp_port);

// �ͷ�
void zkvr_close(zk_vr_t *ins);

#ifdef __cplusplus
}
#endif // c++
