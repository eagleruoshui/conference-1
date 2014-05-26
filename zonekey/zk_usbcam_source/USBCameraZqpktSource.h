#pragma once

#include <cc++/thread.h>
#include <string>

/** ��Ӧһ�� usb camera + zqpkt source
	
 */
class USBCameraZqpktSource : private ost::Thread
{
	int cam_id_, tcp_port_, last_err_;
	ost::Event evt_open_, evt_close_;
	bool quit_;
	std::string url_;
	double fps_;
	int kbitrate_, width_, height_;

public:
	// ��Ҫ�ṩ usb camera �ı�ţ�zqsender �Ķ˿� ...
	USBCameraZqpktSource(int cam_id, int tcp_port);
	~USBCameraZqpktSource(void);

	// ���ñ������������������� fps, bitrate, widthxheight������ x264 ������ʹ�� x264_param_default_preset(&param, "veryfast", "zerolatency");
	void setEncoderParam(double fps, int kbitrate, int width, int height)
	{
		fps_ = fps;
		kbitrate_ = kbitrate;
		width_ = width;
		height_ = height;
	}
	
	// ��ȡ���ɵ� zqpkt url
	const char *getZqpktUrl() const { return url_.c_str(); }

	// �����ɼ������
	int Start();

	// ֹͣ�ɼ������
	void Stop();

private:
	void run();
};
