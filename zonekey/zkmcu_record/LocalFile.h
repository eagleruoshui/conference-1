#pragma once

/// ��Ӧһ��ֻд���ļ�
class LocalFile
{
	HANDLE file_;

public:
	// ����ȴ��������ʧ�ܣ��׳��쳣
	LocalFile(PTCHAR filename);	// ����Ŀ¼����
	~LocalFile(void);

	// д���ļ�
	int Write(const void * data, int len);
};
