#ifndef _audiostream_gips__hh
#define _audiostream_gips__hh

#include <mediastreamer2/msfilter.h>

#ifdef __cplusplus
extern "C" {
#endif // c++

typedef struct AudioStreamGips AudioStreamGips;
typedef void (*as_gips_on_audio_data)(void *audio_data, int size, int len, void *userdata);
/** �½�ʵ����ָ���Լ��� rtpport, rtcpport
 */
MS2_PUBLIC AudioStreamGips *audio_stream_gips_new(int rtpport, int rtcpport);

/** ����һ����Զ�ˣ�ip:rtpport:rtcpport) ����Ƶ�ػ�
 */
MS2_PUBLIC int audio_stream_gips_start(AudioStreamGips *asg, int payload_type, const char *remote_ip, int remote_rtp, int remote_rtcp,	int jitter_comp);

/** ֹͣ
 */
MS2_PUBLIC int audio_stream_gips_stop(AudioStreamGips *asg);

MS2_PUBLIC int audio_stream_record_start(AudioStreamGips *asg, as_gips_on_audio_data, void *userdata);
MS2_PUBLIC int audio_stream_record_stop(AudioStreamGips *asg);
/** �ͷ�
 */
MS2_PUBLIC void audio_stream_gips_destroy(AudioStreamGips *asg);

#ifdef __cplusplus
}
#endif // c++

#endif // audiostream_gips.h
