#pragma once

#include <zonekey/zqpsource.h>

class Zqpkt
{
	double stamp_;
	VARIANT data_;
	bool key_;

public:
	Zqpkt(void);
	~Zqpkt(void);

	void set_pkt(zq_pkt *pkt);

	VARIANT get_data() const { return data_; }	// ����ʹ�� VARIANT ��װ�� array
	DOUBLE get_stamp() const { return stamp_; }	// ���� �� ��λ��ʱ���
	bool is_key_frame() const { return key_; }
};
