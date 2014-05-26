// ZqpsrcReader.h : CZqpsrcReader ������

#pragma once
#include "resource.h"       // ������

#include "zkmcu_mod_i.h"

#include "PackageForAudio.h"
#include "PackageForVideo.h"
#include <zonekey/zqpsource.h>

#if defined(_WIN32_WCE) && !defined(_CE_DCOM) && !defined(_CE_ALLOW_SINGLE_THREADED_OBJECTS_IN_MTA)
#error "Windows CE ƽ̨(�粻�ṩ��ȫ DCOM ֧�ֵ� Windows Mobile ƽ̨)���޷���ȷ֧�ֵ��߳� COM ���󡣶��� _CE_ALLOW_SINGLE_THREADED_OBJECTS_IN_MTA ��ǿ�� ATL ֧�ִ������߳� COM ����ʵ�ֲ�����ʹ���䵥�߳� COM ����ʵ�֡�rgs �ļ��е��߳�ģ���ѱ�����Ϊ��Free����ԭ���Ǹ�ģ���Ƿ� DCOM Windows CE ƽ̨֧�ֵ�Ψһ�߳�ģ�͡�"
#endif

using namespace ATL;


// CZqpsrcReader

class ATL_NO_VTABLE CZqpsrcReader :
	public CComObjectRootEx<CComSingleThreadModel>,
	public CComCoClass<CZqpsrcReader, &CLSID_ZqpsrcReader>,
	public IDispatchImpl<IZqpsrcReader, &IID_IZqpsrcReader, &LIBID_zkmcu_modLib, /*wMajor =*/ 1, /*wMinor =*/ 0>
{
	void *src_;

public:
	CZqpsrcReader()
	{
		src_ = 0;
	}

DECLARE_REGISTRY_RESOURCEID(IDR_ZQPSRCREADER)


BEGIN_COM_MAP(CZqpsrcReader)
	COM_INTERFACE_ENTRY(IZqpsrcReader)
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

	STDMETHOD(Open)(BSTR url);
	STDMETHOD(Close)(void);
	STDMETHOD(GetNextPacket)(IDispatch** package);
};

OBJECT_ENTRY_AUTO(__uuidof(ZqpsrcReader), CZqpsrcReader)
