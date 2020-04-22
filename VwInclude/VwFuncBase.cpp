// VwFunc.cpp: implementation of the CVwFunc class.
//
//////////////////////////////////////////////////////////////////////

#include "StdAfx.h"
#include <windows.h>
#include <tchar.h>
#include <stdio.h>
#include <stdlib.h>
#include "VwFuncBase.h"

#include "shlwapi.h"
#pragma comment( lib, "shlwapi.lib" )

#include <exdisp.h>


/**
 *	@ ���� ȫ�ֺ���
 *	��ǰ�����ǵ��Ի���������ʽ RELEASE ����
 **/
BOOL _vwfunc_is_debug()
{
	#ifdef _DEBUG
		return TRUE;
	#endif

	return FALSE;
}


/**
 *	����һ�鹲���ڴ�
 */
BOOL _vwfunc_malloc_share_memory( DWORD dwSize, LPCTSTR lpcszMemName, VOID ** lppvBuffer, HANDLE hFileMap, LPBOOL lpbCreate, LPSECURITY_ATTRIBUTES lpFileMappingAttributes )
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
BOOL _vwfunc_open_share_memory( LPCTSTR lpcszMemName, VOID ** lppvBuffer, HANDLE hFileMap )
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
 *	��ȡ�ļ�����
 */
BOOL _vwfunc_get_file_mapbuffer( LPCTSTR lpszFilePath, LPTSTR lpszBuffer, DWORD dwSize )
{
	BOOL bRet = FALSE;
	UINT uFileLen = 0;
	
        HANDLE hfile = CreateFile( lpszFilePath, GENERIC_READ, FILE_SHARE_READ, 0, OPEN_EXISTING, 0, 0 );
	if ( INVALID_HANDLE_VALUE != hfile )
	{
		uFileLen = GetFileSize( hfile, 0 );
		if ( uFileLen > 0 )
		{
			if ( uFileLen > CONST_VWFUNCBASE_MAX_FILEMAP_LENGTH )
			{
				uFileLen = CONST_VWFUNCBASE_MAX_FILEMAP_LENGTH;
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
BOOL _vwfunc_get_file_md5( LPCTSTR lpszFilePath, LPTSTR lpszMd5, DWORD dwSize, LPTSTR lpszError )
{
	BOOL bRet = FALSE;
	UINT uFileLen = 0;

        HANDLE hfile = CreateFile( lpszFilePath, GENERIC_READ, FILE_SHARE_READ, 0, OPEN_EXISTING, 0, 0 );
	if ( INVALID_HANDLE_VALUE != hfile )
	{
		uFileLen = GetFileSize( hfile, 0 );
		if ( uFileLen > 0 )
		{
			if ( uFileLen > CONST_VWFUNCBASE_MAX_FILEMAP_LENGTH )
			{
				_tcscpy( lpszError, "�ļ���С�Ѿ����� 100M��" );
				uFileLen = CONST_VWFUNCBASE_MAX_FILEMAP_LENGTH;
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
						_sntprintf( szHexTmp, sizeof(szHexTmp)-sizeof(TCHAR), "%02X", szEncrypt[i] );
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
					_tcscpy( lpszError, "Failed to MapViewOfFile" );
				}
				
				// close map
				CloseHandle( hmap );
			}
			else
			{
				_tcscpy( lpszError, "CreateFileMapping" );
			}
		}
		else
		{
			_tcscpy( lpszError, "0 �����ļ�" );
		}
		
		// close handle
		CloseHandle(hfile);
	}
	else
	{
		_tcscpy( lpszError, "Failed to CreateFile" );
	}
	
	return bRet;
}

/**
 *	��ȡһ���ַ����� MD5 ֵ(32λ�ַ���)
 */
BOOL _vwfunc_get_string_md5( LPCTSTR lpszString, LPTSTR lpszMd5, DWORD dwSize, DWORD dwSpecStringLen )
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
ENUMWINDOWSSYSTYPE _vwfunc_get_shellsystype()
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
 *	is window nt4
 */
BOOL _vwfunc_is_window_nt4()
{
	OSVERSIONINFO osif;
	osif.dwOSVersionInfoSize = sizeof(OSVERSIONINFO);
	GetVersionEx( &osif );
	if ( VER_PLATFORM_WIN32_NT == osif.dwPlatformId && osif.dwMajorVersion <= 4 )
	{
		return TRUE;
	}
	else
	{
		return FALSE;
	}
}

/**
 *	��ȡ Windows Temp Ŀ¼
 */
BOOL _vwfunc_get_window_tempdir( LPTSTR lpszDir, DWORD dwSize )
{
	DWORD dwRet = GetTempPath( dwSize, lpszDir );
	if ( dwRet && dwRet < dwSize )
	{
		if ( '\\' != lpszDir[ _tcslen(lpszDir) - 1 ] )
		{
			lpszDir[ _tcslen(lpszDir) ] = '\\';
		}
		return TRUE;
	}
	//if ( ERROR_PATH_NOT_FOUND == GetLastError() )
	for ( CHAR c = 'c'; c < 'z'; c ++ )
	{
		_sntprintf( lpszDir, dwSize-sizeof(TCHAR), "%c:\\", c );
		if ( PathIsDirectory( lpszDir ) )
		{
			return TRUE;
		}
	}
	return FALSE;
}


/**
 *	�򵥼��ܽ���
 *		xorkey	- ���ܵ� key��Ĭ�ϵ��� 0x79
 *		pData	- ���ݣ�����������Ҳ����������
 *		���������Ļ������ģ�ֻҪ�ͽ����Ϳ��Խ��ܻ��߼���
 */
VOID _vwfunc_xorenc( CHAR * pData, BYTE xorkey /*0XFF*/ )
{
	/*
		pData		- [in/out]	Pointer to a null-terminated string containing the Data
		xorkey		- [in]		Specifies the Encode/Decode key
	*/

	while( pData && *pData )
	{
		*pData = *pData ^ xorkey;
		pData++;
	}
}


/**
 *	@ public
 *	����Դ������ȡ�ļ�������
 *	���뱣֤DestFilename�ǿ��Դ�����
 */
BOOL _vwfunc_extract_file_from_resource( HINSTANCE hInst, LPCTSTR lpcszResType, UINT uResID, LPCTSTR lpszDestFilename )
{
	/*
		hInst			- [in] hInstance
		lpcszResType		- [in] ��Դ���ͣ����磺"TXT"
		uResID			- [in] ��Դ ID
		lpszDestFilename	- [in] ��ȡ��Դ�󣬱��浽���ļ�����Ҫ��ȫ·��

		RETURN			- TRUE / FALSE
	*/

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

	pResBuffer = _vwfunc_get_resource_buffer( hInst, lpcszResType, uResID, &dwSize );
	if ( pResBuffer && dwSize > 0 )
	{
		hFile = CreateFile
			(
				lpszDestFilename, GENERIC_WRITE, 0,
				NULL,
				CREATE_ALWAYS,
				FILE_ATTRIBUTE_NORMAL, NULL
			);                        // no attr. template
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
 *	@ Private
 */
VOID * _vwfunc_get_resource_buffer( HINSTANCE hInst, LPCTSTR lpcszResType, UINT uResID, DWORD * pdwSize )
{
	/*
		hInst			- [in] hInstance
		lpcszResType		- [in] ��Դ���ͣ����磺"TXT"
		uResID			- [in] ��Դ ID
		pdwSize			- [out] ��ȡ��Դ��С

		RETURN			- TRUE / FALSE
	*/

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
BOOL _vwfunc_my_file_exists( LPCTSTR lpcszFile )
{
	/*
		lpcszFile	- [in] �ļ�·��
	*/

	if ( NULL == lpcszFile )
		return FALSE;
	if ( NULL == lpcszFile[0] )	//strlen
		return FALSE;


	FILE * fp = fopen( lpcszFile, "r" );
	if( fp )
	{
		fclose( fp );
		return TRUE;
	}

	return FALSE;
}

/**
 *	��ȡ�ļ���С
 */
DWORD _vwfunc_get_my_filesize( LPCTSTR pszFilePath )
{
	/*
		lpcszFile	- [in] �ļ�·��
		RETURN		- �����ļ����ֽ���
	*/

	if ( NULL == pszFilePath )
		return 0;

	HANDLE hFile		= NULL;
	DWORD dwFileSize	= 0;

	hFile = CreateFile( pszFilePath, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, 0, 0 );
	if ( INVALID_HANDLE_VALUE != hFile )
	{
		dwFileSize = ::GetFileSize( hFile, NULL );
		CloseHandle( hFile );
	}
	
	return dwFileSize;
}

/**
 *	���ļ���Դ������汾
 */
/*
BOOL _vwfunc_get_file_version( LPCTSTR lpszFilePath, LPTSTR lpszVersion, DWORD dwSize )
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
*/

/**
 *	�����ļ���дѡ��
 */
BOOL _vwfunc_file_give_write_attribute( LPCTSTR lpcszFilePath )
{
	/*
		lpcszFilePath	- [in] �ļ�·��
	*/

	if ( NULL == lpcszFilePath )
		return FALSE;
	else if ( 0 == _tcslen(lpcszFilePath) )
		return FALSE;
	if ( FALSE == _vwfunc_my_file_exists( lpcszFilePath ) )
		return FALSE;

	DWORD dwFileAttrs	= 0;
	dwFileAttrs = GetFileAttributes( lpcszFilePath ); 
	if ( dwFileAttrs & FILE_ATTRIBUTE_READONLY )
	{
		SetFileAttributes( lpcszFilePath, (dwFileAttrs & ~FILE_ATTRIBUTE_READONLY) ); 
	}

	return TRUE;
}

/**
 *	��ȡ DLL ���ڵ�ȫ·��
 */
BOOL _vwfunc_get_dll_modulepath( LPVOID lpvInnerAddr, HINSTANCE * hInstance, LPTSTR lpszModulePath, DWORD dwSize )
{
	/*
		lpvInnerAddr		- [in]  ģ��(DLL)������һ��ȫ�ֺ����ĵ�ַ
		hInstance		- [in]  instance
		lpszModulePath		- [out] ����ֵ������ɹ������ػ�ȡ���ĵ�ǰ DLL ����Ŀ¼��ȫ·��
		dwSize			- [in]  ����ֵ��������С
		RETURN			- TRUE / FALSE
	*/

	BOOL  bRet = FALSE;
	MEMORY_BASIC_INFORMATION mbi;

	__try
	{
		if ( lpvInnerAddr )
		{
			if ( VirtualQuery( lpvInnerAddr, &mbi, sizeof(mbi) ) )
			{
				if ( hInstance )
					*hInstance = (HMODULE)mbi.AllocationBase;
				// ..
				if ( GetModuleFileName( (HMODULE)mbi.AllocationBase, lpszModulePath, dwSize ) )
				{
					bRet = TRUE;
				}
			}
		}

/*
		// ���ٵķ��������ױ���
		pwAddrFunc = (WORD*)((DWORD)pwAddrFunc & ( ~1023 ));
		while( pwAddrFunc > (WORD*)0x400000 )
		{
			if ( *pwAddrFunc && *pwAddrFunc == 0x5a4d )
			{
				bRet = TRUE;
				GetModuleFileName( (HMODULE)pwAddrFunc, lpszModulePath, dwSize );
				break;
			}
			pwAddrFunc --;
		}
*/
	}
	__except( EXCEPTION_EXECUTE_HANDLER )
	{
	}

	return bRet;
}


/**
 *	��ȡĳ���ַ���һ���ַ����еĸ���
 */
DWORD _vwfunc_get_chr_count( LPCTSTR lpszString, TCHAR lpChr, UINT uStringLen /* = 0 */ )
{
	/*
		lpszString	- [in] ����ѯ�ַ���
		lpChr		- [in] Ҫ������ַ�
		RETURN		- lpszString �к��� lpChr �ĸ���
	*/

	if ( NULL == lpszString || NULL == lpChr || NULL == lpszString[0] )
		return 0;

	DWORD dwCount	= 0;
	UINT  i		= 0;
	UINT  uLen	= ( 0 == uStringLen ? (UINT)_tcslen(lpszString) : uStringLen );
	
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
 *	����ַ����Ƿ��ǺϷ��� IP ��ַ�����磺211.99.66.14
 */
BOOL _vwfunc_is_valid_ipaddr( LPCTSTR lpszString, UINT uStringLen /* = 0 */ )
{
	/*
		lpszString	- [in] ������ַ���
		RETURN		- TRUE/FALSE
	*/

	if ( NULL == lpszString )
		return FALSE;
	if ( NULL == lpszString[0] )	// strlen
		return FALSE;

	UINT  i		= 0;
	UINT  uLen	= ( 0 == uStringLen ? (UINT)_tcslen(lpszString) : uStringLen );

	//
	//	...
	//
	if ( '.' == lpszString[0] || '.' == lpszString[ uLen - sizeof(TCHAR) ] )
	{
		return FALSE;
	}
	if ( 3 != _vwfunc_get_chr_count( lpszString, '.', uLen ) )
	{
		return FALSE;
	}

	for ( i = 0; i < uLen; i ++ )
	{
		if ( NULL == _tcschr( CONST_VWFUNCBASE_VALIDIPADDRSTRING, lpszString[i] ) )
		{
			return FALSE;
		}
	}
	return TRUE;
}

/**
 *	����ַ����Ƿ��ǺϷ�������
 */
BOOL  _vwfunc_is_valid_domain( LPCTSTR lpszString )
{
	/*
		lpszString	- [in] ������ַ���
		RETURN		- TRUE/FALSE
	*/

	if ( NULL == lpszString )
	{
		return FALSE;
	}
	if ( NULL == lpszString[0] )
	{
		return FALSE;
	}

	UINT  i		= 0;
	UINT  uLen	= (UINT)_tcslen( lpszString );

	//
	//	...
	//
	if ( '.' == lpszString[ uLen - sizeof(TCHAR) ] )
	{
		return FALSE;
	}

	for ( i = 0; i < uLen; i ++ )
	{
		if ( NULL == _tcschr( CONST_VWFUNCBASE_VALIDDOMAINSTRING, lpszString[i] ) )
		{
			return FALSE;
		}
	}
	return TRUE;
}


/**
 *	��������Ƿ��ǹ�������
 */
static inline BOOL _vwfunc_is_inland_domain( LPCTSTR lpcszHost, UINT uHostLen /* = 0 */ )
{
	//
	//	lpcszHost	- [in] HOST
	//	uHostLen	- [in] Length of HOST
	//

	if ( NULL == lpcszHost )
	{
		return FALSE;
	}
	if ( NULL == lpcszHost[0] )	// strlen
	{
		return FALSE;
	}

	BOOL    bRet		= FALSE;
	UINT	uLen		= ( 0 == uHostLen ? (UINT)_tcslen(lpcszHost) : uHostLen );	//	���㳤��
	LPCTSTR lpszMove	= NULL;
	LPCTSTR lpszTag		= NULL;
	TCHAR   szDmExt[ 16 ];


	//	"|.com.cn|.com.tw|.net.cn|.net.tw|.org.cn|.org.tw|.gov.cn|.gov.tw|"
	//	7 = _tcslen(".com.cn")
	if ( uLen > 7 )
	{
		//	ȡ HOST �� 7 λ�ַ�����
		//	7 = _tcslen(".com.cn")
		lpszMove = lpcszHost + uLen - 7;
		if ( lpszMove )
		{
			//	�� HOST �� 7 λ�ַ������� szDmExt
			//	7 = _tcslen(".com.cn")
			//	8 = _tcslen("|.com.cn")
			_sntprintf( szDmExt, sizeof(szDmExt)/sizeof(TCHAR)-1, _T("|%s"), lpszMove );
			szDmExt[ 8 ]	= '|';
			szDmExt[ 9 ]	= 0;
			_tcslwr( szDmExt );

			if ( _tcsstr( CONST_VWFUNCBASE_DOMAINEXTLIST, szDmExt ) )
			{
				bRet = TRUE;
			}

			//
			//lpszTag = _tcsstr( CONST_VWFUNCBASE_DOMAINEXTLIST, szDmExt );
			//if ( lpszTag )
			//{
			//	lpszTag --;
			//	if ( lpszTag && '|' == *lpszTag )
			//	{
			//		lpszTag += ( 1 + _tcslen(lpszMove) );
			//		if ( lpszTag && '|' == *lpszTag )
			//		{
			//			bRet = TRUE;
			//		}
			//	}
			//}
		}
	}

	return bRet;
}

/**
 *	��ȡ Root ����
 */
BOOL _vwfunc_get_root_host( LPCTSTR lpcszHost, LPTSTR lpszRootHost, DWORD dwSize )
{
	/*
		lpcszHost	- [in]  ������ HOST
		lpszRootHost	- [out] ���� ROOT HOST
		dwSize		- [in]  ���ػ������Ĵ�С
		RETURN		- TRUE/FALSE
	*/

	if ( NULL == lpcszHost || NULL == lpszRootHost )
		return FALSE;
	if ( NULL == lpcszHost[0] )	// strlen
		return FALSE;
	
	BOOL   bRet		= FALSE;
	UINT   uHostLen		= (UINT)_tcslen(lpcszHost);
	LPTSTR lpszMove		= NULL;
	DWORD  dwDotNum		= 0;
	INT    i		= 0;
	INT    nDot		= 0;

	if ( _vwfunc_is_valid_ipaddr( lpcszHost ) )
	{
		//	�����һ�� IP ��ַ����ֱ�ӷ��� IP ��ַ�������ټ���
		//	_sntprintf( lpszRootHost, dwSize-sizeof(TCHAR), "%s", lpcszHost );
		memcpy( lpszRootHost, lpcszHost, min(uHostLen,dwSize-sizeof(TCHAR)) );
		lpszRootHost[ min(uHostLen,dwSize-sizeof(TCHAR)) ] = 0;
		return TRUE;
	}
	

	//
	//	��ʼ���� ROOT HOST
	//
	dwDotNum = _vwfunc_get_chr_count( lpcszHost, '.' );
	if ( 0 == dwDotNum || 1 == dwDotNum )
	{
		//
		//	���� xxxxx.com
		//
		bRet = TRUE;
		//	_sntprintf( lpszRootHost, dwSize-sizeof(TCHAR), "%s", lpcszHost );
		memcpy( lpszRootHost, lpcszHost, min(dwSize-sizeof(TCHAR),uHostLen) );
		lpszRootHost[ min(dwSize-sizeof(TCHAR),uHostLen) ] = 0;
	}
	else if ( 2 == dwDotNum && _vwfunc_is_inland_domain(lpcszHost) )
	{
		//
		//	���� xxxxx.com.cn
		//
		bRet = TRUE;
		//	_sntprintf( lpszRootHost, dwSize-sizeof(TCHAR), "%s", lpcszHost );
		memcpy( lpszRootHost, lpcszHost, min(dwSize-sizeof(TCHAR),uHostLen) );
		lpszRootHost[ min(dwSize-sizeof(TCHAR),uHostLen) ] = 0;
	}
	else
	{
		//
		//	3 �����ϣ����� abc.ccc.xxxxx.com ���� abc.xxxxx.com.cn
		//
		if ( _vwfunc_is_inland_domain(lpcszHost) )
		{
			//
			//	�������������� abc.xxxxx.com.cn����Ҫȡ xxxxx.com.cn
			//
			for ( i = uHostLen-sizeof(TCHAR); i >= 0; i -- )
			{
				if ( '.' == lpcszHost[i] )
				{
					nDot ++;
					if ( 3 == nDot )
						break;
				}
			}
			bRet = TRUE;
			//	_sntprintf( lpszRootHost, dwSize-sizeof(TCHAR), "%s", lpcszHost+i+sizeof(TCHAR) );
			memcpy( lpszRootHost, lpcszHost+i+sizeof(TCHAR), min( dwSize-sizeof(TCHAR), _tcslen(lpcszHost+i+sizeof(TCHAR)) ) );
			lpszRootHost[ min( dwSize-sizeof(TCHAR), _tcslen(lpcszHost+i+sizeof(TCHAR)) ) ] = 0;
		}
		else
		{
			//
			//	���� abc.ccc.xxxxx.com / www.xxxxx.com����Ҫȡ xxxxx.com
			//
			for ( i = uHostLen-sizeof(TCHAR); i >= 0; i -- )
			{
				if ( '.' == lpcszHost[i] )
				{
					nDot ++;
					if ( 2 == nDot )
						break;
				}
			}
			bRet = TRUE;
			//	_sntprintf( lpszRootHost, dwSize-sizeof(TCHAR), "%s", lpcszHost+i+sizeof(TCHAR) );
			memcpy( lpszRootHost, lpcszHost+i+sizeof(TCHAR), min( dwSize-sizeof(TCHAR), _tcslen(lpcszHost+i+sizeof(TCHAR)) ) );
			lpszRootHost[ min( dwSize-sizeof(TCHAR), _tcslen(lpcszHost+i+sizeof(TCHAR)) ) ] = 0;
		}
	}
	
	return bRet;
}


/**
 *	��ȡ Root ����2 �����溯����һ����������������ܻ᷵�� .abc.com.cn �����Ľ��
 */
BOOL _vwfunc_get_root_host2( LPCTSTR lpcszHost, LPTSTR lpszRootHost, DWORD dwSize )
{
	//
	//	lpcszHost	- [in]  ������ HOST
	//	lpszRootHost	- [out] ���� ROOT HOST
	//	dwSize		- [in]  ���ػ������Ĵ�С
	//	RETURN		- TRUE/FALSE
	//

	if ( NULL == lpcszHost || NULL == lpszRootHost )
		return FALSE;
	if ( NULL == lpcszHost[0] )	// strlen
		return FALSE;
	
	BOOL   bRet		= FALSE;
	UINT   uHostLen		= (UINT)_tcslen(lpcszHost);
	LPTSTR lpszMove		= NULL;
	DWORD  dwDotNum		= 0;
	INT    i		= 0;
	INT    nDot		= 0;

	if ( _vwfunc_is_valid_ipaddr( lpcszHost ) )
	{
		//	�����һ�� IP ��ַ����ֱ�ӷ��� IP ��ַ�������ټ���
		//	_sntprintf( lpszRootHost, dwSize-sizeof(TCHAR), "%s", lpcszHost );
		memcpy( lpszRootHost, lpcszHost, min(uHostLen,dwSize-sizeof(TCHAR)) );
		lpszRootHost[ min(uHostLen,dwSize-sizeof(TCHAR)) ] = 0;
		return TRUE;
	}
	

	//
	//	��ʼ���� ROOT HOST
	//
	dwDotNum = _vwfunc_get_chr_count( lpcszHost, '.' );
	if ( 0 == dwDotNum || 1 == dwDotNum )
	{
		//
		//	���� xxxxx.com
		//
		bRet = TRUE;
		//	_sntprintf( lpszRootHost, dwSize-sizeof(TCHAR), "%s", lpcszHost );
		memcpy( lpszRootHost, lpcszHost, min(dwSize-sizeof(TCHAR),uHostLen) );
		lpszRootHost[ min(dwSize-sizeof(TCHAR),uHostLen) ] = 0;
	}
	else if ( 2 == dwDotNum && _vwfunc_is_inland_domain(lpcszHost) )
	{
		//
		//	���� xxxxx.com.cn
		//
		bRet = TRUE;
		//	_sntprintf( lpszRootHost, dwSize-sizeof(TCHAR), "%s", lpcszHost );
		memcpy( lpszRootHost, lpcszHost, min(dwSize-sizeof(TCHAR),uHostLen) );
		lpszRootHost[ min(dwSize-sizeof(TCHAR),uHostLen) ] = 0;
	}
	else
	{
		//
		//	3 �����ϣ����� abc.ccc.xxxxx.com ���� abc.xxxxx.com.cn
		//
		if ( _vwfunc_is_inland_domain(lpcszHost) )
		{
			//
			//	�������������� abc.xxxxx.com.cn����Ҫȡ xxxxx.com.cn
			//
			for ( i = uHostLen-sizeof(TCHAR); i >= 0; i -- )
			{
				if ( '.' == lpcszHost[i] )
				{
					nDot ++;
					if ( 3 == nDot )
						break;
				}
			}
			bRet = TRUE;
			//	_sntprintf( lpszRootHost, dwSize-sizeof(TCHAR), "%s", lpcszHost+i+sizeof(TCHAR) );
			memcpy( lpszRootHost, lpcszHost+i, min( dwSize-sizeof(TCHAR), _tcslen(lpcszHost+i) ) );
			lpszRootHost[ min( dwSize-sizeof(TCHAR), _tcslen(lpcszHost+i) ) ] = 0;
		}
		else
		{
			//
			//	���� abc.ccc.xxxxx.com / www.xxxxx.com����Ҫȡ xxxxx.com
			//
			for ( i = uHostLen-sizeof(TCHAR); i >= 0; i -- )
			{
				if ( '.' == lpcszHost[i] )
				{
					nDot ++;
					if ( 2 == nDot )
						break;
				}
			}
			bRet = TRUE;
			//	_sntprintf( lpszRootHost, dwSize-sizeof(TCHAR), "%s", lpcszHost+i+sizeof(TCHAR) );
			memcpy( lpszRootHost, lpcszHost+i, min( dwSize-sizeof(TCHAR), _tcslen(lpcszHost+i) ) );
			lpszRootHost[ min( dwSize-sizeof(TCHAR), _tcslen(lpcszHost+i) ) ] = 0;
		}
	}
	
	return bRet;
}

LPCTSTR _vwfunc_get_root_host_ptr( LPCTSTR lpcszHost )
{
	//
	//	lpcszHost	- [in]  ������ HOST
	//	RETURN		- TRUE/FALSE
	//
	
	if ( NULL == lpcszHost )
	{
		return NULL;
	}
	if ( NULL == lpcszHost[0] )
	{
		return NULL;
	}

	BOOL   bRet		= FALSE;
	UINT   uHostLen;
	LPTSTR lpszMove		= NULL;
	DWORD  dwDotNum		= 0;
	INT    i		= 0;
	INT    nDot		= 0;


	uHostLen = (UINT)_tcslen(lpcszHost);

	if ( _vwfunc_is_valid_ipaddr( lpcszHost ) )
	{
		//	�����һ�� IP ��ַ����ֱ�ӷ��� IP ��ַ�������ټ���
		//	_sntprintf( lpszRootHost, dwSize-sizeof(TCHAR), "%s", lpcszHost );
		return lpcszHost;
	}
	
	
	//
	//	��ʼ���� ROOT HOST
	//
	dwDotNum = _vwfunc_get_chr_count( lpcszHost, '.' );
	if ( 0 == dwDotNum || 1 == dwDotNum )
	{
		//
		//	���� xxxxx.com
		//
		return lpcszHost;
	}
	else if ( 2 == dwDotNum && _vwfunc_is_inland_domain(lpcszHost) )
	{
		//
		//	���� xxxxx.com.cn
		//
		return lpcszHost;
	}
	else
	{
		//
		//	3 �����ϣ����� abc.ccc.xxxxx.com ���� abc.xxxxx.com.cn
		//
		if ( _vwfunc_is_inland_domain(lpcszHost) )
		{
			//
			//	�������������� abc.xxxxx.com.cn����Ҫȡ xxxxx.com.cn
			//
			for ( i = uHostLen-sizeof(TCHAR); i >= 0; i -- )
			{
				if ( '.' == lpcszHost[i] )
				{
					nDot ++;
					if ( 3 == nDot )
						break;
				}
			}

			return lpcszHost+i;
		}
		else
		{
			//
			//	���� abc.ccc.xxxxx.com / www.xxxxx.com����Ҫȡ xxxxx.com
			//
			for ( i = uHostLen-sizeof(TCHAR); i >= 0; i -- )
			{
				if ( '.' == lpcszHost[i] )
				{
					nDot ++;
					if ( 2 == nDot )
						break;
				}
			}

			return lpcszHost+i;
		}
	}
	
	return NULL;
}
LPCTSTR _vwfunc_get_root_host_ptr_ex( LPCTSTR lpcszHost, UINT * uRetLen )
{
	//
	//	lpcszHost	- [in]  ������ HOST�������� "http://" ���� "https://" ��ͷ��
	//	uRetLen		- [out] ���� HOST �ĳ��ȣ������������ NULL�����ֵ�� 0
	//	RETURN		- TRUE/FALSE
	//

	if ( NULL == lpcszHost )
	{
		return NULL;
	}
	if ( NULL == lpcszHost[0] )
	{
		return NULL;
	}
	if ( NULL == uRetLen )
	{
		return NULL;
	}

	LPCTSTR lpcszRet	= NULL;
	UINT   uHostLen;
	LPCTSTR lpcszHostStart	= NULL;
	LPTSTR lpszHostEnd	= NULL;
	LPTSTR lpszPortMark	= NULL;
	LPTSTR lpszMove		= NULL;
	DWORD  dwDotNum		= 0;
	INT    i		= 0;
	INT    nDot		= 0;


	lpcszRet	= NULL;
	*uRetLen	= 0;
	lpcszHostStart	= lpcszHost;

	if ( 0 == _tcsnicmp( lpcszHostStart, _T("http://"), 7 ) )
	{
		lpcszHostStart += 7;
	}
	if ( 0 == _tcsnicmp( lpcszHostStart, _T("https://"), 8 ) )
	{
		lpcszHostStart += 8;
	}

	lpszPortMark	= _tcschr( lpcszHostStart, ':' );
	lpszHostEnd	= _tcschr( lpcszHostStart, '/' );
	if ( lpszHostEnd )
	{
		//
		//	TODO:
		//	δ���� "http://username:password@www.url.com:800/abc.html" �����
		//
		//	������ܴ��˿ڵ����
		//	"http://www.url.com:800/abc.html"
		//
		if ( lpszPortMark && lpszPortMark < lpszHostEnd )
		{
			//	ȷ�����ڶ˿ڷ��ţ�����λ����ȷ
			lpszHostEnd = lpszPortMark;
		}
		//	123/
		uHostLen = (UINT)( lpszHostEnd - lpcszHostStart );
	}
	else
	{
		//
		//	TODO:
		//	δ���� "http://username:password@www.url.com:800" �����
		//
		//	������ܴ��˿ڵ����
		//	"http://www.url.com:800"
		//
		if ( lpszPortMark )
		{
			uHostLen = (UINT)( lpszPortMark - lpcszHostStart );
		}
		else
		{
			uHostLen = (UINT)_tcslen(lpcszHostStart);
		}
	}


	if ( _vwfunc_is_valid_ipaddr( lpcszHostStart, uHostLen ) )
	{
		//	�����һ�� IP ��ַ����ֱ�ӷ��� IP ��ַ�������ټ���
		//	_sntprintf( lpszRootHost, dwSize-sizeof(TCHAR), "%s", lpszHostStart );
		lpcszRet	= lpcszHostStart;
		*uRetLen	= uHostLen;
	}
	else
	{
		//
		//	��ʼ���� ROOT HOST
		//
		dwDotNum = _vwfunc_get_chr_count( lpcszHostStart, '.', uHostLen );
		if ( 0 == dwDotNum || 1 == dwDotNum )
		{
			//
			//	���� xxxxx.com
			//
			lpcszRet	= lpcszHostStart;
			*uRetLen	= uHostLen;
		}
		else if ( 2 == dwDotNum && _vwfunc_is_inland_domain( lpcszHostStart, uHostLen ) )
		{
			//
			//	���� xxxxx.com.cn
			//
			lpcszRet	= lpcszHostStart;
			*uRetLen	= uHostLen;
		}
		else
		{
			//
			//	3 �����ϣ����� abc.ccc.xxxxx.com ���� abc.xxxxx.com.cn
			//
			if ( _vwfunc_is_inland_domain( lpcszHostStart, uHostLen ) )
			{
				//
				//	�������������� abc.xxxxx.com.cn����Ҫȡ xxxxx.com.cn
				//
				for ( i = uHostLen-sizeof(TCHAR); i >= 0; i -- )
				{
					if ( '.' == lpcszHostStart[i] )
					{
						nDot ++;
						if ( 3 == nDot )
						{
							break;
						}
					}
				}

				lpcszRet	= lpcszHostStart + i;
				*uRetLen	= uHostLen - i;
			}
			else
			{
				//
				//	���� abc.ccc.xxxxx.com / www.xxxxx.com����Ҫȡ xxxxx.com
				//
				for ( i = uHostLen-sizeof(TCHAR); i >= 0; i -- )
				{
					if ( '.' == lpcszHostStart[i] )
					{
						nDot ++;
						if ( 2 == nDot )
						{
							break;
						}
					}
				}

				lpcszRet	= lpcszHostStart + i;
				*uRetLen	= uHostLen - i;
			}
		}
	}	
	
	return lpcszRet;
}



/**
 *	�� URL ����ȡ HOST
 */
BOOL _vwfunc_get_host_from_url( LPCTSTR lpcszUrl, LPTSTR lpszHost, DWORD dwSize )
{
	//
	//	lpcszUrl	- [in]  URL : must start with "http://..."
	//	lpszHost	- [out] ���� HOST
	//	dwSize		- [in]  ���� HOST ��������С
	//

	if ( NULL == lpcszUrl || NULL == lpszHost || NULL == lpcszUrl[0] )
	{
		return FALSE;
	}

	BOOL bRet		= FALSE;
	LPTSTR lpszHead		= NULL;
	LPTSTR lpszTail		= NULL;

	__try
	{
		// find "://"
		lpszHead = (LPTSTR)strchr( lpcszUrl, ':' );
		if ( lpszHead )
		{
			lpszHead = CharNext( lpszHead );
			if ( lpszHead && '/' == *lpszHead )
			{
				lpszHead = CharNext( lpszHead );
				if ( lpszHead && '/' == *lpszHead )
				{
					lpszHead = CharNext(lpszHead);
					if ( lpszHead )
					{
						bRet		= TRUE;
						lpszTail	= strchr( lpszHead, '/' );
						if ( lpszTail )
						{
							memcpy( lpszHost, lpszHead, (size_t)( min( lpszTail-lpszHead, dwSize-1 ) ) );
							lpszHost[ min( lpszTail-lpszHead, dwSize-1 ) ] = 0;
						}
						else
						{
							memcpy( lpszHost, lpszHead, (size_t)( min( _tcslen(lpszHead), dwSize-1 ) ) );
							lpszHost[ min( _tcslen(lpszHead), dwSize-1 ) ] = 0;
						}
					}
				}
			}
		}
	}
	__except( EXCEPTION_EXECUTE_HANDLER )
	{
	}

	return bRet;
}

/**
 *	��ȡ URL ������Ĳ�����ֵ
 */
BOOL _vwfunc_get_request_query_string( LPCTSTR lpcszUrl, LPCTSTR lpcszName, LPTSTR lpszValue, DWORD dwSize )
{
	/*
		lpcszUrl	- [in]  URL
		lpcszName	- [in]  ��������
		lpszValue	- [out] ����ֵ
		dwSize		- [in]  ����ֵ�Ļ�������С
	*/

	if ( NULL == lpcszUrl || NULL == lpcszName || NULL == lpszValue )
		return FALSE;

	BOOL	bRet		= FALSE;
	LPTSTR	lpszHead	= NULL;
	LPTSTR	lpszTail	= NULL;
	INT	nLen		= 0;

	lpszHead = (LPTSTR)_tcsstr( lpcszUrl, lpcszName );
	if ( lpszHead )
	{
		lpszHead += _tcslen(lpcszName);
		if ( lpszHead )
		{
			if ( '=' == *lpszHead )
			{
				lpszHead = CharNext(lpszHead);
				if ( lpszHead )
				{
					//lpszTail = _tcschr( lpszHead, '&' );
					lpszTail = strpbrk( lpszHead, " &\r\n" );
					if ( lpszTail )
						nLen = (INT)min( lpszTail-lpszHead, dwSize-1 );
					else
						nLen = (INT)min( _tcslen(lpszHead), dwSize-1 );

					bRet = TRUE;
					memcpy( lpszValue, lpszHead, nLen );
					lpszValue[ nLen ] = 0;
				}
			}
		}
	}

	return bRet;
}

/**
 *	��ȡָ�� Cookie Value
 */
BOOL _vwfunc_get_spec_cookie_value( LPCTSTR lpszCookieString, LPCTSTR lpszCookieName, LPTSTR lpszValue, DWORD dwSize )
{
	/*
		lpszCookieString	- [in]  ���� COOKIE ���ַ���
		lpszCookieName		- [in]  COOKIE ����
		lpszValue		- [out] ����ֵ
		dwSize			- [in]  ����ֵ��������С
		RETURN			- TRUE / FALSE
	*/

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
	
	lpszMove = (LPTSTR)_tcsstr( lpszCookieString, lpszCookieName );
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
 *	�滻���ɼ��Ŀ��Ʒ���Ϊ��_��
 */
TCHAR * _vwfunc_replace_controlchars( LPSTR lpszStr, UINT uLen )
{
	/*
		lpszStr		- [in]
		uLen		- [in]
	*/

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
inline BOOL _vwfunc_memcpy( LPCTSTR lpcszSrc, DWORD dwSrcLen, LPTSTR lpszDst, DWORD dwDstSize, BOOL bRepCtlChr /*= FALSE*/ )
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
		_vwfunc_replace_controlchars( lpszDst, uLen );
	}

	return TRUE;
}

/**
 *	parse url
 */
BOOL _vwfunc_parse_url( LPCTSTR lpcszUrl, DWORD dwUrlLen, STPARSEURL * pstParseUrl )
{
	/*
		lpcszUrl		- [in]  URL
		dwUrlLen		- [in]  URL ����
		pstParseUrl		- [out] ����ֵ
		RETURN			- TRUE / FALSE
	*/

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
			_vwfunc_memcpy( s, ( e - s ), pstParseUrl->szScheme, sizeof( pstParseUrl->szScheme ), TRUE );
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

			_vwfunc_memcpy( s, (e-s), pstParseUrl->szScheme, sizeof( pstParseUrl->szScheme ), TRUE );

			dwUrlLen -= ++e - s;
			s = e;
			goto just_path;
		}
		else
		{
			_vwfunc_memcpy( s, (e-s), pstParseUrl->szScheme, sizeof( pstParseUrl->szScheme ), TRUE );

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
				_vwfunc_memcpy( s, (pp-s), pstParseUrl->szUser, sizeof( pstParseUrl->szUser ), TRUE );
			}

			pp++;
			if ( p - pp > 0 )
			{
				//ret->pass = zend_strndup( pp, (p-pp) );
				_vwfunc_memcpy( pp, (p-pp), pstParseUrl->szPass, sizeof( pstParseUrl->szPass ), TRUE );
			}
		}
		else
		{
			//ret->user = zend_strndup(s, (p-s));
			_vwfunc_memcpy( s, (p-s), pstParseUrl->szUser, sizeof( pstParseUrl->szUser ), TRUE );
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
	_vwfunc_memcpy( s, (p-s), pstParseUrl->szHost, sizeof( pstParseUrl->szHost ), TRUE );

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
			_vwfunc_memcpy( s, (p-s), pstParseUrl->szPath, sizeof( pstParseUrl->szPath ), TRUE );
		}	

		if ( pp )
		{
			if ( pp - ++p )
			{
				//ret->query = estrndup(p, (pp-p));
				_vwfunc_memcpy( p, (pp-p), pstParseUrl->szQuery, sizeof( pstParseUrl->szQuery ), TRUE );
			}
			p = pp;
			goto label_parse;
		}
		else if ( ++p - ue )
		{
			//ret->query = estrndup(p, (ue-p));
			_vwfunc_memcpy( p, (ue-p), pstParseUrl->szQuery, sizeof( pstParseUrl->szQuery ), TRUE );
		}
	}
	else if ( ( p = (char*)memchr(s, '#', (ue - s)) ) )
	{
		if ( p - s )
		{
			//ret->path = estrndup(s, (p-s));
			_vwfunc_memcpy( s, (p-s), pstParseUrl->szPath, sizeof( pstParseUrl->szPath ), TRUE );
		}

		label_parse:
		p++;

		if ( ue - p )
		{
			//ret->fragment = estrndup(p, (ue-p));
			_vwfunc_memcpy( p, (ue-p), pstParseUrl->szFragment, sizeof( pstParseUrl->szFragment ), TRUE );
		}
	}
	else
	{
		//ret->path = estrndup(s, (ue-s));
		_vwfunc_memcpy( s, (ue-s), pstParseUrl->szPath, sizeof( pstParseUrl->szPath ), TRUE );
	}
end:
	return TRUE;
}

/**
 *	url decode
 */
INT inline _vwfunc_char_to_int( char c )
{
	if ( (c >= '0') && (c <= '9') )
		return (c - '0');
	else if ( (c >= 'A') && (c <= 'F') )
		return (c - 'A' + 10);
	else if ( (c >= 'a') && (c <= 'f') )
		return (c - 'a' + 10);
	else
		return 0;
}
VOID _vwfunc_get_url_decode( char *str )
{
	char *pDest = str;
	char *pSrc = str;
	int n;
	
	while ( pSrc && *pSrc )
	{
		if ( '+' == *pSrc )
		{
			*pDest ++ = ' ';
			pSrc ++;
		}
		else if ( '%' == *pSrc )
		{
			if ( ( '\0' != *(pSrc+1) ) && ( '\0' != *(pSrc+2) ) )
			{
				n = _vwfunc_char_to_int(*(pSrc+1)) << 4;
				n += _vwfunc_char_to_int(*(pSrc+2));
				*pDest++ = (char)n;
				pSrc += 3;
			}
			else
			{
				*pDest++ = *pSrc++;
			}
		}
		else
		{
			*pDest++ = *pSrc++;
		}
	}
	if ( pDest )
		*pDest = '\0';
}

/*
	url encode - �������ַ�����FORM����

	����������
		strInput	Ϊ���봮����Ҫ����url�ķ�form����
		nInputLen	Ϊ���봮�ĳ��ȣ�-1��ʾ���� '\0' ��β
		strOutBuf	Ϊ�����������
		nOutBufLen	Ϊ�����������С��������β�� '\0'

	����ֵ��
		������ĳ��ȣ���������β��0

	��� nOutBufLen �ĳ���Ϊ 0���򷵻���Ҫ������������Ĵ�С��������β��0
*/
INT _vwfunc_get_url_encode( LPCTSTR lpcszInput, INT nInputLen, LPTSTR lpszOutBuf, INT nOutBufLen )
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
		{
			nInputLen = (INT)_tcslen(lpcszInput);
		}

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


/**
 *	�����ַ�������
 */
BOOL _vwfunc_swap_string( CHAR * lpszString, INT nLen, INT nLeftStrLen )
{
	//
	//	lpszString	- [in] ����Դ�ַ���
	//	uLen		- [in] ����Դ�ַ����ĳ���
	//	uLeftStrLen	- [in] �����Ҫ�ƶ����ַ����ĳ���
	//
	INT i, j, k;
	CHAR cTemp;

	if ( NULL == lpszString || 0 == nLen || nLeftStrLen >= nLen )
	{
		return FALSE;
	}	

	__try
	{
		for ( i = nLeftStrLen, k = 1;
			i < nLen;
			i++, k++ )
		{
			for ( j = i; j >= k ; j -- )
			{
				cTemp			= lpszString[ j ];
				lpszString[ j ]		= lpszString[ j - 1 ];
				lpszString[ j - 1 ]	= cTemp;
			}
		}
	}
	__except( EXCEPTION_EXECUTE_HANDLER )
	{
	}

	return TRUE;
}


/**
*	��ȡ URL �е��ļ���Ϣ
*/
BOOL _vwfunc_get_fileinfo_from_url( LPCTSTR lpcszFullUri, LPTSTR lpszExt, DWORD dwESize, LPTSTR lpszFile, DWORD dwFSize )
{
	/*
		lpcszFullUri	- [in]  URI
		lpszExt		- [out] �ļ���չ��
		dwESize		- [in]  �ļ���չ����������С
		lpszFile	- [out] �ļ���
		dwFSize		- [in]  �ļ�����������С
	*/
	
	INT	i			= 0;
	INT	nDotStartPos		= 0;
	INT	nFileStartPos		= 0;
	INT	nEndPos			= 0;
	BOOL	bFindDot		= FALSE;
	BOOL	bFindFile		= FALSE;

	if ( NULL == lpcszFullUri || NULL == lpszExt )
		return FALSE;
	
	// in:  "/test/go/aaa.exe?fp=xxx"
	// out: ".exe"
	nEndPos = (INT)_tcslen(lpcszFullUri)-sizeof(TCHAR);
	for ( i = nEndPos; i >= 0; i -- )
	{
		if ( '?' == lpcszFullUri[i] || '#' == lpcszFullUri[i] )
		{
			nEndPos = i - 1;	// �õ���չ�����һ���ַ���λ��
			break;
		}
	}

	for ( i = nEndPos; i >= 0; i -- )
	{
		if ( ! bFindDot && '.' == lpcszFullUri[i] )
		{
			bFindDot = TRUE;
			nDotStartPos = i;		//	�õ���һ���ַ���λ��
		}
		if ( ! bFindFile && '/' == lpcszFullUri[i] )
		{
			bFindFile = TRUE;
			nFileStartPos = i;		//	�õ���һ���ַ���λ��
			break;
		}
	}
	
	//
	//	��չ��
	//
	if ( bFindDot )
	{
		memcpy( lpszExt, lpcszFullUri+nDotStartPos, min(nEndPos-nDotStartPos+1,dwESize-sizeof(TCHAR)) );
		lpszExt[ min(nEndPos-nDotStartPos+1,dwESize-sizeof(TCHAR)) ] = 0;
	}
	
	//
	//	�ļ���
	//
	if ( bFindFile )
	{
		memcpy( lpszFile, lpcszFullUri+nFileStartPos+sizeof(TCHAR), min( nEndPos-nFileStartPos, dwFSize-sizeof(TCHAR) ) );
		lpszFile[ min( nEndPos-nFileStartPos, dwFSize-sizeof(TCHAR) ) ] = 0;
	}
	
	return lpszExt[0] ? TRUE : FALSE;
}





/**
*	����һ�������Ľ���
*/
BOOL _vwfunc_run_block_process( LPCTSTR lpszCmdLine )
{
	if ( NULL == lpszCmdLine )
		return FALSE;
	
	BOOL bRet = FALSE;
	BOOL bCreateProcess = FALSE;
	DWORD dweExitCode = 0;
	STARTUPINFO si;
	PROCESS_INFORMATION pi;
	
	memset( &si, 0, sizeof(si) );
	si.cb = sizeof(si);
	memset( &pi, 0, sizeof(pi) );
	
	// Start the child process. 
	bCreateProcess = CreateProcess
	(
		NULL,			// No module name (use command line). 
		(TCHAR*)lpszCmdLine,		// Command line. 
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
		// Wait until child process exits.
		if ( pi.hProcess )
		{
			if ( WAIT_OBJECT_0 == 
				WaitForSingleObject( pi.hProcess, INFINITE ) )
			{
				GetExitCodeProcess( pi.hProcess, &dweExitCode );
			}
		}
		// Close process and thread handles. 
		if ( pi.hProcess )
			CloseHandle( pi.hProcess );
		if ( pi.hThread )
			CloseHandle( pi.hThread );
	}
	
	return bRet;
}


/**
 *	��һ�� URL
 */
BOOL _vwfunc_lauch_iebrowser( LPCTSTR lpcszUrl )
{
	__try
	{
		IWebBrowser2 * pBrowserApp = NULL;
		WCHAR wszUrlPage[1024];
		VARIANT vtEmpty;

		CoInitialize( NULL );

		if( FAILED( CoCreateInstance(CLSID_InternetExplorer, NULL, CLSCTX_SERVER, IID_IWebBrowser2, (void**)&pBrowserApp) ) )
		{
			CoUninitialize();
			return FALSE;
		}
		
		if (pBrowserApp != NULL)
		{
			// show the browser.
			HWND hwnd = NULL;
#ifdef _AMD64__
			pBrowserApp->get_HWND( (SHANDLE_PTR *)&hwnd );
#else
			pBrowserApp->get_HWND( (long *)&hwnd );
#endif
			if ( hwnd != NULL )
			{
				::SetForegroundWindow(hwnd); 
				//::ShowWindow(hwnd, SW_SHOWMAXIMIZED);
				::ShowWindow(hwnd, SW_SHOW);
			}
			
			// visit the url.
			if ( strlen( lpcszUrl ) > 0 )
			{
				memset( wszUrlPage,0,sizeof(wszUrlPage) );
				MultiByteToWideChar(CP_ACP, 0, lpcszUrl, -1, wszUrlPage, (sizeof(wszUrlPage)/sizeof(WCHAR))-1);
				
				VARIANT varUrl;
				vtEmpty.vt = VT_EMPTY;
				varUrl.vt = VT_BSTR;
				varUrl.bstrVal = SysAllocString(wszUrlPage);
				HRESULT hr = pBrowserApp->Navigate2(&varUrl, &vtEmpty, &vtEmpty, &vtEmpty, &vtEmpty);
				SysFreeString( varUrl.bstrVal ); 
			}
			pBrowserApp->Release();
		}
		CoUninitialize();
	}
	__except( EXCEPTION_EXECUTE_HANDLER )
	{
		CoUninitialize();
		return FALSE;
	}
	return TRUE;
}
