
// test_mcu2.h : PROJECT_NAME Ӧ�ó������ͷ�ļ�
//

#pragma once

#ifndef __AFXWIN_H__
	#error "�ڰ������ļ�֮ǰ������stdafx.h�������� PCH �ļ�"
#endif

#include "resource.h"		// ������


// Ctest_mcu2App:
// �йش����ʵ�֣������ test_mcu2.cpp
//

class Ctest_mcu2App : public CWinApp
{
public:
	Ctest_mcu2App();

// ��д
public:
	virtual BOOL InitInstance();

// ʵ��

	DECLARE_MESSAGE_MAP()
};

extern Ctest_mcu2App theApp;