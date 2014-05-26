#ifndef _zkrtsp_client__hh
#define _zkrtsp_client__hh

/** ���� live555���ṩһ�� rtsp client �ӿڣ�����ͨ�� rtsp ��ȡý����������������˵
	ʵ�� video: h264, audio: aac+speex ����

	����ʹ�����̣�
		zkrtspclient_t *c = zkrtspc_open(url);	// url: rtsp:// ....
		int mc = zkrtspc_media_cnt(c);		// ���ؼ�·media
		for (int i = 0; i < mc; i++) {
			zkrtspc_media_info (c, i, &info);	// ��ȡý����Ϣ
			zkrtspc_media_add_cb(c, i, media_callback, opaque); // ע��ý����ջص�����
		}

		zkrtspc_play(c);			// ��������

		....

		zkrtspc_stop(c);			// ֹͣ����ý�崫��

		zkrtspc_close(c);			// �ͷ� c ��Ӧ��Դ

 */

#ifdef __cplusplus
extern "C" {
#endif

typedef struct zkrtspclient_t zkrtspclient_t;

/* ý�����ԣ����� sdp �л�ȡ */
typedef struct zkrtspclient_media_info
{
	const char *media_type;		// "audio", "video", "....
	const char *codec_name;		// "h264", "mpeg4-generic....
	const char *fmtp_mode;		// AAC-hbr ...
	const char *fmtp_config;	// 1290...
	int clock_rate;
} zkrtspclient_media_info;

/** ý�����ݻص� 
		@param opaque: 
		@param info: ��������
		@param data: ý������ָ��
		@param len: ý�����ݳ���
		@param stamp: ʱ�������

		@warning: ��� len == 0��˵�� media ������
 */
typedef void (*PFN_zkrtspclient_media_cb)(void *opaque, const zkrtspclient_media_info *info, const void *data, int len, double stamp);

/** ��ȡһ����Ӧ rtsp url �� client ʵ����������ֱ�� DESCRIBE ���� */
zkrtspclient_t *zkrtspc_open(const char *url);
void zkrtspc_close (zkrtspclient_t *c);

/** ���� ���� stream ����Ŀ */
int zkrtspc_media_cnt(zkrtspclient_t *c);

/** ���� stream index ������ */
int zkrtspc_media_info(zkrtspclient_t *c, int index, zkrtspclient_media_info *info);

/** ע��ý��ص� */
int zkrtspc_media_set_cb(zkrtspclient_t *c, int index, PFN_zkrtspclient_media_cb proc, void *opaque);

/** ��������ʵ�� */
int zkrtspc_play(zkrtspclient_t *c);

#ifdef __cplusplus
}
#endif

#endif /** zkrtspclient.h */
