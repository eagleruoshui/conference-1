// PackageForAudio.h : CPackageForAudio ������

#pragma once
#include "resource.h"       // ������


#include <zonekey/zqpsource.h>
#include "zkmcu_mod_i.h"
#include "Zqpkt.h"


#if defined(_WIN32_WCE) && !defined(_CE_DCOM) && !defined(_CE_ALLOW_SINGLE_THREADED_OBJECTS_IN_MTA)
#error "Windows CE ƽ̨(�粻�ṩ��ȫ DCOM ֧�ֵ� Windows Mobile ƽ̨)���޷���ȷ֧�ֵ��߳� COM ���󡣶��� _CE_ALLOW_SINGLE_THREADED_OBJECTS_IN_MTA ��ǿ�� ATL ֧�ִ������߳� COM ����ʵ�ֲ�����ʹ���䵥�߳� COM ����ʵ�֡�rgs �ļ��е��߳�ģ���ѱ�����Ϊ��Free����ԭ���Ǹ�ģ���Ƿ� DCOM Windows CE ƽ̨֧�ֵ�Ψһ�߳�ģ�͡�"
#endif

using namespace ATL;


// CPackageForAudio

class ATL_NO_VTABLE CPackageForAudio :
	public CComObjectRootEx<CComSingleThreadModel>,
	public CComCoClass<CPackageForAudio, &CLSID_PackageForAudio>,
	public IDispatchImpl<IPackageForAudio, &IID_IPackageForAudio, &LIBID_zkmcu_modLib, /*wMajor =*/ 1, /*wMinor =*/ 0>,
	public Zqpkt
{
public:
	CPackageForAudio()
	{
	}

DECLARE_REGISTRY_RESOURCEID(IDR_PACKAGEFORAUDIO)


BEGIN_COM_MAP(CPackageForAudio)
	COM_INTERFACE_ENTRY(IPackageForAudio)
	COM_INTERFACE_ENTRY(IDispatch)
END_COM_MAP()


	DECLARE_PROTECT_FINAL_CONSTRUCT()

	HRESULT FinalConstruct()
	{
		return S_OK;
	}

	void FinalRelease()
	{
	}

public:

	STDMETHOD(get_Data)(VARIANT* pVal);
	STDMETHOD(get_stamp)(DOUBLE* pVal);
	STDMETHOD(DataType)(LONG* type);
};

OBJECT_ENTRY_AUTO(__uuidof(PackageForAudio), CPackageForAudio)
