// http.cpp : Defines the entry point for the DLL application.
//

#include "stdafx.h"
#include "http.h"




//////////////////////////////////////////////////////////////////////////
// �ж��Ƿ����Internet����
BOOL HttpHaveInternetConnection()
{
	BOOL  bRet		= FALSE;
	DWORD dwFlag		= 0;

	__try
	{
		//	�����������FALSE����϶�û������
		bRet = InternetGetConnectedState( &dwFlag, 0 );
	}
	__except( EXCEPTION_EXECUTE_HANDLER )
	{
		bRet = FALSE;
	}

	return bRet;
}


/**
 *	��ȡĳ�� URL ������״̬����
 */
DWORD HttpGetStatusCode( LPCTSTR lpcszUrl, LPCTSTR pszAgent, vector<STHTTPCOOKIE> * pvcCookie, LPSTR pszError, DWORD dwTimeout )
{
	/*
		pszUrl		- [in]  URL
		pvcCookie	- [in]  Cookie
		pszError	- [out] ������Ϣ
		dwTimeout	- [in]  ��ʱ����λ����
		
		RETURN		- HTTP STATUS CODE
	*/

	if ( FALSE == HttpHaveInternetConnection() )
	{
		if ( pszError )
			_tcscpy( pszError, _T("��ǰû����������") );
		return FALSE;
	}

	STHTTPTHREAD stHttpThread;

	stHttpThread.dwRet		= GETSTATUSCODE_ERROR;
	stHttpThread.pvcCookie		= pvcCookie;
	stHttpThread.pszUrl		= lpcszUrl;
	stHttpThread.pszAgent		= pszAgent;
	stHttpThread.pszError		= pszError;

	__try
	{
		stHttpThread.hThread = (HANDLE)_beginthreadex
			(
				NULL,
				0,
				HttpGetStatusCodeProc,
				&stHttpThread,
				0,
				&stHttpThread.uThreadID
			);
		if ( NULL == stHttpThread.hThread )
			return FALSE;

		if ( WAIT_OBJECT_0 != WaitForSingleObject( stHttpThread.hThread, dwTimeout ) )
		{
			stHttpThread.dwRet = GETSTATUSCODE_ERROR;
			if ( pszError )
				_tcscpy( pszError, _T("�������ӳ�ʱ") );
		}
		if ( stHttpThread.hThread )
			CloseHandle( stHttpThread.hThread );
	}
	__except( EXCEPTION_EXECUTE_HANDLER )
	{
		if ( pszError )
			_tcscpy( pszError, _T("�߳��쳣�����˳�") );
	}

	return stHttpThread.dwRet;
}
static unsigned int __stdcall HttpGetStatusCodeProc( VOID * p )
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


	HINTERNET hSession		= NULL;
	HINTERNET hHttpFile		= NULL;
	TCHAR szQueryBuf[ MAX_PATH ]	= {0};
	DWORD dwQueryBufLen		= 0;
	BOOL  bQuerySucc		= FALSE;
	BOOL  bFindDateTag		= FALSE;
	vector<STHTTPCOOKIE>::iterator	it;
	STHTTPCOOKIE * pstHttpCookie;
	TCHAR szAgent[ MAX_PATH ]	= {0};
	TCHAR szTemp[ MAX_PATH ]	= {0};


	pstThread->dwRet = GETSTATUSCODE_ERROR;

	__try
	{
		//
		//	���� Cookie
		//
		if ( pstThread->pvcCookie )
		{
			for ( it = pstThread->pvcCookie->begin(); it != pstThread->pvcCookie->end(); it ++ )
			{
				pstHttpCookie = it;
				InternetSetCookie( pstThread->pszUrl, pstHttpCookie->szName, pstHttpCookie->szData );
			}
		}

		if ( pstThread->pszAgent && _tcslen(pstThread->pszAgent) > 0 )
			_sntprintf( szAgent, sizeof(szAgent)-sizeof(TCHAR), _T("%s"), pstThread->pszAgent );
		else
			_sntprintf( szAgent, sizeof(szAgent)-sizeof(TCHAR), _T("%s"), _T(DEFAULT_AGENT_NAME) );

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

				if ( bQuerySucc && _tcslen(szQueryBuf) > 0 )
				{
					//
					//	����Ƿ��� Date: ���
					//	��Ҫ��Ϊ�˼�� HTTP ͷ�Ƿ�����
					//
					if ( strstr( szQueryBuf, "Date:" ) )
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
						pstThread->dwRet = (DWORD)atol(szQueryBuf);
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



//////////////////////////////////////////////////////////////////////////
// ���� URL �õ����صĻ���������
BOOL HttpGetResponse( LPCTSTR pszUrl, LPCTSTR pszAgent, LPTSTR pszResponse, DWORD dwSize, LPTSTR pszError, DWORD dwTimeout )
{
	if ( FALSE == HttpHaveInternetConnection() )
	{
		if ( pszError )
			_tcscpy( pszError, _T("��ǰû����������") );
		return FALSE;
	}

	STHTTPTHREAD stHttpThread;
	
	stHttpThread.bRet		= FALSE;
	stHttpThread.pszUrl		= pszUrl;
	stHttpThread.pszResponse	= pszResponse;
	stHttpThread.dwSize		= dwSize;
	stHttpThread.pszError		= pszError;

	__try
	{
		stHttpThread.hThread = (HANDLE)_beginthreadex
			(
				NULL,
				0,
				HttpGetResponseProc,
				&stHttpThread,
				0,
				&stHttpThread.uThreadID
			);
		if ( NULL == stHttpThread.hThread )
			return FALSE;

		if ( WAIT_OBJECT_0 != WaitForSingleObject( stHttpThread.hThread, dwTimeout ) )
		{
			if ( pszError )
				_tcscpy( pszError, _T("�������ӳ�ʱ") );
		}
		if ( stHttpThread.hThread )
			CloseHandle( stHttpThread.hThread );
	}
	__except( EXCEPTION_EXECUTE_HANDLER )
	{
		if ( pszError )
			_tcscpy( pszError, _T("�߳��쳣�����˳�") );
	}

	return stHttpThread.bRet;
}

static unsigned int __stdcall HttpGetResponseProc( VOID * p )
{
	STHTTPTHREAD * pstThread = (STHTTPTHREAD*)p;
	pstThread->bRet = FALSE;

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
	ULONG uPort			= 80;

	if ( FALSE == ParseUrl( pstThread->pszUrl, szServer, szPath, & uPort ) )
	{
		if ( pstThread->pszError )
			_tcscpy( pstThread->pszError, _T("�޷����� URL ��ַ") );
		return HTTP_THREAD_FAIL;
	}

	HINTERNET hSession		= NULL;
	HINTERNET hConnect		= NULL;
	HINTERNET hRequest		= NULL;
	BOOL bSendRequest		= FALSE;
	DWORD dwBytesRead		= 0;
	TCHAR szAgent[ MAX_PATH ]	= {0};


	__try
	{
		if ( pstThread->pszAgent && _tcslen(pstThread->pszAgent) > 0 )
			_sntprintf( szAgent, sizeof(szAgent)-sizeof(TCHAR), _T("%s"), pstThread->pszAgent );
		else
			_sntprintf( szAgent, sizeof(szAgent)-sizeof(TCHAR), _T("%s"), _T(DEFAULT_AGENT_NAME) );
		
		// ��һ��:��ʼ�� internet DLL �Ự���
		hSession = ::InternetOpen( szAgent, PRE_CONFIG_INTERNET_ACCESS, _T(""), INTERNET_INVALID_PORT_NUMBER, 0 );
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
				// ����������һ��HTTP������
				hRequest = ::HttpOpenRequest(
					hConnect, _T(HTTP_VERB_GET), szPath, HTTP_VERSION, NULL, 0, INTERNET_FLAG_DONT_CACHE, 0 );
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
					////////////////////////////////////////
					::InternetCloseHandle(hRequest);
				}
				else
				{
					if ( pstThread->pszError )
						_tcscpy( pstThread->pszError, _T("HttpOpenRequest ʧ�ܣ���һ��HTTP������ʧ��") );
				}
				////////////////////////////////////////
				::InternetCloseHandle(hConnect);
			}
			else
			{
				if ( pstThread->pszError )
					_tcscpy( pstThread->pszError, _T("InternetConnect ʧ�ܣ���ʼ��HTTP sessionʧ��") );
			}
			////////////////////////////////////////
			::InternetCloseHandle(hSession);
		}
		else
		{
			if ( pstThread->pszError )
				_tcscpy( pstThread->pszError, _T("InternetOpen ʧ�ܣ���ʼ��Internet DLLʧ��") );
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
// POST һ�� FORM ���ҵõ����صĻ���������
BOOL HttpPostForm( LPCTSTR pszUrl, LPTSTR pszPostData, LPCTSTR pszAgent, 
		LPTSTR pszResponse, DWORD dwSize, LPTSTR pszError, DWORD dwTimeout )
{
	if ( FALSE == HttpHaveInternetConnection() )
	{
		if ( pszError )
			_tcscpy( pszError, _T("��ǰû����������") );
		return FALSE;
	}

	STHTTPTHREAD stHttpThread;

	stHttpThread.bRet		= FALSE;
	stHttpThread.pszUrl		= pszUrl;
	stHttpThread.pszAgent		= pszAgent;
	stHttpThread.pszPostData	= pszPostData;
	stHttpThread.pszResponse	= pszResponse;
	stHttpThread.dwSize		= dwSize;
	stHttpThread.pszError		= pszError;

	_try
	{
		stHttpThread.hThread = (HANDLE)_beginthreadex
			(
				NULL,
				0,
				HttpPostFormProc,
				&stHttpThread,
				0,
				&stHttpThread.uThreadID
			);
		if ( NULL == stHttpThread.hThread )
			return FALSE;
		
		if ( WAIT_OBJECT_0 != WaitForSingleObject( stHttpThread.hThread, dwTimeout ) )
		{
			if ( pszError )
				_tcscpy( pszError, _T("�������ӳ�ʱ") );
		}
		if ( stHttpThread.hThread )
			CloseHandle( stHttpThread.hThread );
	}
	__except( EXCEPTION_EXECUTE_HANDLER )
	{
		if ( pszError )
			_tcscpy( pszError, _T("�߳��쳣�����˳�") );
	}

	return stHttpThread.bRet;
}


static unsigned int __stdcall HttpPostFormProc( VOID * p )
{
	STHTTPTHREAD * pstThread = (STHTTPTHREAD*)p;
	pstThread->bRet = FALSE;

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
	ULONG uPort			= 0;

	if ( FALSE == ParseUrl( pstThread->pszUrl, szServer, szPath, & uPort ) )
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
			_sntprintf( szAgent, sizeof(szAgent)-sizeof(TCHAR), _T("%s"), pstThread->pszAgent );
		else
			_sntprintf( szAgent, sizeof(szAgent)-sizeof(TCHAR), _T("%s"), _T(DEFAULT_AGENT_NAME) );

		if ( pstThread->pszHeader && _tcslen(pstThread->pszHeader) > 0 )
			_sntprintf( szHeader, sizeof(szHeader)-sizeof(TCHAR), _T("%s"), pstThread->pszHeader );
		else
			_sntprintf( szHeader, sizeof(szHeader)-sizeof(TCHAR), _T("%s"), _T(HTTP_ACCEPT) );

		// ��һ������ʼ�� internet DLL �Ự��������ǵ�һ�������õĺ���
		hSession = InternetOpen( szAgent, INTERNET_OPEN_TYPE_PRECONFIG, NULL, NULL, 0 );
		if ( hSession )
		{
			// �ڶ�������ʼ�� HTTP session ���Ӿ��
			hConnect = ::InternetConnect
				(
					hSession,		// ��ǰinternet�Ự���
					szServer,		// Server Name
					uPort,			// Server Port
					NULL,			// User Name
					NULL,			// Password
					INTERNET_SERVICE_HTTP,	// Type of service to access
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
						HTTP_VERB_POST,		// HTTP Verb
						szPath,			// Object Name
						HTTP_VERSION,		// Version
						NULL,			// Reference
						&szAcceptType,		// Accept Type															
						dwFlags,		// flag
						NULL			// context call-back point
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
						// ���岽���������� SEND REQUEST WITH POST ARGUEMENTS
						bSendRequest = ::HttpSendRequest
							(
								hRequest,				// handle by returned HttpOpenRequest
								szContentType,				// additional HTTP header
								_tcslen(szContentType),			// additional HTTP header length
								reinterpret_cast<LPVOID>(pstThread->pszPostData),	// additional data in HTTP Post or HTTP Put
								_tcslen(pstThread->pszPostData)			// additional data length
							);
						if ( FALSE == bSendRequest && 12168 == ::GetLastError() )
							bSendRequest = TRUE;

						// ���������õ���������
						if ( bSendRequest )
						{
							bReadFile = InternetReadFile( hRequest, pstThread->pszResponse, pstThread->dwSize-sizeof(TCHAR), &dwBytesRead );
							if ( bReadFile && dwBytesRead > 0 )
								pstThread->bRet = TRUE;
							else
							{
								if ( pstThread->pszError )
									_tcscpy( pstThread->pszError, _T("��ȡ��������������Ϊ��") );
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
// �ϴ��ļ�
BOOL HttpPostFile( LPCTSTR pszUrl, STHTTPFORMDATA & stFormData, LPCTSTR pszAgent, const HWND hMainWnd, UINT uFupMessage, 
		  LPTSTR pszResponse, DWORD dwSize, LPTSTR pszError, DWORD dwTimeout )
{
	if ( FALSE == HttpHaveInternetConnection() )
	{
		if ( pszError )
			_tcscpy( pszError, _T("��ǰû����������") );
		return FALSE;
	}

	STHTTPTHREAD stHttpThread;
	stHttpThread.bRet		= FALSE;
	stHttpThread.pszUrl		= pszUrl;
	stHttpThread.stFormData		= &stFormData;
	stHttpThread.pszAgent		= pszAgent;
	stHttpThread.hMainWnd		= hMainWnd;
	stHttpThread.uFupMessage	= uFupMessage;
	stHttpThread.pszResponse	= pszResponse;
	stHttpThread.dwSize		= dwSize;
	stHttpThread.pszError		= pszError;

	__try
	{
		stHttpThread.hThread = (HANDLE)_beginthreadex
			(
				NULL,
				0,
				HttpPostFileProc,
				&stHttpThread,
				0,
				&stHttpThread.uThreadID
			);
		if ( NULL == stHttpThread.hThread )
			return FALSE;
		
		if ( WAIT_OBJECT_0 != WaitForSingleObject( stHttpThread.hThread, dwTimeout ) )
		{
			if ( pszError )
				_tcscpy( pszError, _T("�������ӳ�ʱ") );
		}
		if ( stHttpThread.hThread )
			CloseHandle( stHttpThread.hThread );
	}
	__except( EXCEPTION_EXECUTE_HANDLER )
	{
		if ( pszError )
			_tcscpy( pszError, _T("�߳��쳣�����˳�") );
	}

	return stHttpThread.bRet;
}

static unsigned int __stdcall HttpPostFileProc( VOID * p )
{
	STHTTPTHREAD * pstThread = (STHTTPTHREAD*)p;
	pstThread->bRet = FALSE;

	TCHAR szServer[MAX_PATH]	= {0};
	TCHAR szPath[MAX_PATH]		= {0};
	ULONG uPort			= 0;

	if ( NULL == pstThread->pszUrl || 0 == _tcslen(pstThread->pszUrl) )
	{
		if ( pstThread->pszError )
			_tcscpy( pstThread->pszError, _T("��������д���") );
		return HTTP_THREAD_FAIL;
	}

	if ( FALSE == ParseUrl( pstThread->pszUrl, szServer, szPath, & uPort ) )
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

	HINTERNET hSession		= NULL;
	HINTERNET hConnect		= NULL;
	HINTERNET hRequest		= NULL;
	BOOL bSendRequest		= FALSE;
	TCHAR szAgent[ MAX_PATH ]	= {0};

	__try
	{
		if ( pstThread->pszAgent && _tcslen(pstThread->pszAgent) > 0 )
			_sntprintf( szAgent, sizeof(szAgent)-sizeof(TCHAR), _T("%s"), pstThread->pszAgent );
		else
			_sntprintf( szAgent, sizeof(szAgent)-sizeof(TCHAR), _T("%s"), _T(DEFAULT_AGENT_NAME) );

		////////////////////////////////////////////////////////////
		// ��һ������ʼ�� internet DLL �Ự���
		hSession = InternetOpen( szAgent, INTERNET_OPEN_TYPE_PRECONFIG, NULL, NULL, 0 );
		if ( hSession )
		{
			////////////////////////////////////////////////////////////
			// �ڶ�������ʼ�� HTTP session ���Ӿ��
			hConnect = ::InternetConnect
				(
					hSession,		// ��ǰinternet�Ự���
					szServer,		// Server Name
					uPort,			// Server Port
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
					if ( TRUE == 
						UserHttpSendReqEx( 
							hRequest, *pstThread->stFormData,
							pstThread->hMainWnd, pstThread->uFupMessage,
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
INT UrlEncode( const TCHAR * pszInput, INT nInputLen, TCHAR * pszOutBuf, INT nOutBufLen )
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
			nInputLen = _tcslen(pszInput);

		if ( 0 == nOutBufLen )
		{
			nDstLen = 0;
			for ( i = 0; i < nInputLen; i++ )
			{
				cCur = pszInput[i];
				if ( ' ' == cCur )
					nDstLen++;
				else if ( !isalnum(cCur) )
					nDstLen += 3;
				else
					nDstLen ++;
			}
			return nDstLen + 1;
		}

		pDst = pszOutBuf;
		nDstLen = 0;
		for ( i = 0; i < nInputLen; i++ )
		{
			cCur = pszInput[i];
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
		return _tcslen(pszOutBuf);
	}
	__except( EXCEPTION_EXECUTE_HANDLER )
	{
		return 0;
	}
}



//////////////////////////////////////////////////////////////////////////
// �ǵ�������
//////////////////////////////////////////////////////////////////////////

BOOL UserHttpSendReqEx( HINTERNET hRequest, STHTTPFORMDATA & stFormData, HWND hMainWnd, UINT uFupMessage, TCHAR * pszError )
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
	

	// --> ��Ϣ����ʼ�ϴ��ļ�
	if ( hMainWnd && uFupMessage > 0 )
		::PostMessage( hMainWnd, uFupMessage, WP_HTTP_START, (INT)(dwFileSize/1024) );


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

			// --> ��Ϣ���ļ��ϴ���
			if ( hMainWnd && uFupMessage > 0 )
				::PostMessage( hMainWnd, uFupMessage, WP_HTTP_PREGRESS, (INT)(dwFileByteSent/1024) );
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

	// --> ��Ϣ���ļ��ϴ�����
	if ( hMainWnd && uFupMessage > 0 )
		::PostMessage( hMainWnd, uFupMessage, WP_HTTP_COMPLETE, 0 );


	return TRUE;
}

BOOL ParseUrl( const TCHAR * pszUrl, TCHAR * pszServer, TCHAR * pszPath, ULONG * puPort )
{
	if ( NULL == pszUrl || NULL == pszServer || NULL == pszPath )
		return FALSE;

	DWORD i			= 0;
	DWORD j			= 0;
	DWORD dwFlags		= 0;		//"/"�ĸ���
	LPCSTR lpcszPort	= NULL;
	
	if ( pszUrl == _tcsstr(pszUrl,_T("http://")) )
		j = 7;		// ��HTTP��ӵ�7����ʼ����
	else
		j = 0;		// û�з���
	
	for ( i = j; i < _tcslen(pszUrl); i++ )
	{
		if ( ':' == pszUrl[i] )
		{
			lpcszPort = pszUrl + i + 1;
			break;
		}

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

	if ( 0 == strlen(pszPath) )
	{
		pszPath[ 0 ] = '/';
	}

	if ( puPort )
	{
		if ( lpcszPort && *lpcszPort )
			*puPort = atol( lpcszPort );
		else
			*puPort = 80;
	}

	return TRUE;
}