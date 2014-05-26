// ZonekeyMCULivingcast.cpp : CZonekeyMCULivingcast ��ʵ��

#include "stdafx.h"
#include "ZonekeyMCULivingcast.h"
#include <assert.h>

// CZonekeyMCULivingcast

STDMETHODIMP CZonekeyMCULivingcast::Open(BSTR url)
{
	_bstr_t Url(url);

	audio_begin_ = -1.0;
	video_begin_ = -1.0;

	Init_Rtmp(&rtmp_);
	if (Open_Rtmp(rtmp_, Url) == 1)
		return S_OK;
	else
		return E_FAIL;
}


STDMETHODIMP CZonekeyMCULivingcast::Close(void)
{
	assert(rtmp_);
	Close_Rtmp(rtmp_);
	UnInit_Rtmp(rtmp_);
	rtmp_ = 0;

	return S_OK;
}

STDMETHODIMP CZonekeyMCULivingcast::SendH264(DOUBLE stamp, VARIANT data, BOOL key)
{
	double s = 0.0;

	// ��ʱ�� stamp Ϊ����
	if (video_begin_ < 0.0) {
		video_begin_ = stamp;
	}
	else {
		s = (stamp - video_begin_) * 1000.0;		// ת��Ϊ����
	}

	void *ptr;
	HRESULT hr = SafeArrayAccessData(data.parray, &ptr);
	if (hr == S_OK) {
		LONG lb, ub;
		hr = SafeArrayGetLBound(data.parray, 1, &lb);
		if (hr != S_OK) return hr;
		hr = SafeArrayGetUBound(data.parray, 1, &ub);
		if (hr != S_OK) return hr;

		SendVideoPacket_Rtmp(rtmp_, ptr, ub - lb + 1, key, s);

		SafeArrayUnaccessData(data.parray);
	}

	return S_OK;
}

STDMETHODIMP CZonekeyMCULivingcast::SendAAC(DOUBLE stamp, VARIANT data)
{
	double s = 0.0;

	// ��ʱ�� stamp Ϊ����
	if (audio_begin_ < 0.0) {
		audio_begin_ = stamp;
	}
	else {
		s = (stamp - audio_begin_) * 1000.0;		// ת��Ϊ����
	}

	void *ptr;
	HRESULT hr = SafeArrayAccessData(data.parray, &ptr);
	if (hr == S_OK) {
		LONG lb, ub;
		hr = SafeArrayGetLBound(data.parray, 1, &lb);
		if (hr != S_OK) return hr;
		hr = SafeArrayGetUBound(data.parray, 1, &ub);
		if (hr != S_OK) return hr;

		SendAudioPacket_Rtmp(rtmp_, ptr, ub - lb + 1, s);

		SafeArrayUnaccessData(data.parray);
	}

	return S_OK;
}
