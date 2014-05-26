/** ����һ·���룬ÿ¼�����Ӧһ�� Member
 */

#pragma once

#include <mediastreamer2/mediastream.h>
#include <cc++/thread.h>
extern "C" {
#	include <libswscale/swscale.h>
#	include <libavcodec/avcodec.h>
}

struct YUVPicture
{
	int x, y, width, height, alpha;
	AVPicture pic;
};

class InputSource
{
	int x_, y_, width_, height_, alpha_; // ��������������
	int want_x_, want_y_, want_width_, want_height_, want_alpha_;	// set_param() ���ã��� save_pic() ������

	ost::Mutex cs_;
	SwsContext *sws_;	// ������
	AVPicture pic_;	// ����Ŀ��
	int last_width_, last_height_;	// ��Ӧ pic_ ��С
	int last_width_valid_, last_height_valid_; // ��Ӧ���һ�� save_pic() �����Ĵ�С��һ��������� last_width_, last_height_ ��ͬ
	bool idle_, valid_;  // �Ƿ����ã��Ƿ��Ѿ� save_pic()

public:
	InputSource(void);
	~InputSource(void);

	bool idle();	// �����Ƿ����
	void employ();	// ռ�ã�����Ϊ�ǿ���
	void disemploy();	// �ͷ�

	int save_pic(MSPicture* pic);	// �յ��µ�ͼƬ�����죬������
	int set_param(int x, int y, int width, int height, int alpha); // 
	YUVPicture get_pic();	// �������µ�ͼƬ

	int x() const { return x_; }
	int y() const { return y_; }
	int width() const { return width_; }
	int height() const { return height_; }
	int alpha() const { return alpha_; }

	int want_x() const { return want_x_; }
	int want_y() const { return want_y_; }
	int want_width() const { return want_width_; }
	int want_height() const { return want_height_; }
	int want_alpha() const { return want_alpha_; }
};
