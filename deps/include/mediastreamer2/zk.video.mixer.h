/** zonekey video mixer filter���� filter Ŀ��Ϊ�˷���ʵ����Ƶ�����еġ���ϯģʽ��

		��ϯģʽ��ÿ· rtp sender ������ͬ��ѹ�������
		rtp recv --> h264 decoder  \                        
		rtp recv --> h264 decoder  --> zonekey video mixer ---> YUV sink (outputs[0] ��� yuv Ԥ��)
		rtp recv --> h264 decoder  /         ^            \
		...           ...                    |             \--> tee (outputs[1] ���h264������һ����� tee filter���ַ���ÿ�������ߣ����߷����� fms �Ϲ��㲥)
											 |		  
										method call	  
							     ���޸� x264 ѹ�����������������߻���֮���.... ��			              


 */

#ifndef _zonekey_video_mixer__hh
#define _zonekey_video_mixer__hh

#ifdef __cplusplus
extern "C" {
#endif // c++

#include "allfilters.h"
#include "mediastream.h"

// ���� ms_filter_lookup_by_name()
#define ZONEKEY_VIDEO_MIXER_NAME		"ZonekeyVideoMixer"

// ͬʱ֧�ֵ������
#define ZONEKEY_VIDEO_MIXER_MAX_CHANNELS 20

// ���
#define ZONEKEY_VIDEO_MIXER_OUTPUT_PREVIEW 0
#define ZONEKEY_VIDEO_MIXER_OUTPUT_H264 1

// һ�� channel �����ԣ�ͨ�� ZONEKEY_METHOD_VIDEO_MIXER_SET_CHANNEL_DESC ����
typedef struct ZonekeyVideoMixerChannelDesc
{
	int id;		// ���Σ�����ָ����Ӧ�ı�� [0..ZONEKEY_VIDEO_MIXER_MAX_CHANNELS)
	int x, y;	// Ŀ�꣬���������е����Ͻ�����
	int width, height;	// Ŀ���С
	int alpha;	// ͸���ȣ�[0..255]
} ZonekeyVideoMixerChannelDesc;

enum ZonekeyVideoMixerZOrderOper
{
	ZONEKEY_VIDEO_MIXER_ZORDER_UP,
	ZONEKEY_VIDEO_MIXER_ZORDER_DOWN,
	ZONEKEY_VIDEO_MIXER_ZORDER_TOP,
	ZONEKEY_VIDEO_MIXER_ZORDER_BOTTOM,
};

// ָ�� id ��ͨ���� z-order
typedef struct ZonekeyVideoMixerZOrder
{
	int id;	// channel ���
	ZonekeyVideoMixerZOrderOper order_oper;	// ִ�е� order ����
} ZonekeyVideoMixerZOrder;

// ���� channel �� zorder ˳��
typedef struct ZonekeyVideoMixerZorderArray
{
	int orders[ZONEKEY_VIDEO_MIXER_MAX_CHANNELS+1];	// ÿ��Ϊ chid����� -1 ˵������û���ˣ����Ǻ���ĸ���ǰ��� :)
} ZonekeyVideoMixerZorderArray;

// ����/ѹ������
typedef struct ZonekeyVideoMixerEncoderSetting
{
	int width, height;		// ������С��ֱ�Ӷ�Ӧ��ѹ������� h264 �Ĵ�С
	double fps;				// ֡��
	int kbps;				// ����
	int gop;				// �൱�ڹؼ�֡�������� fps = 25, gop = 50����ÿ�������һ���ؼ�֡
	// .... ���� x264 �Ĳ�������������������
} ZonekeyVideoMixerEncoderSetting;

// ����״̬��Ϣ
typedef struct ZonekeyVideoMixerStats
{
	int64_t sent_bytes;		// ���͵��ֽ���
	int64_t sent_frames;	// ���͵�֡��

	double avg_fps;		// ƽ��֡��
	double avg_kbps;	// ƽ������

	double last_fps;	// ���� 5��ƽ��֡��
	double last_kbps;	// ���� 5����ƽ������
	double last_qp;		// ���� 5�� x264 ѹ��ƽ�� qp��һ����˵�����ֵά���� 25 ���ڣ�ͼ��Ч�������ܹ����ܵģ�������� 30��˵����������̫���ˣ�

	int last_delta;		// ���µ� delta ֵ������ֵԽСԽ�� :)

	int active_input;	// ��Ծ����������Ŀ

	double time;	// �������˵�ʱ�䣬����
	
} ZonekeyVideoMixerStats;

#define ZONEKEY_ID_VIDEO_MIXER (MSFilterInterfaceBegin+12)

// method id
// ���벢��ռ��һ������ͨ��������ͨ����ţ���Ӧ id
#define ZONEKEY_METHOD_VIDEO_MIXER_GET_CHANNEL				MS_FILTER_METHOD(ZONEKEY_ID_VIDEO_MIXER, 0, int)

// �ͷ�ռ�õ�ͨ��
#define ZONEKEY_METHOD_VIDEO_MIXER_FREE_CHANNEL				MS_FILTER_METHOD(ZONEKEY_ID_VIDEO_MIXER, 1, int)

// ����ͨ������
#define ZONEKEY_METHOD_VIDEO_MIXER_SET_CHANNEL_DESC			MS_FILTER_METHOD(ZONEKEY_ID_VIDEO_MIXER, 2, ZonekeyVideoMixerChannelDesc)
#define ZONEKEY_METHOD_VIDEO_MIXER_GET_CHANNEL_DESC			MS_FILTER_METHOD(ZONEKEY_ID_VIDEO_MIXER, 3, ZonekeyVideoMixerChannelDesc)

// ���û��ģʽ������ zorder
#define ZONEKEY_METHOD_VIDEO_MIXER_SET_ZORDER				MS_FILTER_METHOD(ZONEKEY_ID_VIDEO_MIXER, 4, ZonekeyVideoMixerZOrder)

// ����/��ȡ���ģʽ�������������
#define ZONEKEY_METHOD_VIDEO_MIXER_SET_ENCODER_SETTING		MS_FILTER_METHOD(ZONEKEY_ID_VIDEO_MIXER, 5, ZonekeyVideoMixerEncoderSetting)
#define ZONEKEY_METHOD_VIDEO_MIXER_GET_ENCODER_SETTING		MS_FILTER_METHOD(ZONEKEY_ID_VIDEO_MIXER, 6, ZonekeyVideoMixerEncoderSetting)

// ��ȡ����״̬��Ϣ
#define ZONEKEY_METHOD_VIDEO_MIXER_GET_STATS				MS_FILTER_METHOD(ZONEKEY_ID_VIDEO_MIXER, 7, ZonekeyVideoMixerStats)

// ���ص�ǰ zorder ˳��
#define ZONEKEY_METHOD_VIDEO_MIXER_GET_ZOEDER				MS_FILTER_METHOD(ZONEKEY_ID_VIDEO_MIXER, 8, ZonekeyVideoMixerZorderArray)

// �Ƿ����ã���� 0���򲻽����κ�ѹ����ֱ�ӽ���������
#define ZONEKEY_METHOD_VIDEO_MIXER_ENABLE					MS_FILTER_METHOD(ZONEKEY_ID_VIDEO_MIXER, 9, int)

MS2_PUBLIC void zonekey_video_mixer_register();
MS2_PUBLIC void zonekey_video_mixer_set_log_handler(void (*func)(const char *, ...));

#ifdef __cplusplus
}
#endif // c++

#endif /* zk.video.mixer.h */
