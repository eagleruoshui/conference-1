#ifndef _media_tcpsrv_hh_
#define _media_tcpsrv_hh_

#ifdef __cplusplus
extern "C" {
#endif // c++

typedef void TSContext;
#if !defined WIN32
typedef int SOCKET;
#else
#	include <winsock2.h>
#endif //

/** ÿ��callback������ʹ�ö������̣߳�ʹ���߿���������ص��д�������ʱ�䣬������أ�socket�����ر�
*/
typedef void (*TSNewConnection)(void *opaque, SOCKET sock, int fromport, const char *fromip);

int ts_open (TSContext **handler, int port, const char *bindip, TSNewConnection proc, void *opaque);
int ts_close (TSContext *handler);

struct TSInfo
{
	int thread_worker_total;	//
	int thread_worker_idle;
};

int ts_get_info (TSContext *handler, struct TSInfo *info);

#ifdef __cplusplus
}
#endif // c++

#endif // tcpsrv.h
