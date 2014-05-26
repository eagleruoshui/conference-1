#ifndef _zknet_reactor_hh
#define _zknet_reactor_hh

#ifdef __cplusplus
extern "C" {
#endif // c++

typedef struct zk_reactor_t zk_reactor_t;

#define OP_READ  1
#define OP_WRITE  2
#define OP_EXCEPT  4

// ������
typedef void (*zk_fd_handler)(zk_reactor_t *act, int fd, int types, void *opaque);
typedef void (*zk_delay_handler)(zk_reactor_t *act, void *opaque);

// ��һ��selector���ڲ������������߳�
zk_reactor_t *zk_act_open();
void zk_act_close(zk_reactor_t *actor);

// ��һ�� socket �Ĳ�������� types = 0���������
int zk_act_bind(zk_reactor_t *act, int fd, int types, zk_fd_handler h, void *opaque);

// ��һ����ʱ������delay_seconds Ϊϣ���ӳٵ�����
int zk_act_bind_delay_task(zk_reactor_t *act, double delay_second, zk_delay_handler h, void *opaque);

#ifdef __cplusplus
}
#endif // c=+

#endif // reactor.h
