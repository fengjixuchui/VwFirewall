// VwRandMd5.cpp: implementation of the VwRandMd5 class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "VwRandMd5.h"


//	RandNum Table
static const TCHAR g_ArrVwRandMd5RandNumTable[] =
{
	'0', '1', '2', '3', '4', '5', '6', '7', '8', '9', 'a', 'b', 'c', 'd', 'e', 'f'
};
static const UINT g_uArrVwRandMd5RandNumTableLength = sizeof( g_ArrVwRandMd5RandNumTable ) / sizeof( TCHAR );





//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CVwRandMd5::CVwRandMd5()
{
	memset( m_szSeed, 0, sizeof(m_szSeed) );

	//	���ܼ�������
	memcpy( m_szSeed, g_szVwRandMd5Seed, min( sizeof(g_szVwRandMd5Seed), sizeof(m_szSeed) ) );			
	_vwfunc_xorenc( m_szSeed );
}

CVwRandMd5::~CVwRandMd5()
{
}

/**
 *	��� VwRandMd5 �Ƿ���ȷ
 */
BOOL CVwRandMd5::IsMatch( LPCTSTR lpcszString, LPCTSTR lpcszRandMd5 )
{
	/*
		lpcszString	- [in] ����
		lpcszRandMd5	- [in] MD5 �������ַ���
		RETURN		- TRUE / FALSE
	*/
	if ( NULL == lpcszString || 0 == lpcszString[0] )
		return FALSE;
	if ( NULL == lpcszRandMd5 || 0 == lpcszRandMd5[0] )
		return FALSE;

	BOOL  bRet = FALSE;
	TCHAR szFst2Bit[ 4 ]		= {0};
	TCHAR szTempRandMd5[ 35 ];

	szFst2Bit[ 0 ]	= lpcszRandMd5[ 0 ];
	szFst2Bit[ 1 ]	= lpcszRandMd5[ 1 ];
	//_sntprintf( szFst2Bit, 2, "%s", lpcszRandMd5 );

	this->GetRandMd5( lpcszString, szFst2Bit, szTempRandMd5, sizeof(szTempRandMd5) );
	if ( 0 == _tcsicmp( lpcszRandMd5, szTempRandMd5 ) )
	{
		bRet = TRUE;
	}
	return bRet;
}

/**
 *	�������룬��ȡһ�� VwRandMd5 ֵ
 */
BOOL CVwRandMd5::GetRandMd5( LPCTSTR lpcszString, LPCTSTR lpcszRand, LPTSTR lpszRandMd5, DWORD dwSize )
{
	/*
		lpcszString	- [in]  ����ֵ
		lpcszRand	- [in]  ָ����λ����ַ��������λ NULL ���ɳ���������λ
		lpszRandMd5	- [out] ���� 34 λ�� MD5 ֵ��ǰ�����ֽ���������֣����� 32 ���Ǳ�׼ MD5 ֵ
		dwSize		- [in]  ���ش�С
		RETURN		- TRUE / FALSE
	*/
	if ( dwSize < 35 )
	{
		return FALSE;
	}

	TCHAR szRandNum[3];
	TCHAR szTmpString[ MAX_PATH ];
	TCHAR szOutput[ 33 ];

	if ( NULL == lpcszRand )
	{
		this->GetRandNum( szRandNum, sizeof(szRandNum) );
	}
	else
	{
		szRandNum[ 0 ]	= lpcszRand[ 0 ];
		szRandNum[ 1 ]	= lpcszRand[ 1 ];
		szRandNum[ 2 ]	= 0;
		//_sntprintf( szRandNum, 2, "%s", lpcszRand );
	}

	//	���Դ�����ַ���
	_sntprintf( szTmpString, sizeof(szTmpString)-sizeof(TCHAR), "%s%s%s", szRandNum, m_szSeed, lpcszString );

	//	���� MD5 ֵ
	if ( _vwfunc_get_string_md5( szTmpString, szOutput, sizeof(szOutput) ) )
	{
		//	2 λ��� + 32 λ��׼ MD5 ֵ
		_sntprintf( lpszRandMd5, dwSize-sizeof(TCHAR), "%02s%32s", szRandNum, szOutput );
		lpszRandMd5[ 34 ] = 0;

		return TRUE;
	}

	return FALSE;
}

/**
 *	��ȡ��λ������֣�00 ~ ff
 */
BOOL CVwRandMd5::GetRandNum( LPTSTR lpszRandNum, DWORD dwSize )
{
	/*
		lpszRandNum	- [out] ����
		dwSize		- [in]  ����ֵ��С
		RETURN		- TRUE / FALSE
	*/
	if ( NULL == lpszRandNum || dwSize < 2 )
	{
		return FALSE;
	}

	srand( (unsigned)time(NULL) );
	_sntprintf
	(
		lpszRandNum,
		dwSize-sizeof(TCHAR),
		"%01c%01c",
		g_ArrVwRandMd5RandNumTable[ (UINT)( rand() % g_uArrVwRandMd5RandNumTableLength ) ],
		g_ArrVwRandMd5RandNumTable[ (UINT)( rand() % g_uArrVwRandMd5RandNumTableLength ) ]
	);
	lpszRandNum[ 2 ] = 0;

	return TRUE;
}
