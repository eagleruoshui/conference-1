/** zonekey flv writer���ṩһ������ flv �ļ��Ľӿ�
 */

#pragma once

#ifdef __cplusplus
extern "C" {
#endif // c++

	typedef struct flv_writer_t flv_writer_t;

	/** ��һ�� flv writer 
	 */
	flv_writer_t *flv_writer_open(const char *filename);

	/** ���� */
	void flv_writer_close(flv_writer_t *flv);

	/** д�� h264 ��Ƶ����
	 */
	int flv_writer_save_h264(flv_writer_t *flv, const void *data, int len, 
		int key, double stamp,
		int width, int height);

	/** д�� aac ��Ƶ����
	 */
	int flv_writer_save_aac(flv_writer_t *flv, const void *data, int len, double stamp);

	/** д�� speex ��Ƶ����
	 */
	int flv_writer_save_speex(flv_writer_t *flv, const void *data, int len,
		double stamp,
		int channels, int rate, int bits);


#ifdef __cplusplus
}
#endif // c++
