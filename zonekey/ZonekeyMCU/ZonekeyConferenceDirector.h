#pragma once

#include <mediastreamer2/mediastream.h>
#include <mediastreamer2/msconference.h>
#include <mediastreamer2/zk.video.mixer.h>
#include <mediastreamer2/zk.publisher.h>
#include "zonekeyconference.h"
#include "ZonekeyStreamVideoMixer.h"

/** �����ͨ����

		���������� Tee filter��tee filter �ṩ 10 �� outputs����Ӧ�ģ�0--8 ���� stream �Ļش���9 �������� publisher����Ӧ������ sinks
 */
#define MAX_MIXER_CHANNELS 9

/** ��ϯģʽ��ʹ����Ƶ���� + ��Ƶ���
		
			���֧�� 9 ·��Ƶ��ϣ�9 ·��Ƶ���
 */
class ZonekeyConferenceDirector : public ZonekeyConference
{
public:
	ZonekeyConferenceDirector(void);
	~ZonekeyConferenceDirector(void);

protected:
	virtual ZonekeyStream *createStream(RtpSession *rs, KVS &params, int id);
	virtual void freeStream(ZonekeyStream *s);

	virtual ZonekeySink *createSink(RtpSession *rs, KVS &params, int id);
	virtual void freeSink(ZonekeySink *s);

	// ��Ҫʵ�ֲ������� ...
	virtual int set_params(const char *prop, KVS &params);
	virtual int get_params(const char *prop, KVS &results);

private:
	int insert_video_stream(ZonekeyStreamVideoMixer *s);
	int remove_video_stream(ZonekeyStreamVideoMixer *s);
	int add_video_sink_to_publisher(ZonekeySink *s);
	int del_video_sink_from_publisher(ZonekeySink *s);
	int setting_vencoder(int width, int height, int kbps, double fps, int gop);
	int setting_vstream_desc(int id, int x, int y, int width, int height, int alpha);
	int setting_vstream_zorder(int id, const char *mode);

private:
	MSAudioConference *audio_;		// ��Ƶ���

	// mixer ����� tee���� tee ���ӵ� 
	MSFilter *video_mixer_filter_, *video_tee_filter_;
	MSTicker *ticker_;		// ��������������Ƶ graph
	MSFilter *video_publisher_filter_, *audio_publisher_filter_;
};
