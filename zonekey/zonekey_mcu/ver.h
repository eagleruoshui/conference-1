#pragma once

/**

	2013-8-20: v0.4.8.15 sunkw
		DirectorConference: ���� create_conference() ���� livingcast=[true|false] �����Ƿ����� video mixer


	2013-8-19: v0.4.8.13 sunkw
		info_conference() �ṩ conference ����ʱ�䣬�����������....

	2013-8-8: v0.4.8: sunkw
		�ṩ��ϸ�� rtp stats �� rtcp stats ��Ϣ

	2013-8-6: v0.4.6: sunkw:
		��չ info_conference ���������� JSON ��ʽ����ϸ���������ǿ�����ͳ����Ϣ��׼ȷ�� ....

	2013-7-3: v0.4.4: sunkw:
		�������� dc.get_all_videos ����ָ�� cid ��������Ƶ source/stream ��������
		������������������ video stream ��λ��

	2013-6-18: v0.4.1: sunkw:
		DirectorConference �� audio publisher ʹ�� iLBC ѹ�������

	2013-6-7: v0.4.x: sunkw��
		����֧�֣�addStreamWithPublisher() �� stream ����ϳɣ����� Stream->get_publisher_filter() �ܹ�����һ�� publisher ������֧�ֵ㲥

	2013-4-4: v0.3.4: sunkw:
		���� add_stream( payload type = 0 (PCM uLaw)��������֧������ gips �� client
 */

#define VERSION_STR ("v0.4.8.18: build at: " __TIMESTAMP__)
