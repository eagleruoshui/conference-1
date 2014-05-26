#ifndef _fifo_hh
#define _fifo_hh

/** ʵ��һ���򵥵� fifo */

#ifdef __cplusplus
extern "C" {
#endif /* c++ */

typedef struct fifo_t fifo_t;

fifo_t *fifo_new();
void fifo_free(fifo_t *);

/** ���س��� */
int fifo_size(fifo_t *f);

/** ׷�� */
void fifo_add(fifo_t *f, void *data);

/** ȡ�������� fifo_size() > 0 */
void *fifo_pop(fifo_t *f);


#ifdef __cplusplus
}
#endif /* c++ */

#endif /* fifo.h */
