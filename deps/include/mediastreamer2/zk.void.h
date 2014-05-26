/** ֧��һ�� void source������д���������ݣ�
	֧��һ�� void sink�����Խ��ղ��ص������κ�����
 */

#ifndef _zonekey_void__hh
#define _zonekey_void__hh

#include "allfilters.h"
#include "msfilter.h"

#ifdef __cplusplus
extern "C" {
#endif // /c++

typedef struct ZonekeyVoidSinkCallback
{
	void *opaque;
	void (*on_data)(void *opaque, mblk_t *m);
} ZonekeyVoidCallback;

// filter id
#define ZONEKEY_ID_VOID_SOURCE (MSFilterInterfaceBegin+30)
#define ZONEKEY_ID_VOID_SINK (MSFilterInterfaceBegin+31)

// method
#define ZONEKEY_METHOD_VOID_SOURCE_SEND			MS_FILTER_METHOD(ZONEKEY_ID_VOID_SOURCE, 1, mblk_t *)
#define ZONEKEY_METHOD_VOID_SINK_SET_CALLBACK	MS_FILTER_METHOD(ZONEKEY_ID_VOID_SINK, 1, ZonekeyVoidSinkCallback*)

// ע��
MS2_PUBLIC void zonekey_void_register();

#ifdef __cplusplus
}
#endif // c++

#endif // zk.void.h
