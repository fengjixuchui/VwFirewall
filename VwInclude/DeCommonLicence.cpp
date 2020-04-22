// DeCommonLicence.cpp: implementation of the CDeCommonLicence class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "DeCommonLicence.h"

#include "DeHttp.h"

#include "shlwapi.h"
#pragma comment( lib, "shlwapi.lib" )

#include "DeLib.h"
#pragma comment( lib, "DeLib.lib" )


//	DeRemote V1.0.0.1000 PrKey
//	PrKey = "DeRemoteKey_20080621"
static TCHAR g_szDeCommonLicence_DeRemoteKeyPrKey_V1_0_0_1000[] =
{
	-69, -102, -83, -102, -110, -112, -117, -102, -76, -102, -122, -96, -51, -49, -49, -57, -49, -55, -51, -50, 0
};

//	������� "DeInfoMonitor"
static TCHAR g_szDeCommonLicence_DeInfoMonitorSoftName[] =
{
	-69, -102, -74, -111, -103, -112, -78, -112, -111, -106, -117, -112, -115, 0
};

//	DeInfoMFilter V1.0.0.1000 PrKey
//	PrKey = "DeInfoMonitorKey_20080921"
static TCHAR g_szDeCommonLicence_DeInfoMonitorPrKey_V1_0_0_1000[] =
{
	-69, -102, -74, -111, -103, -112, -78, -112, -111, -106, -117, -112, -115, -76, -102, -122, -96, -51, -49, -49, -57, -49, -58, -51, -50, 0
};


//	"License"
static TCHAR g_szDeCommonLicence_inisec_License[]	= { -77, -106, -100, -102, -111, -116, -102, 0 };

//	"gene"
static TCHAR g_szDeCommonLicence_inikey_Gene[]		= { -104, -102, -111, -102, 0 };

//	"regcode"
static TCHAR g_szDeCommonLicence_inikey_RegCode[]	= { -115, -102, -104, -100, -112, -101, -102, 0 };

//	"actcode"
static TCHAR g_szDeCommonLicence_inikey_ActCode[]	= { -98, -100, -117, -100, -112, -101, -102, 0 };

//	"deremotequeryactcode"
static TCHAR g_szDeCommonLicence_DefaultHttpKey[]	= { -101, -102, -115, -102, -110, -112, -117, -102, -114, -118, -102, -115, -122, -98, -100, -117, -100, -112, -101, -102, 0 };






//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
CDeCommonLicence::CDeCommonLicence()
{
	m_hDllInstance	= NULL;
	m_bInitSucc	= FALSE;

	memset( & m_stCfg, 0, sizeof(m_stCfg) );

	memset( m_szDecIniSecLicense, 0, sizeof(m_szDecIniSecLicense) );
	memset( m_szDecIniKeyGene, 0, sizeof(m_szDecIniKeyGene) );
	memset( m_szDecIniKeyRegCode, 0, sizeof(m_szDecIniKeyRegCode) );
	memset( m_szDecIniKeyActCode, 0, sizeof(m_szDecIniKeyActCode) );

	memset( m_szDecDeInfoMonitorSoftName, 0, sizeof(m_szDecDeInfoMonitorSoftName) );
	memset( m_szDecDeInfoMonitorPrKey_V1_0_0_1000, 0, sizeof(m_szDecDeInfoMonitorPrKey_V1_0_0_1000) );


	//
	//	���� ini �� sec/key ��Ϣ
	//
	memcpy( m_szDecIniSecLicense, g_szDeCommonLicence_inisec_License, min( sizeof(g_szDeCommonLicence_inisec_License), sizeof(m_szDecIniSecLicense) ) );
	delib_xorenc( m_szDecIniSecLicense );
	
	memcpy( m_szDecIniKeyGene, g_szDeCommonLicence_inikey_Gene, min( sizeof(g_szDeCommonLicence_inikey_Gene), sizeof(m_szDecIniKeyGene) ) );
	delib_xorenc( m_szDecIniKeyGene );
	
	memcpy( m_szDecIniKeyRegCode, g_szDeCommonLicence_inikey_RegCode, min( sizeof(g_szDeCommonLicence_inikey_RegCode), sizeof(m_szDecIniKeyRegCode) ) );
	delib_xorenc( m_szDecIniKeyRegCode );
	
	memcpy( m_szDecIniKeyActCode, g_szDeCommonLicence_inikey_ActCode, min( sizeof(g_szDeCommonLicence_inikey_ActCode), sizeof(m_szDecIniKeyActCode) ) );
	delib_xorenc( m_szDecIniKeyActCode );
	
	//	Ĭ�ϵ� HTTP ͨѶԿ��
	memcpy( m_szDecDefaultHttpKey, g_szDeCommonLicence_DefaultHttpKey, min( sizeof(g_szDeCommonLicence_DefaultHttpKey), sizeof(m_szDecDefaultHttpKey) ) );
	delib_xorenc( m_szDecDefaultHttpKey );


	//
	//	���� ������� �Լ� ��������ĸ����汾�� PrKey
	//
	memcpy( m_szDecDeInfoMonitorSoftName, g_szDeCommonLicence_DeInfoMonitorSoftName, min( sizeof(g_szDeCommonLicence_DeInfoMonitorSoftName), sizeof(m_szDecDeInfoMonitorSoftName) ) );
	delib_xorenc( m_szDecDeInfoMonitorSoftName );

	memcpy( m_szDecDeInfoMonitorPrKey_V1_0_0_1000, g_szDeCommonLicence_DeInfoMonitorPrKey_V1_0_0_1000, min( sizeof(g_szDeCommonLicence_DeInfoMonitorPrKey_V1_0_0_1000), sizeof(m_szDecDeInfoMonitorPrKey_V1_0_0_1000) ) );
	delib_xorenc( m_szDecDeInfoMonitorPrKey_V1_0_0_1000 );
	



	//	...
	InitializeCriticalSection( & m_oCriSecKeyFile );
}
CDeCommonLicence::~CDeCommonLicence()
{
	DeleteCriticalSection( & m_oCriSecKeyFile );
}


/**
 *	@ public
 *	��ʼ��������Ϣ
 */
BOOL CDeCommonLicence::InitCfg( STDECOMMONLICENCECFG * pstCfg )
{
	if ( NULL == pstCfg )
	{
		return FALSE;
	}

	__try
	{
		m_stCfg = (*pstCfg);

		if ( 0 == _tcslen( m_stCfg.szHttpKey ) )
		{
			//	Ĭ�ϵ� http ͨѶԿ��
			_sntprintf( m_stCfg.szHttpKey, sizeof(m_stCfg.szHttpKey)-sizeof(TCHAR), _T("%s"), m_szDecDefaultHttpKey );
		}

		if ( _tcslen( m_stCfg.szSoftName ) && _tcslen( m_stCfg.szPrKey ) )
		{
			//	����ע����Ϣ���ļ�ȫ·��
			if ( GetKeyFilePath( m_stCfg.szSoftName, m_szKeyFile, sizeof(m_szKeyFile) ) )
			{
				//	��ʼ�� Licence ��Ϣ
				m_bInitSucc = CDeLicence::InitData( m_hDllInstance, m_stCfg.szPrKey, m_stCfg.bCreateGeneWithDiskSN );
			}
		}
	}
	__except( EXCEPTION_EXECUTE_HANDLER )
	{
	}

	//	..
	return m_bInitSucc;
}

/**
 *	@ public
 *	����Ƿ�����ȷ��ע�����ʽ
 */
BOOL CDeCommonLicence::IsValidLicenseString( LPCTSTR lpcszRegCode )
{
	if ( NULL == lpcszRegCode )
	{
		return FALSE;
	}
	
	UINT  i		= 0;
	UINT  uLen	= _tcslen( lpcszRegCode );
	
	if ( 0 == uLen )
	{
		return FALSE;
	}
	
	for ( i = 0; i < uLen; i ++ )
	{
		if ( NULL == _tcschr( _T("1234567890abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ-"), lpcszRegCode[i] ) )
		{
			return FALSE;
		}
	}
	return TRUE;
}

/**
 *	@ public
 *	��֤�Ƿ�����ȷ��ע����
 */
BOOL CDeCommonLicence::IsValidLicence( LPCTSTR lpcszActCode, BOOL bReLoadConfig /* = FALSE */ )
{
	//
	//	lpcszActCode	- [in] ָ���� ActCode
	//			       �β���Ҳ������ NULL����ʱ�������� ReLoadLicenseInfo() �������ļ��ж�ȡ ActCode
	//	bReLoadConfig	- [in/opt] ָ���Ƿ�������ļ��ж�ȡ
	//	RETURN		- TRUE / FALSE
	//

	if ( ! m_bInitSucc )
	{
		return FALSE;
	}

	if ( NULL == lpcszActCode || bReLoadConfig )
	{
		ReLoadLicenseInfo();
	}

	if ( lpcszActCode )
	{
		//
		//	����û����� ActCode ���ô���Ľ�����֤
		//
		return CDeLicence::IsValidLicence( lpcszActCode );
	}
	else
	{
		//
		//	������ ReLoadLicenseInfo() �������ļ���װ�ص�����֤
		//
		return CDeLicence::IsValidLicence( CDeLicence::mb_stLcInfo.szActCode );
	}
}


/**
 *	@ public
 *	���߼���
 */
LONG CDeCommonLicence::QueryActCodeFromServer( LPCTSTR lpcszRegCode, LPTSTR lpszActCode, DWORD dwSize )
{
	//
	//	lpcszRegCode	- [in] regcode
	//	lpszActCode	- [out] ���شӷ�������ȡ�� actcode
	//	dwSize		- [in] sizeof lpszActCode
	//	RETURN		- TRUE / FALSE
	//

	if ( ! m_bInitSucc )
	{
		return MCH_GET_ACTCODE_ERR_UNKNOWN;
	}

	LONG lnRet		= MCH_GET_ACTCODE_ERR_UNKNOWN;

	CDeHttp cHttp;
	TCHAR szUrl[ MAX_PATH ]	= {0};
	TCHAR szError[ MAX_PATH ]	= {0};

	TCHAR szSrcString[ MAX_PATH ]	= {0};
	TCHAR szMd5Str[ MAX_PATH ]	= {0};

	TCHAR szResponse[ 1024 ]	= {0};
	TCHAR szResult[ 32 ]		= {0};
	TCHAR szActCode[ 64 ]		= {0};
	TCHAR szResMd5Str[ MAX_PATH ]	= {0};
	TCHAR * lpszRespCnt		= NULL;


	//	���Ȼ�ȡͨѶ����
	if ( GetQueryActCodeMd5info( CDeLicence::mb_stLcInfo.szGene, szMd5Str, sizeof(szMd5Str) ) )
	{
		_sntprintf
		(
			szUrl,
			sizeof(szUrl)-sizeof(TCHAR),
			_T("http://rd.desafe.com/?id=2001&softname=%s&softgene=%s&regcode=%s&md5str=%s"),
			m_stCfg.szSoftName, CDeLicence::mb_stLcInfo.szGene, lpcszRegCode, szMd5Str
		);

		cHttp.GetResponse( szUrl, szResponse, sizeof(szResponse), szError, 60*1000 );
		if ( _tcslen(szResponse) )
		{
			lpszRespCnt = szResponse;
			if ( -17 == szResponse[0] && -69 == szResponse[1] && -65 == szResponse[2] )
			{
				//	ȥ�� UTF-8 �����ͷ�� 3 ���ֽ�(EF BB BF)
				lpszRespCnt = szResponse + sizeof(TCHAR) * 3;
			}
			if ( lpszRespCnt && _tcslen(lpszRespCnt) )
			{
				StrTrim( lpszRespCnt, _T("\r\n ") );

				GetCookieValue( lpszRespCnt, _T("result="), szResult, sizeof(szResult) );
				GetCookieValue( lpszRespCnt, _T("actcode="), szActCode, sizeof(szActCode) );
				GetCookieValue( lpszRespCnt, _T("md5str="), szResMd5Str, sizeof(szResMd5Str) );

				//	�����ת���� LONG
				lnRet = atol( szResult );

				//	�����ȡ�ɹ����򽫷��������ص� actcode ���������ػ�����
				if ( 0 == _tcsicmp( _T("0"), szResult ) && _tcslen(szActCode) )
				{
					//	����������㷨
					_sntprintf( szSrcString, sizeof(szSrcString)-sizeof(TCHAR), _T("%s%s"), szResult, szActCode );
					if ( IsValidQueryActCodeData( szResMd5Str, szSrcString ) )
					{
						//	...
						_sntprintf( lpszActCode, dwSize-sizeof(TCHAR), _T("%s"), szActCode );
					}
				}
			}
			else
			{
				lnRet = MCH_GET_ACTCODE_ERR_NETWORK;
			}
		}
		else
		{
			lnRet = MCH_GET_ACTCODE_ERR_NETWORK;
		}
	}

	return lnRet;
}

/**
 *	@ public
 *	get_actcode ͨѶ����
 *	�˺����� ActivateOnline ���������������ʱ�������ͨѶ����
 */
BOOL CDeCommonLicence::GetQueryActCodeMd5info( LPCTSTR lpcszStr, LPTSTR lpszMd5, DWORD dwSize )
{
	if ( ! m_bInitSucc )
	{
		return FALSE;
	}
	if ( NULL == lpcszStr || NULL == lpszMd5 || dwSize <= 32 )
	{
		return FALSE;
	}
	
	TCHAR szString[ MAX_PATH ]	= {0};
	TCHAR szSrc[ MAX_PATH ]		= {0};
	
	_sntprintf( szString, sizeof(szString)-sizeof(TCHAR), _T("%s"), lpcszStr );
	_tcsupr( szString );
	_sntprintf( szSrc, sizeof(szSrc)-sizeof(TCHAR), _T("%s%s"), szString, m_stCfg.szHttpKey );
	
	GetStringMd5( szSrc, lpszMd5, dwSize );
	_tcsupr( lpszMd5 );
	
	return TRUE;
}

/**
 *	@ public
 *	����ں��� ActivateOnline �пͻ��˺ͷ�������ͨѶ�������Ƿ���ȷ
 */
BOOL CDeCommonLicence::IsValidQueryActCodeData( LPCTSTR lpcszMd5, LPCTSTR lpcszStr )
{
	//
	//	$gmd5str	- [in] У�� MD5 ֵ
	//	$str		- [in] ԭʼ����У��˵��ַ���������������������
	//	RETURN		- true / false
	//

	if ( ! m_bInitSucc )
	{
		return FALSE;
	}

	BOOL  bRet		= FALSE;
	TCHAR szMd5Str[ 64 ]	= {0};
	
	if ( GetQueryActCodeMd5info( lpcszStr, szMd5Str, sizeof(szMd5Str) ) )
	{
		if ( 0 == _tcsicmp( lpcszMd5, szMd5Str ) )
		{
			bRet = TRUE;
		}
	}
	
	return bRet;
}

/**
 *	@ public
 *	װ�� License Info
 */
BOOL CDeCommonLicence::ReLoadLicenseInfo()
{
	if ( ! m_bInitSucc )
	{
		return FALSE;
	}

	TCHAR szCkName[ 32 ]		= {0};
	TCHAR szFileBuffer[ 1024 ]	= {0};

	//
	//	��������
	//
	EnterCriticalSection( & m_oCriSecKeyFile );


	if ( m_cDeFileEnc.GetDecodedBufferFromFile( m_szKeyFile, szFileBuffer, sizeof(szFileBuffer) ) )
	{
		//	���ﲻҪ��ȡ gene����Ϊ����Ǽ��������

		//	regcode
		_sntprintf( szCkName, sizeof(szCkName)-sizeof(TCHAR), _T("%s="), m_szDecIniKeyRegCode );
		GetCookieValue( szFileBuffer, szCkName, CDeLicence::mb_stLcInfo.szRegCode, sizeof(CDeLicence::mb_stLcInfo.szRegCode) );

		//	actcode
		_sntprintf( szCkName, sizeof(szCkName)-sizeof(TCHAR), _T("%s="), m_szDecIniKeyActCode );
		GetCookieValue( szFileBuffer, szCkName, CDeLicence::mb_stLcInfo.szActCode, sizeof(CDeLicence::mb_stLcInfo.szActCode) );
	}


	//
	//	�뿪�����
	//
	LeaveCriticalSection( & m_oCriSecKeyFile );

	return TRUE;
}

/**
 *	@ public
 *	���� License Info
 */
BOOL CDeCommonLicence::SaveLicenseInfo( STDELICENSEINFO * pstDeLicenseInfo, BOOL bForceWrite /* = FALSE */ )
{
	if ( ! m_bInitSucc )
	{
		return FALSE;
	}
	if ( NULL == pstDeLicenseInfo )
	{
		return FALSE;
	}

	TCHAR szBuffer[ 1024 ]	= {0};
	STDELICENSEINFO stNewLInfo;	//	Ҫ�������������ѡ������


	//	��װ��һ������
	ReLoadLicenseInfo();

	//	Ĭ��ʹ�������ļ��е�����
	memset( & stNewLInfo, 0, sizeof(stNewLInfo) );
	stNewLInfo	= CDeLicence::mb_stLcInfo;


	//
	//	��������
	//
	EnterCriticalSection( & m_oCriSecKeyFile );


	//	gene
	if ( bForceWrite || _tcslen(pstDeLicenseInfo->szGene) )
	{
		//	ʹ�õ����߸�����
		_sntprintf( stNewLInfo.szGene, sizeof(stNewLInfo.szGene)-sizeof(TCHAR), _T("%s"), pstDeLicenseInfo->szGene );
	}

	//	regcode
	if ( bForceWrite || _tcslen(pstDeLicenseInfo->szRegCode) )
	{
		//	ʹ�õ����߸�����
		_sntprintf( stNewLInfo.szRegCode, sizeof(stNewLInfo.szRegCode)-sizeof(TCHAR), _T("%s"), pstDeLicenseInfo->szRegCode );
	}

	//	actcode
	if ( _tcslen(pstDeLicenseInfo->szActCode) )
	{
		//	ʹ�õ����߸�����
		_sntprintf( stNewLInfo.szActCode, sizeof(stNewLInfo.szActCode)-sizeof(TCHAR), _T("%s"), pstDeLicenseInfo->szActCode );
	}


	//	������ݸ�ʽ��
	_sntprintf
	(
		szBuffer,
		sizeof(szBuffer)-sizeof(TCHAR),
		_T("%s=%s;%s=%s;%s=%s;"),
		m_szDecIniKeyGene, stNewLInfo.szGene,
		m_szDecIniKeyRegCode, stNewLInfo.szRegCode,
		m_szDecIniKeyActCode, stNewLInfo.szActCode
	);

	m_cDeFileEnc.SaveEncryptedBufferToFile( szBuffer, m_szKeyFile );


	//
	//	�뿪�����
	//
	LeaveCriticalSection( & m_oCriSecKeyFile );

	return TRUE;
}




//////////////////////////////////////////////////////////////////////////
//	Private




/**
 *	@ Private
 *	��ȡ����ע����Ϣ���ļ�(*.key)��ȫ·��
 */
BOOL CDeCommonLicence::GetKeyFilePath( LPCTSTR lpcszSoftName, LPTSTR lpszKeyFile, DWORD dwSize )
{
	if ( NULL == lpcszSoftName || 0 == _tcslen(lpcszSoftName) )
	{
		return FALSE;
	}
	if ( NULL == lpszKeyFile || 0 == dwSize )
	{
		return FALSE;
	}

	_sntprintf( lpszKeyFile, dwSize-sizeof(TCHAR), _T("%s\\%s.key"), m_stCfg.szWorkDir, lpcszSoftName );

	return TRUE;
}