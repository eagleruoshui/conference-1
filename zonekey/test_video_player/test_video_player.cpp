// test_video_player.cpp : ����Ӧ�ó������ڵ㡣
//

#include "stdafx.h"
#include "test_video_player.h"
extern "C" {
#	include <libswscale/swscale.h>
}
#include <mediastreamer2/mediastream.h>
#include <mediastreamer2/zk.yuv_sink.h>
#include <mediastreamer2/msrtp.h>
#include <zonekey/video-inf.h>
#include <zonekey/xmpp_uac.h>

#define MAX_LOADSTRING 100
#define MCU_JID "mse_s_000000000000_mcu_1@qddevsrv.zonekey"

// ȫ�ֱ���:
HINSTANCE hInst;								// ��ǰʵ��
TCHAR szTitle[MAX_LOADSTRING];					// �������ı�
TCHAR szWindowClass[MAX_LOADSTRING];			// ����������
HWND _hwnd = NULL;

// �˴���ģ���а����ĺ�����ǰ������:
ATOM				MyRegisterClass(HINSTANCE hInstance);
BOOL				InitInstance(HINSTANCE, int);
LRESULT CALLBACK	WndProc(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK	About(HWND, UINT, WPARAM, LPARAM);

static void my_init();
static uac_token_t *_uac = 0;
int APIENTRY _tWinMain(HINSTANCE hInstance,
                     HINSTANCE hPrevInstance,
                     LPTSTR    lpCmdLine,
                     int       nCmdShow)
{
	UNREFERENCED_PARAMETER(hPrevInstance);
	UNREFERENCED_PARAMETER(lpCmdLine);

	zk_xmpp_uac_init();
	cb_xmpp_uac cbs = { 0, 0, 0, 0, 0 };

	_uac = zk_xmpp_uac_log_in("normaluser@qddevsrv.zonekey", "ddkk1212", &cbs, 0);

 	// TODO: �ڴ˷��ô��롣
	MSG msg;
	HACCEL hAccelTable;

	// ��ʼ��ȫ���ַ���
	LoadString(hInstance, IDS_APP_TITLE, szTitle, MAX_LOADSTRING);
	LoadString(hInstance, IDC_TEST_VIDEO_PLAYER, szWindowClass, MAX_LOADSTRING);
	MyRegisterClass(hInstance);

	// ִ��Ӧ�ó����ʼ��:
	if (!InitInstance (hInstance, nCmdShow))
	{
		return FALSE;
	}

	hAccelTable = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDC_TEST_VIDEO_PLAYER));

	my_init();

	// ����Ϣѭ��:
	while (GetMessage(&msg, NULL, 0, 0))
	{
		if (!TranslateAccelerator(msg.hwnd, hAccelTable, &msg))
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
	}

	return (int) msg.wParam;
}

//
//  ����: MyRegisterClass()
//
//  Ŀ��: ע�ᴰ���ࡣ
//
//  ע��:
//
//    ����ϣ��
//    �˴�������ӵ� Windows 95 �еġ�RegisterClassEx��
//    ����֮ǰ�� Win32 ϵͳ����ʱ������Ҫ�˺��������÷������ô˺���ʮ����Ҫ��
//    ����Ӧ�ó���Ϳ��Ի�ù�����
//    ����ʽ��ȷ�ġ�Сͼ�ꡣ
//
ATOM MyRegisterClass(HINSTANCE hInstance)
{
	WNDCLASSEX wcex;

	wcex.cbSize = sizeof(WNDCLASSEX);

	wcex.style			= CS_HREDRAW | CS_VREDRAW;
	wcex.lpfnWndProc	= WndProc;
	wcex.cbClsExtra		= 0;
	wcex.cbWndExtra		= 0;
	wcex.hInstance		= hInstance;
	wcex.hIcon			= LoadIcon(hInstance, MAKEINTRESOURCE(IDI_TEST_VIDEO_PLAYER));
	wcex.hCursor		= LoadCursor(NULL, IDC_ARROW);
	wcex.hbrBackground	= (HBRUSH)(COLOR_WINDOW+1);
	wcex.lpszMenuName	= MAKEINTRESOURCE(IDC_TEST_VIDEO_PLAYER);
	wcex.lpszClassName	= szWindowClass;
	wcex.hIconSm		= LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_SMALL));

	return RegisterClassEx(&wcex);
}

//
//   ����: InitInstance(HINSTANCE, int)
//
//   Ŀ��: ����ʵ�����������������
//
//   ע��:
//
//        �ڴ˺����У�������ȫ�ֱ����б���ʵ�������
//        ��������ʾ�����򴰿ڡ�
//
BOOL InitInstance(HINSTANCE hInstance, int nCmdShow)
{
   HWND hWnd;

   hInst = hInstance; // ��ʵ������洢��ȫ�ֱ�����

   hWnd = CreateWindow(szWindowClass, szTitle, WS_OVERLAPPEDWINDOW,
      CW_USEDEFAULT, 0, CW_USEDEFAULT, 0, NULL, NULL, hInstance, NULL);

   if (!hWnd)
   {
      return FALSE;
   }

   ShowWindow(hWnd, nCmdShow);
   UpdateWindow(hWnd);

   _hwnd = hWnd;

   return TRUE;
}

//
//  ����: WndProc(HWND, UINT, WPARAM, LPARAM)
//
//  Ŀ��: ���������ڵ���Ϣ��
//
//  WM_COMMAND	- ����Ӧ�ó���˵�
//  WM_PAINT	- ����������
//  WM_DESTROY	- �����˳���Ϣ������
//
//
LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	int wmId, wmEvent;
	PAINTSTRUCT ps;
	HDC hdc;

	switch (message)
	{
	case WM_COMMAND:
		wmId    = LOWORD(wParam);
		wmEvent = HIWORD(wParam);
		// �����˵�ѡ��:
		switch (wmId)
		{
		case IDM_ABOUT:
			DialogBox(hInst, MAKEINTRESOURCE(IDD_ABOUTBOX), hWnd, About);
			break;
		case IDM_EXIT:
			DestroyWindow(hWnd);
			break;
		default:
			return DefWindowProc(hWnd, message, wParam, lParam);
		}
		break;
	case WM_PAINT:
		hdc = BeginPaint(hWnd, &ps);
		// TODO: �ڴ���������ͼ����...
		EndPaint(hWnd, &ps);
		break;
	case WM_DESTROY:
		PostQuitMessage(0);
		break;
	default:
		return DefWindowProc(hWnd, message, wParam, lParam);
	}
	return 0;
}

// �����ڡ������Ϣ�������
INT_PTR CALLBACK About(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
	UNREFERENCED_PARAMETER(lParam);
	switch (message)
	{
	case WM_INITDIALOG:
		return (INT_PTR)TRUE;

	case WM_COMMAND:
		if (LOWORD(wParam) == IDOK || LOWORD(wParam) == IDCANCEL)
		{
			EndDialog(hDlg, LOWORD(wParam));
			return (INT_PTR)TRUE;
		}
		break;
	}
	return (INT_PTR)FALSE;
}


static void yuv_callback(void *opaque, int width, int height, unsigned char *data[4], int stride[4])
{
	static VideoCtx rv = 0;
	if (rv == 0) {
		//rv_open2(&rv, _hwnd, width, height, SWS_BICUBIC);
		rv_open(&rv, _hwnd, width, height);
	}

	if (rv) {
		rv_writepic(rv, PIX_FMT_YUV420P, data, stride);
	}
}

// �յ��ظ�
static void	cb_response(zk_xmpp_uac_msg_token *rsp_token, const char *result, const char *option)
{
	MessageBoxA(0, result, option, MB_OK);
}

static void my_init()
{
	ortp_init();
	ms_init();
	ortp_set_log_level_mask(ORTP_ERROR);

	// only support h264
	rtp_profile_set_payload(&av_profile, 100, &payload_type_h264);

	// ���� rtp recv --> decoder --> zonekey.yuv.sink

	// ���� zonekey.yuv.sink
	zonekey_yuv_sink_register();
	MSFilterDesc *desc = ms_filter_lookup_by_name("ZonekeyYUVSink");
	MSFilter *sink = ms_filter_new_from_desc(desc);

	ZonekeyYUVSinkCallbackParam mp;
	mp.ctx = 0;
	mp.push = yuv_callback;
	ms_filter_call_method(sink, ZONEKEY_METHOD_YUV_SINK_SET_CALLBACK_PARAM, &mp);	// ���ûص�����

	// rtp session
	RtpSession *rtpsess = rtp_session_new(RTP_SESSION_RECVONLY);	// 
//	rtp_session_set_blocking_mode(rtpsess, 0);
	rtp_session_set_rtp_socket_recv_buffer_size(rtpsess, 3*1024*1024);
	rtp_session_set_rtp_socket_send_buffer_size(rtpsess, 3*1024*1024);
	rtp_session_set_local_addr(rtpsess, "172.16.1.104", 50000, 50001);	// 
	//rtp_session_set_remote_addr_and_port(rtpsess, "0.0.0.0", -1, -1);
	rtp_session_set_profile(rtpsess, &av_profile);
	rtp_session_set_payload_type(rtpsess, 100);	// h264

	JBParameters jitter;
	jitter.max_packets = 2000;
	jitter.adaptive = 1;
	jitter.max_size = -1;
	jitter.min_size = jitter.nom_size = 0;	// ms

	rtp_session_set_jitter_buffer_params(rtpsess, &jitter);
	rtp_session_enable_adaptive_jitter_compensation(rtpsess, 0);
	rtp_session_enable_jitter_buffer(rtpsess, 0);

	// rtp recver
	MSFilter *rtp = ms_filter_new(MS_RTP_RECV_ID);
	ms_filter_call_method(rtp, MS_RTP_RECV_SET_SESSION, rtpsess);

	// decoder
	MSFilter *decoder = ms_filter_new(MS_H264_DEC_ID);


	// link
	ms_filter_link(rtp, 0, decoder, 0);
	ms_filter_link(decoder, 0, sink, 0);

	// mstick
	MSTicker *ticker = ms_ticker_new();
	ms_ticker_attach(ticker, rtp);
}
