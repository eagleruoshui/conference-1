#pragma once

#include <string>
#include <vector>
#include <map>

// �����Լ��� ip ��ַ
const char *util_get_myip();

typedef std::map<std::string, std::string> KVS;

// �������� x=0&y=0&width=400&height=300 ֮��Ĳ������ϣ�ע�⣬�����������ظ����������Ľ�����ǰ���
KVS util_parse_options(const char *options);

// ����Ƿ����е� keys �� kvs �ж����ڣ�����в����ڵģ����� false
bool chk_params(const KVS &kvs, char info[1024], const char *key, ...);
