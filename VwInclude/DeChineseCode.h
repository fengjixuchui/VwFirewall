// DeChineseCode.h: interface for the CDeChineseCode class.
//
//////////////////////////////////////////////////////////////////////

#ifndef __CHINESECODE_HEADER__
#define __CHINESECODE_HEADER__


#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000


#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <tchar.h>
#include <stdio.h>
#include <stdlib.h>

#include <string>
using namespace std;



/**
 *	class of CDeChineseCode
 */
class CDeChineseCode  
{
public:
	CDeChineseCode();
	virtual ~CDeChineseCode();

	BOOL UTF8ToUnicode( wchar_t * pOut, char * pText );		//	��UTF-8ת����Unicode
	BOOL UnicodeToUTF8( char * pOut, wchar_t * pText );		//	Unicode ת����UTF-8
	BOOL UnicodeToGB2312( char * pOut, wchar_t uData );		//	��Unicode ת���� GB2312 
	BOOL Gb2312ToUnicode( wchar_t * pOut, char * gbBuffer );	//	GB2312 ת���ɡ�Unicode
	BOOL GB2312ToUTF8( string & pOut, char * pText, int pLen );	//	GB2312 תΪ UTF-8

	//	UTF-8 תΪ GB2312
	BOOL UTF8ToGB2312( CHAR * pszOutBuf, CHAR * pszText, INT nInputLen );

	//	�ж��ַ����Ƿ��� UTF-8 �ַ���
	BOOL IsUtf8_byLiuJunMing( LPCTSTR lpcszString, UINT uLength );
	BOOL IsUTF8Text( LPCTSTR lpcszBuffer, LONG lnLength );


	BOOL IsUTF8( const char * pzInfo );
	BOOL IsGB2312( const char * pzInfo );
	int  IsGB( char *pText );
	bool IsChinese( const char *pzInfo );


/*
	inline BYTE toHex( const BYTE & x );
	inline BYTE toByte( const BYTE & x );
	CString URLDecode( CString sIn );
	CString URLEncode( CString sIn );
*/

};



#endif // __CHINESECODE_HEADER__
