/** �������൱��һ�� yuv ͼ��֧�ֱ�������С������
 */
#pragma once

#include <stdint.h>
extern "C" {
#	include <libavcodec/avcodec.h>
}

class Canvas
{
	int width_, height_;	// ������С
	AVPicture pic_;			// ���ڴ洢����

public:
	Canvas(int width, int height);
	virtual ~Canvas(void);

	int width() const { return width_; }
	int height() const { return height_; }

	void clear();	// ��ջ���

	// �� YUV ��ָ��λ��
	int draw_yuv(unsigned char *data[4], int stride[4],
		int x, int y, int width, int height, int alpha);

	AVPicture get_pic() const { return pic_; }	// ����� mixer thread �е��ã�һ�㲻��Ҫ����
};
