#pragma once

/** ����Ƿ��б��� webcam������У����Դ��л�ȡ h264 ֡���ݣ�������ͨ�� zkmcu_hlp_h264_source �ӿڷ��ͳ�ȥ
 */

#ifndef __cplusplus
extern "C" {
#endif // c++

typedef struct zkmcu_hlp_webcam_t zkmcu_hlp_webcam_t;

// �����Ƿ���ڿ��õ� webcam
int zkmcu_hlp_webcam_exist();

// ���豸
zkmcu_hlp_webcam_t *zkmcu_hlp_webcam_open(int width, int height, double fps, int kbitrate);

// ���豸��ȡ�� h264 ���ݣ�����ģʽ
int zkmcu_hlp_webcam_get_h264(zkmcu_hlp_webcam_t *ctx, const void **data, double *pts);

// �ر��豸
void zkmcu_hlp_webcam_close(zkmcu_hlp_webcam_t *ctx);

#ifndef __cplusplus
}
#endif // c++
