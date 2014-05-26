#ifndef _zonekey_audio_mixer__hh
#define _zonekey_audio_mixer__hh

/** ���� msaudiomixer filter�����Ǳ�����һ· output����������� publisher������㲥

	outputs[0..ZONEKEY_AUDIO_MIXER_PREVIEW_PIN-1] �����������������ߣ�û·����ͬ����Ϊ��Ҫɾ���Լ���������

	ע�⣺Ҫ�� pcm ��ʽΪ 1ch, 16bits, 16000Hz

 */

#include "allfilters.h"
#include "mediastream.h"

#ifdef __cplusplus
extern "C" {
#endif // c++

#define ZONEKEY_AUDIO_MIXER_MAX_CHANNELS 20
#define ZONEKEY_ID_AUDIO_MIXER (MSFilterInterfaceBegin+14)

// ������л��
#define ZONEKEY_AUDIO_MIXER_PREVIEW_PIN (ZONEKEY_AUDIO_MIXER_MAX_CHANNELS-1)

MS2_PUBLIC void zonekey_audio_mixer_register();
MS2_PUBLIC void zonekey_audio_mixer_set_log_handler(void (*func)(const char *, ...));

#ifdef __cplusplus
}
#endif // c++

#endif // zk.audio.mixer.h
