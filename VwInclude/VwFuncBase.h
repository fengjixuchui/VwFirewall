// VwFunc.h: interface for the CVwFunc class.
//
//////////////////////////////////////////////////////////////////////

#ifndef __VWFUNCBASE_HEADER__
#define __VWFUNCBASE_HEADER__

#include <stdio.h>
#include <stdlib.h>
#include <tchar.h>

#include <vector>
using namespace std;

#include <winsvc.h>
#pragma comment( lib, "Version.lib" )

#include "md5.h"
#include "MultiString.h"

/**
 *	��������
 */
#define CONST_VWFUNCBASE_MAX_FILEMAP_LENGTH	( 100 * 1024 * 1024 )
#define CONST_VWFUNCBASE_VALIDIPADDRSTRING	"1234567890."	//	�Ƿ���Ч�� IP ��ַ��������õ��ĳ���
#define CONST_VWFUNCBASE_VALIDDOMAINSTRING	"*.-1234567890abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ"	//	���������ܰ������ַ�
#define CONST_VWFUNCBASE_DOMAINEXTLIST		"|.com.cn|.com.tw|.com.mx|.net.cn|.net.tw|.org.cn|.org.tw|.org.nz|.org.uk|.gov.cn|.gov.tw|.idv.tw|.us.com|.br.com|.cn.com|.de.com|.eu.com|.hu.com|.no.com|.qc.com|.ru.com|.sa.com|.se.com|.uk.com|.uy.com|.za.com|.se.net|.uk.net|"



//////////////////////////////////////////////////////////////////////////

/**
 *	�滻�ַ��������һ���Ӵ�
 */
static void _vwfunc_strchg(char* dest, int len, char* rstr)
{
	int rlen = (int)strlen(rstr);
	memmove(dest+rlen, dest+len, strlen(dest+len)+1);
	memcpy(dest, rstr, rlen);
}
//	�޷��������Ӵ�
static char * _vwfunc_stristr(char* src, char* dst)
{
	for(;(strlen(src)-strlen(dst) >= 0 && *src);src++)
	{
		if (!strnicmp(src, dst, strlen(dst)))
			return src;
	}
	return NULL;
}
//	�滻Ŀ���ַ���������Ӵ�
static void _vwfunc_replace( char* src, char* sFindStr, char* sRepWithStr )
{
	for(char* pos=src;(pos=_vwfunc_stristr(pos, sFindStr))!=NULL;pos=pos+strlen(sRepWithStr))
	{
		_vwfunc_strchg(pos, strlen(sFindStr), sRepWithStr);
	}
}

//////////////////////////////////////////////////////////////////////////

typedef struct tagParseUrl
{
	tagParseUrl()
	{
		memset( this, 0, sizeof(tagParseUrl) );
	}
	
	TCHAR szScheme[ 16 ];		//	e.g. http
	TCHAR szUser[ 32 ];
	TCHAR szPass[ 32 ];
	TCHAR szHost[ 128 ];
	UINT  uPort;
	TCHAR szPath[ 1024 ];
	TCHAR szQuery[ 1024 ];	//	after the question mark ?
	TCHAR szFragment[ MAX_PATH ];	//	after the hashmark #	
	
} STPARSEURL, *PSTPARSEURL;

typedef enum tagWindowsSysType
{
	_OS_WINDOWS_32S,
	_OS_WINDOWS_NT3,
	_OS_WINDOWS_95,
	_OS_WINDOWS_98,
	_OS_WINDOWS_ME,
	_OS_WINDOWS_NT4,
	_OS_WINDOWS_2000,
	_OS_WINDOWS_XP,
	_OS_WINDOWS_2003

} ENUMWINDOWSSYSTYPE;


//	���ص�ǰ�Ƿ��ǵ��Ի���
BOOL _vwfunc_is_debug();

//	����һ�鹲���ڴ�
BOOL _vwfunc_malloc_share_memory( DWORD dwSize, LPCTSTR lpcszMemName, VOID ** lppvBuffer, HANDLE phFileMap, LPBOOL lpbCreate, LPSECURITY_ATTRIBUTES lpFileMappingAttributes = NULL );

//	��һ�鹲���ڴ�
BOOL _vwfunc_open_share_memory( LPCTSTR lpcszMemName, VOID ** lppvBuffer, HANDLE hFileMap );

//	get file map buffer
BOOL _vwfunc_get_file_mapbuffer( LPCTSTR lpszFilePath, LPTSTR lpszBuffer, DWORD dwSize );

//	Get File md5
BOOL _vwfunc_get_file_md5( LPCTSTR lpszFilePath, LPTSTR lpszMd5, DWORD dwSize, LPTSTR lpszError );

//	��ȡһ���ַ����� MD5 ֵ(32λ�ַ���)
BOOL _vwfunc_get_string_md5( LPCTSTR lpszString, LPTSTR lpszMd5, DWORD dwSize, DWORD dwSpecStringLen = 0 );


//	Get Windows system type
ENUMWINDOWSSYSTYPE _vwfunc_get_shellsystype();

//	is window nt4
BOOL _vwfunc_is_window_nt4();

//	��ȡ Windows temp Ŀ¼
BOOL _vwfunc_get_window_tempdir( LPTSTR lpszDir, DWORD dwSize );

//	�򵥼��ܽ���
VOID  _vwfunc_xorenc( CHAR * pData, BYTE xorkey = 0XFF );

//	����Դ������ȡ�ļ�������
BOOL  _vwfunc_extract_file_from_resource( HINSTANCE hInst, LPCTSTR lpcszResType, UINT uResID, LPCTSTR lpszDestFilename );
VOID *_vwfunc_get_resource_buffer( HINSTANCE hInst, LPCTSTR lpcszResType, UINT uResID, DWORD * pdwSize );


//	����ļ��Ƿ����
BOOL  _vwfunc_my_file_exists( LPCTSTR lpcszFile );

//	��ȡ�ļ���С
DWORD _vwfunc_get_my_filesize( LPCTSTR pszFilePath );

//	���ļ���Դ������汾
BOOL _vwfunc_get_file_version( LPCTSTR lpszFilePath, LPTSTR lpszVersion, DWORD dwSize );

//	�����ļ���дѡ��
BOOL _vwfunc_file_give_write_attribute( LPCTSTR lpcszFilePath );

//	��ȡ DLL ���ڵ�ȫ·��
BOOL _vwfunc_get_dll_modulepath( LPVOID lpvInnerAddr, HINSTANCE * hInstance, LPTSTR lpszModulePath, DWORD dwSize );


//////////////////////////////////////////////////////////////////////////


//	��ȡĳ���ַ���һ���ַ����еĸ���
DWORD _vwfunc_get_chr_count( LPCTSTR lpszString, TCHAR lpChr, UINT uStringLen = 0 );

//	����ַ����Ƿ��ǺϷ��� IP ��ַ
BOOL  _vwfunc_is_valid_ipaddr( LPCTSTR lpszString, UINT uStringLen = 0 );

//	����ַ����Ƿ��ǺϷ�������
BOOL  _vwfunc_is_valid_domain( LPCTSTR lpszString );

//	��������Ƿ��ǹ�������
static inline BOOL  _vwfunc_is_inland_domain( LPCTSTR lpcszHost, UINT uHostLen = 0 );

//	��ȡ Root ����
BOOL  _vwfunc_get_root_host( LPCTSTR lpcszHost, LPTSTR lpszRootHost, DWORD dwSize );
BOOL  _vwfunc_get_root_host2( LPCTSTR lpcszHost, LPTSTR lpszRootHost, DWORD dwSize );
LPCTSTR _vwfunc_get_root_host_ptr( LPCTSTR lpcszHost );
LPCTSTR _vwfunc_get_root_host_ptr_ex( LPCTSTR lpcszHost, UINT * uRetLen );

//	�� URL ����ȡ HOST
BOOL  _vwfunc_get_host_from_url( LPCTSTR lpcszUrl, LPTSTR lpszHost, DWORD dwSize );

//	��ȡ URL ������Ĳ�����ֵ
BOOL _vwfunc_get_request_query_string( LPCTSTR lpcszUrl, LPCTSTR lpcszName, LPTSTR lpszValue, DWORD dwSize );

//	��ȡָ�� Cookie Value
BOOL _vwfunc_get_spec_cookie_value( LPCTSTR lpszCookieString, LPCTSTR lpszCookieName, LPTSTR lpszValue, DWORD dwSize );

//	�滻���ɼ��Ŀ��Ʒ���Ϊ��_��
TCHAR * _vwfunc_replace_controlchars( LPSTR lpszStr, UINT uLen );

//	�����ַ���
inline BOOL _vwfunc_memcpy( LPCTSTR lpcszSrc, DWORD dwSrcLen, LPTSTR lpszDst, DWORD dwDstSize, BOOL bRepCtlChr = FALSE );

//	parse url
BOOL _vwfunc_parse_url( LPCTSTR lpcszUrl, DWORD dwUrlLen, STPARSEURL * pstParseUrl );

//	url encode / decode
INT inline _vwfunc_char_to_int( char c );
VOID _vwfunc_get_url_decode( char *str );
INT  _vwfunc_get_url_encode( LPCTSTR lpcszInput, INT nInputLen, LPTSTR lpszOutBuf, INT nOutBufLen );
BOOL _vwfunc_swap_string( CHAR * lpszString, INT nLen, INT nLeftStrLen );

//	�� URL �л�ȡ�ļ���Ϣ
BOOL _vwfunc_get_fileinfo_from_url( LPCTSTR lpcszFullUri, LPTSTR lpszExt, DWORD dwESize, LPTSTR lpszFile, DWORD dwFSize );

//	����һ�������Ľ���
BOOL _vwfunc_run_block_process( LPCTSTR lpszCmdLine );

//	��һ�� URL
BOOL _vwfunc_lauch_iebrowser( LPCTSTR lpcszUrl );



#endif // __VWFUNCBASE_HEADER__
