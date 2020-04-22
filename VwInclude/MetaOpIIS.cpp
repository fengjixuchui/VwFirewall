// MetaOpIIS.cpp: implementation of the CAdsiOpIIS class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "MetaOpIIS.h"





//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CMetaOpIIS::CMetaOpIIS()
{
	m_pIMeta	= NULL;
	m_pIAppAdmin	= NULL;

	m_wszBuf = new WCHAR[ METAOPIIS_BUFFER_SIZE ];
	ZeroMemory( m_wszBuf, METAOPIIS_BUFFER_SIZE*sizeof(WCHAR) );
}
CMetaOpIIS::CMetaOpIIS( STMETASITEINFO * pstSiteinfo )
{
//	memcpy( & m_stSiteInfo, pstSiteinfo, sizeof(STMETASITEINFO) );	
	m_pIMeta	= NULL;
	m_pIAppAdmin	= NULL;

	m_wszBuf = new WCHAR[ METAOPIIS_BUFFER_SIZE ];
	ZeroMemory( m_wszBuf, METAOPIIS_BUFFER_SIZE*sizeof(WCHAR) );
}
CMetaOpIIS & CMetaOpIIS::operator = ( STMETASITEINFO & stSiteinfo )
{
//	memcpy( & m_stSiteInfo, &stSiteinfo, sizeof(STMETASITEINFO) );	
	return *this;
}
CMetaOpIIS::~CMetaOpIIS(void)
{
	m_pIMeta	= NULL;
	m_pIAppAdmin	= NULL;

	if ( m_wszBuf )
	{
		delete [] m_wszBuf;
		m_wszBuf = NULL;
	}
}


/**
 *	@ Public
 *	��ʼ��COM����ȡ�� IID_IDSAdminBase �ӿ�
 */
BOOL CMetaOpIIS::initCom()
{
	HRESULT hRet = NULL;

	//	��ʼ����Ľ��
#ifndef __METAOPIIS_COINITIALIZE__
#define __METAOPIIS_COINITIALIZE__
	hRet = CoInitialize( NULL );
#endif

	if ( m_pIMeta == NULL )
	{
		hRet = CoCreateInstance( CLSID_MSAdminBase, NULL, CLSCTX_LOCAL_SERVER, IID_IMSAdminBase, (void **)&m_pIMeta );
		if ( FAILED(hRet) || m_pIMeta == NULL )
		{
			setLastErrorInfo( 0, "��ʼ��IMSAdminBase�ӿ�ʧ��" );
			return FALSE;
		}
	}

	if ( m_pIAppAdmin == NULL )
	{
		hRet = CoCreateInstance( CLSID_WamAdmin, NULL, CLSCTX_LOCAL_SERVER, IID_IWamAdmin2, (void **)&m_pIAppAdmin );
		if ( FAILED(hRet) || m_pIAppAdmin == NULL )
		{
			setLastErrorInfo( 0, "��ʼ��IWamAdmin2�ӿ�ʧ��" );
			return FALSE;
		}
	}
	return TRUE;
}

/**
 *	@ Public
 *	���� IID_IDSAdminBase �ӿ�
 */
VOID CMetaOpIIS::uninitCom()
{
	if ( m_pIMeta != NULL )
	{
		m_pIMeta.Release();
	}
	if ( m_pIAppAdmin != NULL )
	{
		m_pIAppAdmin.Release();
	}
}

/**
 *	@ Public
 *	�Ƿ��ʼ������
 */
BOOL CMetaOpIIS::isInitComReady()
{
	return ( m_pIMeta && m_pIAppAdmin );
}

/**
 *	@ Public
 *	������վ��
 */
BOOL CMetaOpIIS::createSite( STMETASITEINFO * pstSiteInfo )
{
	//
	//	dwKeyName	- [in] metabase ��վ��ļ���
	//	RETURN		- TRUE / FALSE
	//
	if ( NULL == pstSiteInfo )
	{
		return FALSE;
	}

	//	���ָ�� MSAdminBase��IWamAdmin2 �ӿڵ�����ָ��
	if ( ! isInitComReady() )
	{
		setLastErrorInfo( 0, "����վ��ʱ������ָ�� MSAdminBase��IWamAdmin2 �ӿڵ�����ָ��δ��ʼ��" );
		return FALSE;
	}

	//	��һ������/LM/W3SVC�£���ȡΨһվ�㽡ֵ
	DWORD dwKeyName = this->getNewSiteKey();
	if ( 0 == dwKeyName )
	{
		setLastErrorInfo( 0, "��ȡΨһվ�㽡ֵʧ��" );
		return FALSE;
	}
	_snprintf( pstSiteInfo->szKeyName, sizeof(pstSiteInfo->szKeyName)-sizeof(TCHAR), "%d", dwKeyName );
	//_snwprintf

	
	//	�ڶ�����ʹ�õ�һ���ҳ�����������һ������
	WCHAR wszKeyNameOrg[ METADATA_MAX_NAME_LEN ]	= {0};
 	WCHAR wszBuf[ METAOPIIS_BUFFER_SIZE ]		= {0};
	STMETAOPERRECORD rec;
	INT nSBindingsLen;

	wcscpy( rec.wszKeyPath, L"/LM/W3SVC" );
	MultiByteToWideChar( CP_ACP, 0, pstSiteInfo->szKeyName, strlen(pstSiteInfo->szKeyName)+1, rec.wszKeyName, METAOPIIS_KEYNAME_LEN );
	if ( ! mb_createKey( & rec ) )
	{
		setLastErrorInfo( 0, "�½�վ������ʧ��" );
		return FALSE;
	}
	wcscpy( wszKeyNameOrg, rec.wszKeyName );


	//	�����������������µĸ�������
	//	���� KEYTYPE	MD_KEY_TYPE
	_snwprintf( rec.wszKeyPath, sizeof(rec.wszKeyPath)-sizeof(WCHAR), L"/LM/W3SVC/%s", rec.wszKeyName );
	memset( wszBuf, 0, sizeof(wszBuf) );
	wcscpy( wszBuf, L"IIsWebServer" );
	if ( ! modifyRecordData( & rec, MD_KEY_TYPE, 0, IIS_MD_UT_SERVER, STRING_METADATA, wszBuf, wcslen(wszBuf)*2+2 ) )
	{
		setLastErrorInfo( 0, "���������µĸ�������ʧ��" );
		return FALSE;
	}

	//	1001	����	ServerComment
	if ( 0 == strlen(pstSiteInfo->szServerComment) )
	{
		setLastErrorInfo( 0, "szServerComment ������Ч" );
		return FALSE;
	}
	memset( wszBuf, 0, sizeof(wszBuf) );
	MultiByteToWideChar( CP_ACP, 0, pstSiteInfo->szServerComment, strlen(pstSiteInfo->szServerComment)+1, wszBuf, METAOPIIS_BUFFER_SIZE );
	if ( ! modifyRecordData( & rec, MD_SERVER_COMMENT, METADATA_INHERIT, IIS_MD_UT_SERVER, STRING_METADATA, wszBuf, wcslen(wszBuf)*2+2 ) )
	{
		setLastErrorInfo( ERROR_MODIFY_SCOMMENT, "���� ServerComment ʧ��" );
		return FALSE;		
	}

	//	1002	szAppPoolId
	if ( 0 == strlen(pstSiteInfo->szAppPoolId) )
	{
		setLastErrorInfo( 0, "szAppPoolId ������Ч" );
		return FALSE;
	}
	memset( wszBuf, 0, sizeof(wszBuf) );
	MultiByteToWideChar( CP_ACP, 0, pstSiteInfo->szAppPoolId, strlen(pstSiteInfo->szAppPoolId)+1, wszBuf, METAOPIIS_BUFFER_SIZE );
	if ( ! modifyRecordData( & rec, 9101, METADATA_INHERIT, IIS_MD_UT_SERVER, STRING_METADATA, wszBuf, wcslen(wszBuf)*2+2 ) )
	{
		setLastErrorInfo( 0, "���� szAppPoolId ʧ��" );
		return FALSE;
	}

	//	1003	���� ServerState
	memset( wszBuf, 0, sizeof( wszBuf ) );
	memcpy( wszBuf, &pstSiteInfo->dwServerState, sizeof(DWORD) );
	if ( ! modifyRecordData( & rec, MD_SERVER_STATE, METADATA_VOLATILE, IIS_MD_UT_SERVER, DWORD_METADATA, wszBuf, sizeof(DWORD) ) )
	{
		setLastErrorInfo( ERROR_MODIFY_SSTATE, "���� dwServerState ʧ��" );
	}

	//	1004	dwAllowKeepAlive
	memset( wszBuf, 0, sizeof(wszBuf) );
	memcpy( wszBuf, &pstSiteInfo->dwAllowKeepAlive, sizeof(DWORD) );
	if ( ! modifyRecordData( & rec, MD_ALLOW_KEEPALIVES, METADATA_INHERIT, IIS_MD_UT_FILE, DWORD_METADATA, wszBuf, sizeof(DWORD) ) )
	{
		setLastErrorInfo( 0, "���� dwAllowKeepAlive ʧ��" );
		return FALSE;
	}

	//	1005	dwConnectionTimeout
	memset( wszBuf, 0, sizeof(wszBuf) );
	memcpy( wszBuf, &pstSiteInfo->dwConnectionTimeout, sizeof(DWORD) );
	if ( ! modifyRecordData( & rec, MD_CONNECTION_TIMEOUT, METADATA_INHERIT, IIS_MD_UT_SERVER, DWORD_METADATA, wszBuf, sizeof(DWORD) ) )
	{
		setLastErrorInfo( 0, "���� dwConnectionTimeout ʧ��" );
		return FALSE;
	}

	//	1006 ���ð���Ϣ
	if ( 0 == strlen(pstSiteInfo->szBindings) )
	{
		setLastErrorInfo( 0, "szBindings ������Ч" );
		return FALSE;
	}
	memset( wszBuf, 0, sizeof( wszBuf ) );
	if ( isValidSBindings( pstSiteInfo->szBindings ) )
	{
		MultiByteToWideChar( CP_ACP, 0, pstSiteInfo->szBindings, strlen(pstSiteInfo->szBindings)+1, wszBuf, METAOPIIS_BUFFER_SIZE );
		nSBindingsLen = formatSBindingsToMetabase( wszBuf );
		if ( ! modifyRecordData( & rec, MD_SERVER_BINDINGS, 0, IIS_MD_UT_SERVER, MULTISZ_METADATA, wszBuf, (nSBindingsLen)*2+2 ) )
		{
			setLastErrorInfo( ERROR_MODIFY_SBINDING, "���ð���Ϣʧ��" );
			return FALSE;
		}
	}
	else
	{
		setLastErrorInfo( ERROR_INVALID_SB, "" );
	}

	//	1007	���� MaxConnection
	if ( 0 != pstSiteInfo->lnMaxConnection )
	{
		//memset( wszBuf, 0xB0, sizeof(wszBuf) );
		memset( wszBuf, 0, sizeof(wszBuf) );
		memcpy( wszBuf, &pstSiteInfo->lnMaxConnection, sizeof(DWORD) );
		if ( ! modifyRecordData( & rec, MD_MAX_CONNECTIONS, METADATA_INHERIT, IIS_MD_UT_SERVER, DWORD_METADATA, wszBuf, sizeof(DWORD) ) )
		{
			setLastErrorInfo( ERROR_MODIFY_MAXCONNECTION, "���� MaxConnection ʧ��" );
			return FALSE;
		}
	}

	//	1008	�趨����������
	if ( 0 != pstSiteInfo->lnMaxBindwidth )
	{
		//	������ -1 ��������������
		//LONG lnBindWidth	= 1024 * pstSiteInfo->lnMaxBindwidth;
		LONG lnBindWidth	= pstSiteInfo->lnMaxBindwidth;
		//memset( wszBuf, 0xB0, sizeof(wszBuf) );
		memset( wszBuf, 0, sizeof(wszBuf) );
		memcpy( wszBuf, &lnBindWidth, sizeof(DWORD) );
		if ( ! modifyRecordData( & rec, MD_MAX_BANDWIDTH, METADATA_INHERIT, IIS_MD_UT_SERVER, DWORD_METADATA, wszBuf, sizeof(DWORD) ) )
		{
			setLastErrorInfo( ERROR_MODIFY_MAXCONNECTION, "�趨����������ʧ��" );
			return FALSE;
		}
	}

	//	1009	�����Ƿ�������־
	//		������־��¼��ر� MD_LOG_TYPE
	if ( 0 != pstSiteInfo->lnLogType && 1 != pstSiteInfo->lnLogType )
	{
		pstSiteInfo->lnLogType = 0;
	}
	memset( wszBuf, 0, sizeof( wszBuf ) );
	memcpy( wszBuf, &pstSiteInfo->lnLogType, sizeof(DWORD) );
	if ( ! modifyRecordData( & rec, MD_LOG_TYPE, METADATA_INHERIT, IIS_MD_UT_FILE, DWORD_METADATA, wszBuf, sizeof(DWORD) ) )
	{
		setLastErrorInfo( ERROR_USE_LOGS, "�����Ƿ�������־ʧ��" );
		return FALSE;		
	}

	//	1010	lnLogFileLocaltimeRollover
	memset( wszBuf, 0, sizeof( wszBuf ) );
	memcpy( wszBuf, &pstSiteInfo->lnLogFileLocaltimeRollover, sizeof(DWORD) );
	if ( ! modifyRecordData( & rec, 4015, METADATA_INHERIT, IIS_MD_UT_SERVER, DWORD_METADATA, wszBuf, sizeof(DWORD) ) )
	{
		setLastErrorInfo( 0, "���� lnLogFileLocaltimeRollover ʧ��" );
		return FALSE;		
	}

	//	1011	lnLogFilePeriod
	memset( wszBuf, 0, sizeof( wszBuf ) );
	memcpy( wszBuf, &pstSiteInfo->lnLogFilePeriod, sizeof(DWORD) );
	if ( ! modifyRecordData( & rec, MD_LOGFILE_PERIOD, METADATA_INHERIT, IIS_MD_UT_SERVER, DWORD_METADATA, wszBuf, sizeof(DWORD) ) )
	{
		setLastErrorInfo( 0, "���� lnLogFilePeriod ʧ��" );
		return FALSE;		
	}

	//	1012	lnLogFileTruncateSize
	memset( wszBuf, 0, sizeof( wszBuf ) );
	memcpy( wszBuf, &pstSiteInfo->lnLogFileTruncateSize, sizeof(DWORD) );
	if ( ! modifyRecordData( & rec, MD_LOGFILE_TRUNCATE_SIZE, METADATA_INHERIT, IIS_MD_UT_SERVER, DWORD_METADATA, wszBuf, sizeof(DWORD) ) )
	{
		setLastErrorInfo( 0, "���� lnLogFileTruncateSize ʧ��" );
		return FALSE;		
	}

	//	1013	lnLogExtFileFlags
	memset( wszBuf, 0, sizeof( wszBuf ) );
	memcpy( wszBuf, &pstSiteInfo->lnLogExtFileFlags, sizeof(DWORD) );
	if ( ! modifyRecordData( & rec, MD_LOGEXT_FIELD_MASK, METADATA_INHERIT, IIS_MD_UT_SERVER, DWORD_METADATA, wszBuf, sizeof(DWORD) ) )
	{
		setLastErrorInfo( 0, "���� lnLogExtFileFlags ʧ��" );
		return FALSE;		
	}

	//	1014	szLogFileDirectory
	if ( strlen( pstSiteInfo->szLogFileDirectory ) && PathIsDirectory( pstSiteInfo->szLogFileDirectory ) )
	{
		memset( wszBuf, 0, sizeof(wszBuf) );
		MultiByteToWideChar( CP_ACP, 0, pstSiteInfo->szLogFileDirectory, strlen(pstSiteInfo->szLogFileDirectory)+1, wszBuf, METAOPIIS_BUFFER_SIZE );
		if ( ! modifyRecordData( & rec, MD_LOGFILE_DIRECTORY, METADATA_INHERIT, IIS_MD_UT_SERVER, EXPANDSZ_METADATA, wszBuf, (wcslen(wszBuf)+1)*2 ) )
		{
			setLastErrorInfo( 0, "���� szLogFileDirectory ʧ��" );
			return FALSE;		
		}
	}

	//	1015	szLogPluginClsid
	if ( strlen( pstSiteInfo->szLogPluginClsid ) )
	{
		memset( wszBuf, 0, sizeof(wszBuf) );
		MultiByteToWideChar( CP_ACP, 0, pstSiteInfo->szLogPluginClsid, strlen(pstSiteInfo->szLogPluginClsid)+1, wszBuf, METAOPIIS_BUFFER_SIZE );
		if ( ! modifyRecordData( & rec, MD_LOG_PLUGIN_ORDER, METADATA_INHERIT, IIS_MD_UT_SERVER, STRING_METADATA, wszBuf, (wcslen(wszBuf)+1)*2 ) )
		{
			setLastErrorInfo( 0, "���� szLogPluginClsid ʧ��" );
			return FALSE;		
		}
	}

	//	1016	����Ĭ���ĵ� szDefaultDoc
	if ( strlen( pstSiteInfo->szDefaultDoc ) )
	{
		memset( wszBuf, 0, sizeof(wszBuf) );
		MultiByteToWideChar( CP_ACP, 0, pstSiteInfo->szDefaultDoc, strlen(pstSiteInfo->szDefaultDoc)+1, wszBuf, METAOPIIS_BUFFER_SIZE );
		if ( ! modifyRecordData( & rec, MD_DEFAULT_LOAD_FILE, METADATA_INHERIT, IIS_MD_UT_FILE, STRING_METADATA, wszBuf, (wcslen(wszBuf)+1)*2 ) )
		{
			setLastErrorInfo( ERROR_MODIFY_DEFAULTDOC, "����Ĭ���ĵ�ʧ��" );
			return FALSE;		
		}
	}



	//
	//	���Ĳ����ڸ�վ�������½���һ��Root���������ֵΪ1������
	//
	if ( 0 != wcscmp( rec.wszKeyName, L"1" ) )
	{
		//	�����µ�ֵ�� wszKeyName
		wcscpy( rec.wszKeyName, L"/Root" );
		if ( ! mb_createKey( & rec ) )
		{
			setLastErrorInfo( 0, "�ڸ�վ�������½���һ�� Root ��ʧ��" );
			return FALSE;
		}
		//	��ԭ wszKeyName
		wcscpy( rec.wszKeyName, wszKeyNameOrg );
	}

	//	���岽����Root�������ø�������
	//	���� KEYTYPE / MD_KEY_TYPE
	_snwprintf( rec.wszKeyPath, sizeof(rec.wszKeyPath)-sizeof(WCHAR), L"/LM/W3SVC/%s/Root", rec.wszKeyName );
	memset( wszBuf, 0, sizeof(wszBuf) );
	wcscpy( wszBuf, L"IIsWebVirtualDir" );
	if ( ! modifyRecordData( & rec, MD_KEY_TYPE, 0, IIS_MD_UT_SERVER, STRING_METADATA, wszBuf, wcslen(wszBuf)*2+2 ) )
	{
		setLastErrorInfo( 0, "�� Root �������ø�������ʧ��" );
		return FALSE;		
	}

	//	1017	���� ServerMDir
	if ( 0 == strlen(pstSiteInfo->szServerMDir) || ( ! PathIsDirectory( pstSiteInfo->szServerMDir ) ) )
	{
		setLastErrorInfo( 0, "szServerMDir ������Ч" );
		return FALSE;
	}
	memset( wszBuf, 0, sizeof( wszBuf ) );
	MultiByteToWideChar( CP_ACP, 0, pstSiteInfo->szServerMDir, strlen(pstSiteInfo->szServerMDir)+1, wszBuf, METAOPIIS_BUFFER_SIZE );
	if ( ! modifyRecordData( & rec, MD_VR_PATH, METADATA_INHERIT, IIS_MD_UT_FILE, STRING_METADATA, wszBuf, wcslen(wszBuf)*2+2 ) )
	{
		setLastErrorInfo( ERROR_MODIFY_MAINDIR, "���� Path ʧ��" );
		return FALSE;		
	}

	//	1018	������Ŀ¼��������
	memset( wszBuf, 0, sizeof(wszBuf) );
	memcpy( wszBuf, & pstSiteInfo->dwMDirAccessFlag, sizeof(DWORD) );
	if ( ! modifyRecordData( & rec, MD_ACCESS_PERM, METADATA_INHERIT, IIS_MD_UT_FILE, DWORD_METADATA, wszBuf, sizeof(DWORD) ) )
	{
		setLastErrorInfo( ERROR_MODIFY_MDIRACCESS, "������Ŀ¼��������ʧ��" );
		return FALSE;		
	}

	//	1019	lnDirBrowseFlags
	memset( wszBuf, 0, sizeof(wszBuf) );
	memcpy( wszBuf, & pstSiteInfo->lnDirBrowseFlags, sizeof(DWORD) );
	if ( ! modifyRecordData( & rec, MD_DIRECTORY_BROWSING, METADATA_INHERIT, IIS_MD_UT_FILE, DWORD_METADATA, wszBuf, sizeof(DWORD) ) )
	{
		setLastErrorInfo( 0, "���� lnDirBrowseFlags ����ʧ��" );
		return FALSE;		
	}

	//	1020	dwDontLog
	memset( wszBuf, 0, sizeof(wszBuf) );
	memcpy( wszBuf, & pstSiteInfo->dwDontLog, sizeof(DWORD) );
	if ( ! modifyRecordData( & rec, MD_DONT_LOG, METADATA_INHERIT, IIS_MD_UT_FILE, DWORD_METADATA, wszBuf, sizeof(DWORD) ) )
	{
		setLastErrorInfo( 0, "���� dwDontLog ����ʧ��" );
		return FALSE;		
	}
	//	1021	dwContentIndexed
	memset( wszBuf, 0, sizeof(wszBuf) );
	memcpy( wszBuf, & pstSiteInfo->dwContentIndexed, sizeof(DWORD) );
	if ( ! modifyRecordData( & rec, MD_IS_CONTENT_INDEXED, METADATA_INHERIT, IIS_MD_UT_FILE, DWORD_METADATA, wszBuf, sizeof(DWORD) ) )
	{
		setLastErrorInfo( 0, "���� dwContentIndexed ����ʧ��" );
		return FALSE;		
	}

	//	1022	szHttpExpires
	if ( strlen( pstSiteInfo->szHttpExpires ) )
	{
		memset( wszBuf, 0, sizeof( wszBuf ) );
		MultiByteToWideChar( CP_ACP, 0, pstSiteInfo->szHttpExpires, strlen(pstSiteInfo->szHttpExpires)+1, wszBuf, METAOPIIS_BUFFER_SIZE );
		if ( ! modifyRecordData( & rec, MD_HTTP_EXPIRES, METADATA_INHERIT, IIS_MD_UT_FILE, STRING_METADATA, wszBuf, wcslen(wszBuf)*2+2 ) )
		{
			setLastErrorInfo( 0, "���� szHttpExpires ʧ��" );
			return FALSE;		
		}
	}

	//	1023	lnEnableDocFooter
	memset( wszBuf, 0, sizeof(wszBuf) );
	memcpy( wszBuf, & pstSiteInfo->lnEnableDocFooter, sizeof(DWORD) );
	if ( ! modifyRecordData( & rec, MD_FOOTER_ENABLED, METADATA_INHERIT, IIS_MD_UT_FILE, DWORD_METADATA, wszBuf, sizeof(DWORD) ) )
	{
		setLastErrorInfo( 0, "���� lnEnableDocFooter ����ʧ��" );
		return FALSE;		
	}

	//	1024	szDefaultDocFooter
	if ( strlen( pstSiteInfo->szDefaultDocFooter ) )
	{
		memset( wszBuf, 0, sizeof( wszBuf ) );
		MultiByteToWideChar( CP_ACP, 0, pstSiteInfo->szDefaultDocFooter, strlen(pstSiteInfo->szDefaultDocFooter)+1, wszBuf, METAOPIIS_BUFFER_SIZE );
		if ( ! modifyRecordData( & rec, MD_FOOTER_DOCUMENT, METADATA_INHERIT, IIS_MD_UT_FILE, STRING_METADATA, wszBuf, wcslen(wszBuf)*2+2 ) )
		{
			setLastErrorInfo( 0, "���� szDefaultDocFooter ʧ��" );
			return FALSE;		
		}
	}


	//
	//	����Ӧ�ó���
	//
	memset( wszBuf, 0, sizeof(wszBuf) );
	wcscpy( wszBuf, rec.wszKeyPath );
	_wcsupr( wszBuf );
	if ( ! mb_createApp( pstSiteInfo, wszBuf, 2 ) )
	{
		setLastErrorInfo( ERROR_CREATE_APP, "����Ӧ�ó���ʧ��" );
		return FALSE;
	}
	
	//	������ʹ�� COMMAND ����վ��
	DWORD dwState	= MD_SERVER_COMMAND_START;
	switch( pstSiteInfo->dwServerState )
	{
	case MD_SERVER_COMMAND_START:
		{
			dwState = MD_SERVER_COMMAND_START;
		}
		break;
	case MD_SERVER_COMMAND_STOP:
		{
			dwState = MD_SERVER_COMMAND_STOP;
		}
		break;
	case MD_SERVER_COMMAND_PAUSE:
		{
			dwState = MD_SERVER_COMMAND_PAUSE;
		}
		break;
	}
	rec.wszKeyPath[ wcslen(rec.wszKeyPath) - 5 ] = 0x0000;	//	"/root"
	memset( wszBuf, 0, sizeof(wszBuf) );
	memcpy( wszBuf, &dwState, sizeof(DWORD) );
	if ( ! modifyRecordData( & rec, MD_SERVER_COMMAND, 0, IIS_MD_UT_SERVER, DWORD_METADATA, wszBuf, sizeof(DWORD) ) )
	{
		setLastErrorInfo( ERROR_STATE_COMMAND, "����վ�����ʧ��" );
		return FALSE;
	}

	return TRUE;
}

/**
 *	@ Public
 *	ɾ��վ��
 */
BOOL CMetaOpIIS::deleteSite( STMETASITEINFO * pstSiteInfo )
{
	if ( NULL == pstSiteInfo )
	{
		return FALSE;
	}
	if ( ! isInitComReady() )
	{
		setLastErrorInfo( 0, "ɾ��վ��ʱ������ָ�� MSAdminBase��IWamAdmin2 �ӿڵ�����ָ��δ��ʼ��" );
		return FALSE;
	}

	//	ֱ��ɾ������
	STMETAOPERRECORD rec;
	wcscpy( rec.wszKeyPath, L"/LM/W3SVC" );
	MultiByteToWideChar( CP_ACP, 0, pstSiteInfo->szKeyName, strlen(pstSiteInfo->szKeyName)+1, rec.wszKeyName, MAX_PATH );

	if ( ! mb_deleteKey( & rec ) )
	{
		setLastErrorInfo( 0, "ֱ��ɾ������ ʧ��" );
		return FALSE;		
	}
	
	return TRUE;
}

/**
 *	@ Public
 *	�޸�վ����Ϣ
 */
BOOL CMetaOpIIS::modifySite( STMETASITEINFO * pstSiteInfo )
{
	if ( NULL == pstSiteInfo )
	{
		return FALSE;
	}
	if ( ! isInitComReady() )
	{
		setLastErrorInfo( 0, "�޸�վ����Ϣʱ������ָ�� MSAdminBase��IWamAdmin2 �ӿڵ�����ָ��δ��ʼ��" );
		return FALSE;
	}

	WCHAR wszTemp[METADATA_MAX_NAME_LEN]	= {0};
	WCHAR wszBuf[METAOPIIS_BUFFER_SIZE]	= {0};
	STMETAOPERRECORD rec;
	unsigned int i = 0;
	INT nSBindingsLen;

	//	���ÿ�����޸ķ����������
	if ( 0 == pstSiteInfo->dwOpFlag )
	{
		setLastErrorInfo( 0, "No Site Data need modify" );
		return FALSE;
	}

	//	���KEYNAME
	if ( 0 == strlen(pstSiteInfo->szKeyName) )
	{
		setLastErrorInfo( 0, "Can not localize a site key" );
		return FALSE;
	}

	MultiByteToWideChar(CP_ACP, 0, pstSiteInfo->szKeyName, strlen(pstSiteInfo->szKeyName)+1,
		wszTemp, METAOPIIS_KEYNAME_LEN);

	//	���SERVERCOMMENT
	if ( 0 != strlen(pstSiteInfo->szServerComment) )
	{
		//	�޸ĸ���������
		wcscpy(rec.wszKeyPath, L"/LM/W3SVC/");
		wcscat(rec.wszKeyPath, wszTemp);

		memset( wszBuf, 0, sizeof(wszBuf) );
		MultiByteToWideChar( CP_ACP, 0, pstSiteInfo->szServerComment, strlen(pstSiteInfo->szServerComment), wszBuf, METAOPIIS_BUFFER_SIZE );
		if ( ! modifyRecordData( & rec, MD_SERVER_COMMENT, 0, IIS_MD_UT_SERVER, STRING_METADATA, wszBuf, wcslen(wszBuf)*2+2 ) )
		{
			setLastErrorInfo( ERROR_MODIFY_SCOMMENT, "��� SERVERCOMMENT ʧ��" );
			return FALSE;
		}
	}

	//	���MaxConnection
	if ( 0 != pstSiteInfo->lnMaxConnection )
	{
		//	�޸ĸ���������
		wcscpy(rec.wszKeyPath, L"/LM/W3SVC/");
		wcscat(rec.wszKeyPath, wszTemp);

		memset( wszBuf, 0, sizeof(wszBuf) );
		memcpy(wszBuf, &pstSiteInfo->lnMaxConnection, sizeof(DWORD));
		if ( ! modifyRecordData( & rec, MD_MAX_CONNECTIONS, 0, IIS_MD_UT_SERVER, DWORD_METADATA, wszBuf, sizeof(DWORD) ) )
		{
			setLastErrorInfo( ERROR_MODIFY_MAXCONNECTION, "��� MaxConnection ʧ��" );
			return FALSE;
		}
	}
	
	//	�趨����������
	LONG lnBindWidth = 0;
	if ( -1 != pstSiteInfo->lnMaxBindwidth )
	{
		//	������-1��������������
		lnBindWidth	= 1024*pstSiteInfo->lnMaxBindwidth;
	}
	else
	{
		lnBindWidth = -1;
	}

	memset( wszBuf, 0, sizeof(wszBuf) );
	memcpy( wszBuf, &lnBindWidth, sizeof(DWORD) );
	if ( ! modifyRecordData( & rec, MD_MAX_BANDWIDTH, 0, IIS_MD_UT_SERVER, DWORD_METADATA, wszBuf, sizeof(DWORD) ) )
	{
		setLastErrorInfo( ERROR_MODIFY_MAXCONNECTION, "�趨���������� ʧ��" );
		return FALSE;
	}

	//	���ð���Ϣ
	memset( wszBuf, 0, sizeof(wszBuf) );
	if ( ! isValidSBindings( pstSiteInfo->szBindings ) )
	{
		setLastErrorInfo( ERROR_INVALID_SB, "" );
	}
	else
	{
		MultiByteToWideChar(CP_ACP, 0, pstSiteInfo->szBindings, strlen(pstSiteInfo->szBindings)+1, wszBuf, METAOPIIS_BUFFER_SIZE);
		nSBindingsLen = formatSBindingsToMetabase( wszBuf );
		if ( ! modifyRecordData( & rec, MD_SERVER_BINDINGS, 0, IIS_MD_UT_SERVER, MULTISZ_METADATA, wszBuf, (nSBindingsLen)*2+2 ) )
		{
			setLastErrorInfo( ERROR_MODIFY_SBINDING, "���ð���Ϣ ʧ��" );
			return FALSE;
		}
	}

	//	�����Ƿ�������־
	//	������־��¼��ر� MD_LOG_TYPE
	DWORD dwLogType	= 0;
	switch( pstSiteInfo->lnLogType )
	{
	case 0:				//	��������־
		dwLogType	= 0;
		break;
	case 1:				//	������־
		dwLogType	= 1;
		break;
	}

	memset( wszBuf, 0, sizeof(wszBuf) );
	memcpy(wszBuf, &dwLogType, sizeof(DWORD));
	if ( ! modifyRecordData( & rec, MD_LOG_TYPE, 0, IIS_MD_UT_FILE, DWORD_METADATA, wszBuf, sizeof(DWORD) ) )
	{
		setLastErrorInfo( ERROR_USE_LOGS, "�����Ƿ�������־ ʧ��" );
		return FALSE;		
	}

	//	��ȡServerStateվ��״̬
	DWORD dwServerState	= 4;
	memset( wszBuf, 0, sizeof(wszBuf) );
	if ( ! modifyRecordData( & rec, MD_SERVER_STATE, 0, IIS_MD_UT_SERVER, DWORD_METADATA, wszBuf, METAOPIIS_BUFFER_SIZE*sizeof(WCHAR) ) )
	{
		setLastErrorInfo( 0, "�޸�վ����Ϣʱ����ȡվ��״̬ʧ��" );
	}
	else
	{
		dwServerState =  *(DWORD*)wszBuf;
	}

	wcscat(rec.wszKeyPath, L"/Root");

	//	���AccessMDir.��Ŀ¼��������
	if ( 0 != pstSiteInfo->dwMDirAccessFlag )
	{
		//	�޸ĸ���������
		memset( wszBuf, 0, sizeof(wszBuf) );
		memcpy( wszBuf, &pstSiteInfo->dwMDirAccessFlag, sizeof(DWORD) );
		if ( ! modifyRecordData( & rec, MD_ACCESS_PERM, 0, IIS_MD_UT_FILE, DWORD_METADATA, wszBuf, sizeof(DWORD) ) )
		{
			setLastErrorInfo( ERROR_MODIFY_MDIRACCESS, "" );
			return FALSE;
		}
	}
	
	//	����Ĭ���ĵ� MD_DEFAULT_LOAD_FILE
	if ( 0 != strlen( pstSiteInfo->szDefaultDoc ) )
	{
		memset( wszBuf, 0, sizeof(wszBuf) );
		MultiByteToWideChar(CP_ACP, 0, pstSiteInfo->szDefaultDoc, strlen(pstSiteInfo->szDefaultDoc)+1, wszBuf, METAOPIIS_BUFFER_SIZE);
		if ( ! modifyRecordData( & rec, MD_DEFAULT_LOAD_FILE, 0, IIS_MD_UT_FILE, STRING_METADATA, wszBuf, (wcslen(wszBuf)+1)*2 ) )
		{
			setLastErrorInfo( ERROR_MODIFY_DEFAULTDOC, "����Ĭ���ĵ� ʧ��" );
			return FALSE;		
		}
	}


	//���վ��״̬Ϊ���ںͷ���״̬Ϊֹͣ���ض��򵽹���ҳ��
/*
	if ( 2 == pstSiteInfo->nStatus ) {
		string strUrl(g_ConfigInfo.szExpUrl);	
		strUrl += ",EXACT_DESTINATION";

		MultiByteToWideChar(CP_ACP, 0, strUrl.c_str(), strUrl.length()+1, wszBuf, METAOPIIS_BUFFER_SIZE);
		if ( ! modifyRecordData( MD_HTTP_REDIRECT, 0, IIS_MD_UT_FILE, STRING_METADATA, wszBuf, wcslen(wszBuf)*2+2 ) )
		{
			CErrorOperation::s_dwErrorCode = GetLastError();
			CErrorOperation::WriteErrorLog();
			return false;
		}
	}else {		
		//��������ض���,����������Ŀ¼
		if ( DelHttpRedirect(rec) ) {
			//����Ӧ�ó���
			CreateApp(rec.szKeyPath, 2);
		}
	}
*/

	//	���Ӧ�ó���״̬�����û��
	//APPSTATUS_NOTDEFINED		No application is defined at the specified path. 
	//APPSTATUS_RUNNING			The application is running. 
	//APPSTATUS_STOPPED			The application is not running. 
	DWORD __RPC_FAR dwAppState	= 0;
	HRESULT	hRet		= NULL;
	hRet = m_pIAppAdmin->AppGetStatus( rec.wszKeyPath, &dwAppState );

	//	���ServerState���Ƿ�ж��Ӧ�ó���
	switch( pstSiteInfo->dwServerState )
	{
	case MD_SERVER_COMMAND_START:
		{
			//	����
			if ( APPSTATUS_NOTDEFINED == dwAppState )
			{
				//	û�ж��������¿���
				hRet = mb_createApp( pstSiteInfo, rec.wszKeyPath, 2 );
			}
			else if ( APPSTATUS_STOPPED == dwAppState )
			{
				//	��ͣ��ָ�
				if ( MD_SERVER_COMMAND_PAUSE == dwServerState )
				{
					hRet = m_pIAppAdmin->AppRecover( rec.wszKeyPath, true );
				}
			}
		}
		break;
	case MD_SERVER_COMMAND_STOP:
		{
			//	ֹͣ
			if ( APPSTATUS_NOTDEFINED != dwAppState ) {
				//ж��
				hRet = m_pIAppAdmin->AppUnLoad( rec.wszKeyPath, false );
			}
		}
		break;
	case MD_SERVER_COMMAND_PAUSE:
		{
			//	��ͣ
			if ( APPSTATUS_RUNNING == dwAppState )
			{
				//	ж��
				hRet = m_pIAppAdmin->AppDeleteRecoverable( rec.wszKeyPath, true );
			}
		}
		break;
	}
	

	//	"/Root"
	rec.wszKeyPath[ wcslen(rec.wszKeyPath)-5 ] = 0x0000;

	//	ʹ��COMMAND����վ��
	DWORD dwState	= 0;
	switch( pstSiteInfo->dwServerState )
	{
	case MD_SERVER_COMMAND_START:
		{
			if ( 6 == dwServerState )
			{
				//	ԭ������ͣ�����ھͼ���
				dwState = MD_SERVER_COMMAND_CONTINUE;
			}
			else
			{
				dwState = MD_SERVER_COMMAND_START;
			}
		}
		break;
	case MD_SERVER_COMMAND_STOP:
		{
			dwState = MD_SERVER_COMMAND_STOP;
		}
		break;
	case MD_SERVER_COMMAND_PAUSE:
		{
			dwState = MD_SERVER_COMMAND_PAUSE;
		}
		break;
	}

	memset( wszBuf, 0, sizeof(wszBuf) );
	memcpy( wszBuf, &dwState, sizeof(DWORD) );
	if ( ! modifyRecordData( & rec, MD_SERVER_COMMAND, 0, IIS_MD_UT_SERVER, DWORD_METADATA, wszBuf, sizeof(DWORD) ) )
	{
		setLastErrorInfo( ERROR_STATE_COMMAND, "����վ�����ʧ��" );
		return FALSE;
	}

	return TRUE;
}

/**
 *	@ Public
 *	�� Metabase ���ݿ��л�ȡ����
 */
BOOL CMetaOpIIS::getAllDataFromMetabase( STMETAALLDATA * pstAllData )
{
	if ( NULL == pstAllData )
	{
		return FALSE;
	}

	BOOL bRet					= FALSE;
	HRESULT hRet					= NULL;
	METADATA_HANDLE hLocalMachine			= NULL;
	STMETAKEYNAME stKeyname;
	vector<STMETAKEYNAME> vcSiteKeyNames;
	vector<STMETAKEYNAME>::iterator itk;
	vector<STMETAKEYNAME> vcAppPoolKeyNames;
	vector<STMETAKEYNAME>::iterator itp;
	DWORD nIndex					= 0;
	STMETASITEINFO stSiteInfo;
	STMETAAPPPOOLINFO stAppPoolInfo;

	//	Enum Site Keyname
	hRet = m_pIMeta->OpenKey( METADATA_MASTER_ROOT_HANDLE, L"/LM", METADATA_PERMISSION_READ, METAOPIIS_MD_TIMEOUT, &hLocalMachine );
	if ( SUCCEEDED(hRet) )
	{
		bRet	= TRUE;

		//	ö��վ�� id
		nIndex	= 0;
		vcSiteKeyNames.clear();
		do
		{
			memset( & stKeyname, 0, sizeof(stKeyname) );
			hRet = m_pIMeta->EnumKeys( hLocalMachine, L"/W3SVC", stKeyname.wszKeyName, nIndex );
			if ( SUCCEEDED( hRet ) )
			{
				if ( wcslen( stKeyname.wszKeyName ) && (DWORD)_wtol( stKeyname.wszKeyName ) )
				{
					vcSiteKeyNames.push_back( stKeyname );
				}
			}
			nIndex++;
		}
		while ( SUCCEEDED(hRet) );

		//	ö�� AppPool ����
		nIndex	= 0;
		vcAppPoolKeyNames.clear();
		do
		{
			memset( & stKeyname, 0, sizeof(stKeyname) );
			hRet = m_pIMeta->EnumKeys( hLocalMachine, L"/W3SVC/AppPools", stKeyname.wszKeyName, nIndex );
			if ( SUCCEEDED( hRet ) )
			{
				if ( wcslen( stKeyname.wszKeyName ) )
				{
					vcAppPoolKeyNames.push_back( stKeyname );
				}
			}
			nIndex++;
		}
		while ( SUCCEEDED(hRet) );

		m_pIMeta->CloseKey( hLocalMachine );
	}



	//
	//	ѭ������վ�㣬��ȡ��Ϣ
	//
	for ( itk = vcSiteKeyNames.begin(); itk != vcSiteKeyNames.end(); itk ++ )
	{
		//	�򿪸�վ���ȡ��Ҫ������
		memset( & stSiteInfo, 0, sizeof(stSiteInfo) );
		if ( getSiteDataFromKey( (*itk).wszKeyName, & stSiteInfo ) )
		{
			//	����վ����Ϣ
			pstAllData->vcSite.push_back( stSiteInfo );
		}
	}

	//
	//	ѭ������ AppPool����ȡ��Ϣ
	//
	for ( itp = vcAppPoolKeyNames.begin(); itp != vcAppPoolKeyNames.end(); itp ++ )
	{
		//	�򿪸� AppPool ��ȡ��Ϣ
		memset( & stAppPoolInfo, 0, sizeof(stAppPoolInfo) );
		if ( getAppPoolDataFromKey( (*itp).wszKeyName, & stAppPoolInfo ) )
		{
			//	���� AppPool ��Ϣ
			pstAllData->vcAppPool.push_back( stAppPoolInfo );
		}
	}

	return bRet;
}

/**
 *	@ Public
 *	��ȡĳ�� AppPool ��Ϣ
 */
BOOL CMetaOpIIS::getAppPoolDataFromKey( LPCWSTR lpcwszKeyName, STMETAAPPPOOLINFO * pstAppPoolInfo )
{
	if ( NULL == lpcwszKeyName || 0 == wcslen(lpcwszKeyName) )
	{
		return FALSE;
	}
	if ( NULL == pstAppPoolInfo )
	{
		return FALSE;
	}

	HRESULT hRet					= NULL;
	METADATA_HANDLE hMDAppPool			= NULL;
	METADATA_RECORD MyRecord;
	DWORD dwBufLen					= METAOPIIS_BUFFER_SIZE*sizeof(WCHAR);
	DWORD dwReqBufLen				= 0;
	WCHAR wszKeyPath[ METADATA_MAX_NAME_LEN ]	= {0};
	WCHAR wszBuffer[ METAOPIIS_BUFFER_SIZE ]	= {0};
	LPWSTR lpwszTemp				= NULL;

	//	д�� KEYNAME
	WideCharToMultiByte( CP_ACP, 0, lpcwszKeyName, wcslen(lpcwszKeyName), pstAppPoolInfo->szKeyName, sizeof(pstAppPoolInfo->szKeyName), NULL, NULL );

	//	��·��
	_snwprintf( wszKeyPath, sizeof(wszKeyPath)-sizeof(WCHAR), L"/LM/W3SVC/AppPools/%s", lpcwszKeyName );

	try
	{
		hRet = m_pIMeta->OpenKey( METADATA_MASTER_ROOT_HANDLE, wszKeyPath, METADATA_PERMISSION_READ, METAOPIIS_MD_TIMEOUT, &hMDAppPool );
		if ( ! SUCCEEDED( hRet ) )
		{
			throw L"��KEYNAME��ʧ��";
		}

		//	2001	ȡ������ dwAppPoolState
		//		2������4ֹͣ
		memset( wszBuffer, 0, sizeof(wszBuffer) );
		hRet = getRecordData( hMDAppPool, & MyRecord, MD_APPPOOL_STATE, METADATA_VOLATILE, IIS_MD_UT_SERVER, DWORD_METADATA, wszBuffer, sizeof(DWORD), & dwReqBufLen );
		if ( ! SUCCEEDED( hRet ) && MD_ERROR_DATA_NOT_FOUND != hRet )
		{
			throw "��ȡ dwAppPoolState ״̬ʧ��";
		}
		else
		{
			pstAppPoolInfo->dwAppPoolState = *((DWORD*)wszBuffer);
		}

		//	2002	���� >> ���չ�������(����)
		//		dwPeriodicRestartTime
		memset( wszBuffer, 0, sizeof(wszBuffer) );
		hRet = getRecordData( hMDAppPool, & MyRecord, MD_APPPOOL_PERIODICRESTARTTIME, METADATA_INHERIT, IIS_MD_UT_SERVER, DWORD_METADATA, wszBuffer, sizeof(DWORD), & dwReqBufLen );
		if ( SUCCEEDED(hRet)  )
		{
			pstAppPoolInfo->dwPeriodicRestartTime = *((DWORD*)wszBuffer);
		}
	
		//	2003	���� >> ���չ�������(������Ŀ)
		//		dwPeriodicRestartRequests
		memset( wszBuffer, 0, sizeof(wszBuffer) );
		hRet = getRecordData( hMDAppPool, & MyRecord, MD_APPPOOL_PERIODICRESTARTTIME, METADATA_INHERIT, IIS_MD_UT_SERVER, DWORD_METADATA, wszBuffer, sizeof(DWORD), & dwReqBufLen );
		if ( SUCCEEDED(hRet)  )
		{
			pstAppPoolInfo->dwPeriodicRestartRequests = *((DWORD*)wszBuffer);
		}

		//	2004 ���� >> ������ʱ����չ�������
		//		szPeriodicRestartSchedule
		memset( wszBuffer, 0, sizeof(wszBuffer) );
		hRet = getRecordData( hMDAppPool, & MyRecord, MD_APPPOOL_PERIODICRESTARTSCHEDULE, METADATA_INHERIT, IIS_MD_UT_SERVER, MULTISZ_METADATA, wszBuffer, sizeof(wszBuffer), & dwReqBufLen );
		if ( SUCCEEDED(hRet) )
		{
			//	...
			//	...
			WideCharToMultiByte( CP_ACP, 0, wszBuffer, wcslen(wszBuffer), pstAppPoolInfo->szPeriodicRestartSchedule, sizeof(pstAppPoolInfo->szPeriodicRestartSchedule), NULL, NULL );
		}

		//	2005	���� >> �ڴ���� >> ��������ڴ�(��)
		//		dwPeriodicRestartMemory
		memset( wszBuffer, 0, sizeof(wszBuffer) );
		hRet = getRecordData( hMDAppPool, & MyRecord, MD_APPPOOL_PERIODICRESTARTMEMORY, METADATA_INHERIT, IIS_MD_UT_SERVER, DWORD_METADATA, wszBuffer, sizeof(DWORD), & dwReqBufLen );
		if ( SUCCEEDED(hRet)  )
		{
			pstAppPoolInfo->dwPeriodicRestartMemory = *((DWORD*)wszBuffer);
		}

		//	2006	���� >> �ڴ���� >> ���ʹ�õ��ڴ�(��)
		//		dwPeriodicRestartPrivateMemory
		memset( wszBuffer, 0, sizeof(wszBuffer) );
		hRet = getRecordData( hMDAppPool, & MyRecord, MD_APPPOOL_PERIODICRESTARTPRIVATEMEMORY, METADATA_INHERIT, IIS_MD_UT_SERVER, DWORD_METADATA, wszBuffer, sizeof(DWORD), & dwReqBufLen );
		if ( SUCCEEDED(hRet)  )
		{
			pstAppPoolInfo->dwPeriodicRestartPrivateMemory = *((DWORD*)wszBuffer);
		}

		//	2007	���� >> ���г�ʱ >> �ڿ��д˶ι���ʵ����رչ�������(����)
		//		dwIdleTimeout
		memset( wszBuffer, 0, sizeof(wszBuffer) );
		hRet = getRecordData( hMDAppPool, & MyRecord, MD_APPPOOL_IDLETIMEOUT, METADATA_INHERIT, IIS_MD_UT_SERVER, DWORD_METADATA, wszBuffer, sizeof(DWORD), & dwReqBufLen );
		if ( SUCCEEDED(hRet)  )
		{
			pstAppPoolInfo->dwIdleTimeout = *((DWORD*)wszBuffer);
		}

		//	2008	���� >> ����������� >> ���������������Ϊ(�������)
		//		dwAppPoolQueueLength
		memset( wszBuffer, 0, sizeof(wszBuffer) );
		hRet = getRecordData( hMDAppPool, & MyRecord, MD_APPPOOL_APPPOOLQUEUELENGTH, METADATA_INHERIT, IIS_MD_UT_SERVER, DWORD_METADATA, wszBuffer, sizeof(DWORD), & dwReqBufLen );
		if ( SUCCEEDED(hRet)  )
		{
			pstAppPoolInfo->dwAppPoolQueueLength = *((DWORD*)wszBuffer);
		}

		//	2009	���� >> ���� CPU ���� >> ��� CPU ʹ����(�ٷֱ�)
		//		dwCPULimit
		memset( wszBuffer, 0, sizeof(wszBuffer) );
		hRet = getRecordData( hMDAppPool, & MyRecord, MD_APPPOOL_CPULIMIT, METADATA_INHERIT, IIS_MD_UT_SERVER, DWORD_METADATA, wszBuffer, sizeof(DWORD), & dwReqBufLen );
		if ( SUCCEEDED(hRet)  )
		{
			pstAppPoolInfo->dwCPULimit = *((DWORD*)wszBuffer);
		}

		//	2010	���� >> ���� CPU ���� >> ˢ�� CPU ʹ����(����)
		//		dwCPUResetInterval
		memset( wszBuffer, 0, sizeof(wszBuffer) );
		hRet = getRecordData( hMDAppPool, & MyRecord, MD_APPPOOL_CPURESETINTERVAL, METADATA_INHERIT, IIS_MD_UT_SERVER, DWORD_METADATA, wszBuffer, sizeof(DWORD), & dwReqBufLen );
		if ( SUCCEEDED(hRet)  )
		{
			pstAppPoolInfo->dwCPUResetInterval = *((DWORD*)wszBuffer);
		}

		//	2011	���� >> ���� CPU ���� >> CPU ʹ���ʳ������ʹ����ʱִ�еĲ���
		//		dwCPUAction
		memset( wszBuffer, 0, sizeof(wszBuffer) );
		hRet = getRecordData( hMDAppPool, & MyRecord, MD_APPPOOL_CPUACTION, METADATA_INHERIT, IIS_MD_UT_SERVER, DWORD_METADATA, wszBuffer, sizeof(DWORD), & dwReqBufLen );
		if ( SUCCEEDED(hRet)  )
		{
			pstAppPoolInfo->dwCPUAction = *((DWORD*)wszBuffer);
		}

		//	2012	���� >> Web԰ >> �����������
		//		dwMaxProcesses
		memset( wszBuffer, 0, sizeof(wszBuffer) );
		hRet = getRecordData( hMDAppPool, & MyRecord, MD_APPPOOL_MAXPROCESSES, METADATA_INHERIT, IIS_MD_UT_SERVER, DWORD_METADATA, wszBuffer, sizeof(DWORD), & dwReqBufLen );
		if ( SUCCEEDED(hRet)  )
		{
			pstAppPoolInfo->dwMaxProcesses = *((DWORD*)wszBuffer);
		}

		//	2013	����״�� >> ���� Ping
		//		dwPingingEnabled
		memset( wszBuffer, 0, sizeof(wszBuffer) );
		hRet = getRecordData( hMDAppPool, & MyRecord, MD_APPPOOL_PINGINGENABLED, METADATA_INHERIT, IIS_MD_UT_SERVER, DWORD_METADATA, wszBuffer, sizeof(DWORD), & dwReqBufLen );
		if ( SUCCEEDED(hRet)  )
		{
			pstAppPoolInfo->dwPingingEnabled = *((DWORD*)wszBuffer);
		}

		//	2014	����״�� >> ���� Ping >> ÿ������ʱ�� Ping �����߳�(��)
		//		dwPingInterval
		memset( wszBuffer, 0, sizeof(wszBuffer) );
		hRet = getRecordData( hMDAppPool, & MyRecord, MD_APPPOOL_PINGINTERVAL, METADATA_INHERIT, IIS_MD_UT_SERVER, DWORD_METADATA, wszBuffer, sizeof(DWORD), & dwReqBufLen );
		if ( SUCCEEDED(hRet)  )
		{
			pstAppPoolInfo->dwPingInterval = *((DWORD*)wszBuffer);
		}

		//	2015	����״�� >> ���ÿ���ʧ�ܱ���
		//		dwRapidFailProtection
		memset( wszBuffer, 0, sizeof(wszBuffer) );
		hRet = getRecordData( hMDAppPool, & MyRecord, MD_APPPOOL_RAPIDFAILPROTECTION, METADATA_INHERIT, IIS_MD_UT_SERVER, DWORD_METADATA, wszBuffer, sizeof(DWORD), & dwReqBufLen );
		if ( SUCCEEDED(hRet)  )
		{
			pstAppPoolInfo->dwRapidFailProtection = *((DWORD*)wszBuffer);
		}

		//	2016	����״�� >> ���ÿ���ʧ�ܱ��� >> ʧ����
		//		dwRapidFailProtectionMaxCrashes
		memset( wszBuffer, 0, sizeof(wszBuffer) );
		hRet = getRecordData( hMDAppPool, & MyRecord, MD_APPPOOL_RAPIDFAILPROTECTIONMAXCRASHES, METADATA_INHERIT, IIS_MD_UT_SERVER, DWORD_METADATA, wszBuffer, sizeof(DWORD), & dwReqBufLen );
		if ( SUCCEEDED(hRet)  )
		{
			pstAppPoolInfo->dwRapidFailProtectionMaxCrashes = *((DWORD*)wszBuffer);
		}

		//	2017	����״�� >> ���ÿ���ʧ�ܱ��� >> ʱ���(����)
		//		dwRapidFailProtectionInterval
		memset( wszBuffer, 0, sizeof(wszBuffer) );
		hRet = getRecordData( hMDAppPool, & MyRecord, MD_APPPOOL_RAPIDFAILPROTECTIONINTERVAL, METADATA_INHERIT, IIS_MD_UT_SERVER, DWORD_METADATA, wszBuffer, sizeof(DWORD), & dwReqBufLen );
		if ( SUCCEEDED(hRet)  )
		{
			pstAppPoolInfo->dwRapidFailProtectionInterval = *((DWORD*)wszBuffer);
		}

		//	2018	����״�� >> ����ʱ������ >> �������̱���������ʱ���ڿ�ʼ(��)
		//		dwStartupTimeLimit
		memset( wszBuffer, 0, sizeof(wszBuffer) );
		hRet = getRecordData( hMDAppPool, & MyRecord, MD_APPPOOL_STARTUPTIMELIMIT, METADATA_INHERIT, IIS_MD_UT_SERVER, DWORD_METADATA, wszBuffer, sizeof(DWORD), & dwReqBufLen );
		if ( SUCCEEDED(hRet)  )
		{
			pstAppPoolInfo->dwStartupTimeLimit = *((DWORD*)wszBuffer);
		}

		//	2019	����״�� >> �ر�ʱ������ >> �������̱���������ʱ���ڹر�(��)
		//		dwShutdownTimeLimit
		memset( wszBuffer, 0, sizeof(wszBuffer) );
		hRet = getRecordData( hMDAppPool, & MyRecord, MD_APPPOOL_SHUTDOWNTIMELIMIT, METADATA_INHERIT, IIS_MD_UT_SERVER, DWORD_METADATA, wszBuffer, sizeof(DWORD), & dwReqBufLen );
		if ( SUCCEEDED(hRet)  )
		{
			pstAppPoolInfo->dwShutdownTimeLimit = *((DWORD*)wszBuffer);
		}

		//
		//	������
		//
		if ( hMDAppPool )
		{
			m_pIMeta->CloseKey( hMDAppPool );
		}
	}
	catch ( LPTSTR lpszError )
	{
		setLastErrorInfo( 0, lpszError );
	}

	return TRUE;
}

BOOL CMetaOpIIS::getSiteDataFromKey( LPCWSTR lpcwszKeyName, STMETASITEINFO * pstSiteInfo )
{
	HRESULT hRet					= NULL;
	METADATA_HANDLE hMDSite				= NULL;
	METADATA_HANDLE	hMDSiteRoot			= NULL;
	METADATA_RECORD MyRecord;
	DWORD dwBufLen					= METAOPIIS_BUFFER_SIZE*sizeof(WCHAR);
	DWORD dwReqBufLen				= 0;
	WCHAR wszKeyPath[ METADATA_MAX_NAME_LEN ]	= {0};
	WCHAR wszBuffer[ METAOPIIS_BUFFER_SIZE ]	= {0};
	LPWSTR lpwszTemp				= NULL;


	//	д��KEYNAME
	WideCharToMultiByte( CP_ACP, 0, lpcwszKeyName, wcslen(lpcwszKeyName), pstSiteInfo->szKeyName, sizeof(pstSiteInfo->szKeyName), NULL, NULL );

	//	�ڼ���ǰ����"/"
	_snwprintf( wszKeyPath, sizeof(wszKeyPath)-sizeof(WCHAR), L"/LM/W3SVC/%s", lpcwszKeyName );

	try
	{
		hRet = m_pIMeta->OpenKey( METADATA_MASTER_ROOT_HANDLE, wszKeyPath, METADATA_PERMISSION_READ, METAOPIIS_MD_TIMEOUT, &hMDSite );
		if ( ! SUCCEEDED( hRet ) )
		{
			throw L"��KEYNAME��ʧ��";
		}

		//	1001	ȡ������ MD_SERVER_COMMENT
		memset( wszBuffer, 0, sizeof(wszBuffer) );
		hRet = getRecordData( hMDSite, & MyRecord, MD_SERVER_COMMENT, METADATA_INHERIT, IIS_MD_UT_SERVER, STRING_METADATA, wszBuffer, sizeof(wszBuffer), & dwReqBufLen );
		if ( ! SUCCEEDED(hRet) && MD_ERROR_DATA_NOT_FOUND != hRet )
		{
			throw "��ȡվ������ʧ��";
		}
		else
		{
			WideCharToMultiByte( CP_ACP, 0, wszBuffer, wcslen(wszBuffer), pstSiteInfo->szServerComment, sizeof(pstSiteInfo->szServerComment), NULL, NULL );
		}

		//	1002	ȡ������ AppPoolId
		memset( wszBuffer, 0, sizeof(wszBuffer) );
		hRet = getRecordData( hMDSite, & MyRecord, 9101, METADATA_INHERIT, IIS_MD_UT_SERVER, STRING_METADATA, wszBuffer, sizeof(wszBuffer), & dwReqBufLen );
		if ( ! SUCCEEDED(hRet) && MD_ERROR_DATA_NOT_FOUND != hRet )
		{
			throw "��ȡվ�� AppPoolId ʧ��";
		}
		else
		{
			WideCharToMultiByte( CP_ACP, 0, wszBuffer, wcslen(wszBuffer), pstSiteInfo->szAppPoolId, sizeof(pstSiteInfo->szAppPoolId), NULL, NULL );
		}

		//	1003	ȡ������ MD_SERVER_STATE
		memset( wszBuffer, 0, sizeof(wszBuffer) );
		hRet = getRecordData( hMDSite, & MyRecord, MD_SERVER_STATE, METADATA_VOLATILE, IIS_MD_UT_SERVER, DWORD_METADATA, wszBuffer, sizeof(DWORD), & dwReqBufLen );
		if ( ! SUCCEEDED( hRet ) && MD_ERROR_DATA_NOT_FOUND != hRet )
		{
			throw "��ȡվ��״̬ʧ��";
		}
		else
		{
			pstSiteInfo->dwServerState = *((DWORD*)wszBuffer);
		}

		//	1004	��ȡ������ Http ���ӡ�
		memset( wszBuffer, 0, sizeof(wszBuffer) );
		pstSiteInfo->dwAllowKeepAlive	= 1;
		hRet = getRecordData( hMDSite, & MyRecord, MD_ALLOW_KEEPALIVES, METADATA_INHERIT, IIS_MD_UT_FILE, DWORD_METADATA, wszBuffer, sizeof(DWORD), & dwReqBufLen );
		if ( SUCCEEDED( hRet ) )
		{
			pstSiteInfo->dwAllowKeepAlive = *((DWORD*)wszBuffer);
		}

		//	1005	��ȡ�����ӳ�ʱ��
		memset( wszBuffer, 0, sizeof(wszBuffer) );
		pstSiteInfo->dwConnectionTimeout	= 120;
		hRet = getRecordData( hMDSite, & MyRecord, MD_CONNECTION_TIMEOUT, METADATA_INHERIT, IIS_MD_UT_SERVER, DWORD_METADATA, wszBuffer, sizeof(DWORD), & dwReqBufLen );
		if ( SUCCEEDED( hRet ) )
		{
			pstSiteInfo->dwConnectionTimeout = *((DWORD*)wszBuffer);
		}

		//	1006	ȡ������ MD_SERVER_BINDINGS
		lpwszTemp = new WCHAR[ METAOPIIS_BUFFER_SIZE ];
		memset( lpwszTemp, 0, METAOPIIS_BUFFER_SIZE*sizeof(WCHAR) );
		MyRecord.dwMDIdentifier = MD_SERVER_BINDINGS;
		MyRecord.dwMDAttributes = 0;
		MyRecord.dwMDUserType   = IIS_MD_UT_SERVER;
		MyRecord.dwMDDataType   = MULTISZ_METADATA;
		MyRecord.dwMDDataLen    = METAOPIIS_BUFFER_SIZE*sizeof(WCHAR);    
		MyRecord.pbMDData       = (unsigned char *)lpwszTemp;
		MyRecord.dwMDDataTag    = 0; 

		hRet = m_pIMeta->GetData( hMDSite, NULL, &MyRecord, &dwReqBufLen );
		if ( RETURNCODETOHRESULT(ERROR_INSUFFICIENT_BUFFER) == hRet )
		{
			if ( lpwszTemp )
			{
				delete [] lpwszTemp;
				lpwszTemp = NULL;
			}
			lpwszTemp = new WCHAR[ dwReqBufLen ];
			memset( lpwszTemp, 0, dwReqBufLen*sizeof(WCHAR) );
			MyRecord.dwMDDataLen	= dwReqBufLen;
			MyRecord.pbMDData	= (unsigned char *)lpwszTemp;
			hRet = m_pIMeta->GetData( hMDSite, NULL, &MyRecord, &dwReqBufLen );
			if ( ! SUCCEEDED( hRet ) )
			{
				throw "��ȡվ�������ʧ��";
			}
			dwBufLen = dwReqBufLen;
		}
		dwReqBufLen = dwBufLen;
		if ( wcslen(lpwszTemp) )
		{
			WideCharToMultiByte( CP_ACP, 0, lpwszTemp, dwReqBufLen, pstSiteInfo->szBindings, sizeof(pstSiteInfo->szBindings), NULL, NULL );

			//	�԰���Ϣ���д�����ÿ����ͷ֮��ġ�NULL����Ϊ��|���������һ��Ҳ��Ϊ|
			formatSBindingsToString( pstSiteInfo->szBindings, sizeof(pstSiteInfo->szBindings) );
		}
		else
		{
			strcpy( pstSiteInfo->szBindings, ":80:|" );
		}

		if ( lpwszTemp )
		{
			delete [] lpwszTemp;
			lpwszTemp = NULL;
		}


		//	1007	ȡ������ MD_MAX_CONNECTION
		memset( wszBuffer, 0, sizeof(wszBuffer) );
		pstSiteInfo->lnMaxConnection	= -1;	//	����������
		hRet = getRecordData( hMDSite, & MyRecord, MD_MAX_CONNECTIONS, 0, IIS_MD_UT_SERVER, DWORD_METADATA, wszBuffer, sizeof(DWORD), & dwReqBufLen );
		if ( SUCCEEDED( hRet ) )
		{
			pstSiteInfo->lnMaxConnection = *((LONG*)wszBuffer);
		}
		

		//	1008	��ȡ����������
		memset( wszBuffer, 0, sizeof(wszBuffer) );
		pstSiteInfo->lnMaxBindwidth	= -1;	//	����������
		hRet = getRecordData( hMDSite, & MyRecord, MD_MAX_BANDWIDTH, 0, IIS_MD_UT_SERVER, DWORD_METADATA, wszBuffer, sizeof(DWORD), & dwReqBufLen );
		if ( SUCCEEDED( hRet ) )
		{
			pstSiteInfo->lnMaxBindwidth = *((LONG*)wszBuffer);
		}

		//
		//	��ȡ��־�������
		//

		//	1009	��ȡ�Ƿ�������־ MD_LOG_TYPE
		memset( wszBuffer, 0, sizeof(wszBuffer) );
		pstSiteInfo->lnLogType = 0;	//	��վ >> 0-������ 1-���ò�ʹ��W3C��ʽ
		hRet = getRecordData( hMDSite, & MyRecord, MD_LOG_TYPE, METADATA_INHERIT, IIS_MD_UT_SERVER, DWORD_METADATA, wszBuffer, sizeof(DWORD), & dwReqBufLen );
		if ( SUCCEEDED( hRet ) )
		{
			pstSiteInfo->lnLogType = *((LONG*)wszBuffer);
		}

		//	1010	��ȡ�Ƿ�������־ lnLogFileLocaltimeRollover
		memset( wszBuffer, 0, sizeof(wszBuffer) );
		pstSiteInfo->lnLogFileLocaltimeRollover = 0;
		hRet = getRecordData( hMDSite, & MyRecord, 4015, METADATA_INHERIT, IIS_MD_UT_SERVER, DWORD_METADATA, wszBuffer, sizeof(DWORD), & dwReqBufLen );
		if ( SUCCEEDED( hRet ) )
		{
			pstSiteInfo->lnLogFileLocaltimeRollover = *((LONG*)wszBuffer);
		}

		//	1011	��ȡ ��־��¼��lnLogFilePeriod
		memset( wszBuffer, 0, sizeof(wszBuffer) );
		pstSiteInfo->lnLogFilePeriod = 1;	//	��վ >> ��־ >> 1ÿ�죬4ÿСʱ��2ÿ�ܣ�3ÿ�£�0�������ļ���С/���ļ���С�ﵽ
		hRet = getRecordData( hMDSite, & MyRecord, MD_LOGFILE_PERIOD, METADATA_INHERIT, IIS_MD_UT_SERVER, DWORD_METADATA, wszBuffer, sizeof(DWORD), & dwReqBufLen );
		if ( SUCCEEDED( hRet ) )
		{
			pstSiteInfo->lnLogFilePeriod = *((LONG*)wszBuffer);
		}

		//	1012	��ȡ ��־��¼��lnLogFileTruncateSize
		memset( wszBuffer, 0, sizeof(wszBuffer) );
		pstSiteInfo->lnLogFileTruncateSize = 20971520;	//	��վ >> ��־��¼���ļ���С
		hRet = getRecordData( hMDSite, & MyRecord, MD_LOGFILE_TRUNCATE_SIZE, METADATA_INHERIT, IIS_MD_UT_SERVER, DWORD_METADATA, wszBuffer, sizeof(DWORD), & dwReqBufLen );
		if ( SUCCEEDED( hRet ) )
		{
			pstSiteInfo->lnLogFileTruncateSize = *((LONG*)wszBuffer);
		}

		//	1013	��ȡ ��־��¼��lnLogExtFileFlags
		memset( wszBuffer, 0, sizeof(wszBuffer) );
		pstSiteInfo->lnLogExtFileFlags = 0;		//	��վ >> ��־��¼���߼�ѡ��
		hRet = getRecordData( hMDSite, & MyRecord, MD_LOGEXT_FIELD_MASK, METADATA_INHERIT, IIS_MD_UT_SERVER, DWORD_METADATA, wszBuffer, sizeof(DWORD), & dwReqBufLen );
		if ( SUCCEEDED( hRet ) )
		{
			pstSiteInfo->lnLogExtFileFlags = *((LONG*)wszBuffer);
		}

		//	1014	��ȡ ��־��¼��Ŀ¼ szLogFileDirectory
		memset( wszBuffer, 0, sizeof(wszBuffer) );
		hRet = getRecordData( hMDSite, & MyRecord, MD_LOGFILE_DIRECTORY, METADATA_INHERIT, IIS_MD_UT_SERVER, EXPANDSZ_METADATA, wszBuffer, sizeof(wszBuffer), &dwReqBufLen );
		if ( SUCCEEDED( hRet ) )
		{
			WideCharToMultiByte( CP_ACP, 0, wszBuffer, wcslen(wszBuffer), pstSiteInfo->szLogFileDirectory, sizeof(pstSiteInfo->szLogFileDirectory), NULL, NULL );
		}
		else
		{
			throw "��ȡ��־��¼Ŀ¼ʧ��";
		}

		//	1015	��ȡ ��־��¼��PluginClsid
		memset( wszBuffer, 0, sizeof(wszBuffer) );
		hRet = getRecordData( hMDSite, & MyRecord, MD_LOG_PLUGIN_ORDER, METADATA_INHERIT, IIS_MD_UT_SERVER, STRING_METADATA, wszBuffer, sizeof(wszBuffer), &dwReqBufLen );
		if ( SUCCEEDED( hRet ) )
		{
			WideCharToMultiByte( CP_ACP, 0, wszBuffer, wcslen(wszBuffer), pstSiteInfo->szLogPluginClsid, sizeof(pstSiteInfo->szLogPluginClsid), NULL, NULL );
		}

		//	1016	����Ĭ���ĵ� MD_DEFAULT_LOAD_FILE
		memset( wszBuffer, 0, sizeof(wszBuffer) );
		dwReqBufLen = 0;
		hRet = getRecordData( hMDSite, & MyRecord, MD_DEFAULT_LOAD_FILE, METADATA_INHERIT, IIS_MD_UT_FILE, STRING_METADATA, wszBuffer, sizeof(wszBuffer), &dwReqBufLen );
		if ( SUCCEEDED( hRet ) )
		{
			WideCharToMultiByte( CP_ACP, 0, wszBuffer, wcslen(wszBuffer), pstSiteInfo->szDefaultDoc, sizeof(pstSiteInfo->szDefaultDoc), NULL, NULL);
		}
		else
		{
			if ( MD_ERROR_DATA_NOT_FOUND == hRet )
			{
				_sntprintf( pstSiteInfo->szDefaultDoc, sizeof(pstSiteInfo->szDefaultDoc)-sizeof(TCHAR), "%s", METAOPIIS_DEFAULTDOC );
			}
			else
			{
				throw "��ȡĬ���ĵ�ʧ��";
			}
		}
		if ( 0 == strlen( pstSiteInfo->szDefaultDoc ) )
		{
			_sntprintf( pstSiteInfo->szDefaultDoc, sizeof(pstSiteInfo->szDefaultDoc)-sizeof(TCHAR), "%s", METAOPIIS_DEFAULTDOC );
		}

		//
		//	�ȴ�/RootĿ¼
		//	��ȡ��Ŀ¼����
		//

		hRet = m_pIMeta->OpenKey( hMDSite, L"/Root", METADATA_PERMISSION_READ, METAOPIIS_MD_TIMEOUT, &hMDSiteRoot );
		if ( ! SUCCEEDED( hRet ) )
		{
			throw "��KEYNAME/Root��ʧ��";
		}

		//	1017	��ȡPath����
		memset( wszBuffer, 0, sizeof(wszBuffer) );
		hRet = getRecordData( hMDSiteRoot, & MyRecord, MD_VR_PATH, 0, IIS_MD_UT_FILE, STRING_METADATA, wszBuffer, sizeof(wszBuffer), & dwReqBufLen );
		if ( SUCCEEDED( hRet ) )
		{
			WideCharToMultiByte( CP_ACP, 0, wszBuffer, wcslen(wszBuffer), pstSiteInfo->szServerMDir, sizeof(pstSiteInfo->szServerMDir), NULL, NULL );
		}
		else
		{
			throw "��ȡ��Ŀ¼·��ʧ��";
		}

		//	1018	��ȡ��Ŀ¼��������
		memset( wszBuffer, 0, sizeof(wszBuffer) );
		pstSiteInfo->dwMDirAccessFlag = 513;
		hRet = getRecordData( hMDSiteRoot, & MyRecord, MD_ACCESS_PERM, METADATA_INHERIT, IIS_MD_UT_FILE, DWORD_METADATA, wszBuffer, sizeof(DWORD), & dwReqBufLen );
		if ( SUCCEEDED( hRet ) )
		{
			pstSiteInfo->dwMDirAccessFlag = *((DWORD*)wszBuffer);
		}

		//	1019	��ȡ ��Ŀ¼��Ŀ¼�������
		memset( wszBuffer, 0, sizeof(wszBuffer) );
		hRet = getRecordData( hMDSiteRoot, & MyRecord, MD_DIRECTORY_BROWSING, METADATA_INHERIT, IIS_MD_UT_FILE, DWORD_METADATA, wszBuffer, sizeof(DWORD), & dwReqBufLen );
		if ( SUCCEEDED( hRet ) )
		{
			pstSiteInfo->lnDirBrowseFlags = *((LONG*)wszBuffer);
		}

		//	1020	��ȡ ��Ŀ¼����¼����
		memset( wszBuffer, 0, sizeof(wszBuffer) );
		pstSiteInfo->dwDontLog = 1;
		hRet = getRecordData( hMDSiteRoot, & MyRecord, MD_DONT_LOG, METADATA_INHERIT, IIS_MD_UT_FILE, DWORD_METADATA, wszBuffer, sizeof(DWORD), & dwReqBufLen );
		if ( SUCCEEDED( hRet ) )
		{
			//	1 Ϊ����¼��0Ϊ��¼
			pstSiteInfo->dwDontLog = *((LONG*)wszBuffer);
		}

		//	1021	��ȡ ��Ŀ¼��������Դ
		memset( wszBuffer, 0, sizeof(wszBuffer) );
		pstSiteInfo->dwContentIndexed = 0;
		hRet = getRecordData( hMDSiteRoot, & MyRecord, MD_IS_CONTENT_INDEXED, METADATA_INHERIT, IIS_MD_UT_FILE, DWORD_METADATA, wszBuffer, sizeof(DWORD), & dwReqBufLen );
		if ( SUCCEEDED( hRet ) )
		{
			pstSiteInfo->dwContentIndexed = *((LONG*)wszBuffer);
		}


		//	1022	��ȡ http ͷ >> ���ݹ���
		memset( wszBuffer, 0, sizeof(wszBuffer) );
		hRet = getRecordData( hMDSiteRoot, & MyRecord, MD_HTTP_EXPIRES, METADATA_INHERIT, IIS_MD_UT_FILE, STRING_METADATA, wszBuffer, sizeof(wszBuffer), &dwReqBufLen );
		if ( SUCCEEDED( hRet ) )
		{
			WideCharToMultiByte( CP_ACP, 0, wszBuffer, wcslen(wszBuffer), pstSiteInfo->szHttpExpires, sizeof(pstSiteInfo->szHttpExpires), NULL, NULL);
		}

		//	��ȡ �Զ������


		//	1023	��ȡ �ĵ� >> �����ĵ�ҳ��
		memset( wszBuffer, 0, sizeof(wszBuffer) );
		pstSiteInfo->lnEnableDocFooter = 0;
		hRet = getRecordData( hMDSiteRoot, & MyRecord, MD_FOOTER_ENABLED, METADATA_INHERIT, IIS_MD_UT_FILE, DWORD_METADATA, wszBuffer, sizeof(DWORD), &dwReqBufLen );
		if ( SUCCEEDED( hRet ) )
		{
			pstSiteInfo->lnEnableDocFooter = *((LONG*)wszBuffer);
		}

		//	1024	��ȡ �ĵ� >> �ĵ�ҳ���ļ�·��
		memset( wszBuffer, 0, sizeof(wszBuffer) );
		hRet = getRecordData( hMDSiteRoot, & MyRecord, MD_FOOTER_DOCUMENT, METADATA_INHERIT, IIS_MD_UT_FILE, STRING_METADATA, wszBuffer, sizeof(wszBuffer), &dwReqBufLen );
		if ( SUCCEEDED( hRet ) )
		{
			WideCharToMultiByte( CP_ACP, 0, wszBuffer, wcslen(wszBuffer), pstSiteInfo->szDefaultDocFooter, sizeof(pstSiteInfo->szDefaultDocFooter), NULL, NULL);
		}


		//
		//	������
		//
		if ( hMDSiteRoot )
		{
			m_pIMeta->CloseKey( hMDSiteRoot );
		}
		if ( hMDSite )
		{
			m_pIMeta->CloseKey( hMDSite );
		}
	}
	catch ( LPTSTR lpszError )
	{
		setLastErrorInfo( 0, lpszError );
	}

	return TRUE;
}



//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////







/**
 *	@ Private
 *	��ȡһ�������ڵ� KeyName
 */
DWORD CMetaOpIIS::getNewSiteKey()
{
	DWORD dwRet		= 0;
	SYSTEMTIME st;
	HRESULT	hRet;
	METADATA_HANDLE hLocalMachine			= NULL;
	WCHAR wszKeyName[ METADATA_MAX_NAME_LEN ]	= {0};
	TCHAR szTemp[ MAX_PATH ]			= {0};
	DWORD dwIndex;
	BOOL bExist;
	vector<DWORD> vcKeyNum;
	vector<DWORD>::iterator it;

	//	ȡ�ñ������ݵľ��
	hRet = m_pIMeta->OpenKey( METADATA_MASTER_ROOT_HANDLE, L"/LM", METADATA_PERMISSION_READ, METAOPIIS_MD_TIMEOUT, &hLocalMachine );
	if ( SUCCEEDED(hRet) && hLocalMachine )
	{
		dwIndex = 0;
		while ( SUCCEEDED(hRet) )
		{
			//	ö��WWW����վ��
			hRet = m_pIMeta->EnumKeys( hLocalMachine, L"/W3SVC", wszKeyName, dwIndex );
			if ( SUCCEEDED( hRet ) )
			{
				//	���˵�Info��Filter��
				if ( 0 != wcscmp( wszKeyName, L"" ) &&
					0 != wcscmp( wszKeyName, L"Info" ) && 
					0 != wcscmp( wszKeyName, L"Filters" ) && 
					0 != wcscmp( wszKeyName, L"AppPools" ) )
				{
					vcKeyNum.push_back( (DWORD)_wtol( wszKeyName ) );
				}
			}
			dwIndex ++;
		}
		m_pIMeta->CloseKey( hLocalMachine );
	}
	else
	{
		setLastErrorInfo( 0, "��ȡ�����������ʧ��" );
	}

	//	ѭ��Ŀǰ���е� KeyName
	while( vcKeyNum.size() )
	{
		bExist = FALSE;

		//	����һ���µ� keyvalue
		GetLocalTime( & st );
		_sntprintf( szTemp, sizeof(szTemp)-sizeof(TCHAR), "%04d-%02d-%02d %02d:%02d:%02d", st.wYear, st.wMonth, st.wDay, st.wHour, st.wMinute, st.wSecond );
		dwRet = m_cCrc32.GetCrc32( (unsigned char *)szTemp, strlen(szTemp) );
		if ( dwRet > 999999999 )
		{
			dwRet = (DWORD)( dwRet / 100 );
			if ( dwRet > 999999999 )
			{
				dwRet = (DWORD)( dwRet / 100 );
			}
		}

		for ( it = vcKeyNum.begin(); it != vcKeyNum.end(); it ++ )
		{
			if ( dwRet == (*it) )
			{
				bExist = TRUE;
				break;
			}
		}

		if ( ! bExist )
		{
			break;
		}
	}

	return dwRet;
}


/**
 *	@ Priavte
 *	��ȡһ��վ���µ�ĳ����¼��ֵ
 */
HRESULT CMetaOpIIS::getRecordData( METADATA_HANDLE hKeyName, METADATA_RECORD * pstMRec, 
				DWORD dwMDIdentifier, DWORD dwMDAttributes, DWORD dwMDUserType, DWORD dwMDDataType, LPCWSTR lpcwszBuf, DWORD dwBufLen,
				DWORD * pdwMDRequiredDataLen )
{
	if ( NULL == hKeyName || NULL == pstMRec )
	{
		return -1;
	}

//	BOOL bRet	= FALSE;
//	HRESULT hRet	= NULL;

	pstMRec->dwMDIdentifier	= dwMDIdentifier;
	pstMRec->dwMDAttributes	= dwMDAttributes;
	pstMRec->dwMDUserType	= dwMDUserType;
	pstMRec->dwMDDataType	= dwMDDataType;
	pstMRec->dwMDDataLen	= dwBufLen;    
	pstMRec->pbMDData	= (unsigned char *)(lpcwszBuf);
	pstMRec->dwMDDataTag	= 0;

	return m_pIMeta->GetData( hKeyName, NULL, pstMRec, pdwMDRequiredDataLen );
}

/**
 *	@ Priavte
 *	�޸�һ��վ���µ�ĳ����¼��ֵ
 */
BOOL CMetaOpIIS::modifyRecordData( STMETAOPERRECORD * pstRec, DWORD dwMDIdentifier, DWORD dwMDAttributes, DWORD dwMDUserType, DWORD dwMDDataType, LPCWSTR lpcwszBuf, DWORD dwBufLen )
{
	if ( NULL == pstRec )
	{
		return FALSE;
	}

//	STMETAOPERRECORD rec;
//	memset( & rec, 0, sizeof(rec) );

	pstRec->metaRecord.dwMDIdentifier	= dwMDIdentifier;
	pstRec->metaRecord.dwMDAttributes	= dwMDAttributes;
	pstRec->metaRecord.dwMDUserType		= dwMDUserType;
	pstRec->metaRecord.dwMDDataType		= dwMDDataType;
	pstRec->metaRecord.dwMDDataLen		= dwBufLen;    
	pstRec->metaRecord.pbMDData		= (unsigned char *)(lpcwszBuf);
	pstRec->metaRecord.dwMDDataTag		= 0;

	return ModifyData( pstRec );
}

BOOL CMetaOpIIS::ModifyData( STMETAOPERRECORD * pstRec )
{
	if ( NULL == pstRec )
	{
		return FALSE;
	}
	if ( ! isInitComReady() )
	{
		setLastErrorInfo( 0, "�޸�վ������ʱ��ָ�� MSAdminBase��IWamAdmin2 �ӿڵ�����ָ��δ��ʼ��" );
		return FALSE;
	}
	
	BOOL bRet	= FALSE;
	HRESULT hRet	= NULL;
	METADATA_HANDLE hLocalMachine = NULL;
	METADATA_HANDLE hCurrent      = NULL;
	
	hRet = m_pIMeta->OpenKey( METADATA_MASTER_ROOT_HANDLE, pstRec->wszKeyPath, METADATA_PERMISSION_WRITE, METAOPIIS_MD_TIMEOUT, &hCurrent );
	if ( SUCCEEDED(hRet) && hCurrent )
	{
		//	����һ��������
		hRet = m_pIMeta->SetData( hCurrent, NULL, & pstRec->metaRecord );
		if ( SUCCEEDED(hRet) )
		{
			bRet = TRUE;
		}
		else
		{
			setLastErrorInfo( 0, "����һ��������ʧ��" );
		}
		
		m_pIMeta->CloseKey( hCurrent );
	}
	else
	{
		setLastErrorInfo( 0, "" );
	}

	return bRet;
}




/**
 *	@ Private
 *	CreateApp
 */
BOOL CMetaOpIIS::mb_createApp( STMETASITEINFO * pstSiteInfo, LPWSTR lpszMDPath, DWORD dwAppMode )
{
	if ( NULL == pstSiteInfo )
	{
		return FALSE;
	}
	if ( ! isInitComReady() )
	{
		setLastErrorInfo( 0, "CreateApp������ָ�� MSAdminBase��IWamAdmin2 �ӿڵ�����ָ��δ��ʼ��" );
		return FALSE;
	}

	HRESULT	hRet	= NULL;
	hRet = m_pIAppAdmin->AppCreate2( lpszMDPath, dwAppMode );
	if ( FAILED(hRet) )
	{
		setLastErrorInfo( 0, "%s վ�㽨��Ӧ�ó���ʧ��", pstSiteInfo->szServerComment );
		return FALSE;
	}

	return TRUE;
}


/**
 *	@ Private
 *	����һ����
 */
BOOL CMetaOpIIS::mb_createKey( STMETAOPERRECORD * pstRec )
{
	if ( NULL == pstRec )
	{
		return FALSE;
	}
	if ( ! isInitComReady() )
	{
		setLastErrorInfo( 0, "������ʱ������ָ�� MSAdminBase��IWamAdmin2 �ӿڵ�����ָ��δ��ʼ��" );
		return FALSE;
	}

	BOOL bRet	= FALSE;
	HRESULT hRet	= NULL;
	METADATA_HANDLE hCurrent      = NULL;
	
	hRet = m_pIMeta->OpenKey( METADATA_MASTER_ROOT_HANDLE, pstRec->wszKeyPath, METADATA_PERMISSION_WRITE, METAOPIIS_MD_TIMEOUT, &hCurrent );
	if ( SUCCEEDED(hRet) && hCurrent )
	{
		//	���һ���Ӽ�
		hRet = m_pIMeta->AddKey( hCurrent, pstRec->wszKeyName );
		if ( SUCCEEDED( hRet ) )
		{
			bRet = TRUE;
		}
		else
		{
			setLastErrorInfo( 0, "" );
		}

		m_pIMeta->CloseKey( hCurrent );
	}
	else
	{
		setLastErrorInfo( 0, "" );
	}

	return bRet;
}

/**
 *	@ Private
 *	ɾ��һ����
 */
BOOL CMetaOpIIS::mb_deleteKey( STMETAOPERRECORD * pstRec )
{
	if ( NULL == pstRec )
	{
		return FALSE;
	}
	if ( ! isInitComReady() )
	{
		setLastErrorInfo( 0, "ɾ����ʱ���֣�ָ�� MSAdminBase��IWamAdmin2 �ӿڵ�����ָ��δ��ʼ��" );
		return FALSE;
	}

	HRESULT hRet = NULL;
	METADATA_HANDLE hLocalMachine = NULL;
	METADATA_HANDLE hCurrent      = NULL;
	
	hRet = m_pIMeta->OpenKey( METADATA_MASTER_ROOT_HANDLE, pstRec->wszKeyPath, METADATA_PERMISSION_WRITE, METAOPIIS_MD_TIMEOUT, &hCurrent );
	if ( !SUCCEEDED(hRet) )
	{
		setLastErrorInfo( 0, "" );
		return FALSE;
	}
	
	//ɾ��һ���Ӽ�
	hRet = m_pIMeta->DeleteKey( hCurrent, pstRec->wszKeyName );
	if ( !SUCCEEDED(hRet) )
	{
		m_pIMeta->CloseKey(hCurrent);

		setLastErrorInfo( 0, "ɾ��һ���Ӽ�ʧ��" );
		return FALSE;
	}
	
	m_pIMeta->CloseKey(hCurrent);
	return TRUE;
}

/**
 *	@ Private
 *	��������һ����
 */
BOOL CMetaOpIIS::mb_renameKey( STMETAOPERRECORD * pstRec )
{
	if ( NULL == pstRec )
	{
		return FALSE;
	}
	if ( ! isInitComReady() )
	{
		setLastErrorInfo( 0, "������һ����ʱ������ָ�� MSAdminBase��IWamAdmin2 �ӿڵ�����ָ��δ��ʼ��" );
		return FALSE;
	}

	HRESULT hRet = NULL;
	METADATA_HANDLE hLocalMachine = NULL;
	METADATA_HANDLE hCurrent      = NULL;
	
	hRet = m_pIMeta->OpenKey( METADATA_MASTER_ROOT_HANDLE, pstRec->wszKeyPath, METADATA_PERMISSION_WRITE, METAOPIIS_MD_TIMEOUT, &hCurrent );
	if ( ! SUCCEEDED(hRet) )
	{
		setLastErrorInfo( 0, "" );
		return FALSE;
	}

	//	������һ����
	hRet = m_pIMeta->RenameKey( hCurrent, NULL, pstRec->wszKeyName );
	if ( ! SUCCEEDED(hRet) )
	{
		m_pIMeta->CloseKey(hCurrent);

		setLastErrorInfo( 0, "������һ����ʧ��" );
		return FALSE;
	}

	m_pIMeta->CloseKey(hCurrent);
	return TRUE;
}

/**
 *	@ Private
 *	��ȡһ������ֵ
 */
BOOL CMetaOpIIS::mb_getKeyData( STMETAOPERRECORD * pstRec )
{
	if ( NULL == pstRec )
	{
		return FALSE;
	}

	BOOL bRet			= FALSE;
	HRESULT hRet			= NULL;
	METADATA_HANDLE hLocalMachine	= NULL;
	METADATA_HANDLE hCurrent	= NULL;
	DWORD dwReqBufLen		= 0;

	hRet = m_pIMeta->OpenKey( METADATA_MASTER_ROOT_HANDLE, pstRec->wszKeyPath, METADATA_PERMISSION_READ, METAOPIIS_MD_TIMEOUT, &hCurrent );
	if ( SUCCEEDED( hRet ) )
	{
		//	��ȡһ��������
		hRet = m_pIMeta->GetData( hCurrent, NULL, &pstRec->metaRecord, &dwReqBufLen );
		if ( SUCCEEDED( hRet ) )
		{
			bRet = TRUE;
		}
		else
		{
			setLastErrorInfo( 0, "��ȡһ��������ʧ��" );
		}
		
		m_pIMeta->CloseKey( hCurrent );
	}
	else
	{
		setLastErrorInfo( 0, "" );
	}

	return bRet;
}


BOOL CMetaOpIIS::DelHttpRedirect(STMETAOPERRECORD &rec)
{
	HRESULT hRet = NULL;
	METADATA_HANDLE hLocalMachine	= NULL;
	METADATA_HANDLE hCurrent	= NULL;
	DWORD dwReqBufLen		= 0;
	BOOL  bRet			= TRUE;

	hRet = m_pIMeta->OpenKey( METADATA_MASTER_ROOT_HANDLE, rec.wszKeyPath, METADATA_PERMISSION_WRITE, METAOPIIS_MD_TIMEOUT, &hCurrent );
	if ( ! SUCCEEDED(hRet) )
	{
		setLastErrorInfo( 0, "" );
		return FALSE;
	}
	
	hRet = m_pIMeta->DeleteData(hCurrent, L"", MD_HTTP_REDIRECT, STRING_METADATA);
	if ( !SUCCEEDED(hRet) )
	{
		if ( MD_ERROR_DATA_NOT_FOUND == hRet )
		{
			//û���ض���
			bRet = FALSE;
		}
		else
		{
			setLastErrorInfo( 0, "ɾ���ض���ʧ��" );
			bRet = FALSE;
		}
	}
	else
	{
		bRet = TRUE;
	}

	m_pIMeta->CloseKey(hCurrent);
	return bRet;
}






/**
 *	@ Private
 *	��� Bindings ��Ϣ�Ƿ���Ч
 */
BOOL CMetaOpIIS::isValidSBindings( LPCTSTR lpcszBindings )
{
	if ( NULL == lpcszBindings )
	{
		return FALSE;
	}

	string strSBindings( lpcszBindings );

	//	��ȡ��ͷ�ĸ���
	string::size_type idx1 = 0;
	string::size_type idx2 = 0;
	string strSub;

	do 
	{
		idx2 = strSBindings.find( '|', idx1 );			
		strSub = strSBindings.substr( idx1, idx2 - idx1 );
		if ( ! isValidSingleSBindings( strSub ) )
		{
			return FALSE;
		}
		if ( idx2 == strSBindings.length()-1 )
		{
			break;
		}
		
		idx1 = idx2+1;
	} while( idx2 != string::npos );
	
	return TRUE;
}
BOOL CMetaOpIIS::isValidSingleSBindings( string strSBindings )
{
	//	��顰:���ĸ���
	int iTimes = count( strSBindings.begin(), strSBindings.end(), ':' );
	if ( 2 != iTimes )
	{
		return FALSE;
	}

	string::size_type idx1 = 0;
	string::size_type idx2 = 0;

	//	���IP�Ƿ���Ч
	string strIP;
	idx2 = strSBindings.find_first_of(':', (string::size_type)0);
	if ( idx2 != 0 )
	{
		strIP = strSBindings.substr(0, idx2);
		//	���'.'����Ŀ�Ƿ�Ϊ3��
		iTimes = count(strIP.begin(), strIP.end(), '.');
		if ( 3 != iTimes )
		{
			return FALSE;
		}
		if ( INADDR_NONE == inet_addr( strIP.c_str() ) )
		{
			return FALSE;
		}
	}

	idx1 = idx2+1;
	//	���PORT�Ƿ���Ч
	string strPort;
	idx2 = strSBindings.find_first_of(':', idx1);
	strPort = strSBindings.substr(idx1, idx2-idx1);
	if ( strPort.empty() )
	{
		return FALSE;
	}
	//	����Ƿ��г�����֮��������ַ�
	if ( string::npos != strPort.find_first_not_of( "0123456789" ) )
	{
		return FALSE;
	}

	unsigned short iPort = (unsigned short)atoi(strPort.c_str());
	if ( iPort < 0 || iPort > 65535 )
	{
		return FALSE;
	}
	
	//	�������ͷ�Ƿ���Ч, Ӣ�ģ����֣�'.', '-'
	return TRUE;
}



/**
 *	@ Private
 *	�����ݿ��¼�ĸ�ʽ(�� | �ָ�)ת��Ϊ Metabase ��ʽ(�� NULL �ָ�)��ͬʱ�����䳤��
 */
INT CMetaOpIIS::formatSBindingsToMetabase( LPWSTR pwszSBindings )
{
	//	���ݿ��еĸ�ʽ�ǣ�IP:PORT:HEADER|IP:PORT:HEADER|....|IP:PORT:HEADER|
	//	ֱ�ӽ� "|" �ĳ� NULL
	INT i;
	INT nLen = wcslen( pwszSBindings );
	for ( i = 0; i < nLen; i ++ )
	{
		if ( '|' == pwszSBindings[ i ] )
		{
			pwszSBindings[ i ] = 0x0;
		}
	}
	return nLen;
}

/**
 *	@ Private
 *	�� Metabase ��ʽ(�� NULL �ָ�)ת��Ϊ���ݿ��¼�ĸ�ʽ(�� | �ָ�)
 */
void CMetaOpIIS::formatSBindingsToString( LPSTR lpszBuffer, DWORD dwSize )
{
	INT i;

	//	��ĳ�������м��NULL��Ϊ"|",����������һ��"|"
	for( i = 0; i < dwSize; i++ )
	{
		if ( 0 == lpszBuffer[i] )
		{
			lpszBuffer[ i ] = '|';
			if ( 0 == lpszBuffer[ i + 1 ] )
			{
				break;
			}
		}
	}

	string sbindings(lpszBuffer);
	string::size_type idx = 0;		// start index
	string::size_type idxnew = 0;	// new index
	string::size_type idxsub = 0;	// sub string index
	
	while ( idx != string::npos )
	{
		//	�ԡ�|����ȡ�ַ���
		//string::size_type idxnew = sbindings.find( '|', idx );
		//	�ҵ���:����λ��
		string::size_type idxsub = sbindings.find( ':', idx );
		
		//	�� IP �û�Ϊ��
		sbindings.replace( idx, (idxsub-idx), "|" );
		if ( '|' == sbindings[0] )
		{
			sbindings = sbindings.replace( 0, 1, "" );
		}

		//	������λ�����ַ������ƶ�
		idx = sbindings.find( '|', idx+1 );
	}
	sbindings[ sbindings.length() ] = NULL;
	strcpy( lpszBuffer, sbindings.c_str() );
}

/*
bool CMetaOpIIS::GetBindingData(STMETAOPERRECORD &rec, WCHAR *pwszBuf, DWORD &dwBufLen)
{
	HRESULT hRet = NULL;
	METADATA_HANDLE hCurrent     = NULL;
	DWORD dwReqBufLen			 = 0;
	MultiByteToWideChar(CP_ACP, 0, m_SBinding.szSiteKeyName, strlen(m_SBinding.szSiteKeyName)+1,
		rec.szKeyName, METAOPIIS_KEYNAME_LEN);

	wcscpy(rec.szKeyPath, L"/LM/W3SVC/");
	wcscat(rec.szKeyPath, rec.szKeyName);

	hRet = m_pIMeta->OpenKey(METADATA_MASTER_ROOT_HANDLE, 
		rec.szKeyPath, METADATA_PERMISSION_READ,
		METAOPIIS_MD_TIMEOUT, &hCurrent);
	if ( !SUCCEEDED(hRet) )
	{
		CErrorOperation::s_dwErrorCode = GetLastError();
		CErrorOperation::WriteErrorLog();
		return false;
	}
	
	//��ȡ��վ���ServerBindings����
	rec.metaRecord.dwMDIdentifier	= MD_SERVER_BINDINGS;
	rec.metaRecord.dwMDAttributes	= 0;
	rec.metaRecord.dwMDUserType		= IIS_MD_UT_SERVER;
	rec.metaRecord.dwMDDataType		= MULTISZ_METADATA;
	rec.metaRecord.dwMDDataLen		= METAOPIIS_BUFFER_SIZE;
	rec.metaRecord.pbMDData			= (unsigned char *)pwszBuf;
	rec.metaRecord.dwMDDataTag		= 0;

	hRet = m_pIMeta->GetData(hCurrent, NULL, &rec.metaRecord, 
		&dwReqBufLen);
	if ( RETURNCODETOHRESULT(ERROR_INSUFFICIENT_BUFFER) == hRet )
	{
		delete [] pwszBuf;
		pwszBuf = new WCHAR[dwReqBufLen/sizeof(WCHAR)];
		memset(pwszBuf,0xff, dwReqBufLen/sizeof(WCHAR));
		rec.metaRecord.dwMDDataLen = dwReqBufLen;
		rec.metaRecord.pbMDData = (unsigned char *)pwszBuf;
		hRet = m_pIMeta->GetData(hCurrent, NULL, &rec.metaRecord, 
			&dwReqBufLen);
		if ( !SUCCEEDED(hRet) )		
		{
			m_pIMeta->CloseKey(hCurrent);
			CErrorOperation::s_dwErrorCode = GetLastError();
			CErrorOperation::WriteErrorLog();	
			return false;
		}
		dwBufLen = dwReqBufLen/sizeof(WCHAR);
	}

	m_pIMeta->CloseKey(hCurrent);
	return true;
}
*/


BOOL CMetaOpIIS::AddVDir()
{
	//	����һ������Ŀ¼��
	STMETAOPERRECORD rec;
	WCHAR wszTemp[METAOPIIS_KEYNAME_LEN]		= {0};

	//	���Site_KeyName�Ƿ�Ϊ��
	if ( 0 == strlen(m_VDir.szSiteKeyName) )
	{
		setLastErrorInfo( 0, "վ�����Ϊ��" );
		return FALSE;
	}
	MultiByteToWideChar( CP_ACP, 0, m_VDir.szSiteKeyName, strlen(m_VDir.szSiteKeyName)+1, wszTemp, METAOPIIS_KEYNAME_LEN );
	wcscpy( rec.wszKeyPath, L"/LM/W3SVC/" );
	wcscat( rec.wszKeyPath, wszTemp );
	wcscat( rec.wszKeyPath, L"/Root" );

	memset( wszTemp, 0, sizeof(wszTemp) );
	MultiByteToWideChar( CP_ACP, 0, m_VDir.szKeyName_VirDir, strlen(m_VDir.szKeyName_VirDir)+1, wszTemp, METAOPIIS_KEYNAME_LEN );
	wcscpy(rec.wszKeyName, wszTemp);	

	if ( ! mb_createKey( & rec ) )
	{
		setLastErrorInfo( 0, "" );
		return FALSE;
	}

	//	�޸�����Ŀ¼·��
	WCHAR pwszBuf[MAX_PATH]		= {0};
	memset(pwszBuf, 0, MAX_PATH*2);
	
	MultiByteToWideChar(CP_ACP, 0, m_VDir.szPath_VirDir, strlen(m_VDir.szPath_VirDir)+1, pwszBuf, MAX_PATH);
	wcscat( rec.wszKeyPath, L"/" );
	wcscat( rec.wszKeyPath, wszTemp );
	if ( ! modifyRecordData( & rec, MD_VR_PATH, 0, IIS_MD_UT_FILE, STRING_METADATA, pwszBuf, (wcslen(pwszBuf)+1)*2 ) )
	{
		setLastErrorInfo( 0, "" );
		return FALSE;
	}
	
	//	�޸ķ�������
	memset(pwszBuf, 0, MAX_PATH*2);
	memcpy(pwszBuf, &m_VDir.dwAccessFlag, sizeof(DWORD));
	if ( ! modifyRecordData( & rec, MD_ACCESS_PERM, 0, IIS_MD_UT_FILE, DWORD_METADATA, pwszBuf, sizeof(DWORD) ) )
	{
		setLastErrorInfo( 0, "" );
		return FALSE;
	}
	
	return TRUE;
}

BOOL CMetaOpIIS::DeleteVDir()
{
	//	ɾ��һ������Ŀ¼��
	STMETAOPERRECORD rec;
	WCHAR wszTemp[ METAOPIIS_KEYNAME_LEN ]	= {0};

	MultiByteToWideChar( CP_ACP, 0, m_VDir.szSiteKeyName, strlen(m_VDir.szSiteKeyName)+1, wszTemp, METAOPIIS_KEYNAME_LEN );
	wcscpy( rec.wszKeyPath, L"/LM/W3SVC/" );
	wcscat( rec.wszKeyPath, wszTemp );
	wcscat( rec.wszKeyPath, L"/Root" );
	
	memset( wszTemp, 0, sizeof(wszTemp) );
	MultiByteToWideChar( CP_ACP, 0, m_VDir.szKeyName_VirDir, strlen(m_VDir.szKeyName_VirDir)+1, wszTemp, METAOPIIS_KEYNAME_LEN );
	wcscpy( rec.wszKeyName, wszTemp );	
	
	if ( ! mb_deleteKey( & rec ) )
	{
		setLastErrorInfo( 0, "" );
	}

	return TRUE;
}

BOOL CMetaOpIIS::ModifyVDir()
{
	//	����KEYPATH
	STMETAOPERRECORD rec;

	WCHAR wszTemp[ METAOPIIS_KEYNAME_LEN ]	= {0};
	WCHAR wszBuf[ MAX_PATH ]		= {0};

	MultiByteToWideChar( CP_ACP, 0, m_VDir.szSiteKeyName, strlen(m_VDir.szSiteKeyName)+1, wszTemp, METAOPIIS_KEYNAME_LEN );
	wcscpy( rec.wszKeyPath, L"/LM/W3SVC" );
	wcscat( rec.wszKeyPath, wszTemp );
	wcscat( rec.wszKeyPath, L"/Root" );
	
	memset( wszTemp, 0, sizeof(wszTemp) );
	MultiByteToWideChar( CP_ACP, 0, m_VDir.szKeyName_VirDir, strlen(m_VDir.szKeyName_VirDir)+1, wszTemp, METAOPIIS_KEYNAME_LEN );
	wcscpy( rec.wszKeyName, wszTemp );	

	//	�޸�����Ŀ¼·��
	if ( 0 != strlen(m_VDir.szPath_VirDir) )
	{
		MultiByteToWideChar(CP_ACP, 0, m_VDir.szPath_VirDir, strlen(m_VDir.szPath_VirDir)+1, wszBuf, MAX_PATH);
		wcscat( rec.wszKeyPath, wszTemp );
		if ( ! modifyRecordData( & rec, MD_VR_PATH, 0, IIS_MD_UT_FILE, STRING_METADATA, wszBuf, (wcslen(wszBuf)+1)*2 ) )
		{
			setLastErrorInfo( 0, "" );
			return FALSE;
		}
		
	}
	//	�޸ķ�������
	if ( 0 != m_VDir.dwAccessFlag ) 
	{
		memcpy( wszBuf, &m_VDir.dwAccessFlag, sizeof(DWORD) );
		if ( ! modifyRecordData( & rec, MD_ACCESS_PERM, 0, IIS_MD_UT_FILE, DWORD_METADATA, wszBuf, sizeof(DWORD) ) )
		{
			setLastErrorInfo( 0, "" );
			return FALSE;
		}
	}

	return TRUE;
}


BOOL CMetaOpIIS::HttpRedirect()
{
	//	���û�и�����Ŀ¼�Ӽ�������һ��
	BOOL bRet				= FALSE;
	STMETAOPERRECORD rec;
	WCHAR wszTemp[ METAOPIIS_KEYNAME_LEN ]	= {0};
	WCHAR wszBuf[ MAX_PATH ]		= {0};

	MultiByteToWideChar( CP_ACP, 0, m_VDir.szSiteKeyName, strlen(m_VDir.szSiteKeyName)+1, wszTemp, METAOPIIS_KEYNAME_LEN );
	wcscpy( rec.wszKeyPath, L"/LM/W3SVC/" );
	wcscat( rec.wszKeyPath, wszTemp );
	wcscat( rec.wszKeyPath, L"/Root" );

	memset( wszTemp, 0, sizeof(wszTemp) );
	MultiByteToWideChar( CP_ACP, 0, m_VDir.szKeyName_VirDir, strlen(m_VDir.szKeyName_VirDir)+1, wszTemp, METAOPIIS_KEYNAME_LEN );
	wcscpy( rec.wszKeyName, wszTemp );

	if ( mb_createKey( & rec ) )
	{
		//	�޸�http�ض�λ·��
		MultiByteToWideChar( CP_ACP, 0, m_VDir.szPath_VirDir, strlen(m_VDir.szPath_VirDir)+1, wszBuf, MAX_PATH );
		wcscat( rec.wszKeyPath, L"/" );
		wcscat( rec.wszKeyPath, rec.wszKeyName );
		bRet = modifyRecordData( & rec, MD_HTTP_REDIRECT, 0, IIS_MD_UT_FILE, STRING_METADATA, wszBuf, wcslen(wszBuf)*2+2 );
	}

	return bRet;
}


/**
 *	������������Ϣ
 */
VOID CMetaOpIIS::setLastErrorInfo( DWORD dwOperErrorCode, LPCTSTR lpszFmt, ... )
{
	INT nSize = 0;
	va_list args;

	m_dwLastSysErrorCode	= GetLastError();
	m_dwLastOperErrorCode	= dwOperErrorCode;

	memset( m_szLastErrorInfo, 0, sizeof(m_szLastErrorInfo) );
	va_start( args, lpszFmt );
	nSize = _vsnprintf( m_szLastErrorInfo, sizeof(m_szLastErrorInfo), lpszFmt, args );
	va_end( args );
}



