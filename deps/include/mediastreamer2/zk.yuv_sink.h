#ifndef __zonekey_yuv_sink__hh
#define __zonekey_yuv_sink__hh

#include "allfilters.h"
#include "msfilter.h"

#ifdef __cplusplus
extern "C" {
#endif // c++

/** Ϊ�˷���Ӧ�ó���� yuv sink �ж�ȡÿһ֡����
    ��������һ���ص��������������� yuv ͼ��ʱ��������֮
 */
typedef struct ZonekeyYUVSinkCallbackParam
{
	void *ctx;		// �����ڵ��� push ����.
	void (*push)(void *ctx, int width, int height, unsigned char *data[4], int stride[4]);	// �ص����������յ������� yuv420p ���ݺ󣬽����øú���ָ��.
} ZonekeyYUVSinkCallbackParam;

// filter id
#define ZONEKEY_ID_YUV_SINK (MSFilterInterfaceBegin+11)

// method id
#define ZONEKEY_METHOD_YUV_SINK_SET_CALLBACK_PARAM		MS_FILTER_METHOD(ZONEKEY_ID_YUV_SINK, 1, ZonekeyYUVSinkCallbackParam)
#define ZONEKEY_METHOD_YUV_SINK_SET_IMAGE_WIDTH			MS_FILTER_METHOD(ZONEKEY_ID_YUV_SINK, 2, int)
#define ZONEKEY_METHOD_YUV_SINK_SET_IMAGE_HEIGHT		MS_FILTER_METHOD(ZONEKEY_ID_YUV_SINK, 3, int)

// ע��filter
MS2_PUBLIC void zonekey_yuv_sink_register();

#ifdef __cplusplus
}
#endif // c++

#endif /* zk.yuv_sink.h */
