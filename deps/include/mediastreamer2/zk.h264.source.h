#ifndef __zonekey_h264_source__hh
#define __zonekey_h264_source__hh

#include "allfilters.h"
#include "msfilter.h"

#ifdef __cplusplus
extern "C" {
#endif // c++

typedef struct ZonekeyH264SourceWriterParam
{
	void *ctx; /** ���ڵ��� write */

	/** �ⲿӦ�õ��ã��ṩ h264 ֡���ݣ�data ����Ϊ������frame
		stamp Ϊ ��
	 */
	int (*write)(void *ctx, const void *data, int len, double stamp);
} ZonekeyH264SourceWriterParam;

// �Ǻǣ����Զ�㣬��ֹ��ͻ
#define ZONEKEY_ID_H264_SOURCE (MSFilterInterfaceBegin+10)

#define ZONEKEY_METHOD_H264_SOURCE_GET_WRITER_PARAM MS_FILTER_METHOD(ZONEKEY_ID_H264_SOURCE, 1, ZonekeyH264SourceWriterParam)

// ע��filter
MS2_PUBLIC void zonekey_h264_source_register();

#ifdef __cplusplus
}
#endif // c++

#endif /* zk.h264.source.h */
