#pragma once

// Ϊ�˷��� c# ���ã��� c# �����յ� OrtpEvent ֮�󣬽��� rtcp_hlp_xx ����

#ifdef __cplusplus
extern "C" {
#endif // c++

typedef struct rtcp_hlp_t rtcp_hlp_t;
typedef struct rtcp_hlp_pkg_t rtcp_hlp_pkg_t;
typedef struct rtcp_hlp_report_block_t rtcp_hlp_report_block_t;

// c# �����յ� rtcp ֪ͨ����Ҫ���ȵ������������Ȼ�󣬴Ӽ�����ȡ��Ϣ....
rtcp_hlp_t *rtcp_hlp_open(void *evt);
void rtcp_hlp_close(rtcp_hlp_t *ctx);

// ������һ�� rtcp ������Ӧÿ�� rtcp_hlp_t Ӧ��ѭ������ next��ֱ������ 0
rtcp_hlp_pkg_t *rtcp_hlp_next_pkg(rtcp_hlp_t *ctx);
int rtcp_hlp_pkg_is_sr(rtcp_hlp_pkg_t *pkg);
int rtcp_hlp_pkg_is_rr(rtcp_hlp_pkg_t *pkg);

// ������һ�� report block��ѭ����ֱ������0
rtcp_hlp_report_block_t *rtcp_hlp_report_block_next(rtcp_hlp_pkg_t *pkg);
unsigned int rtcp_hlp_report_block_ssrc(rtcp_hlp_report_block_t *rb);
unsigned int rtcp_hlp_report_block_fraction_lost(rtcp_hlp_report_block_t *rb);
unsigned int rtcp_hlp_report_block_cum_lost(rtcp_hlp_report_block_t *rb);
unsigned int rtcp_hlp_report_block_jitter(rtcp_hlp_report_block_t *rb);

#ifdef __cplusplus
}
#endif // c++
