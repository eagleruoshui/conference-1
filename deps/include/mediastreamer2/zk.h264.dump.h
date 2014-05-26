/**  render filter, һ������������ h264 rtp recver �ϣ����յ��� rtp ���Ϊ h264 frame(slice)��Ȼ�����
 */

#ifndef _zonekey_h264_dump__hh
#define _zonekey_h264_dump__hh

#include "mediastream.h"
#include "allfilters.h"

#ifdef __cplusplus
extern "C" {
#endif

#define ZONEKEY_ID_H264_DUMP (MSFilterInterfaceBegin+15)

/** ���ڲ��յ������� frame �󣬻ص�
 */
typedef struct ZonekeyH264DumpParam
{
	void *opaque;
	void (*push)(void *opaque, const void *data, int len, double stamp, int key);
} ZonekeyH264DumpParam;

// method id
#define ZONEKEY_METHOD_H264_DUMP_SET_CALLBACK_PARAM		MS_FILTER_METHOD(ZONEKEY_ID_H264_DUMP, 1, ZonekeyH264DumpParam)

MS2_PUBLIC void zonekey_h264_dump_register();

#ifdef __cplusplus
}
#endif

#endif 
