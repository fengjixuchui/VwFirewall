// VwFunc.cpp: implementation of the CVwFunc class.
//
//////////////////////////////////////////////////////////////////////

#include "StdAfx.h"
#include <windows.h>
#include <tchar.h>
#include <stdio.h>
#include <stdlib.h>
#include "VwFuncIni.h"

#include "shlwapi.h"
#pragma comment( lib, "shlwapi.lib" )




/**
*	@ ˽�к���
*	������Ҫ���µ��ļ��б�
*/
BOOL _vwfunc_ini_parse_section_line( LPCTSTR lpcszIniFile, LPCTSTR lpcszSection, vector<STINISECTIONLINE> & vcSection, BOOL bParseValue /*=FALSE*/ )
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
 *	���� Acp ��ȡ��ֵ
 */
BOOL _vwfunc_ini_get_value_with_acpkey( LPCTSTR lpszIniFile, LPCTSTR lpszDomain, LPCTSTR lpszKey, LPTSTR lpszValue, DWORD dwSize )
{
	//
	//	lpszIniFile	- [in]  INI �ļ�·��
	//	lpszDomain	- [in]  Domain
	//	lpszKey		- [in]  Key
	//	lpszValue	- [out] ����KEYֵ
	//	dwSize		- [in]  ����KEYֵ�Ļ�������С
	//	RETURN		- TRUE / FALSE
	//

	BOOL bRet	= FALSE;
	UINT uAcp	= GetACP();
	TCHAR szAcpKey[ MAX_PATH ] = {0};

	_sntprintf( szAcpKey, sizeof(szAcpKey)-sizeof(TCHAR), "%s_%d", lpszKey, uAcp );
	GetPrivateProfileString( lpszDomain, szAcpKey, "", lpszValue, dwSize, lpszIniFile );
	if ( _tcslen( lpszValue ) > 0 )
	{
		bRet = TRUE;
	}
	else
	{
		GetPrivateProfileString( lpszDomain, lpszKey, "", lpszValue, dwSize, lpszIniFile );
		if ( _tcslen( lpszValue ) > 0 )
		{
			bRet = TRUE;
		}
	}

	return bRet;
}

