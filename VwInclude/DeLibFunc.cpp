// DeLib.cpp: implementation of the CDeLib class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "DeLibFunc.h"

#include "crc32.h"
#include "MultiString.h"
#include "md5.h"

#include "shlwapi.h"
#pragma comment( lib, "shlwapi.lib" )

//	for :: _delib_get_file_version
#pragma comment( lib, "Version.lib" )



/**
 *	�Ƿ��ǵ��Ի���
 */
BOOL _delib_is_debug()
{
	#ifdef _DEBUG
		return TRUE;
	#endif

	return FALSE;
}

/**
 *	��ȡ����������
 */
DWORD _delib_get_processor_number()
{
	SYSTEM_INFO sysinfo;
	GetSystemInfo( &sysinfo );
	return sysinfo.dwNumberOfProcessors;
}


/**
 *	WINDOWS NT/98�°�ȫ�ػ���ע���ĵ��ú���
 */
VOID _delib_shutdown_system( UINT ShutdownFlag )
{
	OSVERSIONINFO oi;
	oi.dwOSVersionInfoSize = sizeof(OSVERSIONINFO);
	GetVersionEx( &oi );

	//	����� NT/2000 �������������Ȩ��
	if ( VER_PLATFORM_WIN32_NT == oi.dwPlatformId )
	{
		HANDLE handle;
		TOKEN_PRIVILEGES tkp;
		
		OpenProcessToken( GetCurrentProcess(), TOKEN_ADJUST_PRIVILEGES | TOKEN_QUERY, &handle );
		LookupPrivilegeValue( NULL, SE_SHUTDOWN_NAME, &tkp.Privileges[0].Luid );

		tkp.PrivilegeCount		= 1;  // one privilege to set    
		tkp.Privileges[ 0 ].Attributes	= SE_PRIVILEGE_ENABLED;

		AdjustTokenPrivileges( handle, FALSE, &tkp, 0, (PTOKEN_PRIVILEGES)NULL, 0 );
	}

	//	WIN98 �¹ػ���ע��ʱ��ֱ�ӵ������溭���ȿ�
	::ExitWindowsEx( ShutdownFlag, 0 );
}

/**
 *	дϵͳ�¼���־
 */
BOOL _delib_report_systemevent( LPCTSTR lpcszSrcName, DWORD dwEventID, WORD wCategory, WORD wNumStrings, LPCTSTR * lpStrings )
{
	//
	//	lpcszSrcName	- [in] event source name
	//	dwEventID	- [in] event identifier
	//	wCategory	- [in] event category
	//	wNumStrings	- [in] count of insert strings
	//	lpStrings	- [in] insert strings
	//	RETURN		VOID
	//

	HANDLE hEventLog;
	BOOL bReport	= FALSE;

	//	Get a handle to the event log.
	hEventLog = RegisterEventSource
	(
		NULL,		//	use local computer
		lpcszSrcName	//	event source name
	);
	if ( hEventLog )
	{
		//	Report the event.
		bReport = ReportEvent
		(
			hEventLog,		//	event log handle 
			EVENTLOG_ERROR_TYPE,	//	event type 
			wCategory,		//	event category  
			dwEventID,		//	event identifier 
			NULL,			//	no user security identifier 
			wNumStrings,		//	number of substitution strings 
			0,			//	no data 
			lpStrings,		//	pointer to strings 
			NULL			//	no data
		);
		if ( ! bReport )
		{
			//	Could not report the event.
		}

		DeregisterEventSource( hEventLog );
	}
	else
	{
		//	Could not register the event source.
	}

	return bReport;
}

/**
 *	����һ�������Ľ���
 */
DWORD _delib_create_process( LPCTSTR lpszCmdLine, BOOL bBlocked )
{
	//
	//	lpszCmdLine	- [in] �������̵�������
	//	bBlocked	- [in] �Ƿ�����
	//	RETURN		�����´����Ľ��� ID
	//

	if ( NULL == lpszCmdLine )
	{
		return FALSE;
	}

	DWORD dwProcessId	= 0;
	BOOL  bCreateProcess	= FALSE;
	DWORD dweExitCode	= 0;
	STARTUPINFO si;
	PROCESS_INFORMATION pi;

	memset( &si, 0, sizeof(si) );
	si.cb = sizeof(si);
	memset( &pi, 0, sizeof(pi) );

	//
	//	Start the child process.
	//
	bCreateProcess = CreateProcess
	(
		NULL,			//	No module name (use command line). 
		(TCHAR*)lpszCmdLine,	//	Command line. 
		NULL,			//	Process handle not inheritable. 
		NULL,			//	Thread handle not inheritable. 
		FALSE,			//	Set handle inheritance to FALSE. 
		0,			//	No creation flags. 
		NULL,			//	Use parent's environment block. 
		NULL,			//	Use parent's starting directory. 
		&si,			//	Pointer to STARTUPINFO structure.
		&pi			//	Pointer to PROCESS_INFORMATION structure.
	);
	if ( bCreateProcess )
	{
		dwProcessId = pi.dwProcessId;

		if ( bBlocked )
		{
			//	Wait until child process exits.
			if ( pi.hProcess )
			{
				//	�û�ָ��Ҫ��������
				if ( WAIT_OBJECT_0 == WaitForSingleObject( pi.hProcess, INFINITE ) )
				{
					GetExitCodeProcess( pi.hProcess, &dweExitCode );
				}

				//	Close process and thread handles.
				CloseHandle( pi.hProcess );
			}
			
			if ( pi.hThread )
			{
				CloseHandle( pi.hThread );
			}
		}
		else
		{
			//	�û�δָ��Ҫ�ȴ�����ô��ֱ�ӷ��ؽ��̵� ID
			//	�û������Ҫ���̾���Ļ������Դ������ ID ͨ�� OpenProcess ��������ȡ
		}
	}

	return dwProcessId;
}

/**
 *	����һ�������Ľ���
 */
BOOL _delib_run_block_process( LPCTSTR lpszCmdLine )
{
	if ( NULL == lpszCmdLine )
	{
		return FALSE;
	}

	BOOL bRet = FALSE;
	BOOL bCreateProcess = FALSE;
	DWORD dweExitCode = 0;
	STARTUPINFO si;
	PROCESS_INFORMATION pi;
	
	memset( &si, 0, sizeof(si) );
	si.cb = sizeof(si);
	memset( &pi, 0, sizeof(pi) );

	//	Start the child process. 
	bCreateProcess = CreateProcess
	(
		NULL,			// No module name (use command line). 
		(TCHAR*)lpszCmdLine,	// Command line. 
		NULL,			// Process handle not inheritable. 
		NULL,			// Thread handle not inheritable. 
		FALSE,			// Set handle inheritance to FALSE. 
		0,			// No creation flags. 
		NULL,			// Use parent's environment block. 
		NULL,			// Use parent's starting directory. 
		&si,			// Pointer to STARTUPINFO structure.
		&pi			// Pointer to PROCESS_INFORMATION structure.
	);
	if ( bCreateProcess )
	{
		bRet = TRUE;

		//	Wait until child process exits.
		if ( pi.hProcess )
		{
			if ( WAIT_OBJECT_0 == 
				WaitForSingleObject( pi.hProcess, INFINITE ) )
			{
				GetExitCodeProcess( pi.hProcess, &dweExitCode );
			}
		}
		//	Close process and thread handles. 
		if ( pi.hProcess )
		{
			CloseHandle( pi.hProcess );
		}
		if ( pi.hThread )
		{
			CloseHandle( pi.hThread );
		}
	}

	return bRet;
}


/**
 *	����һ�鹲���ڴ�
 */
BOOL _delib_alloc_share_memory( DWORD dwSize, LPCTSTR lpcszMemName, VOID ** lppvBuffer, HANDLE hFileMap, LPBOOL lpbCreate, LPSECURITY_ATTRIBUTES lpFileMappingAttributes )
{
	//
	//	dwSize		- [in]  Ҫ������ڴ��С
	//	lpcszMemName	- [in]  �����ڴ�����
	//	lpvBuffer	- [out] ���ع����ڴ�����ַ
	//	hFileMap	- [out] �ļ����
	//	lpbCreate	- [out] �Ƿ����´�����
	//	RETURN		- TRUE / FALSE
	//
	if ( 0 == dwSize )
	{
		return FALSE;
	}
	if ( NULL == lpcszMemName )
	{
		return FALSE;
	}
	
	BOOL bRet	= FALSE;
	
	
	//
	//	�����ڴ�ӳ��
	//
	*lppvBuffer	= NULL;
	*lpbCreate	= FALSE;
	
	hFileMap = OpenFileMapping( FILE_MAP_READ|FILE_MAP_WRITE, FALSE, lpcszMemName );
	if ( ! hFileMap )
	{
		//	FileMap �����ڣ�Ҫ����֮
		*lpbCreate = TRUE;
		hFileMap = CreateFileMapping( INVALID_HANDLE_VALUE, lpFileMappingAttributes, PAGE_READWRITE, 0, dwSize, lpcszMemName );
	}
	if ( hFileMap )
	{
		(*lppvBuffer) = (LPVOID)MapViewOfFile( hFileMap, FILE_MAP_READ|FILE_MAP_WRITE, 0, 0, 0 );
		if ( (*lppvBuffer) )
		{
			bRet = TRUE;
		}
	}
	
	//	..
	return bRet;
}

/**
 *	��һ�鹲���ڴ�
 */
BOOL _delib_open_share_memory( LPCTSTR lpcszMemName, VOID ** lppvBuffer, HANDLE hFileMap )
{
	//
	//	lpcszMemName	- [in]  �����ڴ�����
	//	lpvBuffer	- [out] ���ع����ڴ�����ַ
	//	hFileMap	- [out] �ļ����
	//	RETURN		- TRUE / FALSE
	//
	if ( NULL == lpcszMemName )
	{
		return FALSE;
	}
	
	BOOL bRet	= FALSE;
	
	//
	//	�����ڴ�ӳ��
	//
	*lppvBuffer	= NULL;
	
	hFileMap = OpenFileMapping( FILE_MAP_READ|FILE_MAP_WRITE, FALSE, lpcszMemName );
	if ( hFileMap )
	{
		(*lppvBuffer) = (LPVOID)MapViewOfFile( hFileMap, FILE_MAP_READ|FILE_MAP_WRITE, 0, 0, 0 );
		if ( (*lppvBuffer) )
		{
			bRet = TRUE;
		}
	}
	DWORD aaa = GetLastError();
	
	//	..
	return bRet;
}


/**
 *	@ public
 *	����Դ������ȡ�ļ�������
 *	���뱣֤DestFilename�ǿ��Դ�����
 */
BOOL _delib_extract_file_from_resource( HINSTANCE hInst, LPCTSTR lpcszResType, UINT uResID, LPCTSTR lpszDestFilename )
{
	//
	//	hInst			- [in] hInstance
	//	lpcszResType		- [in] ��Դ���ͣ����磺"TXT"
	//	uResID			- [in] ��Դ ID
	//	lpszDestFilename	- [in] ��ȡ��Դ�󣬱��浽���ļ�����Ҫ��ȫ·��
	//	RETURN			- TRUE / FALSE
	//

	if ( NULL == hInst )
		return FALSE;
	if ( NULL == lpcszResType || 0 == strlen( lpcszResType ) )
		return FALSE;
	if ( NULL == lpszDestFilename || 0 == strlen( lpszDestFilename ) )
		return FALSE;
	
	
	BOOL  bRet		= FALSE;
	DWORD dwSize		= 0;
	PVOID pResBuffer	= NULL;
	HANDLE hFile		= NULL;
	DWORD dwWriten		= 0;
	
	pResBuffer = _delib_get_resource_buffer( hInst, lpcszResType, uResID, &dwSize );
	if ( pResBuffer && dwSize > 0 )
	{
		hFile = CreateFile
		(
			lpszDestFilename, GENERIC_WRITE, 0,
			NULL,
			CREATE_ALWAYS,
			FILE_ATTRIBUTE_NORMAL, NULL
		);		// no attr. template
		if ( hFile )
		{
			::WriteFile( hFile, pResBuffer, dwSize, &dwWriten, NULL );
			::CloseHandle( hFile);
			bRet = TRUE;
		}
	}

	return bRet;
}

/**
 *	��ȡ��Դ������
 */
PVOID _delib_get_resource_buffer( HINSTANCE hInst, LPCTSTR lpcszResType, UINT uResID, DWORD * pdwSize )
{
	//
	//	hInst			- [in] hInstance
	//	lpcszResType		- [in] ��Դ���ͣ����磺"TXT"
	//	uResID			- [in] ��Դ ID
	//	pdwSize			- [out] ��ȡ��Դ��С
	//
	//	RETURN			- TRUE / FALSE
	//

	if ( NULL == hInst )
		return FALSE;
	if ( NULL == lpcszResType || NULL == lpcszResType[0] )
		return FALSE;
	
	HRSRC hRes;
	HGLOBAL hResLoad;
	
	hRes = FindResource( hInst, MAKEINTRESOURCE(uResID), lpcszResType ); 
	if ( hRes )
	{
		hResLoad = LoadResource( hInst, hRes );
		if ( hResLoad )
		{
			if ( pdwSize )
			{
				*pdwSize = SizeofResource( hInst, hRes );
			}
			return LockResource( hResLoad );
		}
	}
	return NULL;
}



/**
 *	����ļ��Ƿ����
 */
BOOL _delib_file_exists( LPCTSTR lpcszFile )
{
	//
	//	lpcszFile	- [in] �ļ�·��
	//

	if ( NULL == lpcszFile )
	{
		return FALSE;
	}
	if ( NULL == lpcszFile[0] )	//strlen
	{
		return FALSE;
	}

	FILE * fp = fopen( lpcszFile, "r" );
	if( fp )
	{
		fclose( fp );
		return TRUE;
	}

	return FALSE;
}

/**
 *	��ȡ�ļ���С����λΪ���ֽ�
 */
DWORD _delib_get_file_size( LPCTSTR lpszFilePath )
{
	if ( NULL == lpszFilePath || 0 == _tcslen(lpszFilePath) )
	{
		return 0;
	}

	HANDLE hFile		= NULL;
	DWORD dwFileSize	= 0;
	hFile = CreateFile( lpszFilePath, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, 0, 0 );
	if ( INVALID_HANDLE_VALUE != hFile )
	{
		dwFileSize = ::GetFileSize( hFile, NULL );
		CloseHandle( hFile );
	}

	return dwFileSize;
}

/**
 *	�����ļ�Ϊ��д
 */
BOOL _delib_set_file_writable( LPCTSTR lpcszFilePath )
{
	//
	//	lpcszFilePath	- [in] �ļ�·��
	//

	if ( NULL == lpcszFilePath )
	{
		return FALSE;
	}
	else if ( NULL == lpcszFilePath[0] )
	{
		return FALSE;
	}
	if ( FALSE == _delib_file_exists( lpcszFilePath ) )
	{
		return FALSE;
	}

	DWORD dwFileAttrs	= 0;
	dwFileAttrs = GetFileAttributes( lpcszFilePath ); 
	if ( dwFileAttrs & FILE_ATTRIBUTE_READONLY )
	{
		SetFileAttributes( lpcszFilePath, (dwFileAttrs & ~FILE_ATTRIBUTE_READONLY) ); 
	}

	return TRUE;
}

/**
 *	���ļ���Դ������汾
 */
BOOL _delib_get_file_version( LPCTSTR lpszFilePath, LPTSTR lpszVersion, DWORD dwSize )
{
	//
	//	lpszFilePath	- [in]  �ļ�·��
	//	lpszVersion	- [out] ����ֵ���ļ��汾��
	//	dwSize		- [in]  ����ֵ��������С
	//	RETURN		- TRUE / FALSE
	//

	if ( NULL == lpszFilePath )
	{
		return FALSE;
	}
	
	TCHAR szVersionBuffer[ 8192 ]	= {0};
	DWORD dwVerSize			= 0;
	DWORD dwHandle			= 0;
	
	dwVerSize = GetFileVersionInfoSize( lpszFilePath, &dwHandle );
	if ( 0 == dwVerSize || dwVerSize > (sizeof(szVersionBuffer)-1) )
	{
		return FALSE;
	}
	
	if ( GetFileVersionInfo( lpszFilePath, 0, dwVerSize, szVersionBuffer ) )
	{	
		VS_FIXEDFILEINFO * pInfo;
		unsigned int nInfoLen;
		
		if( VerQueryValue( szVersionBuffer, "\\", (void**)&pInfo, &nInfoLen ) )
		{	
			memset( lpszVersion, 0, dwSize );
			_snprintf( lpszVersion, dwSize-1, "%d.%d.%d.%d", 
				HIWORD( pInfo->dwFileVersionMS ), LOWORD( pInfo->dwFileVersionMS ), 
				HIWORD( pInfo->dwFileVersionLS ), LOWORD( pInfo->dwFileVersionLS ) );
			return TRUE;
		}
	}
	
	return FALSE;
}

/**
 *	��ȡ�ļ�����
 */
BOOL _delib_get_file_mapbuffer( LPCTSTR lpszFilePath, LPTSTR lpszBuffer, DWORD dwSize )
{
	BOOL bRet = FALSE;
	UINT uFileLen = 0;
	
        HANDLE hfile = CreateFile( lpszFilePath, GENERIC_READ, FILE_SHARE_READ, 0, OPEN_EXISTING, 0, 0 );
	if ( INVALID_HANDLE_VALUE != hfile )
	{
		uFileLen = GetFileSize( hfile, 0 );
		if ( uFileLen > 0 )
		{
			if ( uFileLen > DELIB_MAX_FILEMAP_LENGTH )
			{
				uFileLen = DELIB_MAX_FILEMAP_LENGTH;
			}

			HANDLE hmap = CreateFileMapping( hfile, 0, PAGE_READONLY, 0, (DWORD)uFileLen, 0 );
			if ( hmap )
			{
				BYTE* pView = (BYTE*)MapViewOfFile( hmap, FILE_MAP_READ, 0, 0, (SIZE_T)uFileLen );
				if ( pView )
				{
					// ..
					bRet = TRUE;
					_sntprintf( lpszBuffer, dwSize-sizeof(TCHAR), "%s", (TCHAR*)pView );
					
					// close view
					UnmapViewOfFile(pView);
				}
				// close map
				CloseHandle( hmap );
			}
		}
		// close handle
		CloseHandle(hfile);
	}

	return bRet;
}

/**
 *	��ȡ�ļ��� md5 ֵ
 */
BOOL _delib_get_file_md5( LPCTSTR lpszFilePath, LPTSTR lpszMd5, DWORD dwSize, LPTSTR lpszError )
{
	BOOL bRet = FALSE;
	UINT uFileLen = 0;

        HANDLE hfile = CreateFile( lpszFilePath, GENERIC_READ, FILE_SHARE_READ, 0, OPEN_EXISTING, 0, 0 );
	if ( INVALID_HANDLE_VALUE != hfile )
	{
		uFileLen = GetFileSize( hfile, 0 );
		if ( uFileLen > 0 )
		{
			if ( uFileLen > DELIB_MAX_FILEMAP_LENGTH )
			{
				_tcscpy( lpszError, _T("FileSize too long. must less then 100M.") );
				uFileLen = DELIB_MAX_FILEMAP_LENGTH;
			}
			HANDLE hmap = CreateFileMapping( hfile, 0, PAGE_READONLY, 0, (DWORD)uFileLen, 0 );
			if ( hmap )
			{
				BYTE* pView = (BYTE*)MapViewOfFile( hmap, FILE_MAP_READ, 0, 0, (SIZE_T)uFileLen );
				if ( pView )
				{
					//	Get MD5
					MD5_CTX	m_md5;
					unsigned char szEncrypt[ 16 ] = {0};
					
					m_md5.MD5Update( (unsigned char *)pView, uFileLen );
					m_md5.MD5Final( szEncrypt );
					
					TCHAR szHexTmp[ 32 ]		= {0};
					TCHAR szOutput[ MAX_PATH ]	= {0};
					
					for ( INT i = 0; i < sizeof(szEncrypt); i ++ )
					{
						memset( szHexTmp, 0, sizeof(szHexTmp) );
						_sntprintf( szHexTmp, sizeof(szHexTmp)-sizeof(TCHAR), _T("%02X"), szEncrypt[i] );
						_tcscat( szOutput, szHexTmp );
					}
					
					// ..
					bRet = TRUE;
					_sntprintf( lpszMd5, dwSize-sizeof(TCHAR), _T("%s"), szOutput );
					
					// close view
					UnmapViewOfFile(pView);
				}
				else
				{
					_tcscpy( lpszError, _T("Failed to MapViewOfFile") );
				}
				
				// close map
				CloseHandle( hmap );
			}
			else
			{
				_tcscpy( lpszError, _T("Failed to CreateFileMapping") );
			}
		}
		else
		{
			_tcscpy( lpszError, _T("Empty file") );
		}
		
		// close handle
		CloseHandle(hfile);
	}
	else
	{
		_tcscpy( lpszError, _T("Failed to CreateFile") );
	}
	
	return bRet;
}

/**
 *	��ȡһ���ַ����� MD5 ֵ(32λ�ַ���)
 */
BOOL _delib_get_string_md5( LPCTSTR lpszString, LPTSTR lpszMd5, DWORD dwSize, DWORD dwSpecStringLen )
{
	/*
		lpszString	- [in]  �������ַ���
		lpszMd5		- [out] ���� MD5 ֵ�ַ���������
		dwSize		- [in]  ���� MD5 ֵ�ַ�������
		dwSpecStringLen	- [in]  ָ������ lpszString ��ͷ��ʼ�Ķ��ٸ��ֽ�
		RETURN		- TRUE / FALSE
	*/
	if ( NULL == lpszString || NULL == lpszMd5 )
		return FALSE;
	if ( dwSize < 32 )
		return FALSE;

	MD5_CTX	m_md5;
	unsigned char szEncrypt[16];
	unsigned char c;
	INT i			= 0;
	
	memset( szEncrypt, 0, sizeof(szEncrypt) );
	if ( dwSpecStringLen > 0 )
		m_md5.MD5Update( (unsigned char *)lpszString, dwSpecStringLen );
	else
		m_md5.MD5Update( (unsigned char *)lpszString, _tcslen(lpszString) );
	m_md5.MD5Final( szEncrypt );
	
	// ..
	for ( i = 0; i < 16; i++ )
	{
		c = szEncrypt[i] / 16;
		lpszMd5[i*2]	= ( c < 10 ) ? ( '0' + c ) : ( 'a' + c - 10 );
		c = szEncrypt[i] % 16;
		lpszMd5[i*2+1]	= ( c < 10 ) ? ( '0' + c ) : ( 'a' + c - 10 );
	}

	// ..
	lpszMd5[ min( 32, dwSize-sizeof(TCHAR) ) ] = 0;
	
	return TRUE;
}


/**
 *	Get Windows system type
 */
ENUMWINDOWSSYSTYPE _delib_get_shellsystype()
{
	ENUMWINDOWSSYSTYPE ShellType;
	DWORD winVer;
	OSVERSIONINFO * osvi;

	winVer = GetVersion();
	if ( winVer < 0x80000000 )
	{
		// NT
		ShellType = _OS_WINDOWS_NT3;
		osvi = (OSVERSIONINFO *)malloc(sizeof(OSVERSIONINFO));
		if ( NULL != osvi )
		{
			memset( osvi, 0, sizeof(OSVERSIONINFO) );
			osvi->dwOSVersionInfoSize = sizeof(OSVERSIONINFO);
			GetVersionEx( osvi );
			if ( 4L == osvi->dwMajorVersion )
			{
				ShellType = _OS_WINDOWS_NT4;
			}
			else if ( 5L == osvi->dwMajorVersion && 0L == osvi->dwMinorVersion )
			{
				ShellType = _OS_WINDOWS_2000;
			}
			else if ( 5L == osvi->dwMajorVersion && 1L == osvi->dwMinorVersion )
			{
				ShellType = _OS_WINDOWS_XP;
			}
			else if ( 5L == osvi->dwMajorVersion && 2L == osvi->dwMinorVersion )
			{
				ShellType = _OS_WINDOWS_2003;
			}
			if ( osvi )
				free( osvi );
		}
	}
	else if ( LOBYTE(LOWORD(winVer)) < 4 )
	{
		ShellType = _OS_WINDOWS_32S;
	}
	else
	{
		ShellType = _OS_WINDOWS_95;
		osvi = (OSVERSIONINFO *)malloc(sizeof(OSVERSIONINFO));
		if ( NULL != osvi )
		{
			memset( osvi, 0, sizeof(OSVERSIONINFO) );
			osvi->dwOSVersionInfoSize = sizeof(OSVERSIONINFO);
			GetVersionEx( osvi );
			if ( 4L == osvi->dwMajorVersion && 10L == osvi->dwMinorVersion )
			{
				ShellType = _OS_WINDOWS_98;
			}
			else if ( 4L == osvi->dwMajorVersion && 90L == osvi->dwMinorVersion )
			{
				ShellType = _OS_WINDOWS_ME;
			}
			if ( osvi )
				free(osvi);
		}
	}
	
	return ShellType;
}

/**
 *	�����ַ��������а�
 */
BOOL  _delib_copy_text_to_clipboard( LPCTSTR lpszString, HWND hWndNewOwner /* = NULL */ )
{
	if ( NULL == lpszString || 0 == _tcslen(lpszString) )
	{
		return FALSE;
	}

	HGLOBAL hClipBuffer;
	TCHAR * buffer;

	if ( ::OpenClipboard( hWndNewOwner ) )
	{
		::EmptyClipboard();

		hClipBuffer	= ::GlobalAlloc( GMEM_DDESHARE, _tcslen(lpszString) + sizeof(TCHAR) );
		buffer		= (TCHAR*)::GlobalLock( hClipBuffer );
		_tcscpy( buffer, lpszString );
		::GlobalUnlock( hClipBuffer );
		::SetClipboardData( CF_TEXT, hClipBuffer );
		::CloseClipboard();
	}

	return TRUE;
}


/**
 *	��ȡ CRC32 ֵ
 */
DWORD _delib_get_crc32( IN CONST BYTE * pBinBuf, IN INT nBufLen )
{
	CCrc32 cCrc32;
	return cCrc32.GetCrc32( pBinBuf, nBufLen );
}


/**
 *	����ĳ�� INI �ļ��� section �� vector
 */
BOOL _delib_ini_parse_section_line( LPCTSTR lpcszIniFile, LPCTSTR lpcszSection, vector<STINISECTIONLINE> & vcSection, BOOL bParseValue /* = FALSE */ )
{
	//
	//	lpcszIniFile	- [in] INI �ļ�·��
	//	lpcszSection	- [in] Section Name
	//	vcSection	- [out] ����ֵ
	//	bParseValue	- [in/opt] �Ƿ�Ҫ����� name=value �Գɹ�
	//	RETURN		- TRUE / FALSE
	//
	
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
	STINISECTIONLINE stLine;
	vector <STINISECTIONLINE>::iterator it;
	BOOL bExist				= FALSE;
	BOOL bParseValueSucc			= FALSE;
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
			
			//	���� name=value ��
			bParseValueSucc = ( 2 == _stscanf( *p, "%[^=]=%s", stLine.szName, stLine.szValue ) ? TRUE : FALSE );
			
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
				if ( bParseValue )
				{
					//	������Ҫ����� name=value ��
					if ( bParseValueSucc )
					{
						//	�����������ӵ�����
						vcSection.push_back( stLine );
					}
				}
				else
				{
					//	�����������ӵ�����
					vcSection.push_back( stLine );
				}
			}
			
			//	..
			p ++;
		}
		free( ppList );
	}
	
	return TRUE;
}


/**
 *	��ȡ�ļ���
 */
BOOL _delib_extract_filepath( LPCTSTR lpcszFileName, LPTSTR lpszFullPath, DWORD dwSize )
{
	if ( NULL == lpcszFileName )
	{
		return FALSE;
	}
	
	INT nFLen	= _tcslen( lpcszFileName );
	INT nCopyLen	= min( dwSize, ( nFLen - _tcslen( PathFindFileName(lpcszFileName) ) ) );

	memset( lpszFullPath, 0, dwSize );
	memcpy( lpszFullPath, lpcszFileName, nCopyLen );

	return TRUE;
}



/**
 *	�򵥼��ܽ���
 *		xorkey	- ���ܵ� key��Ĭ�ϵ��� 0x79
 *		pData	- ���ݣ�����������Ҳ����������
 *		���������Ļ������ģ�ֻҪ�ͽ����Ϳ��Խ��ܻ��߼���
 */
VOID _delib_xorenc( CHAR * pData, BYTE xorkey /* = 0XFF */ )
{
	//
	//	pData		- [in/out]	Pointer to a null-terminated string containing the Data
	//	xorkey		- [in]		Specifies the Encode/Decode key
	//
	
	while ( pData && *pData )
	{
		*pData = *pData ^ xorkey;
		pData++;
	}
}


/**
 *	��ȡĳ���ַ���һ���ַ����еĸ���
 */
DWORD _delib_get_chr_count( LPCTSTR lpszString, TCHAR lpChr )
{
	//
	//	lpszString	- [in] ����ѯ�ַ���
	//	lpChr		- [in] Ҫ������ַ�
	//	RETURN		- lpszString �к��� lpChr �ĸ���
	//

	if ( NULL == lpszString || NULL == lpChr || NULL == lpszString[0] )
		return 0;
	
	DWORD dwCount	= 0;
	UINT  i		= 0;
	UINT  uLen	= _tcslen(lpszString);
	
	for ( i = 0; i < uLen; i ++ )
	{
		if ( lpszString[i] && lpChr == lpszString[i] )
		{
			dwCount ++;
		}
	}
	return dwCount;
}

/**
 *	��ȡָ�����Ƶ� Cookie Value
 */
BOOL _delib_get_cookie_value( LPCTSTR lpszCookieString, LPCTSTR lpszCookieName, LPTSTR lpszValue, DWORD dwSize )
{
	//
	//	lpszCookieString	- [in]  ���� COOKIE ���ַ���
	//	lpszCookieName		- [in]  COOKIE ����
	//	lpszValue		- [out] ����ֵ
	//	dwSize			- [in]  ����ֵ��������С
	//	RETURN			- TRUE / FALSE
	//

	if ( NULL == lpszCookieString || NULL == lpszCookieName || NULL == lpszValue )
	{
		return FALSE;
	}
	if ( NULL == lpszCookieString[0] || NULL == lpszCookieName[0] )
	{
		return FALSE;
	}
	
	BOOL	bRet		= FALSE;
	LPTSTR	lpszHead	= NULL;
	LPTSTR	lpszMove	= NULL;
	LONG	uValueLen	= 0;
	
	lpszMove = _tcsstr( lpszCookieString, lpszCookieName );
	if ( lpszMove )
	{
		lpszHead = lpszMove + _tcslen(lpszCookieName);
		if ( lpszHead )
		{
			//
			//	�������ݵ�����ֵ������
			//
			memcpy( lpszValue, lpszHead, dwSize-sizeof(TCHAR) );
			
			//
			//	����β��
			//
			lpszMove = strpbrk( lpszValue, " ;\r\n" );
			if ( lpszMove )
			{
				*lpszMove = 0;
			}
			
			bRet = TRUE;
		}
	}
	return bRet;
}


/**
 *	����Ƿ�����Ч�� SYSTEMTIME ֵ
 */
BOOL _delib_is_valid_systemtime( SYSTEMTIME * pst )
{
	if ( pst && 
		pst->wMonth >= 1 && pst->wMonth <= 12 &&
		pst->wDay >= 1 && pst->wDay <= 31 &&
		pst->wHour >= 0 && pst->wHour <= 23 &&
		pst->wMinute >= 0 && pst->wMinute <= 59 &&
		pst->wSecond >= 0 && pst->wSecond <= 59 &&
		pst->wDayOfWeek >= 0 && pst->wDayOfWeek <= 6 )
	{
		return TRUE;
	}
	return FALSE;
}

/**
 *	��ȡ SYSTEMTIME ���ַ���ֵ
 */
BOOL _delib_get_string_systemtime( SYSTEMTIME * pst, LPTSTR lpszBuffer, DWORD dwSize )
{
	if ( pst )
	{
		_sntprintf
		(
			lpszBuffer,
			dwSize-sizeof(TCHAR),
			"%04d-%02d-%02d %02d:%02d:%02d",
			pst->wYear, pst->wMonth, pst->wDay, pst->wHour, pst->wMinute, pst->wSecond
		);
		return TRUE;
	}
	return FALSE;
}


/**
 *	�� sInput �� sDelimiter �ָȻ�󱣴浽 vector ��
 */
INT _delib_split_string( const string & sInput, const string & sDelimiter, vector<string> & vcResult, BOOL bIncludeEmpties )
{
	//
	//	sInput			- [in]  Դ�ַ���
	//	sDelimiter		- [in]  �ָ�����
	//	vcResult		- [out] ���
	//	bIncludeEmpties		- [in]  �Ƿ�����ս��
	//	RETURN			- Number of delimiters found
	//
	// ////////////////////////////////////////////////////////////////////////
	//	eg.
	//
	//	string sInput;
	//	string sDelimiter;
	//	vector<string> vcCName;
	//	string sItem;
	//
	//	//	split name & value to vecor
	//	sInput		= "xing|amei|jack|";
	//	sDelimiter	= "|";
	//	vcCName.clear();
	//	m_pcDeDLib->m_pfn_dedlib_split_string( sInput, sDelimiter, vcCName, FALSE );
	//

	int iPos = 0;
	int newPos = -1;
	int sizeS2 = (int)sDelimiter.size();
	int isize = (int)sInput.size();
	
	if( 
		( isize == 0 )
		||
		( sizeS2 == 0 )
		)
	{
		return 0;
	}
	
	vector<int> positions;
	
	newPos = sInput.find( sDelimiter, 0 );
	
	if( newPos < 0 )
	{ 
		return 0; 
	}
	
	int numFound = 0;
	
	while( newPos >= iPos )
	{
		numFound++;
		positions.push_back(newPos);
		iPos = newPos;
		newPos = sInput.find( sDelimiter, iPos + sizeS2 );
	}
	
	if( numFound == 0 )
	{
		return 0;
	}
	
	for( int i=0; i <= (int)positions.size(); ++i )
	{
		string s("");
		if( i == 0 ) 
		{ 
			s = sInput.substr( i, positions[i] ); 
		}
		int offset = positions[i-1] + sizeS2;
		if( offset < isize )
		{
			if( i == positions.size() )
			{
				s = sInput.substr(offset);
			}
			else if( i > 0 )
			{
				s = sInput.substr( positions[i-1] + sizeS2, positions[i] - positions[i-1] - sizeS2 );
			}
		}
		if( bIncludeEmpties || ( s.size() > 0 ) )
		{
			vcResult.push_back( s );
		}
	}
	return numFound;
}








/**
 *	����ַ����Ƿ��ǺϷ��� IP ��ַ�����磺211.99.66.14
 */
BOOL _delib_is_valid_ipaddr( LPCTSTR lpszString )
{
	//
	//	lpszString	- [in] ������ַ���
	//	RETURN		- TRUE/FALSE
	//

	if ( NULL == lpszString )
		return FALSE;
	if ( NULL == lpszString[0] )	// strlen
		return FALSE;
	
	UINT  i		= 0;
	UINT  uLen	= _tcslen(lpszString);
	
	//
	//	...
	//
	if ( '.' == lpszString[0] || '.' == lpszString[ uLen - sizeof(TCHAR) ] )
		return FALSE;
	if ( 3 != _delib_get_chr_count( lpszString, '.' ) )
		return FALSE;
	
	for ( i = 0; i < uLen; i ++ )
	{
		if ( NULL == _tcschr( DELIB_VALIDIPADDRSTRING, lpszString[i] ) )
		{
			return FALSE;
		}
	}
	return TRUE;
}

/**
 *	���ַ����� IP ��ַת��������
 */
DWORD _delib_get_ipaddr_value_from_string( LPCSTR lpcszIpAddr )
{
	if ( NULL == lpcszIpAddr || 0 == lpcszIpAddr[ 0 ] )
	{
		return 0;
	}

	//	INADDR_NONE �������һ���Ϸ��� IP ��ַ
	return inet_addr( lpcszIpAddr );
}


/**
 *	��ȡ���б��ص� IP ��ַ
 */
BOOL _delib_get_all_locipaddr( vector<STDEIPADDR> & vcIpAddr )
{
	BOOL  bRet		= FALSE;
	WSADATA WSData;
	HOSTENT * pst		= NULL;
	TCHAR ** pplist		= NULL;
	TCHAR * pTemp		= NULL;
	INT i			= 0;
	DWORD dwIndex		= 0;
	TCHAR szBuffer[128]	= {0};
	STDEIPADDR stIpAddr;

	try
	{
		if ( 0 == WSAStartup( MAKEWORD(VERSION_MAJOR,VERSION_MINOR), &WSData ) )
		{
			if ( 0 == gethostname( szBuffer, sizeof(szBuffer) ) )
			{
				pst = gethostbyname(szBuffer);
				if ( pst )
				{
					pplist = pst->h_addr_list;
					while( pplist[i] )
					{
						//	..
						pTemp = inet_ntoa( *((LPIN_ADDR)pplist[i]) );
						//	..
						if ( pTemp && _delib_is_valid_ipaddr( pTemp ) )
						{
							bRet = TRUE;

							memset( &stIpAddr, 0, sizeof(stIpAddr) );
							_sntprintf( stIpAddr.szIpAddr, sizeof(stIpAddr.szIpAddr)-sizeof(TCHAR), "%s", pTemp );
							vcIpAddr.push_back( stIpAddr );

							dwIndex ++;
						}
						i++;
					}
				}
			}
			// ..
			WSACleanup();
		}
	}
	catch (...)
	{
	}
	
	// ..
	return bRet;
}

/**
 *	��ȡ MAC ��ַ
 */
BOOL _delib_get_mac_address( LPCTSTR lpcszNetBiosName, LPTSTR lpszMacAddress, DWORD dwSize )
{
	/*
		lpcszNetBiosName	- [in]
		lpszMacAddress		- [out] 
		dwSize			- [in]
		RETURN			- TRUE / FALSE
	*/
	if ( NULL == lpcszNetBiosName || 0 == strlen( lpcszNetBiosName ) )
	{
		return FALSE;
	}
	if ( NULL == lpszMacAddress || 0 == dwSize )
	{
		return FALSE;
	}

	BOOL bRet		= FALSE;
	TCHAR szBiosName[ 64 ]	= {0};
	STASTAT Adapter;
	NCB ncb;
	UCHAR uRetCode;

	//	copy bios name
	_sntprintf( szBiosName, sizeof(szBiosName)-sizeof(TCHAR), "%s", lpcszNetBiosName );

	memset( & ncb, 0, sizeof(ncb) );
	ncb.ncb_command		= NCBRESET;
	ncb.ncb_lana_num	= 0;

	uRetCode = Netbios( &ncb );
	if ( 0 == uRetCode )
	{
		memset( & ncb, 0, sizeof(ncb) );
		ncb.ncb_command		= NCBASTAT;
		ncb.ncb_lana_num	= 0;

		//	��д
		_tcsupr( szBiosName );

		FillMemory( ncb.ncb_callname, NCBNAMSZ - 1, 0x20 );
		_sntprintf( (LPTSTR)ncb.ncb_callname, sizeof(ncb.ncb_callname)-sizeof(TCHAR), "%s", szBiosName );

		ncb.ncb_callname[ strlen( szBiosName ) ] = 0x20;
		ncb.ncb_callname[ NCBNAMSZ ] = 0x0;

		ncb.ncb_buffer = (unsigned char *) &Adapter;
		ncb.ncb_length = sizeof(Adapter);

		uRetCode = Netbios( &ncb );
		if ( 0 == uRetCode )
		{
			bRet = TRUE;
			_sntprintf
			(
				lpszMacAddress,
				dwSize-sizeof(TCHAR),
				"%02x-%02x-%02x-%02x-%02x-%02x", 
				Adapter.adapt.adapter_address[0],
				Adapter.adapt.adapter_address[1],
				Adapter.adapt.adapter_address[2],
				Adapter.adapt.adapter_address[3],
				Adapter.adapt.adapter_address[4],
				Adapter.adapt.adapter_address[5]
			);
		}
	}

	return bRet;
}


//	��ȡӲ�����к�
BOOL _delib_get_hdisk_serialnumber( LPTSTR lpszSerialNumber, DWORD dwSize )
{
	BOOL  bRet	= FALSE;
	TCHAR szNameBuf[ 12 ];
	DWORD dwSerialNumber;
	DWORD dwMaxLen; 
	DWORD dwFileFlag;
	TCHAR szSysNameBuf[ 10 ];

	bRet = ::GetVolumeInformation
	(
		"c:\\",
		szNameBuf,
		sizeof(szNameBuf), 
		& dwSerialNumber, 
		& dwMaxLen,
		& dwFileFlag,
		szSysNameBuf,
		sizeof(szSysNameBuf)
	);
	if ( bRet )
	{
		_sntprintf( lpszSerialNumber, dwSize-sizeof(TCHAR), "%x", dwSerialNumber );
	}

	return bRet;
}