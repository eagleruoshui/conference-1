#pragma once

#include <string>
#include <vector>
#include <map>

// �����Լ��� ip ��ַ
const char *util_get_myip();
const char *util_get_mymac();

const char *util_get_myip_real();	// ���ر���ip

typedef std::map<std::string, std::string> KVS;

// �������� x=0&y=0&width=400&height=300 ֮��Ĳ������ϣ�ע�⣬�����������ظ����������Ľ�����ǰ���
KVS util_parse_options(const char *options);
std::string util_encode_options(KVS &params);

// ����Ƿ����е� keys �� kvs �ж����ڣ�����в����ڵģ����� false
bool chk_params(const KVS &kvs, char info[1024], const char *key, ...);
const char *get_param_value(const KVS &kvs, const char *key);

// ����������ִ�е�����
double util_time_uptime();

// ���ص�ǰʱ��
double util_time_now();
