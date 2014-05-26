#ifndef _zk_media_audio_win_common__hh
#define _zk_media_audio_win_common__hh

#ifdef __cplusplus
extern "C" {
#endif // c++

// ���ò���. 
typedef struct zkMediaAudioConfig
{
	int channels;		// 
	int bits;			// ������С��16bits...
	int rate;			// ������
	int frame_size;		// �ص�֪ͨ��һ�δ�С���ֽڴ�С. 
} zkMediaAudioConfig;

// �ص�����ԭ��, ��������Чʱ������ capture��˵�������Ѿ���ã����� player��˵����Ҫ�������ṩ����. 
// ������������������Ӧ�þ��췵��
typedef void (*PFN_zkMediaAudioCallback)(void *opaque, void *pcm, int len);

#ifdef __cplusplus
}
#endif // c++

#endif 
