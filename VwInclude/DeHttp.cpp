// http.cpp : Defines the entry point for the DLL application.
//

#include "stdafx.h"
#include "DeHttp.h"
#include "BindStatusCallback.h"
#include <stdio.h>
#include <stdlib.h>


#include "shlwapi.h"
#pragma comment( lib, "shlwapi.lib" )

#include "Urlmon.h"
#pragma comment( lib, "Urlmon.lib" )

//#include "DeLib.h"
//#pragma comment( lib, "DeLib.lib" )




//////////////////////////////////////////////////////////////////////////
//
// �ж��Ƿ����Internet����
BOOL CDeHttp::HaveInternetConnection()
{
	BOOL  bRet		= FALSE;
	DWORD dwFlag		= 0;

	__try
	{
		//	�����������FALSE����϶�û������
		bRet = InternetGetConnectedState( &dwFlag, 0 );

		//	���OFFLINE����λ���������û�����ӣ�Ҳ������ʵ���������Ӵ���
		//	Ϊ�˾����ܲ������û�������ʹ�ã�Ҳ��Ϊ��û������
		//if ( dwFlag & INTERNET_CONNECTION_OFFLINE )
		//	return FALSE;

		//	��������������һ�����ӣ�����Ϊ��������
		//	��ΪINTERNET_CONNECTION_CONFIGURED����λҲ������û������
		//if ( ( dwFlag & INTERNET_CONNECTION_LAN ) ||
		//	( dwFlag & INTERNET_CONNECTION_MODEM ) ||
		//	( dwFlag & INTERNET_CONNECTION_PROXY ) )
		//{
		//	return TRUE;
		//}
	}
	__except( EXCEPTION_EXECUTE_HANDLER )
	{
		bRet = FALSE;
	}

	return bRet;
}


//////////////////////////////////////////////////////////////////////////
// ��ȡ URL �Լ��Է���������Ϣ
BOOL CDeHttp::GetHttpInfo( LPCTSTR lpcszUrl, STHTTPINFO & stHttp, LPTSTR lpszError, DWORD dwTimeout )
{
	if ( FALSE == HaveInternetConnection() )
	{
		if ( lpszError )
			_tcscpy( lpszError, _T("��ǰû����������") );
		return FALSE;
	}

	STHTTPTHREAD stHttpThread;

	stHttpThread.pvThisCls		= this;
	stHttpThread.bRet		= FALSE;
	stHttpThread.pszUrl		= lpcszUrl;
	stHttpThread.stHttpInfo		= &stHttp;
	stHttpThread.pszError		= lpszError;

	__try
	{
		stHttpThread.hThread = (HANDLE)_beginthreadex
			(
				NULL,
				0,
				(PBEGINTHREADEX_THREADFUNC)GetHttpInfoProc,
				&stHttpThread,
				0,
				(PBEGINTHREADEX_THREADID)&stHttpThread.uThreadID
			);
		if ( NULL == stHttpThread.hThread )
		{
			return FALSE;
		}

		if ( WAIT_OBJECT_0 != WaitForSingleObject( stHttpThread.hThread, dwTimeout ) )
		{
			TerminateThread( stHttpThread.hThread, 0 );
			while ( WAIT_TIMEOUT == WaitForSingleObject( stHttpThread.hThread, 1000 ) )
			{
				TerminateThread( stHttpThread.hThread, 0 );
			}

			if ( lpszError )
				_tcscpy( lpszError, _T("�������ӳ�ʱ") );
		}
		if ( stHttpThread.hThread )
		{
			CloseHandle( stHttpThread.hThread );
		}
	}
	__except( EXCEPTION_EXECUTE_HANDLER )
	{
		if ( lpszError )
		{
			_tcscpy( lpszError, _T("�߳��쳣�����˳�") );
		}
	}

	return stHttpThread.bRet;
}

DWORD WINAPI CDeHttp::GetHttpInfoProc( LPVOID p )
{
	STHTTPTHREAD * pstThread = (STHTTPTHREAD*)p;
	pstThread->bRet = FALSE;

	if ( NULL == pstThread->pszUrl )
	{
		if ( pstThread->pszError )
			_tcscpy( pstThread->pszError, _T("���������ַ����") );
		return HTTP_THREAD_FAIL;
	}
	if ( 0 == _tcslen(pstThread->pszUrl) )
	{
		if ( pstThread->pszError )
			_tcscpy( pstThread->pszError, _T("�������Ϊ��") );
		return HTTP_THREAD_FAIL;
	}

	
	HINTERNET hSession	= NULL;
	HINTERNET hHttpFile	= NULL;
	BOOL  bQuerySucc	= FALSE;
	TCHAR szQueryBuf[ 128 ]	= {0};
	DWORD dwQueryBufLen	= sizeof(szQueryBuf);

	__try
	{
		hSession = ::InternetOpen( DEFAULT_AGENT_NAME, PRE_CONFIG_INTERNET_ACCESS, _T(""), INTERNET_INVALID_PORT_NUMBER, 0 );
		if ( hSession )
		{
			hHttpFile = ::InternetOpenUrl( hSession, pstThread->pszUrl, NULL, 0, INTERNET_FLAG_EXISTING_CONNECT, 0 );
			if ( hHttpFile )
			{
				memset( szQueryBuf, 0, sizeof(szQueryBuf) );
				dwQueryBufLen = sizeof(szQueryBuf);
				bQuerySucc = ::HttpQueryInfo ( hHttpFile, HTTP_QUERY_CONTENT_LENGTH, szQueryBuf, &dwQueryBufLen, NULL );
				if ( bQuerySucc && _tcslen(szQueryBuf) > 0 )
				{
					pstThread->stHttpInfo->dwHttpFileLen = (DWORD)_ttol(szQueryBuf);
				}

				memset( szQueryBuf, 0, sizeof(szQueryBuf) );
				dwQueryBufLen = sizeof(szQueryBuf);
				bQuerySucc = ::HttpQueryInfo ( hHttpFile, HTTP_QUERY_STATUS_CODE, szQueryBuf, &dwQueryBufLen, NULL );
				if ( bQuerySucc && _tcslen(szQueryBuf) > 0 )
				{
					pstThread->stHttpInfo->dwHttpStatusCode = (DWORD)_ttol(szQueryBuf);
				}
				
				memset( szQueryBuf, 0, sizeof(szQueryBuf) );
				dwQueryBufLen = sizeof(szQueryBuf);
				bQuerySucc = ::HttpQueryInfo ( hHttpFile, HTTP_QUERY_LAST_MODIFIED, szQueryBuf, &dwQueryBufLen, NULL );
				if ( bQuerySucc && _tcslen(szQueryBuf) > 0 )
				{
					_sntprintf
					( 
						pstThread->stHttpInfo->szHttpLastModified, 
						sizeof(pstThread->stHttpInfo->szHttpLastModified)/sizeof(TCHAR)-1, 
						_T("%s"), szQueryBuf
					);
				}

				memset( szQueryBuf, 0, sizeof(szQueryBuf) );
				dwQueryBufLen = sizeof(szQueryBuf);
				bQuerySucc = ::HttpQueryInfo ( hHttpFile, HTTP_QUERY_SERVER, szQueryBuf, &dwQueryBufLen, NULL );
				if ( bQuerySucc && _tcslen(szQueryBuf) > 0 )
				{
					_sntprintf
					( 
						pstThread->stHttpInfo->szHttpServerInfo,
						sizeof(pstThread->stHttpInfo->szHttpServerInfo)/sizeof(TCHAR)-1,
						_T("%s"), szQueryBuf
					);
				}

				pstThread->bRet = TRUE;

				::InternetCloseHandle( hHttpFile );
			}

			::InternetCloseHandle( hSession );
		}
	}
	__except( EXCEPTION_EXECUTE_HANDLER )
	{
		if ( pstThread->pszError )
			_tcscpy( pstThread->pszError, _T("�߳��쳣�����˳�") );
		if ( hHttpFile )
			::InternetCloseHandle( hHttpFile );
		if ( hSession )
			::InternetCloseHandle( hSession );
	}

	return ( TRUE == pstThread->bRet ) ? HTTP_THREAD_SUCC : HTTP_THREAD_FAIL;
}



//////////////////////////////////////////////////////////////////////////
// �ж�ĳ�� URL �Ƿ���������
BOOL CDeHttp::IsUrlConnectOk( LPCTSTR lpcszUrl, LPTSTR lpszError, DWORD dwTimeout )
{
	if ( NULL == lpcszUrl )
	{
		return FALSE;
	}
	else if ( 0 == _tcslen(lpcszUrl) )
	{
		return FALSE;
	}

	if ( FALSE == HaveInternetConnection() )
	{
		if ( lpszError )
			_tcscpy( lpszError, _T("��ǰû����������") );
		return FALSE;
	}

	// ��ȡ״̬��
	DWORD dwStatusCode = GetStatusCode( lpcszUrl, lpszError, dwTimeout );
	if ( HTTP_STATUS_OK == dwStatusCode )
	{
		return TRUE;
	}
	else
	{
		if ( lpszError )
			_sntprintf( lpszError, MAX_PATH-1, _T("%d"), dwStatusCode );
		return FALSE;
	}

}


//////////////////////////////////////////////////////////////////////////
// ��ȡĳ�� URL ������״̬����
DWORD CDeHttp::GetStatusCode( LPCTSTR lpcszUrl, LPTSTR lpszError, DWORD dwTimeout )
{
	if ( FALSE == HaveInternetConnection() )
	{
		if ( lpszError )
			_tcscpy( lpszError, _T("��ǰû����������") );
		return FALSE;
	}

	STHTTPTHREAD stHttpThread;

	stHttpThread.pvThisCls	= this;
	stHttpThread.dwRet	= GETSTATUSCODE_ERROR;
	stHttpThread.pszUrl	= lpcszUrl;
	stHttpThread.pszError	= lpszError;

	__try
	{
		stHttpThread.hThread = (HANDLE)_beginthreadex
			(
				NULL,
				0,
				(PBEGINTHREADEX_THREADFUNC)GetStatusCodeProc,
				&stHttpThread,
				0,
				(PBEGINTHREADEX_THREADID)&stHttpThread.uThreadID
			);
		if ( NULL == stHttpThread.hThread )
			return FALSE;
		
		if ( WAIT_OBJECT_0 != WaitForSingleObject( stHttpThread.hThread, dwTimeout ) )
		{
			TerminateThread( stHttpThread.hThread, 0 );
			while ( WAIT_TIMEOUT == WaitForSingleObject( stHttpThread.hThread, 1000 ) )
			{
				TerminateThread( stHttpThread.hThread, 0 );
			}

			stHttpThread.dwRet = GETSTATUSCODE_ERROR;
			if ( lpszError )
				_tcscpy( lpszError, _T("�������ӳ�ʱ") );
		}
		if ( stHttpThread.hThread )
			CloseHandle( stHttpThread.hThread );
	}
	__except( EXCEPTION_EXECUTE_HANDLER )
	{
		if ( lpszError )
		{
			_tcscpy( lpszError, _T("�߳��쳣�����˳�") );
		}
	}

	return stHttpThread.dwRet;
}
DWORD WINAPI CDeHttp::GetStatusCodeProc( LPVOID p )
{
	STHTTPTHREAD * pstThread = (STHTTPTHREAD*)p;

	if ( NULL == pstThread->pszUrl )
	{
		if ( pstThread->pszError )
			_tcscpy( pstThread->pszError, _T("���������ַ����") );
		return GETSTATUSCODE_ERROR;
	}
	if ( 0 == _tcslen(pstThread->pszUrl) )
	{
		if ( pstThread->pszError )
			_tcscpy( pstThread->pszError, _T("�������Ϊ��") );
		return GETSTATUSCODE_ERROR;
	}


	HINTERNET hSession	= NULL;
	HINTERNET hHttpFile	= NULL;
	TCHAR szQueryBuf[ 32 ]	= {0};
	DWORD dwQueryBufLen	= 0;
	BOOL  bQuerySucc	= FALSE;

	pstThread->dwRet = GETSTATUSCODE_ERROR;
	__try
	{
		hSession = ::InternetOpen( DEFAULT_AGENT_NAME, PRE_CONFIG_INTERNET_ACCESS, _T(""), INTERNET_INVALID_PORT_NUMBER, 0 );
		if ( hSession )
		{
			hHttpFile = ::InternetOpenUrl( hSession, pstThread->pszUrl, NULL, 0, INTERNET_FLAG_EXISTING_CONNECT, 0 );
			if ( hHttpFile )
			{
				//	�õ�״̬��
				dwQueryBufLen	= sizeof(szQueryBuf);
				bQuerySucc	= ::HttpQueryInfo
					(
						hHttpFile,
						HTTP_QUERY_STATUS_CODE,
						szQueryBuf,
						&dwQueryBufLen,
						NULL
					);
				if ( bQuerySucc && _tcslen(szQueryBuf) > 0 )
				{
					pstThread->dwRet = (DWORD)_ttol(szQueryBuf);
				}

				::InternetCloseHandle( hHttpFile );
			}

			::InternetCloseHandle( hSession );
		}
	}
	__except( EXCEPTION_EXECUTE_HANDLER )
	{
		if ( pstThread->pszError )
		{
			_tcscpy( pstThread->pszError, _T("�߳��쳣�����˳�") );
		}
		if ( hHttpFile )
		{
			::InternetCloseHandle( hHttpFile );
		}
		if ( hSession )
		{
			::InternetCloseHandle( hSession );
		}
	}

	return pstThread->dwRet;
}

/**
 *	��ȡĳ�� URL ������״̬����
 */
DWORD CDeHttp::GetStatusCodeEx( LPCTSTR lpcszUrl, LPCTSTR lpszAgent, vector<STHTTPCOOKIE> * pvcCookie, LPTSTR lpszError, DWORD dwTimeout )
{
	//
	//	pszUrl		- [in]  URL
	//	pvcCookie	- [in]  Cookie
	//	pszError	- [out] ������Ϣ
	//	dwTimeout	- [in]  ��ʱ����λ����
	//	
	//	RETURN		- HTTP STATUS CODE
	//

	if ( FALSE == HaveInternetConnection() )
	{
		if ( lpszError )
			_tcscpy( lpszError, _T("��ǰû����������") );
		return FALSE;
	}
	
	STHTTPTHREAD stHttpThread;
	
	stHttpThread.pvThisCls	= this;
	stHttpThread.dwRet	= GETSTATUSCODE_ERROR;
	stHttpThread.pszUrl	= lpcszUrl;
	stHttpThread.pszAgent	= lpszAgent;
	stHttpThread.pvcCookie	= pvcCookie;
	stHttpThread.pszError	= lpszError;
	stHttpThread.dwTimeout	= dwTimeout;

	__try
	{
		stHttpThread.hThread = (HANDLE)_beginthreadex
			(
				NULL,
				0,
				(PBEGINTHREADEX_THREADFUNC)GetStatusCodeExProc,
				&stHttpThread,
				0,
				(PBEGINTHREADEX_THREADID)&stHttpThread.uThreadID
			);
		if ( stHttpThread.hThread )
		{
			if ( WAIT_OBJECT_0 != WaitForSingleObject( stHttpThread.hThread, dwTimeout ) )
			{
				TerminateThread( stHttpThread.hThread, 0 );
				while ( WAIT_TIMEOUT == WaitForSingleObject( stHttpThread.hThread, 1000 ) )
				{
					TerminateThread( stHttpThread.hThread, 0 );
				}

				stHttpThread.dwRet = GETSTATUSCODE_ERROR;
				if ( lpszError )
				{
					_tcscpy( lpszError, _T("�������ӳ�ʱ") );
				}
			}

			if ( stHttpThread.hThread )
			{
				CloseHandle( stHttpThread.hThread );
			}
		}
	}
	__except( EXCEPTION_EXECUTE_HANDLER )
	{
		if ( lpszError )
		{
			_tcscpy( lpszError, _T("�߳��쳣�����˳�") );
		}
	}
	
	return stHttpThread.dwRet;
}
DWORD WINAPI CDeHttp::GetStatusCodeExProc( LPVOID p )
{
	STHTTPTHREAD * pstThread = (STHTTPTHREAD*)p;

	if ( NULL == pstThread->pszUrl )
	{
		if ( pstThread->pszError )
		{
			_tcscpy( pstThread->pszError, _T("���������ַ����") );
		}
		return GETSTATUSCODE_ERROR;
	}
	if ( 0 == _tcslen(pstThread->pszUrl) )
	{
		if ( pstThread->pszError )
		{
			_tcscpy( pstThread->pszError, _T("�������Ϊ��") );
		}
		return GETSTATUSCODE_ERROR;
	}


	HINTERNET hSession		= NULL;
	HINTERNET hHttpFile		= NULL;
	TCHAR szQueryBuf[ 4096 ]	= {0};
	DWORD dwQueryBufLen		= 0;
	BOOL  bQuerySucc		= FALSE;
	BOOL  bFindDateTag		= FALSE;
	vector<STHTTPCOOKIE>::iterator	it;
	//STHTTPCOOKIE * pstHttpCookie;
	TCHAR szAgent[ MAX_PATH ]	= {0};
	TCHAR szTemp[ MAX_PATH ]	= {0};


	pstThread->dwRet = GETSTATUSCODE_ERROR;

	try
	{
		//
		//	���� Cookie
		//
		if ( pstThread->pvcCookie )
		{
			for ( it = pstThread->pvcCookie->begin(); it != pstThread->pvcCookie->end(); it ++ )
			{
				//pstHttpCookie = it;
				InternetSetCookie( pstThread->pszUrl, (*it).szName, (*it).szData );
			}
		}

		if ( pstThread->pszAgent && _tcslen(pstThread->pszAgent) > 0 )
		{
			_sntprintf( szAgent, sizeof(szAgent)/sizeof(TCHAR)-1, _T("%s"), pstThread->pszAgent );
		}
		else
		{
			_sntprintf( szAgent, sizeof(szAgent)/sizeof(TCHAR)-1, _T("%s"), DEFAULT_AGENT_NAME );
		}

		hSession = ::InternetOpen( szAgent, INTERNET_OPEN_TYPE_DIRECT, NULL, NULL, NULL );
		if ( hSession )
		{
			hHttpFile = ::InternetOpenUrl( hSession, pstThread->pszUrl, NULL, 0, INTERNET_FLAG_EXISTING_CONNECT, 0 );
			if ( hHttpFile )
			{
				//	ȷ�� HTTP ͷ���� Date: ��Ϣ
				memset( szQueryBuf, 0, sizeof(szQueryBuf) );
				dwQueryBufLen	= sizeof(szQueryBuf);
				bQuerySucc	= ::HttpQueryInfo
					(
						hHttpFile,
						HTTP_QUERY_RAW_HEADERS_CRLF,
						szQueryBuf,
						&dwQueryBufLen,
						NULL
					);

				DWORD dwLastError = GetLastError();
	
				if ( bQuerySucc && _tcslen(szQueryBuf) > 0 )
				{
					//
					//	����Ƿ��� Date: ���
					//	��Ҫ��Ϊ�˼�� HTTP ͷ�Ƿ�����
					//
					if ( _tcsstr( szQueryBuf, _T("Date:") ) )
					{
						bFindDateTag	= TRUE;
					}

					//	�õ�״̬��
					memset( szQueryBuf, 0, sizeof(szQueryBuf) );
					dwQueryBufLen	= sizeof(szQueryBuf);
					bQuerySucc	= ::HttpQueryInfo
						(
							hHttpFile,
							HTTP_QUERY_STATUS_CODE,		//HTTP_QUERY_RAW_HEADERS_CRLF
							szQueryBuf,
							&dwQueryBufLen,
							NULL
						);
					if ( bQuerySucc && _tcslen(szQueryBuf) > 0 )
					{
						pstThread->dwRet = (DWORD)_ttol(szQueryBuf);
					}
				}

				if ( ! bFindDateTag && 200 == pstThread->dwRet )
				{
					//
					//	HTTP ͷ�����������Ϊ����
					//
					pstThread->dwRet = GETSTATUSCODE_ERROR;
				}

				::InternetCloseHandle( hHttpFile );
			}

			::InternetCloseHandle( hSession );
		}
	}
	catch( ... )
	{
		if ( pstThread->pszError )
		{
			_tcscpy( pstThread->pszError, _T("�߳��쳣�����˳�") );
		}
		if ( hHttpFile )
		{
			::InternetCloseHandle( hHttpFile );
		}
		if ( hSession )
		{
			::InternetCloseHandle( hSession );
		}
	}

	return pstThread->dwRet;
}






/**
 *	��ȡ���� Header
 */
BOOL CDeHttp::GetHttpHeader( LPCTSTR lpcszUrl, LPTSTR lpszHeader, DWORD dwSize, LPTSTR lpszError, DWORD dwTimeout )
{
	/*
		lpcszUrl	- [in]  �����ʵ� URL
		lpszHeader	- [out] ���ص����� HTTP ͷ
		dwSize		- [in]  ���ػ�������С
		lpszError	- [out] �������ֵĴ�����Ϣ
		dwTimeout	- [in]  ��ʱʱ��
		RETURN		- TRUE / FALSE
	*/

	if ( FALSE == HaveInternetConnection() )
	{
		if ( lpszError )
			_tcscpy( lpszError, _T("��ǰû����������") );
		return FALSE;
	}

	STHTTPTHREAD stHttpThread;

	stHttpThread.pvThisCls		= this;
	stHttpThread.bRet		= FALSE;
	stHttpThread.pszUrl		= lpcszUrl;
	stHttpThread.pszHeader		= lpszHeader;
	stHttpThread.dwSize		= dwSize;
	stHttpThread.pszError		= lpszError;
	
	__try
	{
		stHttpThread.hThread = (HANDLE)_beginthreadex
			(
				NULL,
				0,
				(PBEGINTHREADEX_THREADFUNC)GetHttpHeaderProc,
				&stHttpThread,
				0,
				(PBEGINTHREADEX_THREADID)&stHttpThread.uThreadID
			);
		if ( NULL == stHttpThread.hThread )
		{
			return FALSE;
		}

		if ( WAIT_OBJECT_0 != WaitForSingleObject( stHttpThread.hThread, dwTimeout ) )
		{
			TerminateThread( stHttpThread.hThread, 0 );
			while ( WAIT_TIMEOUT == WaitForSingleObject( stHttpThread.hThread, 1000 ) )
			{
				TerminateThread( stHttpThread.hThread, 0 );
			}

			if ( lpszError )
			{
				_tcscpy( lpszError, _T("�������ӳ�ʱ") );
			}
		}
		if ( stHttpThread.hThread )
		{
			CloseHandle( stHttpThread.hThread );
		}
	}
	__except( EXCEPTION_EXECUTE_HANDLER )
	{
		if ( lpszError )
		{
			_tcscpy( lpszError, _T("�߳��쳣�����˳�") );
		}
	}

	return stHttpThread.bRet;
}
DWORD WINAPI CDeHttp::GetHttpHeaderProc( LPVOID p )
{
	STHTTPTHREAD * pstThread = (STHTTPTHREAD*)p;
	
	if ( NULL == pstThread->pszUrl )
	{
		if ( pstThread->pszError )
			_tcscpy( pstThread->pszError, _T("���������ַ����") );
		return HTTP_THREAD_FAIL;
	}
	if ( 0 == _tcslen(pstThread->pszUrl) )
	{
		if ( pstThread->pszError )
			_tcscpy( pstThread->pszError, _T("�������Ϊ��") );
		return HTTP_THREAD_FAIL;
	}
	if ( NULL == pstThread->pszHeader || 0 == pstThread->dwSize )
	{
		if ( pstThread->pszError )
			_tcscpy( pstThread->pszError, _T("Header ���ܲ�������ȷ") );
		return HTTP_THREAD_FAIL;
	}

	HINTERNET hSession	= NULL;
	HINTERNET hHttpFile	= NULL;
	BOOL  bQuerySucc	= FALSE;

	pstThread->bRet = FALSE;
	__try
	{
		hSession = ::InternetOpen( DEFAULT_AGENT_NAME, PRE_CONFIG_INTERNET_ACCESS, _T(""), INTERNET_INVALID_PORT_NUMBER, 0 );
		if ( hSession )
		{
			hHttpFile = ::InternetOpenUrl( hSession, pstThread->pszUrl, NULL, 0, INTERNET_FLAG_EXISTING_CONNECT, 0 );
			if ( hHttpFile )
			{
				// ��ȡ�� CRLF Ϊ�ָ�� Header
				bQuerySucc = ::HttpQueryInfo
					(
						hHttpFile,
						HTTP_QUERY_RAW_HEADERS_CRLF,
						pstThread->pszHeader,
						&pstThread->dwSize,
						NULL
					);
				if ( bQuerySucc && _tcslen(pstThread->pszHeader) > 0 )
				{
					pstThread->bRet = TRUE;
				}
				
				::InternetCloseHandle( hHttpFile );
			}
			
			::InternetCloseHandle( hSession );
		}
	}
	__except( EXCEPTION_EXECUTE_HANDLER )
	{
		if ( pstThread->pszError )
			_tcscpy( pstThread->pszError, _T("�߳��쳣�����˳�") );
		if ( hHttpFile )
			::InternetCloseHandle( hHttpFile );
		if ( hSession )
			::InternetCloseHandle( hSession );
	}
	
	return ( TRUE == pstThread->bRet ) ? HTTP_THREAD_SUCC : HTTP_THREAD_FAIL;
}

/**
 *	��ȡ���� Header
 */
BOOL CDeHttp::GetHttpHeaderEx(
	LPCTSTR lpcszUrl, vector<STHTTPHEADERVAR> & vcHttpHeaderVar,
	LPTSTR lpszHeader, DWORD dwSize, LPTSTR lpszError, DWORD dwTimeout )
{
	/*
		lpcszUrl	- [in]  �����ʵ� URL
		vcHttpHeaderVar	- [in]  HTTP ͷ����ֵ
		lpszHeader	- [out] ���ص����� HTTP ͷ
		dwSize		- [in]  ���ػ�������С
		lpszError	- [out] �������ֵĴ�����Ϣ
		dwTimeout	- [in]  ��ʱʱ��
		RETURN		- TRUE / FALSE
	*/

	if ( FALSE == HaveInternetConnection() )
	{
		if ( lpszError )
			_tcscpy( lpszError, _T("��ǰû����������") );
		return FALSE;
	}

	STHTTPTHREAD stHttpThread;

	stHttpThread.pvThisCls		= this;
	stHttpThread.bRet		= FALSE;
	stHttpThread.pszUrl		= lpcszUrl;
	stHttpThread.pvcHttpHeaderVar	= & vcHttpHeaderVar;
	stHttpThread.pszHeader		= lpszHeader;
	stHttpThread.dwSize		= dwSize;
	stHttpThread.pszError		= lpszError;

	__try
	{
		stHttpThread.hThread = (HANDLE)_beginthreadex
			(
				NULL,
				0,
				(PBEGINTHREADEX_THREADFUNC)GetHttpHeaderExProc,
				&stHttpThread,
				0,
				(PBEGINTHREADEX_THREADID)&stHttpThread.uThreadID
			);
		if ( NULL == stHttpThread.hThread )
		{
			return FALSE;
		}

		if ( WAIT_OBJECT_0 != WaitForSingleObject( stHttpThread.hThread, dwTimeout ) )
		{
			TerminateThread( stHttpThread.hThread, 0 );
			while ( WAIT_TIMEOUT == WaitForSingleObject( stHttpThread.hThread, 1000 ) )
			{
				TerminateThread( stHttpThread.hThread, 0 );
			}

			if ( lpszError )
			{
				_tcscpy( lpszError, _T("�������ӳ�ʱ") );
			}
		}
		if ( stHttpThread.hThread )
		{
			CloseHandle( stHttpThread.hThread );
		}
	}
	__except( EXCEPTION_EXECUTE_HANDLER )
	{
		if ( lpszError )
		{
			_tcscpy( lpszError, _T("�߳��쳣�����˳�") );
		}
	}

	return stHttpThread.bRet;
}
DWORD WINAPI CDeHttp::GetHttpHeaderExProc( LPVOID p )
{
	STHTTPTHREAD * pstThread = (STHTTPTHREAD*)p;
	CDeHttp * pCHttp = (CDeHttp*)pstThread->pvThisCls;

	if ( NULL == pCHttp )
	{
		if ( pstThread->pszError )
			_tcscpy( pstThread->pszError, _T("�ڲ������޷��ҵ����ַ") );
		return HTTP_THREAD_FAIL;
	}
	if ( NULL == pstThread->pszUrl )
	{
		if ( pstThread->pszError )
			_tcscpy( pstThread->pszError, _T("���������ַ����") );
		return HTTP_THREAD_FAIL;
	}
	if ( 0 == _tcslen(pstThread->pszUrl) )
	{
		if ( pstThread->pszError )
			_tcscpy( pstThread->pszError, _T("�������Ϊ��") );
		return HTTP_THREAD_FAIL;
	}
	if ( NULL == pstThread->pszHeader || 0 == pstThread->dwSize )
	{
		if ( pstThread->pszError )
			_tcscpy( pstThread->pszError, _T("Header ���ܲ�������ȷ") );
		return HTTP_THREAD_FAIL;
	}

	const TCHAR * szAcceptType = HTTP_ACCEPT_TYPE;
	HINTERNET hSession	= NULL;
	//HINTERNET hHttpFile	= NULL;
	HINTERNET hConnect	= NULL;
	HINTERNET hRequest	= NULL;
	DWORD dwFlags		= 0;
	BOOL bAddHeaders	= FALSE;
	BOOL bSendRequest	= FALSE;
	DWORD dwBytesRead	= 0;
	BOOL  bQuerySucc	= FALSE;
	vector<STHTTPHEADERVAR> vcHttpHeaderVar;
	vector<STHTTPHEADERVAR>::iterator it;
	TCHAR szServer[MAX_PATH]	= {0};
	TCHAR szPath[ 1024 ]		= {0};
	TCHAR szHeadLine[MAX_PATH]	= {0};
#ifdef UNICODE
	STDEHTTPDEPARSEURLW stParseUrl;
#else
	STDEHTTPDEPARSEURL stParseUrl;
#endif

	if ( pCHttp->dehttp_delib_parse_url( pstThread->pszUrl, (DWORD)_tcslen(pstThread->pszUrl), & stParseUrl ) )
	{
		_sntprintf( szServer, sizeof(szServer)/sizeof(TCHAR)-1, _T("%s"), stParseUrl.szHost );
		_sntprintf( szPath, sizeof(szPath)/sizeof(TCHAR)-1, _T("%s?%s"), stParseUrl.szPath, stParseUrl.szQuery );
	}
	else
	{
		if ( pstThread->pszError )
			_tcscpy( pstThread->pszError, _T("�޷����� URL ��ַ") );
		return HTTP_THREAD_FAIL;
	}

	pstThread->bRet = FALSE;
	try
	{
		//	��һ������ʼ�� internet DLL �Ự��������ǵ�һ�������õĺ���
		hSession = ::InternetOpen( DEFAULT_AGENT_NAME, PRE_CONFIG_INTERNET_ACCESS, _T(""), INTERNET_INVALID_PORT_NUMBER, 0 );
		if ( hSession )
		{
			//	�ڶ�������ʼ�� HTTP session ���Ӿ��
			hConnect = ::InternetConnect( hSession, szServer, DEFAULT_PORT, NULL, NULL, INTERNET_SERVICE_HTTP, 0, 0 );
			if ( hConnect )
			{
				//	����������һ��HTTP������
				hRequest = ::HttpOpenRequest
					(
						hConnect, HTTP_VERB_GET, szPath,
						HTTP_VERSION, NULL, 0,
						INTERNET_FLAG_DONT_CACHE|INTERNET_FLAG_NO_AUTO_REDIRECT, 0
					);
				if ( hRequest ) 
				{
					//	��ӿ�����Ҫ�� HTTP ͷ
					bAddHeaders	= TRUE;
					if ( pstThread->pvcHttpHeaderVar )
					{
						vcHttpHeaderVar = *pstThread->pvcHttpHeaderVar;
						for ( it = vcHttpHeaderVar.begin(); it != vcHttpHeaderVar.end(); it ++ )
						{
							_sntprintf( szHeadLine, sizeof(szHeadLine)/sizeof(TCHAR)-1, _T("%s"), (*it).szHeadLine );
							bAddHeaders &= ::HttpAddRequestHeaders
							(
								hRequest,
								szHeadLine,
								(DWORD)_tcslen( szHeadLine ),
								HTTP_ADDREQ_FLAG_REPLACE|HTTP_ADDREQ_FLAG_ADD
							);
						}
					}

					if ( bAddHeaders )
					{
						//	���Ĳ�����������
						bSendRequest = ::HttpSendRequest( hRequest, NULL, 0, 0, 0 );
						if ( bSendRequest )
						{
							//	��ȡ�� CRLF Ϊ�ָ�� Header
							bQuerySucc = ::HttpQueryInfo
								(
									hRequest,
									HTTP_QUERY_RAW_HEADERS_CRLF,
									pstThread->pszHeader,
									&pstThread->dwSize,
									NULL
								);
							if ( bQuerySucc && _tcslen(pstThread->pszHeader) > 0 )
							{
								pstThread->bRet = TRUE;
							}
						}
					}
					else
					{
						if ( pstThread->pszError )
							_tcscpy( pstThread->pszError, _T("��� HTTP ͷ����Ϣʧ��") );
					}

					::InternetCloseHandle( hRequest );
				}
				else
				{
					if ( pstThread->pszError )
						_tcscpy( pstThread->pszError, _T("HttpOpenRequest ʧ�ܣ���һ�� HTTP ������ʧ��") );
				}

				::InternetCloseHandle( hConnect );
			}
			else
			{
				if ( pstThread->pszError )
					_tcscpy( pstThread->pszError, _T("InternetConnect ʧ��") );
			}

			::InternetCloseHandle(hSession);
		}
		else
		{
			if ( pstThread->pszError )
				_tcscpy( pstThread->pszError, _T("��ʼ�� Internet DLL ʧ��") );
		}
	}
	catch ( ... )
	//__except( EXCEPTION_EXECUTE_HANDLER )
	{
		if ( pstThread->pszError )
			_tcscpy( pstThread->pszError, _T("�߳��쳣�����˳�") );
		if ( hRequest )
			::InternetCloseHandle( hRequest );
		if ( hConnect )
			::InternetCloseHandle( hConnect );
		if ( hSession )
			::InternetCloseHandle( hSession );
	}
	
	return ( TRUE == pstThread->bRet ) ? HTTP_THREAD_SUCC : HTTP_THREAD_FAIL;
}


/**
 *	Get Response
 */
BOOL CDeHttp::GetResponse( LPCTSTR lpcszUrl, LPTSTR lpszResponse, DWORD dwSize, LPTSTR lpszError, DWORD dwTimeout )
{
	//
	//	lpcszUrl	- [in]
	//	lpszResponse	- [in]
	//	dwSize		- [in]
	//	lpszError	- [in]
	//	dwTimeout	- [in] ��λ����
	//	RETURN		- [in]
	//

	if ( FALSE == HaveInternetConnection() )
	{
		if ( lpszError )
			_tcscpy( lpszError, _T("��ǰû����������") );
		return FALSE;
	}

	STHTTPTHREAD stHttpThread;

	stHttpThread.pvThisCls		= this;
	stHttpThread.bRet		= FALSE;
	stHttpThread.pszUrl		= lpcszUrl;
	stHttpThread.pszResponse	= lpszResponse;
	stHttpThread.dwSize		= dwSize;
	stHttpThread.pszError		= lpszError;

	try
	{
		stHttpThread.hThread = (HANDLE)_beginthreadex
			(
				NULL,
				0,
				(PBEGINTHREADEX_THREADFUNC)GetResponseProc,
				&stHttpThread,
				0,
				(PBEGINTHREADEX_THREADID)&stHttpThread.uThreadID
			);
		if ( NULL == stHttpThread.hThread )
			return FALSE;

		if ( WAIT_OBJECT_0 != WaitForSingleObject( stHttpThread.hThread, dwTimeout ) )
		{
			TerminateThread( stHttpThread.hThread, 0 );
			while ( WAIT_TIMEOUT == WaitForSingleObject( stHttpThread.hThread, 1000 ) )
			{
				TerminateThread( stHttpThread.hThread, 0 );
			}

			if ( lpszError )
				_tcscpy( lpszError, _T("�������ӳ�ʱ") );
		}
		if ( stHttpThread.hThread )
			CloseHandle( stHttpThread.hThread );
	}
	catch ( ... )
	//__except( EXCEPTION_EXECUTE_HANDLER )
	{
		if ( lpszError )
			_tcscpy( lpszError, _T("�߳��쳣�����˳�") );
	}

	return stHttpThread.bRet;
}
DWORD WINAPI CDeHttp::GetResponseProc( LPVOID p )
{	
	STHTTPTHREAD * pstThread = (STHTTPTHREAD*)p;
	CDeHttp * pCHttp = (CDeHttp*)pstThread->pvThisCls;
	pstThread->bRet = FALSE;
	
	if ( NULL == pCHttp )
	{
		if ( pstThread->pszError )
			_tcscpy( pstThread->pszError, _T("�ڲ������޷��ҵ����ַ") );
		return HTTP_THREAD_FAIL;
	}
	if ( NULL == pstThread->pszUrl || NULL == pstThread->pszResponse )
	{
		if ( pstThread->pszError )
			_tcscpy( pstThread->pszError, _T("������ַ��Ч") );
		return HTTP_THREAD_FAIL;
	}
	if ( 0 == _tcslen(pstThread->pszUrl) )
	{
		if ( pstThread->pszError )
			_tcscpy( pstThread->pszError, _T("�����������Ϊ��") );
		return HTTP_THREAD_FAIL;
	}

	TCHAR szServer[MAX_PATH]	= {0};
	TCHAR szPath[MAX_PATH]		= {0};
#ifdef UNICODE
	STDEHTTPDEPARSEURLW stParseUrl;
#else
	STDEHTTPDEPARSEURL stParseUrl;
#endif

	if ( pCHttp->dehttp_delib_parse_url( pstThread->pszUrl, (DWORD)_tcslen(pstThread->pszUrl), & stParseUrl ) )
	{
		_sntprintf( szServer, sizeof(szServer)/sizeof(TCHAR)-1, _T("%s"), stParseUrl.szHost );
		_sntprintf( szPath, sizeof(szPath)/sizeof(TCHAR)-1, _T("%s?%s"), stParseUrl.szPath, stParseUrl.szQuery );
	}
	else
	{
		if ( pstThread->pszError )
			_tcscpy( pstThread->pszError, _T("�޷����� URL ��ַ") );
		return HTTP_THREAD_FAIL;
	}

	/*
	if ( FALSE == pCHttp->ParseUrl( pstThread->pszUrl, szServer, szPath ) )
	{
		if ( pstThread->pszError )
			_tcscpy( pstThread->pszError, _T("�޷����� URL ��ַ") );
		return HTTP_THREAD_FAIL;
	}
	*/

	HINTERNET hSession	= NULL;
	HINTERNET hConnect	= NULL;
	HINTERNET hRequest	= NULL;
	BOOL bSendRequest	= FALSE;
	DWORD dwBytesRead	= 0;

	try
	{
		// ��һ��:��ʼ�� internet DLL �Ự���
		hSession = ::InternetOpen( DEFAULT_AGENT_NAME, PRE_CONFIG_INTERNET_ACCESS, _T(""), INTERNET_INVALID_PORT_NUMBER, 0 );
		if ( hSession )
		{
			// �ڶ�������ʼ�� HTTP session
			hConnect = ::InternetConnect
				(
					hSession,		// ��ǰinternet�Ự���
					szServer,		// server name
					INTERNET_INVALID_PORT_NUMBER,
					NULL,			// "",//user name
					NULL,			// password
					INTERNET_SERVICE_HTTP,	// Type of service to access
					0, 0
				);
			if ( hConnect )
			{
				//	����������һ��HTTP������
				hRequest = ::HttpOpenRequest( hConnect, HTTP_VERB_GET, szPath, HTTP_VERSION, NULL, 0, INTERNET_FLAG_DONT_CACHE, 0 );
				if ( hRequest )
				{
					// ���Ĳ�����������
					bSendRequest = ::HttpSendRequest( hRequest, NULL, 0, 0, 0 );
					if ( bSendRequest )
					{
						/*
						while( dwBufferSize > 0 && 
							InternetReadFile( hRequest, pstThread->pszResponse+dwTotalRead, dwBufferSize, &dwBytesRead ) )
						{
							if ( dwBytesRead == 0 )
							{
								break;
							}
							dwTotalRead	+= dwBytesRead;
							dwBufferSize	-= dwBytesRead;
						}
						*/

						if ( pCHttp->ReadInternetFileBuffer( hRequest, pstThread->pszResponse, pstThread->dwSize ) )
						{
							pstThread->bRet = TRUE;
						}
						else
						{
							if ( pstThread->pszError )
								_tcscpy( pstThread->pszError, _T("��ȡ������������ϢΪ��") );
						}

						/*
						DWORD dwBytesRead;
						BOOL  bReadFile = InternetReadFile
							(
								hRequest, pstThread->pszResponse,
								pstThread->dwSize-sizeof(TCHAR),
								&dwBytesRead
							);
						if ( bReadFile && dwBytesRead > 0 )
						{
							pstThread->bRet = TRUE;
						}
						else
						{
							if ( pstThread->pszError )
								_tcscpy( pstThread->pszError, _T("��ȡ������������ϢΪ��") );
						}
						*/
					}
					else
					{
						if ( pstThread->pszError )
							_stprintf( pstThread->pszError, _T("����һ�� %s ����ʧ��, ErrCode=%d"), pstThread->pszUrl, GetLastError() );
					}

					::InternetCloseHandle(hRequest);
				}
				else
				{
					if ( pstThread->pszError )
						_tcscpy( pstThread->pszError, _T("HttpOpenRequest ʧ�ܣ���һ��HTTP������ʧ��") );
				}

				::InternetCloseHandle(hConnect);
			}
			else
			{
				if ( pstThread->pszError )
					_tcscpy( pstThread->pszError, _T("InternetConnect ʧ�ܣ���ʼ��HTTP sessionʧ��") );
			}

			::InternetCloseHandle(hSession);
		}
		else
		{
			if ( pstThread->pszError )
				_tcscpy( pstThread->pszError, _T("InternetOpen ʧ�ܣ���ʼ��Internet DLLʧ��") );
		}
	}
	catch ( ... )
	//__except( EXCEPTION_EXECUTE_HANDLER )
	{
		if ( pstThread->pszError )
			_tcscpy( pstThread->pszError, _T("�߳��쳣�����˳�") );
		if ( hRequest )
			::InternetCloseHandle(hRequest);
		if ( hConnect )
			::InternetCloseHandle(hConnect);
		if ( hSession )
			::InternetCloseHandle(hSession);
	}

	return ( TRUE == pstThread->bRet ) ? HTTP_THREAD_SUCC : HTTP_THREAD_FAIL;

}

/**
 *	�� HINTERNET hRequest �����ȡ�����ķ�������Ӧ����
 */
BOOL CDeHttp::ReadInternetFileBuffer( HINTERNET hRequest, TCHAR * pszBuffer, DWORD dwSize )
{
	TCHAR szTempBuffer[ MAX_PATH ];
	LONGLONG  lnlnBufferSize;
	DWORD dwBytesRead;
	DWORD dwTotalRead;

	if ( NULL == hRequest || NULL == pszBuffer || 0 == dwSize )
	{
		return FALSE;
	}

	__try
	{
		lnlnBufferSize	= dwSize-sizeof(TCHAR);
		dwTotalRead	= 0;
		memset( szTempBuffer, 0, sizeof(szTempBuffer) );

		while ( InternetReadFile( hRequest, szTempBuffer, sizeof(szTempBuffer), &dwBytesRead ) )
		{
			if ( dwBytesRead == 0 )
			{
				break;
			}
			if ( lnlnBufferSize <= 0 )
			{
				break;
			}

			memcpy( pszBuffer+dwTotalRead, szTempBuffer, min( dwBytesRead, lnlnBufferSize ) );

			dwTotalRead	+= dwBytesRead;
			lnlnBufferSize	-= dwBytesRead;

			memset( szTempBuffer, 0, sizeof(szTempBuffer) );
		}
	}
	__except( EXCEPTION_EXECUTE_HANDLER )
	{
	}

	return TRUE;
}

/**
 *	Get Response
 */
BOOL CDeHttp::GetResponseByProxy( LPCTSTR lpcszUrl, LPCTSTR lpcszProxy, LPTSTR lpszResponse, DWORD dwSize, LPTSTR lpszError, DWORD dwTimeout )
{
	//
	//	lpcszUrl	- [in]
	//	lpcszProxy	- [in] ���������
	//	lpszResponse	- [in]
	//	dwSize		- [in]
	//	lpszError	- [in]
	//	dwTimeout	- [in] ��λ����
	//	RETURN		- [in]
	//

	if ( FALSE == HaveInternetConnection() )
	{
		if ( lpszError )
			_tcscpy( lpszError, _T("��ǰû����������") );
		return FALSE;
	}

	STHTTPTHREAD stHttpThread;

	stHttpThread.pvThisCls		= this;
	stHttpThread.bRet		= FALSE;
	stHttpThread.pszUrl		= lpcszUrl;
	stHttpThread.lpcszProxy		= lpcszProxy;
	stHttpThread.pszResponse	= lpszResponse;
	stHttpThread.dwSize		= dwSize;
	stHttpThread.pszError		= lpszError;

	try
	{
		stHttpThread.hThread = (HANDLE)_beginthreadex
			(
				NULL,
				0,
				(PBEGINTHREADEX_THREADFUNC)GetResponseByProxyProc,
				&stHttpThread,
				0,
				(PBEGINTHREADEX_THREADID)&stHttpThread.uThreadID
			);
		if ( NULL == stHttpThread.hThread )
			return FALSE;

		if ( WAIT_OBJECT_0 != WaitForSingleObject( stHttpThread.hThread, dwTimeout ) )
		{
			TerminateThread( stHttpThread.hThread, 0 );
			while ( WAIT_TIMEOUT == WaitForSingleObject( stHttpThread.hThread, 1000 ) )
			{
				TerminateThread( stHttpThread.hThread, 0 );
			}

			if ( lpszError )
			{
				_tcscpy( lpszError, _T("�������ӳ�ʱ") );
			}
		}

		if ( stHttpThread.hThread )
		{
			CloseHandle( stHttpThread.hThread );
		}
	}
	catch ( ... )
	//__except( EXCEPTION_EXECUTE_HANDLER )
	{
		if ( lpszError )
			_tcscpy( lpszError, _T("�߳��쳣�����˳�") );
	}

	return stHttpThread.bRet;
}
DWORD WINAPI CDeHttp::GetResponseByProxyProc( LPVOID p )
{
	STHTTPTHREAD * pstThread = (STHTTPTHREAD*)p;
	CDeHttp * pCHttp = (CDeHttp*)pstThread->pvThisCls;
	pstThread->bRet = FALSE;

	if ( NULL == pCHttp )
	{
		if ( pstThread->pszError )
			_tcscpy( pstThread->pszError, _T("�ڲ������޷��ҵ����ַ") );
		return HTTP_THREAD_FAIL;
	}
	if ( NULL == pstThread->pszUrl || NULL == pstThread->pszResponse )
	{
		if ( pstThread->pszError )
			_tcscpy( pstThread->pszError, _T("������ַ��Ч") );
		return HTTP_THREAD_FAIL;
	}
	if ( 0 == _tcslen(pstThread->pszUrl) )
	{
		if ( pstThread->pszError )
			_tcscpy( pstThread->pszError, _T("�����������Ϊ��") );
		return HTTP_THREAD_FAIL;
	}

	TCHAR szServer[MAX_PATH]	= {0};
	TCHAR szPath[MAX_PATH]		= {0};
#ifdef UNICODE
	STDEHTTPDEPARSEURLW stParseUrl;
#else
	STDEHTTPDEPARSEURL stParseUrl;
#endif

	if ( pCHttp->dehttp_delib_parse_url( pstThread->pszUrl, (DWORD)_tcslen(pstThread->pszUrl), & stParseUrl ) )
	{
		_sntprintf( szServer, sizeof(szServer)/sizeof(TCHAR)-1, _T("%s"), stParseUrl.szHost );
		_sntprintf( szPath, sizeof(szPath)/sizeof(TCHAR)-1, _T("%s?%s"), stParseUrl.szPath, stParseUrl.szQuery );
	}
	else
	{
		if ( pstThread->pszError )
			_tcscpy( pstThread->pszError, _T("�޷����� URL ��ַ") );
		return HTTP_THREAD_FAIL;
	}

	/*
	if ( FALSE == pCHttp->ParseUrl( pstThread->pszUrl, szServer, szPath ) )
	{
		if ( pstThread->pszError )
			_tcscpy( pstThread->pszError, _T("�޷����� URL ��ַ") );
		return HTTP_THREAD_FAIL;
	}
	*/

	HINTERNET hSession	= NULL;
	HINTERNET hConnect	= NULL;
	HINTERNET hRequest	= NULL;
	BOOL bSendRequest	= FALSE;
	DWORD dwBytesRead	= 0;


	try
	{
		// ��һ��:��ʼ�� internet DLL �Ự���
		hSession = ::InternetOpen( DEFAULT_AGENT_NAME, INTERNET_OPEN_TYPE_PROXY, pstThread->lpcszProxy, pstThread->lpcszProxy, 0 );
		if ( hSession )
		{
			// �ڶ�������ʼ�� HTTP session
			hConnect = ::InternetConnect
				(
					hSession,		// ��ǰinternet�Ự���
					szServer,		// server name
					INTERNET_INVALID_PORT_NUMBER,
					NULL,			// "",//user name
					NULL,			// password
					INTERNET_SERVICE_HTTP,	// Type of service to access
					0, 0
				);
			if ( hConnect )
			{
				//	����������һ��HTTP������
				hRequest = ::HttpOpenRequest( hConnect, HTTP_VERB_GET, szPath, HTTP_VERSION, NULL, 0, INTERNET_FLAG_DONT_CACHE, 0 );
				if ( hRequest )
				{
					// ���Ĳ�����������
					bSendRequest = ::HttpSendRequest( hRequest, NULL, 0, 0, 0 );
					if ( bSendRequest )
					{
						DWORD dwBytesRead;
						BOOL  bReadFile = InternetReadFile
							(
								hRequest, pstThread->pszResponse,
								pstThread->dwSize-sizeof(TCHAR),
								&dwBytesRead
							);
						if ( bReadFile && dwBytesRead > 0 )
						{
							pstThread->bRet = TRUE;
						}
						else
						{
							if ( pstThread->pszError )
								_tcscpy( pstThread->pszError, _T("��ȡ������������ϢΪ��") );
						}
					}
					else
					{
						if ( pstThread->pszError )
							_stprintf( pstThread->pszError, _T("����һ�� %s ����ʧ��, ErrCode=%d"), pstThread->pszUrl, GetLastError() );
					}

					::InternetCloseHandle(hRequest);
				}
				else
				{
					if ( pstThread->pszError )
						_tcscpy( pstThread->pszError, _T("HttpOpenRequest ʧ�ܣ���һ��HTTP������ʧ��") );
				}

				::InternetCloseHandle(hConnect);
			}
			else
			{
				if ( pstThread->pszError )
					_tcscpy( pstThread->pszError, _T("InternetConnect ʧ�ܣ���ʼ��HTTP sessionʧ��") );
			}

			::InternetCloseHandle(hSession);
		}
		else
		{
			if ( pstThread->pszError )
				_tcscpy( pstThread->pszError, _T("InternetOpen ʧ�ܣ���ʼ��Internet DLLʧ��") );
		}
	}
	catch ( ... )
	//__except( EXCEPTION_EXECUTE_HANDLER )
	{
		if ( pstThread->pszError )
			_tcscpy( pstThread->pszError, _T("�߳��쳣�����˳�") );
		if ( hRequest )
			::InternetCloseHandle(hRequest);
		if ( hConnect )
			::InternetCloseHandle(hConnect);
		if ( hSession )
			::InternetCloseHandle(hSession);
	}

	return ( TRUE == pstThread->bRet ) ? HTTP_THREAD_SUCC : HTTP_THREAD_FAIL;

}




/**
 *	Get Response
 */
BOOL CDeHttp::GetResponseEx( LPCTSTR lpcszUrl, vector<STHTTPHEADERVAR> & vcHttpHeaderVar, LPTSTR lpszResponse, DWORD dwSize, LPTSTR lpszError, DWORD dwTimeout )
{
	if ( FALSE == HaveInternetConnection() )
	{
		if ( lpszError )
			_tcscpy( lpszError, _T("��ǰû����������") );
		return FALSE;
	}

	STHTTPTHREAD stHttpThread;

	stHttpThread.pvThisCls		= this;
	stHttpThread.bRet		= FALSE;
	stHttpThread.pszUrl		= lpcszUrl;
	stHttpThread.pvcHttpHeaderVar	= & vcHttpHeaderVar;
	stHttpThread.pszResponse	= lpszResponse;
	stHttpThread.dwSize		= dwSize;
	stHttpThread.pszError		= lpszError;

	__try
	{
		stHttpThread.hThread = (HANDLE)_beginthreadex
			(
				NULL,
				0,
				(PBEGINTHREADEX_THREADFUNC)GetResponseExProc,
				&stHttpThread,
				0,
				(PBEGINTHREADEX_THREADID)&stHttpThread.uThreadID
			);
		if ( NULL == stHttpThread.hThread )
			return FALSE;

		if ( WAIT_OBJECT_0 != WaitForSingleObject( stHttpThread.hThread, dwTimeout ) )
		{
			TerminateThread( stHttpThread.hThread, 0 );
			while ( WAIT_TIMEOUT == WaitForSingleObject( stHttpThread.hThread, 1000 ) )
			{
				TerminateThread( stHttpThread.hThread, 0 );
			}

			if ( lpszError )
				_tcscpy( lpszError, _T("�������ӳ�ʱ") );
		}
		if ( stHttpThread.hThread )
			CloseHandle( stHttpThread.hThread );
	}
	__except( EXCEPTION_EXECUTE_HANDLER )
	{
		if ( lpszError )
			_tcscpy( lpszError, _T("�߳��쳣�����˳�") );
	}

	return stHttpThread.bRet;
}
DWORD WINAPI CDeHttp::GetResponseExProc( LPVOID p )
{
	STHTTPTHREAD * pstThread = (STHTTPTHREAD*)p;
	CDeHttp * pCHttp = (CDeHttp*)pstThread->pvThisCls;
	pstThread->bRet = FALSE;

	if ( NULL == pCHttp )
	{
		if ( pstThread->pszError )
			_tcscpy( pstThread->pszError, _T("�ڲ������޷��ҵ����ַ") );
		return HTTP_THREAD_FAIL;
	}
	if ( NULL == pstThread->pszUrl || NULL == pstThread->pszResponse )
	{
		if ( pstThread->pszError )
			_tcscpy( pstThread->pszError, _T("������ַ��Ч") );
		return HTTP_THREAD_FAIL;
	}
	if ( 0 == _tcslen(pstThread->pszUrl) )
	{
		if ( pstThread->pszError )
			_tcscpy( pstThread->pszError, _T("�����������Ϊ��") );
		return HTTP_THREAD_FAIL;
	}

	TCHAR szServer[MAX_PATH]	= {0};
	TCHAR szPath[MAX_PATH]		= {0};
#ifdef UNICODE
	STDEHTTPDEPARSEURLW stParseUrl;
#else
	STDEHTTPDEPARSEURL stParseUrl;
#endif

	if ( pCHttp->dehttp_delib_parse_url( pstThread->pszUrl, (DWORD)_tcslen(pstThread->pszUrl), & stParseUrl ) )
	{
		_sntprintf( szServer, sizeof(szServer)/sizeof(TCHAR)-1, _T("%s"), stParseUrl.szHost );
		_sntprintf( szPath, sizeof(szPath)/sizeof(TCHAR)-1, _T("%s?%s"), stParseUrl.szPath, stParseUrl.szQuery );
	}
	else
	{
		if ( pstThread->pszError )
			_tcscpy( pstThread->pszError, _T("�޷����� URL ��ַ") );
		return HTTP_THREAD_FAIL;
	}

	HINTERNET hSession	= NULL;
	HINTERNET hConnect	= NULL;
	HINTERNET hRequest	= NULL;
	BOOL bSendRequest	= FALSE;
	DWORD dwBytesRead	= 0;
	BOOL bAddHeaders	= FALSE;
	vector<STHTTPHEADERVAR> vcHttpHeaderVar;
	vector<STHTTPHEADERVAR>::iterator it;
	TCHAR szHeadLine[MAX_PATH]	= {0};


	try
	{
		// ��һ��:��ʼ�� internet DLL �Ự���
		hSession = ::InternetOpen( DEFAULT_AGENT_NAME, PRE_CONFIG_INTERNET_ACCESS, _T(""), INTERNET_INVALID_PORT_NUMBER, 0 );
		if ( hSession )
		{
			// �ڶ�������ʼ�� HTTP session
			hConnect = ::InternetConnect
				(
					hSession,		// ��ǰinternet�Ự���
					szServer,		// server name
					INTERNET_INVALID_PORT_NUMBER,
					NULL,			// "",//user name
					NULL,			// password
					INTERNET_SERVICE_HTTP,	// Type of service to access
					0, 0
				);
			if ( hConnect )
			{
				//	����������һ��HTTP������
				hRequest = ::HttpOpenRequest( hConnect, HTTP_VERB_GET, szPath, HTTP_VERSION, NULL, 0, INTERNET_FLAG_DONT_CACHE, 0 );
				if ( hRequest )
				{
					//	��ӿ�����Ҫ�� HTTP ͷ
					bAddHeaders	= TRUE;
					if ( pstThread->pvcHttpHeaderVar )
					{
						vcHttpHeaderVar = *pstThread->pvcHttpHeaderVar;
						for ( it = vcHttpHeaderVar.begin(); it != vcHttpHeaderVar.end(); it ++ )
						{
							_sntprintf( szHeadLine, sizeof(szHeadLine)/sizeof(TCHAR)-1, _T("%s"), (*it).szHeadLine );
							bAddHeaders &= ::HttpAddRequestHeaders
							(
								hRequest,
								szHeadLine,
								(DWORD)_tcslen( szHeadLine ),
								HTTP_ADDREQ_FLAG_REPLACE|HTTP_ADDREQ_FLAG_ADD
							);
						}
					}
					
					if ( bAddHeaders )
					{
						// ���Ĳ�����������
						bSendRequest = ::HttpSendRequest( hRequest, NULL, 0, 0, 0 );
						if ( bSendRequest )
						{
							DWORD dwBytesRead;
							BOOL  bReadFile = InternetReadFile
								(
									hRequest, pstThread->pszResponse,
									pstThread->dwSize-sizeof(TCHAR),
									&dwBytesRead
								);
							if ( bReadFile && dwBytesRead > 0 )
							{
								pstThread->bRet = TRUE;
							}
							else
							{
								if ( pstThread->pszError )
									_tcscpy( pstThread->pszError, _T("��ȡ������������ϢΪ��") );
							}
						}
						else
						{
							if ( pstThread->pszError )
								_stprintf( pstThread->pszError, _T("����һ�� %s ����ʧ��, ErrCode=%d"), pstThread->pszUrl, GetLastError() );
						}
					}
					else
					{
						if ( pstThread->pszError )
							_tcscpy( pstThread->pszError, _T("��� HTTP ͷ����Ϣʧ��") );
					}
					::InternetCloseHandle( hRequest );
				}
				else
				{
					if ( pstThread->pszError )
						_tcscpy( pstThread->pszError, _T("HttpOpenRequest ʧ�ܣ���һ��HTTP������ʧ��") );
				}
				::InternetCloseHandle(hConnect);
			}
			else
			{
				if ( pstThread->pszError )
					_tcscpy( pstThread->pszError, _T("InternetConnect ʧ�ܣ���ʼ��HTTP sessionʧ��") );
			}
			::InternetCloseHandle( hSession );
		}
		else
		{
			if ( pstThread->pszError )
				_tcscpy( pstThread->pszError, _T("InternetOpen ʧ�ܣ���ʼ��Internet DLLʧ��") );
		}
	}
	catch ( ... )
	//__except( EXCEPTION_EXECUTE_HANDLER )
	{
		if ( pstThread->pszError )
			_tcscpy( pstThread->pszError, _T("�߳��쳣�����˳�") );
		if ( hRequest )
			::InternetCloseHandle(hRequest);
		if ( hConnect )
			::InternetCloseHandle(hConnect);
		if ( hSession )
			::InternetCloseHandle(hSession);
	}

	return ( TRUE == pstThread->bRet ) ? HTTP_THREAD_SUCC : HTTP_THREAD_FAIL;
}




/**
 *	POST һ�� FORM ���ҵõ����صĻ���������
 */
BOOL CDeHttp::PostForm( LPCTSTR lpcszUrl, LPTSTR lpszPostData, LPTSTR lpszResponse, DWORD dwSize, LPTSTR lpszError, DWORD dwTimeout )
{
	if ( FALSE == HaveInternetConnection() )
	{
		if ( lpszError )
			_tcscpy( lpszError, _T("��ǰû����������") );
		return FALSE;
	}

	STHTTPTHREAD stHttpThread;

	stHttpThread.pvThisCls		= this;
	stHttpThread.bRet		= FALSE;
	stHttpThread.pszUrl		= lpcszUrl;
	stHttpThread.pszPostData	= lpszPostData;
	stHttpThread.pszResponse	= lpszResponse;
	stHttpThread.dwSize		= dwSize;
	stHttpThread.pszError		= lpszError;

	__try
	{
		stHttpThread.hThread = (HANDLE)_beginthreadex
			(
				NULL,
				0,
				(PBEGINTHREADEX_THREADFUNC)PostFormProc,
				&stHttpThread,
				0,
				(PBEGINTHREADEX_THREADID)&stHttpThread.uThreadID
			);
		if ( NULL == stHttpThread.hThread )
			return FALSE;
		
		if ( WAIT_OBJECT_0 != WaitForSingleObject( stHttpThread.hThread, dwTimeout ) )
		{
			TerminateThread( stHttpThread.hThread, 0 );
			while ( WAIT_TIMEOUT == WaitForSingleObject( stHttpThread.hThread, 1000 ) )
			{
				TerminateThread( stHttpThread.hThread, 0 );
			}

			if ( lpszError )
				_tcscpy( lpszError, _T("�������ӳ�ʱ") );
		}
		if ( stHttpThread.hThread )
			CloseHandle( stHttpThread.hThread );
	}
	__except( EXCEPTION_EXECUTE_HANDLER )
	{
		if ( lpszError )
			_tcscpy( lpszError, _T("�߳��쳣�����˳�") );
	}

	return stHttpThread.bRet;
}

BOOL CDeHttp::PostFormEx( LPCTSTR lpcszUrl, LPTSTR lpszPostData, LPTSTR lpszAgent, LPTSTR lpszHeader, 
				LPTSTR lpszResponse, DWORD dwSize, LPTSTR lpszError, DWORD dwTimeout )
{
	if ( FALSE == HaveInternetConnection() )
	{
		if ( lpszError )
			_tcscpy( lpszError, _T("��ǰû����������") );
		return FALSE;
	}

	STHTTPTHREAD stHttpThread;
	
	stHttpThread.bRet		= FALSE;
	stHttpThread.pszUrl		= lpcszUrl;
	stHttpThread.pszAgent		= lpszAgent;	// for V1.0.0.2
	stHttpThread.pszHeader		= lpszHeader;	// for V1.0.0.2
	stHttpThread.pszPostData	= lpszPostData;
	stHttpThread.pszResponse	= lpszResponse;
	stHttpThread.dwSize		= dwSize;
	stHttpThread.pszError		= lpszError;

	_try
	{
		stHttpThread.hThread = (HANDLE)_beginthreadex
			(
				NULL,
				0,
				(PBEGINTHREADEX_THREADFUNC)PostFormProc,
				&stHttpThread,
				0,
				(PBEGINTHREADEX_THREADID)&stHttpThread.uThreadID
			);
		if ( NULL == stHttpThread.hThread )
			return FALSE;
		
		if ( WAIT_OBJECT_0 != WaitForSingleObject( stHttpThread.hThread, dwTimeout ) )
		{
			TerminateThread( stHttpThread.hThread, 0 );
			while ( WAIT_TIMEOUT == WaitForSingleObject( stHttpThread.hThread, 1000 ) )
			{
				TerminateThread( stHttpThread.hThread, 0 );
			}

			if ( lpszError )
				_tcscpy( lpszError, _T("�������ӳ�ʱ") );
		}
		if ( stHttpThread.hThread )
			CloseHandle( stHttpThread.hThread );
	}
	__except( EXCEPTION_EXECUTE_HANDLER )
	{
		if ( lpszError )
			_tcscpy( lpszError, _T("�߳��쳣�����˳�") );
	}
	
	return stHttpThread.bRet;
	
}

DWORD WINAPI CDeHttp::PostFormProc( LPVOID p )
{
	STHTTPTHREAD * pstThread = (STHTTPTHREAD*)p;
	CDeHttp * pCHttp = (CDeHttp*)pstThread->pvThisCls;
	pstThread->bRet = FALSE;
	
	if ( NULL == pCHttp )
	{
		if ( pstThread->pszError )
			_tcscpy( pstThread->pszError, _T("�ڲ������޷��ҵ����ַ") );
		return HTTP_THREAD_FAIL;
	}
	if ( NULL == pstThread->pszUrl || 0 == _tcslen(pstThread->pszUrl) )
	{
		if ( pstThread->pszError )
			_tcscpy( pstThread->pszError, _T("��������д���") );
		return HTTP_THREAD_FAIL;
	}

	const TCHAR * szAcceptType = HTTP_ACCEPT_TYPE;
	const TCHAR * szContentType = _T("Content-Type: application/x-www-form-urlencoded\r\n");

	TCHAR szServer[MAX_PATH]	= {0};
	TCHAR szPath[MAX_PATH]		= {0};
#ifdef UNICODE
	STDEHTTPDEPARSEURLW stParseUrl;
#else
	STDEHTTPDEPARSEURL stParseUrl;
#endif
	
	if ( pCHttp->dehttp_delib_parse_url( pstThread->pszUrl, (DWORD)_tcslen(pstThread->pszUrl), & stParseUrl ) )
	{
		_sntprintf( szServer, sizeof(szServer)/sizeof(TCHAR)-sizeof(TCHAR), _T("%s"), stParseUrl.szHost );
		_sntprintf( szPath, sizeof(szPath)/sizeof(TCHAR)-sizeof(TCHAR), _T("%s?%s"), stParseUrl.szPath, stParseUrl.szQuery );
	}
	else
	{
		if ( pstThread->pszError )
			_tcscpy( pstThread->pszError, _T("�޷����� URL ��ַ") );
		return HTTP_THREAD_FAIL;
	}

	if ( 0 == _tcslen(szServer) )
	{
		if ( pstThread->pszError )
			_tcscpy( pstThread->pszError, _T("������������Ϊ��") );
		return HTTP_THREAD_FAIL;
	}
	if ( 0 == _tcslen(szPath) )
	{
		if ( pstThread->pszError )
			_tcscpy( pstThread->pszError, _T("����·������Ϊ��") );
		return HTTP_THREAD_FAIL;
	}
	if ( NULL == pstThread->pszPostData )
	{
		if ( pstThread->pszError )
			_tcscpy( pstThread->pszError, _T("��������ָ����Ч") );
		return HTTP_THREAD_FAIL;
	}
	else if ( 0 == _tcslen(pstThread->pszPostData) )
	{
		if ( pstThread->pszError )
			_tcscpy( pstThread->pszError, _T("�������ݲ���Ϊ��") );
		return HTTP_THREAD_FAIL;
	}


	HINTERNET hSession		= NULL;
	HINTERNET hConnect		= NULL;
	HINTERNET hRequest		= NULL;
	DWORD dwFlags			= 0;

	BOOL bAddHeaders		= FALSE;
	BOOL bSendRequest		= FALSE;
	BOOL bReadFile			= FALSE;
	DWORD dwBytesRead		= 0;

	TCHAR szAgent[ MAX_PATH ]	= {0};
	TCHAR szHeader[ 1024 ]		= {0};
	TCHAR szSubHeader[ MAX_PATH ]	= {0};

	__try
	{
		if ( pstThread->pszAgent && _tcslen(pstThread->pszAgent) > 0 )
		{
			_sntprintf( szAgent, sizeof(szAgent)/sizeof(TCHAR)-1, _T("%s"), pstThread->pszAgent );
		}
		else
		{
			_sntprintf( szAgent, sizeof(szAgent)/sizeof(TCHAR)-1, _T("%s"), DEFAULT_AGENT_NAME );
		}

		if ( pstThread->pszHeader && _tcslen(pstThread->pszHeader) > 0 )
		{
			_sntprintf( szHeader, sizeof(szHeader)/sizeof(TCHAR)-1, _T("%s"), pstThread->pszHeader );
		}
		else
		{
			_sntprintf( szHeader, sizeof(szHeader)/sizeof(TCHAR)-1, _T("%s"), HTTP_ACCEPT );
		}

		// ��һ������ʼ�� internet DLL �Ự��������ǵ�һ�������õĺ���
		hSession = InternetOpen( szAgent, INTERNET_OPEN_TYPE_PRECONFIG, NULL, NULL, 0 );
		if ( hSession )
		{
			// �ڶ�������ʼ�� HTTP session ���Ӿ��
			hConnect = ::InternetConnect
				(
					hSession,		//	��ǰinternet�Ự���
					szServer,		//	Server Name
					DEFAULT_PORT,		//	Server Port
					NULL,			//	User Name
					NULL,			//	Password
					INTERNET_SERVICE_HTTP,	//	Type of service to access
					0, 0
				);
			if ( hConnect )
			{
				////////////////////////////////////////////////////////////
				// ����������һ��HTTP������
				dwFlags = INTERNET_FLAG_KEEP_CONNECTION | INTERNET_FLAG_NO_CACHE_WRITE | INTERNET_FLAG_FORMS_SUBMIT;
				hRequest =:: HttpOpenRequest
					(
						hConnect,
						HTTP_VERB_POST,		//	HTTP Verb
						szPath,			//	Object Name
						HTTP_VERSION,		//	Version
						NULL,			//	Reference
						&szAcceptType,		//	Accept Type															
						dwFlags,		//	flag
						NULL			//	context call-back point
					);
				if ( hRequest ) 
				{
					// ���Ĳ������ HTTP Э��ͷ
					bAddHeaders	= TRUE;

					PTCHAR pszMove	= szHeader;
					PTCHAR pszTail	= NULL;
					pszTail = _tcsstr( pszMove, _T("\r\n") );
					while( _tcslen(pszMove) > 0 )
					{
						if ( pszTail )
						{
							* pszTail = '\0';
							pszTail += sizeof(TCHAR)*2;
						}
						_sntprintf( szSubHeader, sizeof(szSubHeader)/sizeof(TCHAR)-1, _T("%s\r\n"), pszMove );
						bAddHeaders &= ::HttpAddRequestHeaders
							(
								hRequest,
								szSubHeader,
								(DWORD)_tcslen(szSubHeader),
								HTTP_ADDREQ_FLAG_REPLACE|HTTP_ADDREQ_FLAG_ADD
							);
						if ( pszTail )
						{
							pszMove = pszTail;
							pszTail = _tcsstr( pszMove, _T("\r\n") );
						}
						else
							break;
					}
					if ( bAddHeaders )
					{
						//	���岽���������� SEND REQUEST WITH POST ARGUEMENTS
						bSendRequest = ::HttpSendRequest
							(
								hRequest,				// handle by returned HttpOpenRequest
								szContentType,				// additional HTTP header
								(DWORD)_tcslen(szContentType),			// additional HTTP header length
								reinterpret_cast<LPVOID>(pstThread->pszPostData),	// additional data in HTTP Post or HTTP Put
								(DWORD)_tcslen(pstThread->pszPostData)			// additional data length
							);
						if ( FALSE == bSendRequest && 12168 == ::GetLastError() )
						{
							bSendRequest = TRUE;
						}

						//	���������õ���������
						if ( bSendRequest )
						{
							bReadFile = InternetReadFile( hRequest, pstThread->pszResponse, pstThread->dwSize-sizeof(TCHAR), &dwBytesRead );
							if ( bReadFile && dwBytesRead > 0 )
							{
								pstThread->bRet = TRUE;
							}
							else
							{
								if ( pstThread->pszError )
								{
									_tcscpy( pstThread->pszError, _T("��ȡ��������������Ϊ��") );
								}
							}
						}
						else
						{
							if ( pstThread->pszError )
							{
								_stprintf( pstThread->pszError,
									_T("����һ�� %s ����ʧ��, ErrCode=%d"),
									pstThread->pszUrl, GetLastError() );
							}
						}
					}
					else
					{
						if ( pstThread->pszError )
							_tcscpy( pstThread->pszError, _T("��� HTTP ͷ����Ϣʧ��") );
					}

					//////////////////////////////////////////////////
					::InternetCloseHandle(hRequest);
				}
				else
				{
					if ( pstThread->pszError )
						_tcscpy( pstThread->pszError, _T("HttpOpenRequest ʧ�ܣ���һ�� HTTP ������ʧ��") );
				}

				//////////////////////////////////////////////////
				::InternetCloseHandle(hConnect);
			}
			else
			{
				if ( pstThread->pszError )
					_tcscpy( pstThread->pszError, _T("InternetConnect ʧ��") );
			}

			//////////////////////////////////////////////////
			::InternetCloseHandle(hSession);
		}
		else
		{
			if ( pstThread->pszError )
				_tcscpy( pstThread->pszError, _T("��ʼ�� Internet DLL ʧ��") );
		}

	}
	__except( EXCEPTION_EXECUTE_HANDLER )
	{
		if ( pstThread->pszError )
			_tcscpy( pstThread->pszError, _T("�߳��쳣�����˳�") );
		if ( hRequest )
			::InternetCloseHandle(hRequest);
		if ( hConnect )
			::InternetCloseHandle(hConnect);
		if ( hSession )
			::InternetCloseHandle(hSession);
	}

	return ( TRUE == pstThread->bRet ) ? HTTP_THREAD_SUCC : HTTP_THREAD_FAIL;

}


//////////////////////////////////////////////////////////////////////////
//
// ��ָ�� URL ��ȡ���ݲ����浽�ļ�
BOOL CDeHttp::GetFile( LPCTSTR lpcszUrl, LPTSTR lpszDestFile, BOOL bTextFile, LPTSTR lpszError, DWORD dwTimeout )
{
	if ( FALSE == HaveInternetConnection() )
	{
		if ( lpszError )
			_tcscpy( lpszError, _T("��ǰû����������") );
		return FALSE;
	}

	STHTTPTHREAD stHttpThread;

	stHttpThread.pvThisCls		= this;
	stHttpThread.bRet		= FALSE;
	stHttpThread.pszUrl		= lpcszUrl;
	stHttpThread.pszDestFile	= lpszDestFile;
	stHttpThread.bTextFile		= bTextFile;
	stHttpThread.pszError		= lpszError;

	__try
	{
		stHttpThread.hThread = (HANDLE)_beginthreadex
			(
				NULL,
				0,
				(PBEGINTHREADEX_THREADFUNC)GetFileProc,
				&stHttpThread,
				0,
				(PBEGINTHREADEX_THREADID)&stHttpThread.uThreadID
			);
		if ( NULL == stHttpThread.hThread )
			return FALSE;

		if ( WAIT_OBJECT_0 != WaitForSingleObject( stHttpThread.hThread, dwTimeout ) )
		{
			TerminateThread( stHttpThread.hThread, 0 );
			while ( WAIT_TIMEOUT == WaitForSingleObject( stHttpThread.hThread, 1000 ) )
			{
				TerminateThread( stHttpThread.hThread, 0 );
			}

			if ( lpszError )
				_tcscpy( lpszError, _T("�������ӳ�ʱ") );
		}
		if ( stHttpThread.hThread )
			CloseHandle( stHttpThread.hThread );
	}
	__except( EXCEPTION_EXECUTE_HANDLER )
	{
		if ( lpszError )
			_tcscpy( lpszError, _T("�߳��쳣�����˳�") );
	}

	return stHttpThread.bRet;
}


BOOL CDeHttp::GetFileEx( LPCTSTR lpcszUrl, LPTSTR lpszAgent, LPTSTR lpszHeader, LPCTSTR lpcszDestFile, BOOL bTextFile, LPTSTR lpszError, DWORD dwTimeout )
{
	if ( FALSE == HaveInternetConnection() )
	{
		if ( lpszError )
			_tcscpy( lpszError, _T("��ǰû����������") );
		return FALSE;
	}
	
	STHTTPTHREAD stHttpThread;

	stHttpThread.pvThisCls		= this;
	stHttpThread.bRet		= FALSE;
	stHttpThread.pszUrl		= lpcszUrl;
	stHttpThread.pszAgent		= lpszAgent;	// for V1.0.0.2
	stHttpThread.pszHeader		= lpszHeader;	// for V1.0.0.2
	stHttpThread.pszDestFile	= lpcszDestFile;
	stHttpThread.bTextFile		= bTextFile;
	stHttpThread.pszError		= lpszError;
	
	__try
	{
		stHttpThread.hThread = (HANDLE)_beginthreadex
			(
				NULL,
				0,
				(PBEGINTHREADEX_THREADFUNC)GetFileProc,
				&stHttpThread,
				0,
				(PBEGINTHREADEX_THREADID)&stHttpThread.uThreadID
			);
		if ( NULL == stHttpThread.hThread )
			return FALSE;
		
		if ( WAIT_OBJECT_0 != WaitForSingleObject( stHttpThread.hThread, dwTimeout ) )
		{
			TerminateThread( stHttpThread.hThread, 0 );
			while ( WAIT_TIMEOUT == WaitForSingleObject( stHttpThread.hThread, 1000 ) )
			{
				TerminateThread( stHttpThread.hThread, 0 );
			}

			if ( lpszError )
				_tcscpy( lpszError, _T("�������ӳ�ʱ") );
		}
		if ( stHttpThread.hThread )
			CloseHandle( stHttpThread.hThread );
	}
	__except( EXCEPTION_EXECUTE_HANDLER )
	{
		if ( lpszError )
			_tcscpy( lpszError, _T("�߳��쳣�����˳�") );
	}
	
	return stHttpThread.bRet;
}

DWORD WINAPI CDeHttp::GetFileProc( LPVOID p )
{
	STHTTPTHREAD * pstThread = (STHTTPTHREAD*)p;
	pstThread->bRet = FALSE;

	if ( NULL == pstThread->pszUrl || NULL == pstThread->pszDestFile )
	{
		if ( pstThread->pszError )
			_tcscpy( pstThread->pszError, _T("���������ַ������") );
		return HTTP_THREAD_FAIL;
	}
	else if ( 0 == _tcslen(pstThread->pszUrl) || 0 == _tcslen(pstThread->pszDestFile) )
	{
		if ( pstThread->pszError )
			_tcscpy( pstThread->pszError, _T("������������пյĲ���") );
		return HTTP_THREAD_FAIL;
	}

	HINTERNET hInternet	= NULL;
	HINTERNET hHttpConnect	= NULL;

	DWORD dwTotalRead	= 0;
	DWORD dwBufferSize	= 0;
	DWORD dwBytesRead	= 0;
	LPVOID pBuffer		= NULL;
	FILE * fp		= NULL;

	TCHAR szAgent[ MAX_PATH ]	= {0};
	TCHAR szHeader[ 1024 ]		= {0};
	TCHAR szSubHeader[ MAX_PATH ]	= {0};
	
	__try
	{
		if ( pstThread->pszAgent && _tcslen(pstThread->pszAgent) > 0 )
			_sntprintf( szAgent, sizeof(szAgent)/sizeof(TCHAR)-1, _T("%s"), pstThread->pszAgent );
		else
			_sntprintf( szAgent, sizeof(szAgent)/sizeof(TCHAR)-1, _T("%s"), DEFAULT_AGENT_NAME );
		
		if ( pstThread->pszHeader && _tcslen(pstThread->pszHeader) > 0 )
			_sntprintf( szHeader, sizeof(szHeader)/sizeof(TCHAR)-1, _T("%s"), pstThread->pszHeader );
		else
			_sntprintf( szHeader, sizeof(szHeader)/sizeof(TCHAR)-1, _T("%s"), HTTP_ACCEPT );


		hInternet = InternetOpen( szAgent, INTERNET_OPEN_TYPE_PRECONFIG, NULL, NULL, 0 );
		if ( hInternet )
		{
			DWORD dwFlag = INTERNET_FLAG_NO_CACHE_WRITE|INTERNET_FLAG_PRAGMA_NOCACHE|INTERNET_FLAG_RELOAD;
			hHttpConnect = InternetOpenUrl
				(
					hInternet,
					pstThread->pszUrl, 
					szHeader, 
					(DWORD)_tcslen(szHeader), 
					dwFlag,
					0
				);

			dwTotalRead = 0;
			if ( hHttpConnect )
			{
				dwBufferSize	= 300*1024;	// 300K Buffer
				dwBytesRead	= 0;
				pBuffer		= malloc( dwBufferSize );
				if ( NULL == pBuffer )
				{
					if ( pstThread->pszError )
						_tcscpy( pstThread->pszError, _T("�ڴ治��") );
					return HTTP_THREAD_FAIL;
				}
				if ( pstThread->bTextFile )
					fp = _tfopen( pstThread->pszDestFile, _T("wt") );
				else
					fp = _tfopen( pstThread->pszDestFile, _T("wb") );
				if ( fp )
				{
					while( InternetReadFile( hHttpConnect, pBuffer, dwBufferSize, &dwBytesRead ) )
					{
						if ( dwBytesRead == 0 ) 
							break;
						fwrite( pBuffer, dwBytesRead, 1, fp );
						dwTotalRead += dwBytesRead;
					}
					pstThread->bRet = TRUE;
					fclose( fp );
					if ( 0 == dwTotalRead )
					{
						if ( pstThread->pszError )
							_tcscpy( pstThread->pszError, _T("��ȡָ����ҳ������Ϊ��") );
					}
				}
				else
				{
					if ( pstThread->pszError )
						_tcscpy( pstThread->pszError, _T("�޷����������ļ�����ȷ�ϴ����Ƿ��д") );
				}
				if ( pBuffer )
				{
					free( pBuffer );
					pBuffer = NULL;
				}

				::InternetCloseHandle( hHttpConnect );
			}
			else
			{
				if ( pstThread->pszError )
					_tcscpy( pstThread->pszError, _T("InternetOpenUrl ʧ�ܣ��޷����ӵ�ָ�� URL") );
			}

			::InternetCloseHandle( hInternet );
		}
		else
		{
			if ( pstThread->pszError )
				_tcscpy( pstThread->pszError, _T("InternetOpen ʧ�ܣ�����������������") );
		}
	}
	__except( EXCEPTION_EXECUTE_HANDLER )
	{
		if ( pstThread->pszError )
			_tcscpy( pstThread->pszError, _T("�߳��쳣�����˳�") );
		if ( hHttpConnect )
			::InternetCloseHandle( hHttpConnect );
		if ( hInternet )
			::InternetCloseHandle( hInternet );
	}

	return ( TRUE == pstThread->bRet ) ? HTTP_THREAD_SUCC : HTTP_THREAD_FAIL;

}


//////////////////////////////////////////////////////////////////////////
// �ϴ��ļ�
BOOL CDeHttp::PostFile( LPCTSTR lpcszUrl, STHTTPFORMDATA & stFormData, const HWND hMainWnd,
				LPTSTR lpszResponse, DWORD dwSize, LPTSTR lpszError, DWORD dwTimeout )
{
	if ( FALSE == HaveInternetConnection() )
	{
		if ( lpszError )
			_tcscpy( lpszError, _T("��ǰû����������") );
		return FALSE;
	}

	STHTTPTHREAD stHttpThread;

	stHttpThread.pvThisCls		= this;
	stHttpThread.bRet		= FALSE;
	stHttpThread.pszUrl		= lpcszUrl;
	stHttpThread.stFormData		= &stFormData;
	stHttpThread.hMainWnd		= hMainWnd;
	stHttpThread.pszResponse	= lpszResponse;
	stHttpThread.dwSize		= dwSize;
	stHttpThread.pszError		= lpszError;

	__try
	{
		stHttpThread.hThread = (HANDLE)_beginthreadex
			(
				NULL,
				0,
				(PBEGINTHREADEX_THREADFUNC)PostFileProc,
				&stHttpThread,
				0,
				(PBEGINTHREADEX_THREADID)&stHttpThread.uThreadID
			);
		if ( NULL == stHttpThread.hThread )
			return FALSE;
		
		if ( WAIT_OBJECT_0 != WaitForSingleObject( stHttpThread.hThread, dwTimeout ) )
		{
			TerminateThread( stHttpThread.hThread, 0 );
			while ( WAIT_TIMEOUT == WaitForSingleObject( stHttpThread.hThread, 1000 ) )
			{
				TerminateThread( stHttpThread.hThread, 0 );
			}

			if ( lpszError )
				_tcscpy( lpszError, _T("�������ӳ�ʱ") );
		}
		if ( stHttpThread.hThread )
			CloseHandle( stHttpThread.hThread );
	}
	__except( EXCEPTION_EXECUTE_HANDLER )
	{
		if ( lpszError )
			_tcscpy( lpszError, _T("�߳��쳣�����˳�") );
	}

	return stHttpThread.bRet;
}

DWORD WINAPI CDeHttp::PostFileProc( LPVOID p )
{
	TCHAR szServer[MAX_PATH]	= {0};
	TCHAR szPath[MAX_PATH]		= {0};

	STHTTPTHREAD * pstThread = (STHTTPTHREAD*)p;
	CDeHttp * pCHttp = (CDeHttp*)pstThread->pvThisCls;
	pstThread->bRet = FALSE;

	if ( NULL == pCHttp )
	{
		if ( pstThread->pszError )
			_tcscpy( pstThread->pszError, _T("�ڲ������޷��ҵ����ַ") );
		return HTTP_THREAD_FAIL;
	}
	if ( NULL == pstThread->pszUrl || 0 == _tcslen(pstThread->pszUrl) )
	{
		if ( pstThread->pszError )
			_tcscpy( pstThread->pszError, _T("��������д���") );
		return HTTP_THREAD_FAIL;
	}

#ifdef UNICODE
	STDEHTTPDEPARSEURLW stParseUrl;
#else
	STDEHTTPDEPARSEURL stParseUrl;
#endif

	if ( pCHttp->dehttp_delib_parse_url( pstThread->pszUrl, (DWORD)_tcslen(pstThread->pszUrl), & stParseUrl ) )
	{
		_sntprintf( szServer, sizeof(szServer)/sizeof(TCHAR)-1, _T("%s"), stParseUrl.szHost );
		_sntprintf( szPath, sizeof(szPath)/sizeof(TCHAR)-1, _T("%s?%s"), stParseUrl.szPath, stParseUrl.szQuery );
	}
	else
	{
		if ( pstThread->pszError )
			_tcscpy( pstThread->pszError, _T("�޷����� URL ��ַ") );
		return HTTP_THREAD_FAIL;
	}
	/*
	if ( FALSE == pCHttp->ParseUrl( pstThread->pszUrl, szServer, szPath ) )
	{
		if ( pstThread->pszError )
			_tcscpy( pstThread->pszError, _T("�޷����� URL ��ַ") );
		return HTTP_THREAD_FAIL;
	}
	*/
	
	if ( 0 == _tcslen(szServer) )
	{
		if ( pstThread->pszError )
			_tcscpy( pstThread->pszError, _T("������������Ϊ��") );
		return HTTP_THREAD_FAIL;
	}
	if ( 0 == _tcslen(szPath) )
	{
		if ( pstThread->pszError )
			_tcscpy( pstThread->pszError, _T("����·������Ϊ��") );
		return HTTP_THREAD_FAIL;
	}

	HINTERNET hSession		= NULL;
	HINTERNET hConnect		= NULL;
	HINTERNET hRequest		= NULL;
	BOOL bSendRequest		= FALSE;

	__try
	{
		////////////////////////////////////////////////////////////
		// ��һ������ʼ�� internet DLL �Ự���
		hSession = InternetOpen( DEFAULT_AGENT_NAME, INTERNET_OPEN_TYPE_PRECONFIG, NULL, NULL, 0 );
		if ( hSession )
		{
			////////////////////////////////////////////////////////////
			// �ڶ�������ʼ�� HTTP session ���Ӿ��
			hConnect = ::InternetConnect
				(
					hSession,		// ��ǰinternet�Ự���
					szServer,		// Server Name
					DEFAULT_PORT,		// Server Port
					NULL,			// User Name
					NULL,			// Password
					INTERNET_SERVICE_HTTP,	// Type of service to access
					0,
					0
				);
			if ( hConnect )
			{
				////////////////////////////////////////////////////////////
				// ����������һ�� HTTP �������Ӿ��
				DWORD dwFlags = INTERNET_FLAG_KEEP_CONNECTION|INTERNET_FLAG_NO_CACHE_WRITE|INTERNET_FLAG_FORMS_SUBMIT;
				hRequest =:: HttpOpenRequest
					(
						hConnect,
						HTTP_VERB_POST,		// HTTP Verb
						szPath,			// Uri, Object Name
						HTTP_VERSION,		// Version
						NULL,			// Reference
						NULL,			// Accept Type // &szAcceptType
						dwFlags,
						NULL			// context call-back point
					);
				if ( hRequest && INVALID_HANDLE_VALUE != hRequest )
				{
					////////////////////////////////////////////////////////////
					// ���Ĳ����������ݲ����շ���Ӧ������
					if ( pCHttp->UserHttpSendReqEx( 
							hRequest, *pstThread->stFormData, pstThread->hMainWnd, 
							pstThread->pszError ) )
					{
						DWORD dwBytesRead;
						if ( InternetReadFile( hRequest, pstThread->pszResponse, pstThread->dwSize-sizeof(TCHAR), &dwBytesRead ) )
						{
							if ( dwBytesRead > 0 )
								pstThread->bRet = TRUE;
						}
					}

					::InternetCloseHandle( hRequest );
				}
				else
				{
					if ( pstThread->pszError )
						_tcscpy( pstThread->pszError, _T("��һ�� HTTP ������ʧ��") );
				}

				::InternetCloseHandle( hConnect );
			}
			else
			{
				if ( pstThread->pszError )
					_tcscpy( pstThread->pszError, _T("��ʼ�� HTTP Session ʧ��") );
			}

			::InternetCloseHandle( hSession );
		}
		else
		{
			if ( pstThread->pszError )
				_tcscpy( pstThread->pszError, _T("��ʼ�� Internet DLL ʧ��") );
		}
	}
	__except( EXCEPTION_EXECUTE_HANDLER )
	{
		if ( pstThread->pszError )
			_tcscpy( pstThread->pszError, _T("�߳��쳣�����˳�") );
		if ( hRequest )
			::InternetCloseHandle( hRequest );
		if ( hConnect )
			::InternetCloseHandle( hConnect );
		if ( hSession )
			::InternetCloseHandle( hSession );
	}

	return ( TRUE == pstThread->bRet ) ? HTTP_THREAD_SUCC : HTTP_THREAD_FAIL;

}





//////////////////////////////////////////////////////////////////////////
// ����һ���Ƚϴ���ļ����ú����ܳ�ʱ����������
BOOL CDeHttp::DownloadFile( LPCTSTR lpcszUrl, LPTSTR lpszDestFile, const HWND hMainWnd, BOOL bAddNoise, LPTSTR lpszError, DWORD dwTimeout )
{
	if ( FALSE == HaveInternetConnection() )
	{
		if ( lpszError )
			_tcscpy( lpszError, _T("��ǰû����������") );
		return FALSE;
	}

	if ( NULL == lpcszUrl || 0 == _tcslen(lpcszUrl) )
	{
		if ( lpszError )
			_tcscpy( lpszError, _T("��Ҫ���������ص� URL ����������") );
		return HTTP_THREAD_FAIL;
	}
	if ( NULL == lpszDestFile || 0 == _tcslen(lpszDestFile) )
	{
		if ( lpszError )
			_tcscpy( lpszError, _T("���ص����ص��ļ�������������") );
		return HTTP_THREAD_FAIL;
	}

	STHTTPTHREAD stHttpThread;

	stHttpThread.pvThisCls		= this;
	stHttpThread.bRet		= FALSE;
	stHttpThread.pszUrl		= lpcszUrl;
	stHttpThread.pszDestFile	= lpszDestFile;
	stHttpThread.pszError		= lpszError;
	stHttpThread.hMainWnd		= hMainWnd;
	stHttpThread.bAddNoise		= bAddNoise;
	stHttpThread.dwTimeout		= dwTimeout;

	stHttpThread.hThread = (HANDLE)_beginthreadex
		(
			NULL,
			0,
			(PBEGINTHREADEX_THREADFUNC)DownloadFileProc,
			&stHttpThread,
			0,
			(PBEGINTHREADEX_THREADID)&stHttpThread.uThreadID
		);
	if ( NULL == stHttpThread.hThread )
		return FALSE;
	
	if ( WAIT_OBJECT_0 != WaitForSingleObject( stHttpThread.hThread, dwTimeout ) )
	{
		TerminateThread( stHttpThread.hThread, 0 );
		while ( WAIT_TIMEOUT == WaitForSingleObject( stHttpThread.hThread, 1000 ) )
		{
			TerminateThread( stHttpThread.hThread, 0 );
		}

		if ( lpszError )
			_tcscpy( lpszError, _T("�������ӳ�ʱ") );
	}
	if ( stHttpThread.hThread )
		CloseHandle( stHttpThread.hThread );
	
	return stHttpThread.bRet;
}

DWORD WINAPI CDeHttp::DownloadFileProc( LPVOID p )
{
	CCallback callback;
	STHTTPTHREAD * pstThread = (STHTTPTHREAD*)p;
	pstThread->bRet = FALSE;

	TCHAR szUrlTmp[ DEFAULT_BUF_SIZE ]	= {0};
	TCHAR szUrl[ DEFAULT_BUF_SIZE ]		= {0};
	TCHAR * pszJin				= NULL;

	__try
	{
		_sntprintf( szUrlTmp, sizeof(szUrlTmp)/sizeof(TCHAR)-1, _T("%s"), pstThread->pszUrl );
		pszJin = _tcsstr( szUrlTmp, _T("#") );
		if ( pszJin )
			szUrlTmp[ (pszJin - szUrlTmp) ] = 0;

		if ( FALSE == pstThread->bAddNoise )
		{
			_sntprintf( szUrl, sizeof(szUrl)/sizeof(TCHAR)-1, _T("%s"), szUrlTmp );
		}
		else
		{
			if ( _tcsstr( szUrlTmp, CHECK_NOISE_FLAG ) )
			{
				_sntprintf( szUrl, sizeof(szUrl)/sizeof(TCHAR)-1,
					_T("%s%s%d"), szUrlTmp, URL_NOISE_2, GetTickCount() );
			}
			else
			{
				_sntprintf( szUrl, sizeof(szUrl)/sizeof(TCHAR)-1,
					_T("%s%s%d"), szUrlTmp, URL_NOISE_1, GetTickCount() );
			}
		}

		callback.m_hWnd		= pstThread->hMainWnd;
		callback.m_dwTimeout	= pstThread->dwTimeout;
		callback.m_dwStart	= GetTickCount();

		HRESULT hRet = URLDownloadToFile( NULL, szUrl, pstThread->pszDestFile, 0, &callback );
		if ( S_OK ==  hRet)
		{
			// ���سɹ�
			pstThread->bRet = TRUE;
		}
		else
		{
			// ����ʧ��
			DWORD dwErr = GetLastError();
			pstThread->bRet = FALSE;
		}
	}
	__except( EXCEPTION_EXECUTE_HANDLER )
	{
		// �����쳣ʧ��
		if ( pstThread->pszError )
			_tcscpy( pstThread->pszError, _T("�߳��쳣�����˳�") );
	}

	// �������ֵ��ʵû��ʲô����
	return ( TRUE == pstThread->bRet ) ? HTTP_THREAD_SUCC : HTTP_THREAD_FAIL;
}



//////////////////////////////////////////////////////////////////////////
/*
	�������ַ�����FORM����

	����������
		strInputΪ���봮����Ҫ����url�ķ�form����
		nInputLenΪ���봮�ĳ��ȣ�-1��ʾ���� '\0' ��β
		strOutBufΪ�����������
		nOutBufLenΪ�����������С��������β�� '\0'

	����ֵ��
		������ĳ��ȣ���������β��0
	
	���nOutBufLen�ĳ���Ϊ0���򷵻���Ҫ������������Ĵ�С��������β��0
*/
INT CDeHttp::UrlEncode( LPCTSTR lpcszInput, INT nInputLen, LPTSTR lpszOutBuf, INT nOutBufLen )
{
	unsigned char cCur;
	TCHAR * pDst		= NULL;
	INT   i			= 0;
	INT   nDstLen		= 0;
	TCHAR szBuf[4]		= {0};
	INT   nBufLen		= 0;

	__try
	{
		if ( -1 == nInputLen )
			nInputLen = (INT)_tcslen(lpcszInput);

		if ( 0 == nOutBufLen )
		{
			nDstLen = 0;
			for ( i = 0; i < nInputLen; i++ )
			{
				cCur = lpcszInput[i];
				if ( ' ' == cCur )
					nDstLen++;
				else if ( !isalnum(cCur) )
					nDstLen += 3;
				else
					nDstLen ++;
			}
			return nDstLen + 1;
		}

		pDst = lpszOutBuf;
		nDstLen = 0;
		for ( i = 0; i < nInputLen; i++ )
		{
			cCur = lpcszInput[i];
			if ( ' ' == cCur )
			{
				*szBuf = '+';
				*(szBuf+1) = 0;
			}
			else if ( ! isalnum(cCur) )
			{
				_sntprintf( szBuf, sizeof(szBuf)/sizeof(TCHAR)-1, _T("%%%2.2X"), cCur );
			}	
			else
			{
				*szBuf = cCur;
				*(szBuf+1) = 0;
			}
			nBufLen = (INT)_tcslen( szBuf );
			if ( nBufLen + nDstLen >= nOutBufLen )
				break;
			_tcscpy( pDst, szBuf );
			nDstLen += nBufLen;
			pDst += nBufLen;
		}
		return (INT)_tcslen(lpszOutBuf);
	}
	__except( EXCEPTION_EXECUTE_HANDLER )
	{
		return 0;
	}
}





//////////////////////////////////////////////////////////////////////////
// ˽�к���
//////////////////////////////////////////////////////////////////////////


BOOL CDeHttp::UserHttpSendReqEx( HINTERNET hRequest, STHTTPFORMDATA & stFormData, HWND hMainWnd, TCHAR * pszError )
{
	////////////////////////////////////////////////////////////
	HANDLE hFile				= NULL;
	DWORD dwFileSize			= 0;
	hFile = CreateFile( stFormData.szUserFileValue, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, 0, 0 );
	if ( INVALID_HANDLE_VALUE == hFile )
	{
		if ( pszError )
			_sntprintf( pszError, MAX_PATH-1, _T("�޷����ļ���%s"), stFormData.szUserFileValue );
		return FALSE;
	}
	else
	{
		dwFileSize = ::GetFileSize( hFile, NULL );
		CloseHandle( hFile );
	}


	////////////////////////////////////////////////////////////
	DWORD dwTotalPostSize			= 0;
	TCHAR szFormExtra[ MAX_PATH ]		= {0};
	TCHAR szFormUserFile[ MAX_PATH ]	= {0};
	TCHAR szFormFiles[ MAX_PATH ]		= {0};
	TCHAR szFormSubmit[ MAX_PATH ]		= {0};

	dwTotalPostSize = 0;

	// 1 form -> files
	if ( 0 == _tcslen(stFormData.szFilesInput) )
	{	
		_tcscpy( stFormData.szFilesInput, DEFAULT_FORM_INPUT_FILES );
	}

	_sntprintf( szFormFiles, sizeof(szFormFiles)/sizeof(TCHAR)-1,
		_T("--%s\r\nContent-Disposition: form-data; name=\"%s\"\r\n\r\n%s\r\n\r\n"),
		DEFAULT_BOUNDRY_TAG,
		stFormData.szFilesInput, stFormData.szFilesValue );
	dwTotalPostSize += (DWORD)_tcslen( szFormFiles );

	// 2 form -> Extra
	_sntprintf( szFormExtra, sizeof(szFormExtra)/sizeof(TCHAR)-1,
		_T("--%s\r\nContent-Disposition: form-data; name=\"%s\"\r\n\r\n%s\r\n\r\n"),
		DEFAULT_BOUNDRY_TAG,
		stFormData.szExtraInput, stFormData.szExtraValue );
	dwTotalPostSize += _tcslen( szFormExtra );

	// 3 form -> userfile
	if ( 0 == _tcslen(stFormData.szUserFileInput) )
		_tcscpy( stFormData.szUserFileInput, DEFAULT_FORM_INPUT_USERFILE );
	_sntprintf( szFormUserFile, sizeof(szFormUserFile)/sizeof(TCHAR)-1,
		_T("--%s\r\nContent-Disposition: form-data; name=\"%s\"; filename=\"%s\"\r\nContent-Type: */*\r\n%s\r\n\r\n"),
		DEFAULT_BOUNDRY_TAG,
		stFormData.szUserFileInput, stFormData.szUserFileValue,
		_T("Content-Type: application/octet-stream"));
	dwTotalPostSize += _tcslen( szFormUserFile );
	dwTotalPostSize += dwFileSize;

	// 4 form -> submit
	_sntprintf( szFormSubmit, sizeof(szFormSubmit)/sizeof(TCHAR)-1,
		_T("\r\n\r\n--%s\r\nContent-Disposition: form-data; name=\"submit\"\r\n\r\nsubmit\r\n--%s--"),
		DEFAULT_BOUNDRY_TAG, DEFAULT_BOUNDRY_TAG );
	dwTotalPostSize += _tcslen( szFormSubmit );


	TCHAR szHttpHeader[ DEFAULT_BUF_SIZE ]	= {0};

	_tcscpy( szHttpHeader, _T("") );
	if ( TRUE == stFormData.bGZipOpen )
	{
		_tcscpy( szHttpHeader, _T("Accept-Encoding: gzip, deflate\r\n") );
	}
	_tcscat( szHttpHeader, _T("Accept-Language: zh-cn\r\n") );
	_tcscat( szHttpHeader, _T("Content-Type: multipart/form-data; boundary=") );
	_tcscat( szHttpHeader, DEFAULT_BOUNDRY_TAG );
	_tcscat( szHttpHeader, _T("\r\n") );


	////////////////////////////////////////////////////////////
	INTERNET_BUFFERS BufferIn;

	BufferIn.dwStructSize		= sizeof( INTERNET_BUFFERS ); // Must be set or error will occur
	BufferIn.Next			= NULL; 
	BufferIn.lpcszHeader		= szHttpHeader;
	BufferIn.dwHeadersLength	= (DWORD)_tcslen(szHttpHeader);
	BufferIn.dwHeadersTotal		= (DWORD)_tcslen(szHttpHeader);
	BufferIn.lpvBuffer		= NULL;
	BufferIn.dwBufferLength		= 0;
	BufferIn.dwBufferTotal		= dwTotalPostSize; // This is the only member used other than dwStructSize
	BufferIn.dwOffsetLow		= 0;
	BufferIn.dwOffsetHigh		= 0;

	if ( FALSE == ::HttpSendRequestEx( hRequest, &BufferIn, NULL, 0, 0) )
	{
		_sntprintf( pszError, MAX_PATH-1, _T("Error on HttpSendRequestEx %d"), GetLastError() );
		return FALSE;
	}


	BOOL bWriteFile				= FALSE;
	DWORD dwWritten				= 0;
	DWORD dwRead				= 0;
	DWORD dwFileByteSent			= 0;
	TCHAR szFileBuffer[ DEFAULT_BUF_SIZE ] = {0};
	


	// 1 sending .......... <files> �ļ����б�
	bWriteFile = ::InternetWriteFile( hRequest, 
		szFormFiles, (DWORD)_tcslen(szFormFiles), &dwWritten );

	// 2 sending .......... <Extra> һЩ��������
	bWriteFile = ::InternetWriteFile( hRequest, 
		szFormExtra, (DWORD)_tcslen(szFormExtra), &dwWritten );

	// 3 sending .......... <userfile> �ļ�
	bWriteFile = ::InternetWriteFile( hRequest, 
		szFormUserFile, (DWORD)_tcslen(szFormUserFile), &dwWritten );

	hFile = ::CreateFile( stFormData.szUserFileValue, 
				GENERIC_READ, FILE_SHARE_READ, 0, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, 0 );
	if ( hFile && INVALID_HANDLE_VALUE != hFile )
	{
		while ( ::ReadFile( hFile, szFileBuffer, sizeof(szFileBuffer), &dwRead, 0 ) && dwRead > 0 )
		{
			dwWritten = 0;
			bWriteFile = ::InternetWriteFile( hRequest, szFileBuffer, dwRead, &dwWritten );
			dwFileByteSent += dwRead;
			memset( szFileBuffer, 0, sizeof(szFileBuffer) );
			
			if ( hMainWnd )
				::PostMessage( hMainWnd, UM_HTTP_PREGRESS, (INT)(dwFileByteSent/1024), (INT)(dwFileSize/1024) );
		}
		if ( hFile )
			::CloseHandle( hFile );
	}

	// 4 sending .......... <submit>
	bWriteFile = ::InternetWriteFile( hRequest, szFormSubmit, (DWORD)_tcslen(szFormSubmit), &dwWritten );


	if ( FALSE == ::HttpEndRequest( hRequest, NULL, HSR_ASYNC | HSR_INITIATE, 0 ) )
	{
		_sntprintf( pszError, MAX_PATH-1, _T("Error on HttpEndRequest %d"), GetLastError() );
		return FALSE;
	}

	if ( hMainWnd )
		::PostMessage( hMainWnd, UM_HTTP_COMPLETE, 0, 0 );


	return TRUE;
}




BOOL CDeHttp::ParseUrl( const TCHAR * pszUrl, TCHAR * pszServer, TCHAR * pszPath )
{
	if ( NULL == pszUrl || NULL == pszServer || NULL == pszPath )
		return FALSE;

	DWORD i					= 0;
	DWORD j					= 0;
	DWORD dwFlags				= 0;		//"/"�ĸ���
	
	if ( pszUrl == _tcsstr(pszUrl,_T("http://")) )
		j = 7;		// ��HTTP��ӵ�7����ʼ����
	else
		j = 0;		// û�з���
	
	for ( i = j; i < _tcslen(pszUrl); i++ )
	{
		if ( 1 == dwFlags || '/' == pszUrl[i] )
		{
			if ( 1 == dwFlags )
			{
				pszPath[ i-j-1 ] = pszUrl[i];
			}
			else
			{
				j = i;
				dwFlags = 1;
			}
		}
		else
		{
			pszServer[i-j] = pszUrl[i];
		}
	}

	return TRUE;
}






/**
 *	parse url
 */
BOOL CDeHttp::dehttp_delib_parse_url(LPCSTR lpcszUrl, DWORD dwUrlLen, STDEHTTPDEPARSEURL * pstParseUrl )
{
	return dehttp_delib_parse_urlA( lpcszUrl, dwUrlLen, pstParseUrl );
}
BOOL CDeHttp::dehttp_delib_parse_urlW( LPCWSTR lpcwszUrl, DWORD dwUrlLen, STDEHTTPDEPARSEURLW * pstParseUrlW )
{
	if ( NULL == lpcwszUrl || 0 == lpcwszUrl[0] || 0 == dwUrlLen )
	{
		return FALSE;
	}
	if ( NULL == pstParseUrlW )
	{
		return FALSE;
	}

	BOOL bRet;
	CHAR * pszTmpUrl;
	STDEHTTPDEPARSEURL stParseUrl;

	bRet		= FALSE;

	__try
	{
		pszTmpUrl	= new CHAR[ dwUrlLen + 1 ];
		if ( pszTmpUrl )
		{
			memset( pszTmpUrl, 0, dwUrlLen + 1 );
			memset( &stParseUrl, 0, sizeof(stParseUrl) );
			
			WideCharToMultiByte( CP_ACP, 0, lpcwszUrl, dwUrlLen, pszTmpUrl, dwUrlLen, NULL, NULL );
			if ( dehttp_delib_parse_urlA( pszTmpUrl, strlen(pszTmpUrl), &stParseUrl ) )
			{
				memset( pstParseUrlW, 0, sizeof(STDEHTTPDEPARSEURLW) );
				MultiByteToWideChar( CP_ACP, 0, stParseUrl.szFragment, strlen(stParseUrl.szFragment), pstParseUrlW->szFragment, sizeof(pstParseUrlW->szFragment)/2 );
				MultiByteToWideChar( CP_ACP, 0, stParseUrl.szHost, strlen(stParseUrl.szHost), pstParseUrlW->szHost, sizeof(pstParseUrlW->szHost)/2 );
				MultiByteToWideChar( CP_ACP, 0, stParseUrl.szPass, strlen(stParseUrl.szPass), pstParseUrlW->szPass, sizeof(pstParseUrlW->szPass)/2 );
				MultiByteToWideChar( CP_ACP, 0, stParseUrl.szPath, strlen(stParseUrl.szPath), pstParseUrlW->szPath, sizeof(pstParseUrlW->szPath)/2 );
				MultiByteToWideChar( CP_ACP, 0, stParseUrl.szQuery, strlen(stParseUrl.szQuery), pstParseUrlW->szQuery, sizeof(pstParseUrlW->szQuery)/2 );
				MultiByteToWideChar( CP_ACP, 0, stParseUrl.szScheme, strlen(stParseUrl.szScheme), pstParseUrlW->szScheme, sizeof(pstParseUrlW->szScheme)/2 );
				MultiByteToWideChar( CP_ACP, 0, stParseUrl.szUser, strlen(stParseUrl.szUser), pstParseUrlW->szUser, sizeof(pstParseUrlW->szUser)/2 );

				bRet = TRUE;
			}

			delete [] pszTmpUrl;
			pszTmpUrl = NULL;
		}
	}
	__except( EXCEPTION_EXECUTE_HANDLER )
	{
	}

	return bRet;
}
BOOL CDeHttp::dehttp_delib_parse_urlA( LPCSTR lpcszUrl, DWORD dwUrlLen, STDEHTTPDEPARSEURL * pstParseUrl )
{
	//
	//	lpcszUrl		- [in]  URL
	//	dwUrlLen		- [in]  URL ����
	//	pstParseUrl		- [out] ����ֵ
	//	RETURN			- TRUE / FALSE
	//

	if ( NULL == lpcszUrl || 0 == lpcszUrl[0] )
		return FALSE;
	if ( NULL == pstParseUrl )
		return FALSE;


	BOOL bRet	= FALSE;
	UINT uBufLen	= 0;

	char port_buf[6];
//	php_url * ret = (php_url*)calloc( 1, sizeof(php_url) );
	char const *s, *e, *p, *pp, *ue;

	s = lpcszUrl;
	ue = s + dwUrlLen;

	//
	//	parse scheme
	//
	if ( ( e = (char*)memchr( s, ':', dwUrlLen ) ) && ( e - s ) )
	{
		//	validate scheme
		p = s;
		while ( p < e )
		{
			//	scheme = 1*[ lowalpha | digit | "+" | "-" | "." ]
			if ( ! isalpha(*p) && !isdigit(*p) && *p != '+' && *p != '.' && *p != '-')
			{
				if ( e + 1 < ue )
				{
					goto parse_port;
				}
				else
				{
					goto just_path;
				}
			}
			p++;
		}

		if ( *( e + 1 ) == '\0' )
		{
			//	only scheme is available
			dehttp_delibfunc_memcpy( s, ( e - s ), pstParseUrl->szScheme, sizeof( pstParseUrl->szScheme ), TRUE );
			goto end;
		}

		/**
		 *	certain schemas like mailto: and zlib: may not have any / after them
		 *	this check ensures we support those.
		 */
		if ( *(e+1) != '/' )
		{
			/**
			 *	check if the data we get is a port this allows us to 
			 *	correctly parse things like a.com:80
			 */
			p = e + 1;
			while ( isdigit(*p) )
			{
				p++;
			}

			if ( (*p) == '\0' || *p == '/' )
			{
				goto parse_port;
			}

			dehttp_delibfunc_memcpy( s, (e-s), pstParseUrl->szScheme, sizeof( pstParseUrl->szScheme ), TRUE );

			dwUrlLen -= ++e - s;
			s = e;
			goto just_path;
		}
		else
		{
			dehttp_delibfunc_memcpy( s, (e-s), pstParseUrl->szScheme, sizeof( pstParseUrl->szScheme ), TRUE );

			if ( *(e+2) == '/' )
			{
				s = e + 3;
				if ( 0 == strnicmp( "file", pstParseUrl->szScheme, sizeof("file") ) )
				{
					if ( *(e + 3) == '/' )
					{
						/**
						 *	support windows drive letters as in:
						 *	file:///c:/somedir/file.txt
						 */
						if ( *(e + 5) == ':' )
						{
							s = e + 4;
						}
						goto nohost;
					}
				}
			}
			else
			{
				if ( 0 == strnicmp( "file", pstParseUrl->szScheme, sizeof("file") ) )
				{
					s = e + 1;
					goto nohost;
				}
				else
				{
					dwUrlLen -= ++e - s;
					s = e;
					goto just_path;
				}	
			}
		}
	}
	else if (e)
	{
		//
		//	no scheme, look for port
		//
		parse_port:
		p = e + 1;
		pp = p;

		while ( pp-p < 6 && isdigit(*pp) )
		{
			pp++;
		}

		if ( pp - p < 6 && (*pp == '/' || *pp == '\0') )
		{
			memcpy( port_buf, p, (pp-p) );
			port_buf[pp-p] = '\0';
			pstParseUrl->uPort = atoi(port_buf);
		}
		else
		{
			goto just_path;
		}
	}
	else
	{
		just_path:
		ue = s + dwUrlLen;
		goto nohost;
	}
	
	e = ue;
	
	if ( ! ( p = (char*)memchr(s, '/', (ue - s)) ) )
	{
		if ( ( p = (char*)memchr(s, '?', (ue - s)) ) )
		{
			e = p;
		}
		else if ( ( p = (char*)memchr(s, '#', (ue - s) ) ) )
		{
			e = p;
		}
	}
	else
	{
		e = p;
	}	

	//
	//	check for login and password
	//
	if ( ( p = (char*)memchr(s, '@', (e-s)) ) )
	{
		if ( ( pp = (char*)memchr(s, ':', (p-s)) ) )
		{
			if ( ( pp - s ) > 0 )
			{
				//ret->user = zend_strndup( s, (pp-s) );
				dehttp_delibfunc_memcpy( s, (pp-s), pstParseUrl->szUser, sizeof( pstParseUrl->szUser ), TRUE );
			}

			pp++;
			if ( p - pp > 0 )
			{
				//ret->pass = zend_strndup( pp, (p-pp) );
				dehttp_delibfunc_memcpy( pp, (p-pp), pstParseUrl->szPass, sizeof( pstParseUrl->szPass ), TRUE );
			}
		}
		else
		{
			//ret->user = zend_strndup(s, (p-s));
			dehttp_delibfunc_memcpy( s, (p-s), pstParseUrl->szUser, sizeof( pstParseUrl->szUser ), TRUE );
		}

		s = p + 1;
	}

	//
	//	check for port
	//
	if ( *s == '[' && *(e-1) == ']' )
	{
		/*
			Short circuit portscan, 
			we're dealing with an 
			IPv6 embedded address
		*/
		p = s;
	}
	else
	{
		/*
			memrchr is a GNU specific extension
			Emulate for wide compatability
		*/
		for ( p = e; *p != ':' && p >= s; p-- );
		{
		}
	}

	if ( p >= s && *p == ':' )
	{
		if ( ! pstParseUrl->uPort )
		{
			p ++;
			if ( e - p > 5 )
			{
				//	port cannot be longer then 5 characters
				//STR_FREE( ret->scheme );
				//STR_FREE( ret->user );
				//STR_FREE( ret->pass );
				//free( ret );
				return FALSE;
			}
			else if ( e - p > 0 )
			{
				memcpy(port_buf, p, (e-p));
				port_buf[e-p] = '\0';
				pstParseUrl->uPort = atoi(port_buf);
			}
			p--;
		}	
	}
	else
	{
		p = e;
	}
	
	//
	//	check if we have a valid host, if we don't reject the string as url
	//
	if ( ( p - s ) < 1 )
	{
		//STR_FREE(ret->scheme);
		//STR_FREE(ret->user);
		//STR_FREE(ret->pass);
		//free(ret);
		return FALSE;
	}

	//ret->host = estrndup(s, (p-s));
	dehttp_delibfunc_memcpy( s, (p-s), pstParseUrl->szHost, sizeof( pstParseUrl->szHost ), TRUE );

	if ( e == ue )
	{
		return TRUE;
	}

	s = e;
	
	nohost:
	
	if ( ( p = (char*)memchr( s, '?', (ue - s) ) ) )
	{
		pp = strchr( s, '#' );
		
		if ( pp && pp < p )
		{
			p = pp;
			pp = strchr( pp + 2, '#' );
		}

		if ( p - s )
		{
			//ret->path = estrndup(s, (p-s));
			dehttp_delibfunc_memcpy( s, (p-s), pstParseUrl->szPath, sizeof( pstParseUrl->szPath ), TRUE );
		}	

		if ( pp )
		{
			if ( pp - ++p )
			{
				//ret->query = estrndup(p, (pp-p));
				dehttp_delibfunc_memcpy( p, (pp-p), pstParseUrl->szQuery, sizeof( pstParseUrl->szQuery ), TRUE );
			}
			p = pp;
			goto label_parse;
		}
		else if ( ++p - ue )
		{
			//ret->query = estrndup(p, (ue-p));
			dehttp_delibfunc_memcpy( p, (ue-p), pstParseUrl->szQuery, sizeof( pstParseUrl->szQuery ), TRUE );
		}
	}
	else if ( ( p = (char*)memchr(s, '#', (ue - s)) ) )
	{
		if ( p - s )
		{
			//ret->path = estrndup(s, (p-s));
			dehttp_delibfunc_memcpy( s, (p-s), pstParseUrl->szPath, sizeof( pstParseUrl->szPath ), TRUE );
		}

		label_parse:
		p++;

		if ( ue - p )
		{
			//ret->fragment = estrndup(p, (ue-p));
			dehttp_delibfunc_memcpy( p, (ue-p), pstParseUrl->szFragment, sizeof( pstParseUrl->szFragment ), TRUE );
		}
	}
	else
	{
		//ret->path = estrndup(s, (ue-s));
		dehttp_delibfunc_memcpy( s, (ue-s), pstParseUrl->szPath, sizeof( pstParseUrl->szPath ), TRUE );
	}
end:
	return TRUE;
}


/**
 *	�滻���ɼ��Ŀ��Ʒ���Ϊ��_��
 */
TCHAR * CDeHttp::dehttp_delibfunc_replace_controlchars( LPSTR lpszStr, UINT uLen )
{
	//
	//	lpszStr		- [in]
	//	uLen		- [in]
	//
	
	if ( NULL == lpszStr )
	{
		return NULL;
	}
	
	unsigned char *s = (unsigned char *)lpszStr;
	unsigned char *e = (unsigned char *)lpszStr + uLen;
	
	while ( s < e )
	{
		if ( iscntrl( *s ) )
		{
			*s = '_';
		}
		s++;
	}
	
	return lpszStr;
}

/**
*	�����ַ���
*/
BOOL CDeHttp::dehttp_delibfunc_memcpy( LPCTSTR lpcszSrc, DWORD dwSrcLen, LPTSTR lpszDst, DWORD dwDstSize, BOOL bRepCtlChr /*= FALSE*/ )
{
	if ( NULL == lpcszSrc || NULL == lpszDst )
		return FALSE;
	if ( 0 == dwSrcLen || 0 == dwDstSize )
		return FALSE;
	
	UINT uLen = min( dwSrcLen, dwDstSize-sizeof(TCHAR) );
	
	memcpy( lpszDst, lpcszSrc, uLen );
	lpszDst[ uLen ] = 0;
	
	if ( bRepCtlChr )
	{
		dehttp_delibfunc_replace_controlchars( lpszDst, uLen );
	}
	
	return TRUE;
}