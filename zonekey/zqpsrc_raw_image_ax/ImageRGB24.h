// ImageRGB24.h : CImageRGB24 ������

#pragma once
#include "resource.h"       // ������



#include "zqpsrc_raw_image_ax_i.h"



#if defined(_WIN32_WCE) && !defined(_CE_DCOM) && !defined(_CE_ALLOW_SINGLE_THREADED_OBJECTS_IN_MTA)
#error "Windows CE ƽ̨(�粻�ṩ��ȫ DCOM ֧�ֵ� Windows Mobile ƽ̨)���޷���ȷ֧�ֵ��߳� COM ���󡣶��� _CE_ALLOW_SINGLE_THREADED_OBJECTS_IN_MTA ��ǿ�� ATL ֧�ִ������߳� COM ����ʵ�ֲ�����ʹ���䵥�߳� COM ����ʵ�֡�rgs �ļ��е��߳�ģ���ѱ�����Ϊ��Free����ԭ���Ǹ�ģ���Ƿ� DCOM Windows CE ƽ̨֧�ֵ�Ψһ�߳�ģ�͡�"
#endif

using namespace ATL;


// CImageRGB24

class ATL_NO_VTABLE CImageRGB24 :
	public CComObjectRootEx<CComSingleThreadModel>,
	public CComCoClass<CImageRGB24, &CLSID_Zonekey_RawImage_RGB24>,
	public IDispatchImpl<IZonekey_RawImage_RGB24, &IID_IZonekey_RawImage_RGB24, &LIBID_zqpsrc_raw_image_axLib, /*wMajor =*/ 1, /*wMinor =*/ 0>
{
	AVPicture pic_;
	int width_, height_;

public:
	CImageRGB24()
	{
	}

DECLARE_REGISTRY_RESOURCEID(IDR_IMAGERGB24)


BEGIN_COM_MAP(CImageRGB24)
	COM_INTERFACE_ENTRY(IZonekey_RawImage_RGB24)
	COM_INTERFACE_ENTRY(IDispatch)
END_COM_MAP()



	DECLARE_PROTECT_FINAL_CONSTRUCT()

	HRESULT FinalConstruct()
	{
		return S_OK;
	}

	void FinalRelease()
	{
		avpicture_free(&pic_);
	}

public:

	AVPicture &prepare(PixelFormat pf, int width, int height)
	{
		width_ = width, height_ = height;
		avpicture_alloc(&pic_, pf, width, height);
		return pic_;
	}
	STDMETHOD(get_Width)(LONG* pVal);
	STDMETHOD(get_Height)(LONG* pVal);
	STDMETHOD(get_BytesPerLine)(LONG* pVal);
	STDMETHOD(get_Data)(VARIANT* pVal);
};

OBJECT_ENTRY_AUTO(__uuidof(Zonekey_RawImage_RGB24), CImageRGB24)
