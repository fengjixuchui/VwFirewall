// http.cpp : Defines the entry point for the DLL application.
//

#include "stdafx.h"
#include "VwHttp.h"
#include "BindStatusCallback.h"
#include <stdio.h>
#include <stdlib.h>


//////////////////////////////////////////////////////////////////////////
//
// �ж��Ƿ����Internet����
BOOL CVwHttp::HaveInternetConnection()
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
BOOL CVwHttp::GetHttpInfo( LPCTSTR lpcszUrl, STHTTPINFO & stHttp, LPTSTR lpszError, DWORD dwTimeout )
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

DWORD WINAPI CVwHttp::GetHttpInfoProc( LPVOID p )
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
	TCHAR szQueryBuf[ 64 ]	= {0};
	DWORD dwQueryBufLen	= sizeof(szQueryBuf);

	__try
	{
		hSession = ::InternetOpen( DEFAULT_AGENT_NAME, PRE_CONFIG_INTERNET_ACCESS, "", INTERNET_INVALID_PORT_NUMBER, 0 );
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
					pstThread->stHttpInfo->dwHttpFileLen = (DWORD)atol(szQueryBuf);
				}

				memset( szQueryBuf, 0, sizeof(szQueryBuf) );
				dwQueryBufLen = sizeof(szQueryBuf);
				bQuerySucc = ::HttpQueryInfo ( hHttpFile, HTTP_QUERY_STATUS_CODE, szQueryBuf, &dwQueryBufLen, NULL );
				if ( bQuerySucc && _tcslen(szQueryBuf) > 0 )
				{
					pstThread->stHttpInfo->dwHttpStatusCode = (DWORD)atol(szQueryBuf);
				}
				
				memset( szQueryBuf, 0, sizeof(szQueryBuf) );
				dwQueryBufLen = sizeof(szQueryBuf);
				bQuerySucc = ::HttpQueryInfo ( hHttpFile, HTTP_QUERY_LAST_MODIFIED, szQueryBuf, &dwQueryBufLen, NULL );
				if ( bQuerySucc && _tcslen(szQueryBuf) > 0 )
				{
					_sntprintf
					( 
						pstThread->stHttpInfo->szHttpLastModified, 
						sizeof(pstThread->stHttpInfo->szHttpLastModified)-sizeof(TCHAR), 
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
						sizeof(pstThread->stHttpInfo->szHttpServerInfo)-sizeof(TCHAR),
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
BOOL CVwHttp::IsUrlConnectOk( LPCTSTR lpcszUrl, LPTSTR lpszError, DWORD dwTimeout )
{
	if ( NULL == lpcszUrl )
		return FALSE;
	else if ( 0 == _tcslen(lpcszUrl) )
		return FALSE;

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
			_sntprintf( lpszError, MAX_PATH-sizeof(TCHAR), _T("%d"), dwStatusCode );
		return FALSE;
	}

}


//////////////////////////////////////////////////////////////////////////
// ��ȡĳ�� URL ������״̬����
DWORD CVwHttp::GetStatusCode( LPCTSTR lpcszUrl, LPTSTR lpszError, DWORD dwTimeout )
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
			_tcscpy( lpszError, _T("�߳��쳣�����˳�") );
	}

	return stHttpThread.dwRet;
}
DWORD WINAPI CVwHttp::GetStatusCodeProc( LPVOID p )
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
				// �õ�״̬��
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
					pstThread->dwRet = (DWORD)atol(szQueryBuf);

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

	return pstThread->dwRet;
}

/**
 *	��ȡ���� Header
 */
BOOL CVwHttp::GetHttpHeader( LPCTSTR lpcszUrl, LPTSTR lpszHeader, DWORD dwSize, LPTSTR lpszError, DWORD dwTimeout )
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
DWORD WINAPI CVwHttp::GetHttpHeaderProc( LPVOID p )
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
BOOL CVwHttp::GetHttpHeaderEx(
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
DWORD WINAPI CVwHttp::GetHttpHeaderExProc( LPVOID p )
{
	STHTTPTHREAD * pstThread = (STHTTPTHREAD*)p;
	CVwHttp * pCHttp = (CVwHttp*)pstThread->pvThisCls;

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

	const TCHAR * szAcceptType = _T(HTTP_ACCEPT_TYPE);
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
	STPARSEURL stParseUrl;

	if ( _vwfunc_parse_url( pstThread->pszUrl, _tcslen(pstThread->pszUrl), & stParseUrl ) )
	{
		_sntprintf( szServer, sizeof(szServer)-sizeof(TCHAR), "%s", stParseUrl.szHost );
		_sntprintf( szPath, sizeof(szPath)-sizeof(TCHAR), "%s?%s", stParseUrl.szPath, stParseUrl.szQuery );
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
							_sntprintf( szHeadLine, sizeof(szHeadLine)-sizeof(TCHAR), "%s", (*it).szHeadLine );
							bAddHeaders &= ::HttpAddRequestHeaders
							(
								hRequest,
								szHeadLine,
								_tcslen( szHeadLine ),
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
BOOL CVwHttp::GetResponse( LPCTSTR lpcszUrl, LPTSTR lpszResponse, DWORD dwSize, LPTSTR lpszError, DWORD dwTimeout )
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
DWORD WINAPI CVwHttp::GetResponseProc( LPVOID p )
{
	STHTTPTHREAD * pstThread = (STHTTPTHREAD*)p;
	CVwHttp * pCHttp = (CVwHttp*)pstThread->pvThisCls;
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
	STPARSEURL stParseUrl;

	if ( _vwfunc_parse_url( pstThread->pszUrl, _tcslen(pstThread->pszUrl), & stParseUrl ) )
	{
		_sntprintf( szServer, sizeof(szServer)-sizeof(TCHAR), "%s", stParseUrl.szHost );
		_sntprintf( szPath, sizeof(szPath)-sizeof(TCHAR), "%s?%s", stParseUrl.szPath, stParseUrl.szQuery );
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
BOOL CVwHttp::GetResponseEx( LPCTSTR lpcszUrl, vector<STHTTPHEADERVAR> & vcHttpHeaderVar, LPTSTR lpszResponse, DWORD dwSize, LPTSTR lpszError, DWORD dwTimeout )
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
DWORD WINAPI CVwHttp::GetResponseExProc( LPVOID p )
{
	STHTTPTHREAD * pstThread = (STHTTPTHREAD*)p;
	CVwHttp * pCHttp = (CVwHttp*)pstThread->pvThisCls;
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
	STPARSEURL stParseUrl;

	if ( _vwfunc_parse_url( pstThread->pszUrl, _tcslen(pstThread->pszUrl), & stParseUrl ) )
	{
		_sntprintf( szServer, sizeof(szServer)-sizeof(TCHAR), "%s", stParseUrl.szHost );
		_sntprintf( szPath, sizeof(szPath)-sizeof(TCHAR), "%s?%s", stParseUrl.szPath, stParseUrl.szQuery );
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
							_sntprintf( szHeadLine, sizeof(szHeadLine)-sizeof(TCHAR), "%s", (*it).szHeadLine );
							bAddHeaders &= ::HttpAddRequestHeaders
							(
								hRequest,
								szHeadLine,
								_tcslen( szHeadLine ),
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
BOOL CVwHttp::PostForm( LPCTSTR lpcszUrl, LPTSTR lpszPostData, LPTSTR lpszResponse, DWORD dwSize, LPTSTR lpszError, DWORD dwTimeout )
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

BOOL CVwHttp::PostFormEx( LPCTSTR lpcszUrl, LPTSTR lpszPostData, LPTSTR lpszAgent, LPTSTR lpszHeader, 
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

DWORD WINAPI CVwHttp::PostFormProc( LPVOID p )
{
	STHTTPTHREAD * pstThread = (STHTTPTHREAD*)p;
	CVwHttp * pCHttp = (CVwHttp*)pstThread->pvThisCls;
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

	const TCHAR * szAcceptType = _T(HTTP_ACCEPT_TYPE);
	const TCHAR * szContentType = _T("Content-Type: application/x-www-form-urlencoded\r\n");

	TCHAR szServer[MAX_PATH]	= {0};
	TCHAR szPath[MAX_PATH]		= {0};
	STPARSEURL stParseUrl;
	
	if ( _vwfunc_parse_url( pstThread->pszUrl, _tcslen(pstThread->pszUrl), & stParseUrl ) )
	{
		_sntprintf( szServer, sizeof(szServer)-sizeof(TCHAR), "%s", stParseUrl.szHost );
		_sntprintf( szPath, sizeof(szPath)-sizeof(TCHAR), "%s?%s", stParseUrl.szPath, stParseUrl.szQuery );
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
			_sntprintf( szAgent, sizeof(szAgent)-sizeof(TCHAR), _T("%s"), pstThread->pszAgent );
		}
		else
		{
			_sntprintf( szAgent, sizeof(szAgent)-sizeof(TCHAR), _T("%s"), _T(DEFAULT_AGENT_NAME) );
		}

		if ( pstThread->pszHeader && _tcslen(pstThread->pszHeader) > 0 )
		{
			_sntprintf( szHeader, sizeof(szHeader)-sizeof(TCHAR), _T("%s"), pstThread->pszHeader );
		}
		else
		{
			_sntprintf( szHeader, sizeof(szHeader)-sizeof(TCHAR), _T("%s"), _T(HTTP_ACCEPT) );
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
						_sntprintf( szSubHeader, sizeof(szSubHeader)-sizeof(TCHAR), _T("%s\r\n"), pszMove );
						bAddHeaders &= ::HttpAddRequestHeaders
							(
								hRequest,
								szSubHeader,
								_tcslen(szSubHeader),
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
								_tcslen(szContentType),			// additional HTTP header length
								reinterpret_cast<LPVOID>(pstThread->pszPostData),	// additional data in HTTP Post or HTTP Put
								_tcslen(pstThread->pszPostData)			// additional data length
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
BOOL CVwHttp::GetFile( LPCTSTR lpcszUrl, LPTSTR lpszDestFile, BOOL bTextFile, LPTSTR lpszError, DWORD dwTimeout )
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


BOOL CVwHttp::GetFileEx( LPCTSTR lpcszUrl, LPTSTR lpszAgent, LPTSTR lpszHeader, LPCTSTR lpcszDestFile, BOOL bTextFile, LPTSTR lpszError, DWORD dwTimeout )
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

DWORD WINAPI CVwHttp::GetFileProc( LPVOID p )
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
			_sntprintf( szAgent, sizeof(szAgent)-sizeof(TCHAR), _T("%s"), pstThread->pszAgent );
		else
			_sntprintf( szAgent, sizeof(szAgent)-sizeof(TCHAR), _T("%s"), _T(DEFAULT_AGENT_NAME) );
		
		if ( pstThread->pszHeader && _tcslen(pstThread->pszHeader) > 0 )
			_sntprintf( szHeader, sizeof(szHeader)-sizeof(TCHAR), _T("%s"), pstThread->pszHeader );
		else
			_sntprintf( szHeader, sizeof(szHeader)-sizeof(TCHAR), _T("%s"), _T(HTTP_ACCEPT) );


		hInternet = InternetOpen( _T(szAgent), INTERNET_OPEN_TYPE_PRECONFIG, NULL, NULL, 0 );
		if ( hInternet )
		{
			DWORD dwFlag = INTERNET_FLAG_NO_CACHE_WRITE|INTERNET_FLAG_PRAGMA_NOCACHE|INTERNET_FLAG_RELOAD;
			hHttpConnect = InternetOpenUrl
				(
					hInternet,
					pstThread->pszUrl, 
					szHeader, 
					_tcslen(szHeader), 
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
					fp = fopen( pstThread->pszDestFile, "wt" );
				else
					fp = fopen( pstThread->pszDestFile, "wb" );
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
BOOL CVwHttp::PostFile( LPCTSTR lpcszUrl, STHTTPFORMDATA & stFormData, const HWND hMainWnd,
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

DWORD WINAPI CVwHttp::PostFileProc( LPVOID p )
{
	TCHAR szServer[MAX_PATH]	= {0};
	TCHAR szPath[MAX_PATH]		= {0};

	STHTTPTHREAD * pstThread = (STHTTPTHREAD*)p;
	CVwHttp * pCHttp = (CVwHttp*)pstThread->pvThisCls;
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

	STPARSEURL stParseUrl;
	
	if ( _vwfunc_parse_url( pstThread->pszUrl, _tcslen(pstThread->pszUrl), & stParseUrl ) )
	{
		_sntprintf( szServer, sizeof(szServer)-sizeof(TCHAR), "%s", stParseUrl.szHost );
		_sntprintf( szPath, sizeof(szPath)-sizeof(TCHAR), "%s?%s", stParseUrl.szPath, stParseUrl.szQuery );
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
BOOL CVwHttp::DownloadFile( LPCTSTR lpcszUrl, LPTSTR lpszDestFile, const HWND hMainWnd, BOOL bAddNoise, LPTSTR lpszError, DWORD dwTimeout )
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
		if ( lpszError )
			_tcscpy( lpszError, _T("�������ӳ�ʱ") );
	}
	if ( stHttpThread.hThread )
		CloseHandle( stHttpThread.hThread );
	
	return stHttpThread.bRet;
}

DWORD WINAPI CVwHttp::DownloadFileProc( LPVOID p )
{
	CCallback callback;
	STHTTPTHREAD * pstThread = (STHTTPTHREAD*)p;
	pstThread->bRet = FALSE;

	TCHAR szUrlTmp[ DEFAULT_BUF_SIZE ]	= {0};
	TCHAR szUrl[ DEFAULT_BUF_SIZE ]		= {0};
	TCHAR * pszJin				= NULL;

	__try
	{
		_snprintf( szUrlTmp, sizeof(szUrlTmp)-sizeof(TCHAR), _T("%s"), pstThread->pszUrl );
		pszJin = _tcsstr( szUrlTmp, _T("#") );
		if ( pszJin )
			szUrlTmp[ (pszJin - szUrlTmp) ] = 0;

		if ( FALSE == pstThread->bAddNoise )
		{
			_sntprintf( szUrl, sizeof(szUrl), _T("%s"), szUrlTmp );
		}
		else
		{
			if ( _tcsstr( szUrlTmp, CHECK_NOISE_FLAG ) )
			{
				_sntprintf( szUrl, sizeof(szUrl)-sizeof(TCHAR),
					_T("%s%s%d"), szUrlTmp, URL_NOISE_2, GetTickCount() );
			}
			else
			{
				_sntprintf( szUrl, sizeof(szUrl)-sizeof(TCHAR),
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
INT CVwHttp::UrlEncode( LPCTSTR lpcszInput, INT nInputLen, LPTSTR lpszOutBuf, INT nOutBufLen )
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
			nInputLen = _tcslen(lpcszInput);

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
			else if ( !isalnum(cCur) )
				sprintf( szBuf, _T("%%%2.2X"), cCur );
			else
			{
				*szBuf = cCur;
				*(szBuf+1) = 0;
			}
			nBufLen = _tcslen( szBuf );
			if ( nBufLen + nDstLen >= nOutBufLen )
				break;
			_tcscpy( pDst, szBuf );
			nDstLen += nBufLen;
			pDst += nBufLen;
		}
		return _tcslen(lpszOutBuf);
	}
	__except( EXCEPTION_EXECUTE_HANDLER )
	{
		return 0;
	}
}





//////////////////////////////////////////////////////////////////////////
// ˽�к���
//////////////////////////////////////////////////////////////////////////


BOOL CVwHttp::UserHttpSendReqEx( HINTERNET hRequest, STHTTPFORMDATA & stFormData, HWND hMainWnd, TCHAR * pszError )
{
	////////////////////////////////////////////////////////////
	HANDLE hFile				= NULL;
	DWORD dwFileSize			= 0;
	hFile = CreateFile( stFormData.szUserFileValue, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, 0, 0 );
	if ( INVALID_HANDLE_VALUE == hFile )
	{
		if ( pszError )
			_sntprintf( pszError, MAX_PATH-sizeof(TCHAR), _T("�޷����ļ���%s"), stFormData.szUserFileValue );
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
		_tcscpy( stFormData.szFilesInput, _T(DEFAULT_FORM_INPUT_FILES) );
	_sntprintf( szFormFiles, sizeof(szFormFiles)-sizeof(TCHAR),
		_T("--%s\r\nContent-Disposition: form-data; name=\"%s\"\r\n\r\n%s\r\n\r\n"),
		_T(DEFAULT_BOUNDRY_TAG), 
		_T(stFormData.szFilesInput), _T(stFormData.szFilesValue) );
	dwTotalPostSize += _tcslen( szFormFiles );

	// 2 form -> Extra
	_sntprintf( szFormExtra, sizeof(szFormExtra)-sizeof(TCHAR),
		_T("--%s\r\nContent-Disposition: form-data; name=\"%s\"\r\n\r\n%s\r\n\r\n"),
		_T(DEFAULT_BOUNDRY_TAG), 
		_T(stFormData.szExtraInput), _T(stFormData.szExtraValue) );
	dwTotalPostSize += _tcslen( szFormExtra );

	// 3 form -> userfile
	if ( 0 == _tcslen(stFormData.szUserFileInput) )
		_tcscpy( stFormData.szUserFileInput, _T(DEFAULT_FORM_INPUT_USERFILE) );
	_sntprintf( szFormUserFile, sizeof(szFormUserFile)-sizeof(TCHAR),
		_T("--%s\r\nContent-Disposition: form-data; name=\"%s\"; filename=\"%s\"\r\nContent-Type: */*\r\n%s\r\n\r\n"),
		_T(DEFAULT_BOUNDRY_TAG), 
		_T(stFormData.szUserFileInput), _T(stFormData.szUserFileValue),
		_T("Content-Type: application/octet-stream"));
	dwTotalPostSize += _tcslen( szFormUserFile );
	dwTotalPostSize += dwFileSize;

	// 4 form -> submit
	_sntprintf( szFormSubmit, sizeof(szFormSubmit)-sizeof(TCHAR),
		_T("\r\n\r\n--%s\r\nContent-Disposition: form-data; name=\"submit\"\r\n\r\nsubmit\r\n--%s--"),
		_T(DEFAULT_BOUNDRY_TAG), _T(DEFAULT_BOUNDRY_TAG) );
	dwTotalPostSize += _tcslen( szFormSubmit );


	TCHAR szHttpHeader[ DEFAULT_BUF_SIZE ]	= {0};

	_tcscpy( szHttpHeader, _T("") );
	if ( TRUE == stFormData.bGZipOpen )
	{
		_tcscpy( szHttpHeader, _T("Accept-Encoding: gzip, deflate\r\n") );
	}
	_tcscat( szHttpHeader, _T("Accept-Language: zh-cn\r\n") );
	_tcscat( szHttpHeader, _T("Content-Type: multipart/form-data; boundary=") );
	_tcscat( szHttpHeader, _T(DEFAULT_BOUNDRY_TAG) );
	_tcscat( szHttpHeader, _T("\r\n") );


	////////////////////////////////////////////////////////////
	INTERNET_BUFFERS BufferIn;

	BufferIn.dwStructSize		= sizeof( INTERNET_BUFFERS ); // Must be set or error will occur
	BufferIn.Next			= NULL; 
	BufferIn.lpcszHeader		= _T(szHttpHeader);
	BufferIn.dwHeadersLength	= _tcslen(szHttpHeader);
	BufferIn.dwHeadersTotal		= _tcslen(szHttpHeader);
	BufferIn.lpvBuffer		= NULL;
	BufferIn.dwBufferLength		= 0;
	BufferIn.dwBufferTotal		= dwTotalPostSize; // This is the only member used other than dwStructSize
	BufferIn.dwOffsetLow		= 0;
	BufferIn.dwOffsetHigh		= 0;

	if ( FALSE == ::HttpSendRequestEx( hRequest, &BufferIn, NULL, 0, 0) )
	{
		_sntprintf( pszError, MAX_PATH-sizeof(TCHAR), _T("Error on HttpSendRequestEx %d"), GetLastError() );
		return FALSE;
	}


	BOOL bWriteFile				= FALSE;
	DWORD dwWritten				= 0;
	DWORD dwRead				= 0;
	DWORD dwFileByteSent			= 0;
	TCHAR szFileBuffer[ DEFAULT_BUF_SIZE ] = {0};
	


	// 1 sending .......... <files> �ļ����б�
	bWriteFile = ::InternetWriteFile( hRequest, 
		szFormFiles, _tcslen(szFormFiles), &dwWritten );

	// 2 sending .......... <Extra> һЩ��������
	bWriteFile = ::InternetWriteFile( hRequest, 
		szFormExtra, _tcslen(szFormExtra), &dwWritten );

	// 3 sending .......... <userfile> �ļ�
	bWriteFile = ::InternetWriteFile( hRequest, 
		szFormUserFile, _tcslen(szFormUserFile), &dwWritten );

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
	bWriteFile = ::InternetWriteFile( hRequest, szFormSubmit, _tcslen(szFormSubmit), &dwWritten );


	if ( FALSE == ::HttpEndRequest( hRequest, NULL, HSR_ASYNC | HSR_INITIATE, 0 ) )
	{
		_sntprintf( pszError, MAX_PATH-sizeof(TCHAR), _T("Error on HttpEndRequest %d"), GetLastError() );
		return FALSE;
	}

	if ( hMainWnd )
		::PostMessage( hMainWnd, UM_HTTP_COMPLETE, 0, 0 );


	return TRUE;
}




BOOL CVwHttp::ParseUrl( const TCHAR * pszUrl, TCHAR * pszServer, TCHAR * pszPath )
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

