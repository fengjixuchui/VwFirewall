#include "StdAfx.h"
#include "DeErrorLog.h"

#include "shlwapi.h"
#pragma comment( lib, "shlwapi.lib" )



//////////////////////////////////////////////////////////////////////////
//
CDeErrorLog::CDeErrorLog()
{
	m_logfp	= NULL;
	memset( m_szLogDir, 0, sizeof(m_szLogDir) );
	memset( m_szLogFileName, 0, sizeof(m_szLogFileName) );

	m_dwWriteCount	= 0;
	m_bShareWrite	= TRUE;
	m_dwLogType	= CDEERRORLOG_LOGTYPE_TEXT;

	Init( NULL, NULL, TRUE );
}

CDeErrorLog::~CDeErrorLog()
{
	CloseLogFile();
}

/**
 *	@ Public
 *	��ʼ��
 */
VOID CDeErrorLog::Init( HINSTANCE hInstance, LPCTSTR lpcszModuleName, BOOL bShareWrite /* = TRUE */ )
{
	TCHAR szExePath[ MAX_PATH ]	= {0};
	TCHAR szDriver[ _MAX_DRIVE ]	= {0};
	TCHAR szPath[ _MAX_DIR ]	= {0};

	GetModuleFileName( hInstance, szExePath, sizeof(szExePath) );
	_splitpath( szExePath, szDriver, szPath, 0, 0 );
	_sntprintf( m_szModDir, sizeof(m_szModDir)-sizeof(TCHAR), _T("%s\\%s"), szDriver, szPath );
	_sntprintf( m_szLogDir, sizeof(m_szLogDir)-sizeof(TCHAR), _T("%s\\Logs\\"), m_szModDir );

	if ( ! PathIsDirectory(m_szLogDir) )
	{
		CreateDirectory( m_szLogDir, NULL );
	}

	if ( lpcszModuleName && _tcslen(lpcszModuleName) )
	{
		//	��������ģ�����Ƶ� LOG Ŀ¼
		_sntprintf( m_szModuleName, sizeof(m_szModuleName)-sizeof(TCHAR), _T("%s"), lpcszModuleName );
		_sntprintf( m_szLogDir, sizeof(m_szLogDir)-sizeof(TCHAR), _T("%s\\Logs\\%s\\"), m_szModDir, m_szModuleName );

		if ( ! PathIsDirectory(m_szLogDir) )
		{
			CreateDirectory( m_szLogDir, NULL );
		}
	}

	//	�Ƿ� ShareWrite
	m_bShareWrite = bShareWrite;
}

/**
 *	@ Public
 *	������־����
 */
VOID CDeErrorLog::SetLogType( DWORD dwLogType )
{
	if ( CDEERRORLOG_LOGTYPE_TEXT == dwLogType || 
		CDEERRORLOG_LOGTYPE_HTML == dwLogType )
	{
		m_dwLogType = dwLogType;
	}
}


/**
 *	@ Public
 *	д��־
 */
BOOL CDeErrorLog::WriteErrorLog( DWORD dwErrorCode, LPCTSTR lpszErrorDesc )
{
	if ( NULL == lpszErrorDesc )
	{
		return FALSE;
	}

	TCHAR * lpTemp = NULL;
	TCHAR szErrInfo[ MAX_PATH ];
	TCHAR szErrorTime[ 32 ];
	LPCTSTR lpcszErrInfo	= NULL;

	TCHAR szCurrLogFileName[ MAX_PATH ]	= {0};
	HANDLE hFile	= NULL;
	DWORD dwDummy	= 0;
	

	if ( ! GetLogErrorTime( szErrorTime, sizeof(szErrorTime) ) )
	{
		return FALSE;
	}

	//	д������Ϣ����־�ļ�
	__try
	{
		if ( CDEERRORLOG_LOGTYPE_TEXT == m_dwLogType )
		{
			if ( 0 != dwErrorCode )
			{
				FormatMessage
				(
					FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM,
					NULL,
					dwErrorCode,
					LANG_NEUTRAL,
					(LPTSTR)&lpTemp,
					0,
					NULL
				);
				//	��ȡϵͳ������Ϣ�����Ҹ�ʽ�����
				_sntprintf( szErrInfo, sizeof(szErrInfo)-sizeof(TCHAR), _T("[%s]\t%s: %s"), szErrorTime, lpszErrorDesc, lpTemp );
			}
			else
			{
				_sntprintf( szErrInfo, sizeof(szErrInfo)-sizeof(TCHAR), _T("[%s]\t%s\r\n"), szErrorTime, lpszErrorDesc );
			}

			lpcszErrInfo = szErrInfo;
		}
		else if ( CDEERRORLOG_LOGTYPE_HTML == m_dwLogType )
		{
			lpcszErrInfo = lpszErrorDesc;
		}

		if ( m_bShareWrite )
		{
			if ( GetLogFileName( szCurrLogFileName, sizeof(szCurrLogFileName) ) )
			{
				hFile = CreateFile( szCurrLogFileName, GENERIC_WRITE, FILE_SHARE_WRITE, NULL, OPEN_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL );
				if ( INVALID_HANDLE_VALUE != hFile )
				{
					//	�ƶ��ļ�ָ�뵽�ļ�β��
					SetFilePointer( hFile, 0, 0, FILE_END );

					//	��������Ϣ��ӵ��ļ�
					WriteFile( hFile, lpcszErrInfo, _tcslen(lpcszErrInfo), &dwDummy, NULL );
					
					//	��ӻ���
					//	WriteFile( hFile, _T("\r\n"), 2, &dwDummy, NULL );
					
					CloseHandle( hFile );
				}
			}
		}
		else
		{
			if ( OpenLogFile() )
			{
				if ( m_logfp )
				{
					fprintf( m_logfp, _T("%s"), lpcszErrInfo );

					// ..
					//	if ( m_dwWriteCount >= 10 )
					//{
					//	m_dwWriteCount = 0;
					//	fflush( m_logfp );
					//}
					//m_dwWriteCount ++;
				}
			}
		}
	}
	__except( EXCEPTION_EXECUTE_HANDLER )
	{
		return FALSE;
	}

	return TRUE;
}

/**
 *	@ Public
 *	��ʽ��д��־
 */
BOOL CDeErrorLog::WriteErrorLogEx( DWORD dwErrorCode, LPCTSTR lpszFmt, ... )
{
	BOOL  bRet			= FALSE;
	INT   nSize			= 0;
	TCHAR szErrorDesc[ MAX_PATH ]	= {0};;
	va_list args;
	
	__try
	{
		va_start( args, lpszFmt );
		nSize = _vsnprintf( szErrorDesc, sizeof(szErrorDesc)-sizeof(TCHAR), lpszFmt, args );
		va_end( args );
		// ..
		bRet = WriteErrorLog( dwErrorCode, szErrorDesc );
	}
	__except( EXCEPTION_EXECUTE_HANDLER )
	{
		bRet = FALSE;
	}

	return bRet;
}




//////////////////////////////////////////////////////////////////////////
//	Private




BOOL CDeErrorLog::OpenLogFile()
{
	BOOL bRet = FALSE;
	TCHAR szCurrLogFileName[ MAX_PATH ]	= {0};

	if ( GetLogFileName( szCurrLogFileName, sizeof(szCurrLogFileName) ) )
	{
		if ( 0 != _tcsicmp( szCurrLogFileName, m_szLogFileName ) )
		{
			//	�ر��ļ����
			CloseLogFile();

			_sntprintf( m_szLogFileName, sizeof(m_szLogFileName)-sizeof(TCHAR), _T("%s"), szCurrLogFileName );
			m_logfp = fopen( m_szLogFileName, _T("a") );
		}

		bRet = m_logfp ? TRUE : FALSE;
	}

	return bRet;
}

BOOL CDeErrorLog::CloseLogFile()
{
	if ( m_bShareWrite )
	{
		return FALSE;
	}

	if ( m_logfp )
	{
		fclose( m_logfp );
		m_logfp = NULL;
	}

	return TRUE;
}

BOOL CDeErrorLog::GetLogFileName( LPTSTR lpszLogFileName, DWORD dwSize )
{
	if ( NULL == lpszLogFileName )
	{
		return FALSE;
	}

	if ( 0 == _tcslen(m_szLogDir) )
	{
		return FALSE;
	}
	else
	{
		if ( FALSE == PathIsDirectory(m_szLogDir) )
		{
			CreateDirectory( m_szLogDir, NULL );
		}
	}

	SYSTEMTIME sysTime;
	GetLocalTime( &sysTime );

	_sntprintf
	(
		lpszLogFileName, dwSize-sizeof(TCHAR),
		_T("%s%d-%d-%d.%s"),
		m_szLogDir,
		sysTime.wYear, sysTime.wMonth, sysTime.wDay,
		( CDEERRORLOG_LOGTYPE_HTML == m_dwLogType ? "html" : "log" )
	);
	
	return TRUE;
}

BOOL CDeErrorLog::GetLogErrorTime( LPTSTR lpszErrorTime, DWORD dwSize )
{
	if ( NULL == lpszErrorTime )
	{
		return FALSE;
	}
	
	SYSTEMTIME sysTime;
	GetLocalTime( &sysTime );
	
	_sntprintf
	(
		lpszErrorTime, dwSize-sizeof(TCHAR), _T("%d-%02d-%02d %02d:%02d:%02d"),
		sysTime.wYear, sysTime.wMonth, sysTime.wDay, sysTime.wHour, sysTime.wMinute, sysTime.wSecond
	);

	return TRUE;
}