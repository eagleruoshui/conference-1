// PackageForAudio.cpp : CPackageForAudio ��ʵ��

#include "stdafx.h"
#include "PackageForAudio.h"


// CPackageForAudio

STDMETHODIMP CPackageForAudio::get_Data(VARIANT* pVal)
{
	// TODO: �ڴ����ʵ�ִ���
	*pVal = get_data();
	return S_OK;
}


STDMETHODIMP CPackageForAudio::get_stamp(DOUBLE* pVal)
{
	// TODO: �ڴ����ʵ�ִ���
	*pVal = Zqpkt::get_stamp();
	return S_OK;
}


STDMETHODIMP CPackageForAudio::DataType(LONG* type)
{
	// TODO: �ڴ����ʵ�ִ���
	*type = 2;
	return S_OK;
}
