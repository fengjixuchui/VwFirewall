//////////////////////////////////////////////////////////////////////
//
// Written by Michael Dunn (mdunn at inreach dot com).  Copyright and all
// that stuff.  Use however you like but give me credit where it's due.  I'll
// know if you don't. bwa ha ha ha ha!
// 
// Release history:
//   December 24, 1999: Version 1.0.  First release.
//
//////////////////////////////////////////////////////////////////////

// BindStatusCallback.cpp: implementation of the CBindStatusCallback class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "BindStatusCallback.h"
#include <assert.h>




CCallback::CCallback()
{
	m_pbAbort		= NULL;
	m_hWnd			= NULL;
	m_dwStart		= 0;
	m_dwTimeout		= 0;
	m_LastProgress		= 0;
	m_dwProgressChange	= GetTickCount();
}

HRESULT CCallback::OnProgress ( ULONG ulProgress,   ULONG ulProgressMax,
                                ULONG ulStatusCode, LPCWSTR wszStatusText )
{
	UNREFERENCED_PARAMETER(ulStatusCode);

	__try
	{
		if( NULL != m_hWnd && 
			( BINDSTATUS_DOWNLOADINGDATA == ulStatusCode || 
			BINDSTATUS_BEGINDOWNLOADDATA == ulStatusCode  ||
			BINDSTATUS_ENDDOWNLOADDATA == ulStatusCode ) )
		{
			PostMessage( m_hWnd, UM_HTTP_PREGRESS, ulProgress, ulProgressMax );
		}

		if ( NULL != m_pbAbort )	// ��ֹ����
		{
			if ( *m_pbAbort )
				return E_ABORT;
		}

		DWORD dwCurrent = GetTickCount();
		//����Ӧǿ�Ƴ�ʱ�������ޱ仯��ʱ��
		if ( m_LastProgress != ulProgress )
		{
			//�����б仯�������½��Ȳ�����
			m_LastProgress = ulProgress;		
			m_dwProgressChange = dwCurrent;
		}
		else
		{
			//�����ޱ仯����ʱ����
			if ( dwCurrent - m_dwProgressChange > DOWNLOAD_TIME_FORCEFAIL )
				return E_ABORT;
		}

		if ( 0 != m_dwTimeout )// ���������س�ʱ����(�ļ�δ������ϳ�ʱ)
		{
			if ( dwCurrent - m_dwStart > m_dwTimeout )
				return E_ABORT; //��ʱ
		}
	}
	__except( EXCEPTION_EXECUTE_HANDLER )
	{
		assert( false );
	}

	return S_OK;
	UNREFERENCED_PARAMETER( wszStatusText );
	UNREFERENCED_PARAMETER( ulProgressMax );
	UNREFERENCED_PARAMETER( ulProgress );
}


HRESULT CCallback::GetBindInfo( DWORD *grfBINDF, BINDINFO *pbindinfo )
{
	// ǿ�Ʋ�ʹ�� Cache
	*grfBINDF |= BINDF_NOWRITECACHE | BINDF_PRAGMA_NO_CACHE;
	return S_OK;
	UNREFERENCED_PARAMETER( pbindinfo );
}
