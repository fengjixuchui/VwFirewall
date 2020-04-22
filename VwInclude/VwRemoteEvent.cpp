// VwRemoteEvent.cpp: implementation of the CVwRemoteEvent class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "VwRemoteEvent.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CVwRemoteEvent::CVwRemoteEvent()
{
	m_bInitSucc = FALSE;
	memset( m_szVwEventHost, 0, sizeof( m_szVwEventHost ) );
	memset( m_szFilename, 0, sizeof( m_szFilename ) );
	memset( m_szModVersion, 0, sizeof( m_szModVersion ) );
	memset( m_szMac, 0, sizeof( m_szMac ) );
}

CVwRemoteEvent::~CVwRemoteEvent()
{
}

BOOL CVwRemoteEvent::Init( LPCTSTR lpcszModFilePath )
{
	/*
		lpcszModFilePath	- [in] ��ִ���ļ��Լ���·��
		RETURN			- TRUE / FALSE
	*/
	if ( NULL == lpcszModFilePath || ! PathFileExists( lpcszModFilePath ) )
	{
		return FALSE;
	}

	_sntprintf( m_szFilename, sizeof(m_szFilename)-sizeof(TCHAR), "%s", PathFindFileName( lpcszModFilePath ) );
	_tcslwr( m_szFilename );

	//	���� vwevent.xingworld.net
	memcpy( m_szVwEventHost, g_szVwEventHost, min( sizeof(g_szVwEventHost), sizeof(m_szVwEventHost) ) );			
	_vwfunc_xorenc( m_szVwEventHost );

	//	��ȡ�ļ��汾
	if ( _vwfunc_get_file_version( lpcszModFilePath, m_szModVersion, sizeof(m_szModVersion) ) )
	{
		m_bInitSucc = TRUE;

		//	��ȡ MAC ��ַ
		_vwfunc_get_mac_address( "*", m_szMac, sizeof(m_szMac) );
		
		//	��ȡӲ�����к�
		_vwfunc_get_hdisk_serialnumber( m_szHDiskSN, sizeof(m_szHDiskSN) );

		return TRUE;
	}

	return FALSE;
}


/**
 *	���� event ��Ϣ
 */
BOOL CVwRemoteEvent::SendEvent( LPCTSTR lpcszEvent )
{
	/*
		lpcszType	- [in] install/uninstall
		RETURN		- TRUE / FALSE
	*/
	if ( ! m_bInitSucc )
	{
		return FALSE;
	}
	if ( NULL == lpcszEvent || 0 == _tcslen( lpcszEvent ) )
	{
		return FALSE;
	}

	CVwHttp http;

	if ( ! http.HaveInternetConnection() )
	{
		return FALSE;
	}

	TCHAR szUrl[ MAX_PATH ]		= {0};
	TCHAR szResponse[ MAX_PATH ]	= {0};
	TCHAR szError[ MAX_PATH ]	= {0};

	//	��������
	_snprintf
	(
		szUrl,
		sizeof(szUrl)-sizeof(TCHAR),
		"http://%s/?file=%s&ver=%s&cid=%s-%s&event=%s",
		m_szVwEventHost,
		m_szFilename,
		m_szModVersion,
		m_szHDiskSN,
		m_szMac,
		lpcszEvent
	);
	return http.GetResponse( szUrl, szResponse, sizeof( szResponse ), szError, 30*1000 );
}