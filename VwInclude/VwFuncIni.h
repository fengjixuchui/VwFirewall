// VwFunc.h: interface for the CVwFunc class.
//
//////////////////////////////////////////////////////////////////////

#ifndef __VWFUNCINI_HEADER__
#define __VWFUNCINI_HEADER__

#include <stdio.h>
#include <stdlib.h>
#include <tchar.h>

#include <vector>
using namespace std;

//#include <winsvc.h>
//#pragma comment( lib, "Version.lib" )


#include "MultiString.h"


/**
 *	�ṹ�嶨��
 */
typedef struct tagIniSectionLine
{
	tagIniSectionLine()
	{
		memset( this, 0, sizeof(tagIniSectionLine) );
	}
	TCHAR szLine[ MAX_PATH ];
	TCHAR szName[ 64 ];
	TCHAR szValue[ MAX_PATH ];

}STINISECTIONLINE, *PSTINISECTIONLINE;



//	����ĳ�� INI �ļ��� section ����
BOOL _vwfunc_ini_parse_section_line( LPCTSTR lpcszIniFile, LPCTSTR lpcszSection, vector<STINISECTIONLINE> & vcSection, BOOL bParseValue = FALSE );

//	���� Acp ��ȡ��ֵ
BOOL _vwfunc_ini_get_value_with_acpkey( LPCTSTR lpszIniFile, LPCTSTR lpszDomain, LPCTSTR lpszKey, LPTSTR lpszValue, DWORD dwSize );



#endif // __VWFUNCINI_HEADER__
