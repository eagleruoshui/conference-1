/** �� c# ��ָ��һ�� hwnd��ָ�� server_ip, rtp_port, rtcp_port�����ܹ��� hwnd �в�����Ƶ
 */
#ifndef _zkmcu_hlp_h264_render__hh
#define _zkmcu_hlp_h264_render__hh

#include <ortp/ortp.h>

#ifdef __cplusplus
extern "C" {
#endif


typedef struct zkmcu_hlp_h264_render_t zkmcu_hlp_h264_render_t;

/** render ʹ�� PostMessage(hwnd, 0x7789, ....) �ķ�ʽ֪ͨ����Ƶ����
	���յ��ؼ�֡��֮�� ....
 */
zkmcu_hlp_h264_render_t *zkmcu_hlp_h264_render_open(void *hwnd, const char *server_ip, int server_rtp_port, int server_rtcp_port);
void zkmcu_hlp_h264_render_close(zkmcu_hlp_h264_render_t *ins);

typedef  void (*on_rtcp_notify)(zkmcu_hlp_h264_render_t *ins, OrtpEvent *ev, void *userdata);

/** @warning */
void zkmcu_hlp_h264_render_set_notify(zkmcu_hlp_h264_render_t *ins, on_rtcp_notify cb, void *userdata);

void zkmcu_hlp_h264_render_get_rtp_stat(zkmcu_hlp_h264_render_t *ins, rtp_stats_t *stat);

#ifdef __cplusplus
}
#endif

#endif
