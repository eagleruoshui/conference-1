
// test_mcu2.cpp : ����Ӧ�ó��������Ϊ��
//

#include "stdafx.h"
#include "test_mcu2.h"
#include "test_mcu2Dlg.h"
#include "../cJSON/cJSON.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// Ctest_mcu2App

BEGIN_MESSAGE_MAP(Ctest_mcu2App, CWinApp)
	ON_COMMAND(ID_HELP, &CWinApp::OnHelp)
END_MESSAGE_MAP()


static void test_json()
{
	const char *str = "{\/*** abcd */{\"key\": 12}}";

	cJSON *item = cJSON_Parse(str);

}

// Ctest_mcu2App ����

Ctest_mcu2App::Ctest_mcu2App()
{
	// ֧����������������
	m_dwRestartManagerSupportFlags = AFX_RESTART_MANAGER_SUPPORT_RESTART;

	// TODO: �ڴ˴���ӹ�����룬
	// ��������Ҫ�ĳ�ʼ�������� InitInstance ��

	test_json();

}


// Ψһ��һ�� Ctest_mcu2App ����

Ctest_mcu2App theApp;


// Ctest_mcu2App ��ʼ��

BOOL Ctest_mcu2App::InitInstance()
{
	// ���һ�������� Windows XP �ϵ�Ӧ�ó����嵥ָ��Ҫ
	// ʹ�� ComCtl32.dll �汾 6 ����߰汾�����ÿ��ӻ���ʽ��
	//����Ҫ InitCommonControlsEx()�����򣬽��޷��������ڡ�
	INITCOMMONCONTROLSEX InitCtrls;
	InitCtrls.dwSize = sizeof(InitCtrls);
	// ��������Ϊ��������Ҫ��Ӧ�ó�����ʹ�õ�
	// �����ؼ��ࡣ
	InitCtrls.dwICC = ICC_WIN95_CLASSES;
	InitCommonControlsEx(&InitCtrls);
	AfxInitRichEdit2();

	CWinApp::InitInstance();
	CoInitialize(0);

	ortp_init();
	ms_init();

	rtp_profile_set_payload(&av_profile, 100, &payload_type_h264);
	rtp_profile_set_payload(&av_profile, 110, &payload_type_speex_wb);
	
	zonekey_yuv_sink_register();
	zonekey_h264_source_register();

	zk_xmpp_uac_init();

	// ���� shell ���������Է��Ի������
	// �κ� shell ����ͼ�ؼ��� shell �б���ͼ�ؼ���
	CShellManager *pShellManager = new CShellManager;

	// ��׼��ʼ��
	// ���δʹ����Щ���ܲ�ϣ����С
	// ���տ�ִ���ļ��Ĵ�С����Ӧ�Ƴ�����
	// ����Ҫ���ض���ʼ������
	// �������ڴ洢���õ�ע�����
	// TODO: Ӧ�ʵ��޸ĸ��ַ�����
	// �����޸�Ϊ��˾����֯��
	SetRegistryKey(_T("Ӧ�ó��������ɵı���Ӧ�ó���"));

	Ctest_mcu2Dlg dlg;
	m_pMainWnd = &dlg;
	INT_PTR nResponse = dlg.DoModal();
	if (nResponse == IDOK)
	{
		// TODO: �ڴ˷��ô����ʱ��
		//  ��ȷ�������رնԻ���Ĵ���
	}
	else if (nResponse == IDCANCEL)
	{
		// TODO: �ڴ˷��ô����ʱ��
		//  ��ȡ�������رնԻ���Ĵ���
	}

	// ɾ�����洴���� shell ��������
	if (pShellManager != NULL)
	{
		delete pShellManager;
	}

	// ���ڶԻ����ѹرգ����Խ����� FALSE �Ա��˳�Ӧ�ó���
	//  ����������Ӧ�ó������Ϣ�á�
	return FALSE;
}

