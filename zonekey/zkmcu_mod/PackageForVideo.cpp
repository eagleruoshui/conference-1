// PackageForVideo.cpp : CPackageForVideo ��ʵ��

#include "stdafx.h"
#include "PackageForVideo.h"


// CPackageForVideo



STDMETHODIMP CPackageForVideo::get_Data(VARIANT* pVal)
{
	*pVal = get_data();
	return S_OK;
}


STDMETHODIMP CPackageForVideo::get_Stamp(DOUBLE* pVal)
{
	*pVal = get_stamp();
	return S_OK;
}


STDMETHODIMP CPackageForVideo::get_Key(VARIANT_BOOL* pVal)
{
	// TODO: �ڴ����ʵ�ִ���
	*pVal = is_key_frame() ? -1 : 0;
	return S_OK;
}


STDMETHODIMP CPackageForVideo::DataType(LONG* t)
{
	// TODO: �ڴ����ʵ�ִ���
	*t = 1;
	return S_OK;
}
