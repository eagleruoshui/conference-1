// ZonekeyMCU_MOD.h : CZonekeyMCU_MOD ������

#pragma once
#include "resource.h"       // ������

#include "zkmcu_mod_i.h"
#include "_IZonekeyMCU_MODEvents_CP.h"
#include "NormalUser.h"
#include "SinkBase.h"
#include "flv_writer.h"
#include <zonekey/rtmp_inte.h>

#if defined(_WIN32_WCE) && !defined(_CE_DCOM) && !defined(_CE_ALLOW_SINGLE_THREADED_OBJECTS_IN_MTA)
#error "Windows CE ƽ̨(�粻�ṩ��ȫ DCOM ֧�ֵ� Windows Mobile ƽ̨)���޷���ȷ֧�ֵ��߳� COM ���󡣶��� _CE_ALLOW_SINGLE_THREADED_OBJECTS_IN_MTA ��ǿ�� ATL ֧�ִ������߳� COM ����ʵ�ֲ�����ʹ���䵥�߳� COM ����ʵ�֡�rgs �ļ��е��߳�ģ���ѱ�����Ϊ��Free����ԭ���Ǹ�ģ���Ƿ� DCOM Windows CE ƽ̨֧�ֵ�Ψһ�߳�ģ�͡�"
#endif

using namespace ATL;

/// ��Ӧ CBError �� code
enum ErrorCode
{
	EC_CREATE_SINK = -100,		// �޷������� mcu �ĵ㲥
};

// CZonekeyMCU_MOD

class ATL_NO_VTABLE CZonekeyMCU_MOD :
	public CComObjectRootEx<CComSingleThreadModel>,
	public CComCoClass<CZonekeyMCU_MOD, &CLSID_ZonekeyMCU_MOD>,
	public IConnectionPointContainerImpl<CZonekeyMCU_MOD>,
	public CProxy_IZonekeyMCU_MODEvents<CZonekeyMCU_MOD>,
	public IDispatchImpl<IZonekeyMCU_MOD, &IID_IZonekeyMCU_MOD, &LIBID_zkmcu_modLib, /*wMajor =*/ 1, /*wMinor =*/ 0>,
	public IDispatchImpl<_IZonekeyMCU_MODEvents, &__uuidof(_IZonekeyMCU_MODEvents), &LIBID_zkmcu_modLib, /* wMajor = */ 1, /* wMinor = */ 0>
{
	_bstr_t mcu_jid_;
	LONG cid_, payload_, sid_;
	HANDLE th_;
	bool quit_;
	flv_writer_t *flv_;
	void *rtmp_;

	static unsigned __stdcall proc_run(void *arg);
	static void cb_data(void *q, double stamp, void *data, int len, bool key)
	{
		((CZonekeyMCU_MOD*)q)->cb_data(stamp, data, len, key);
	}
	void cb_data(double stamp, void *data, int len, bool key);
	unsigned proc_run();
	/// ���봴��һ�� sink������ɹ������� sinkid��ʧ���򷵻� -1����ʱһ�������糬ʱ��!!
	int create_sink(NormalUser *user, int cid, int sid, int payload, std::string &mcu_ip, int &mcu_rtp_port, int &mcu_rtcp_port);
	void destroy_sink(NormalUser *user, int sink_id, int cid);

public:
	CZonekeyMCU_MOD()
	{
		th_ = 0;
		flv_ = 0;
		rtmp_ = 0;
	}

	DECLARE_REGISTRY_RESOURCEID(IDR_ZONEKEYMCU_MOD)

	BEGIN_COM_MAP(CZonekeyMCU_MOD)
		COM_INTERFACE_ENTRY(IZonekeyMCU_MOD)
		COM_INTERFACE_ENTRY2(IDispatch, _IZonekeyMCU_MODEvents)
		COM_INTERFACE_ENTRY(IConnectionPointContainer)
		COM_INTERFACE_ENTRY(_IZonekeyMCU_MODEvents)
	END_COM_MAP()

	BEGIN_CONNECTION_POINT_MAP(CZonekeyMCU_MOD)
		CONNECTION_POINT_ENTRY(__uuidof(_IZonekeyMCU_MODEvents))
	END_CONNECTION_POINT_MAP()


	DECLARE_PROTECT_FINAL_CONSTRUCT()

	HRESULT FinalConstruct()
	{
		return S_OK;
	}

	void FinalRelease()
	{
	}

public:
	STDMETHOD(get_MCUJid)(BSTR* pVal);
	STDMETHOD(put_MCUJid)(BSTR newVal);
	STDMETHOD(Start)(LONG cid, LONG payload, LONG sid);
	STDMETHOD(Stop)(void);

	// _IZonekeyMCU_MODEvents Methods
public:
	STDMETHOD_(void, CBMediaFrameGot)(LONG payload, DOUBLE stamp, VARIANT data, BOOL key)
	{
		// �ڴ˴���Ӻ���ʵ�֡�
		if (flv_ || rtmp_) {
		}
		else {
			Fire_CBMediaFrameGot(payload, stamp, data, key);
		}

	}
	STDMETHOD_(void, CBError)(LONG code, BSTR some_info)
	{
		// �ڴ˴���Ӻ���ʵ�֡�
		if (flv_ || rtmp_) {
		}
		else {
			Fire_CBError(code, some_info);
		}
	}
	STDMETHOD(put_RecordFilename)(BSTR newVal);
	STDMETHOD(put_RtmpURL)(BSTR newVal);
};

OBJECT_ENTRY_AUTO(__uuidof(ZonekeyMCU_MOD), CZonekeyMCU_MOD)
