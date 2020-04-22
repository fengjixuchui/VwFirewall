// DeRemoteLicence.cpp: implementation of the CDeRemoteLicence class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "DeRemoteLicence.h"

#include "DeLib.h"
#pragma comment( lib, "DeLib.lib" )



//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CDeRemoteLicence::CDeRemoteLicence()
{
	m_hDllInstance	= NULL;
	m_bInitSucc	= FALSE;
	memset( m_szPrKey, 0, sizeof(m_szPrKey) );

	memset( m_szDecIniSecLicense, 0, sizeof(m_szDecIniSecLicense) );
	memset( m_szDecIniKeyGene, 0, sizeof(m_szDecIniKeyGene) );
	memset( m_szDecIniKeyRegCode, 0, sizeof(m_szDecIniKeyRegCode) );
	memset( m_szDecIniKeyActCode, 0, sizeof(m_szDecIniKeyActCode) );

	//
	//	���� ini �� sec/key ��Ϣ
	//
	memcpy( m_szDecIniSecLicense, g_szDeRemoteLicence_inisec_License, min( sizeof(g_szDeRemoteLicence_inisec_License), sizeof(m_szDecIniSecLicense) ) );
	delib_xorenc( m_szDecIniSecLicense );

	memcpy( m_szDecIniKeyGene, g_szDeRemoteLicence_inikey_Gene, min( sizeof(g_szDeRemoteLicence_inikey_Gene), sizeof(m_szDecIniKeyGene) ) );
	delib_xorenc( m_szDecIniKeyGene );

	memcpy( m_szDecIniKeyRegCode, g_szDeRemoteLicence_inikey_RegCode, min( sizeof(g_szDeRemoteLicence_inikey_RegCode), sizeof(m_szDecIniKeyRegCode) ) );
	delib_xorenc( m_szDecIniKeyRegCode );

	memcpy( m_szDecIniKeyActCode, g_szDeRemoteLicence_inikey_ActCode, min( sizeof(g_szDeRemoteLicence_inikey_ActCode), sizeof(m_szDecIniKeyActCode) ) );
	delib_xorenc( m_szDecIniKeyActCode );


	//	...
	InitializeCriticalSection( & m_oCriSecKeyFile );


	//	��ȡ�� DLL �� Instance �� DLL ��ȫ·��
	delib_get_modulefile_path( &__cderemotelicence_global_voidfunc, &m_hDllInstance, szDllFilePath, sizeof(szDllFilePath) );

	//	��������Ʒ����ɼ��� Key
	BuildKey();
	
	//	��ʼ�� Licence ��Ϣ
	CDeLicence::InitData( m_hDllInstance, m_szPrKey );

}
CDeRemoteLicence::~CDeRemoteLicence()
{
	DeleteCriticalSection( & m_oCriSecKeyFile );
}

//	��֤�Ƿ�����ȷ��ע����
BOOL CDeRemoteLicence::IsValidDeRemoteLicence( LPCTSTR lpcszActCode, BOOL bReLoadConfig /* = FALSE */ )
{
	if ( bReLoadConfig )
	{
		ReLoadLicenseInfo();
	}

	return CDeLicence::IsValidLicence( lpcszActCode );
}

/**
 *	װ�� License Info
 */
BOOL CDeRemoteLicence::ReLoadLicenseInfo()
{
	TCHAR szCkName[ 32 ]		= {0};
	TCHAR szFileBuffer[ 1024 ]	= {0};

	//	��������
	EnterCriticalSection( & m_oCriSecKeyFile );

	//	m_cDeFileEnc.EncryptFile( CModuleInfoDeRemote::m_szKeyFile, CModuleInfoDeRemote::m_szKeyDecFile );
	if ( m_cDeFileEnc.GetDecodedBufferFromFile( CModuleInfoDeRemote::m_szKeyFile, szFileBuffer, sizeof(szFileBuffer) ) )
	{
		//	���ﲻҪ��ȡ gene����Ϊ����Ǽ��������

		//	regcode
		_sntprintf( szCkName, sizeof(szCkName)-sizeof(TCHAR), _T("%s="), m_szDecIniKeyRegCode );
		delib_get_cookie_value( szFileBuffer, szCkName, CDeLicence::mb_stLcInfo.szRegCode, sizeof(CDeLicence::mb_stLcInfo.szRegCode) );

		//	actcode
		_sntprintf( szCkName, sizeof(szCkName)-sizeof(TCHAR), _T("%s="), m_szDecIniKeyActCode );
		delib_get_cookie_value( szFileBuffer, szCkName, CDeLicence::mb_stLcInfo.szActCode, sizeof(CDeLicence::mb_stLcInfo.szActCode) );
	}


	//
	//	��ȡ������Ϣ
	//
/*
	//mb_pfnGetPrivateProfileString( m_szDecIniSecLicense, m_szDecIniKeyGene, _T(""), CDeLicence::m_szGene, sizeof(CDeLicence::m_szGene), CModuleInfoDeRemote::m_szKeyDecFile );
	//_tcsupr( CDeLicence::m_szGene );

	mb_pfnGetPrivateProfileString( m_szDecIniSecLicense, m_szDecIniKeyRegCode, _T(""), CDeLicence::mb_stLcInfo.szRegCode, sizeof(CDeLicence::mb_stLcInfo.szRegCode), CModuleInfoDeRemote::m_szKeyDecFile );
	_tcsupr( CDeLicence::mb_stLcInfo.szRegCode );

	mb_pfnGetPrivateProfileString( m_szDecIniSecLicense, m_szDecIniKeyActCode, _T(""), CDeLicence::mb_stLcInfo.szActCode, sizeof(CDeLicence::mb_stLcInfo.szActCode), CModuleInfoDeRemote::m_szKeyDecFile );
	_tcsupr( CDeLicence::mb_stLcInfo.szActCode );
	
	//m_cDeFileEnc.EncryptFile( CModuleInfoDeRemote::m_szKeyDecFile, CModuleInfoDeRemote::m_szKeyFile );
	//_tunlink( CModuleInfoDeRemote::m_szKeyDecFile );
*/

	//	�뿪�����
	LeaveCriticalSection( & m_oCriSecKeyFile );

	return TRUE;
}

/**
 *	���� License Info
 */
BOOL CDeRemoteLicence::SaveLicenseInfo( STDELICENSEINFO * pstDeLicenseInfo, BOOL bForceWrite /* = FALSE */ )
{
	if ( NULL == pstDeLicenseInfo )
	{
		return FALSE;
	}

	TCHAR szBuffer[ 1024 ]	= {0};
	STDELICENSEINFO stNewLInfo;	//	Ҫ�������������ѡ������


	//	��װ��һ������
	ReLoadLicenseInfo();


	//	��������
	EnterCriticalSection( & m_oCriSecKeyFile );
	//m_cDeFileEnc.EncryptFile( CModuleInfoDeRemote::m_szKeyFile, CModuleInfoDeRemote::m_szKeyDecFile );

	//	Ĭ��ʹ�������ļ��е�����
	memset( & stNewLInfo, 0, sizeof(stNewLInfo) );
	stNewLInfo	= CDeLicence::mb_stLcInfo;

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


	_sntprintf
	(
		szBuffer,
		sizeof(szBuffer)-sizeof(TCHAR),
		_T("%s=%s;%s=%s;%s=%s;"),
		m_szDecIniKeyGene, stNewLInfo.szGene,
		m_szDecIniKeyRegCode, stNewLInfo.szRegCode,
		m_szDecIniKeyActCode, stNewLInfo.szActCode
	);

	m_cDeFileEnc.SaveEncryptedBufferToFile( szBuffer, CModuleInfoDeRemote::m_szKeyFile );

/*
	//
	//	д��������Ϣ
	//

	//	д�� gene
	if ( bForceWrite || _tcslen(pstDeLicenseInfo->szGene) )
	{
		_sntprintf( szTemp, sizeof(szTemp)-sizeof(TCHAR), _T("\"%s\""), pstDeLicenseInfo->szGene );
		mb_pfnWritePrivateProfileString( m_szDecIniSecLicense, m_szDecIniKeyGene, szTemp, CModuleInfoDeRemote::m_szKeyDecFile );
	}

	//	д�� regcode
	if ( bForceWrite || _tcslen(pstDeLicenseInfo->szRegCode) )
	{
		_sntprintf( szTemp, sizeof(szTemp)-sizeof(TCHAR), _T("\"%s\""), pstDeLicenseInfo->szRegCode );
		mb_pfnWritePrivateProfileString( m_szDecIniSecLicense, m_szDecIniKeyRegCode, szTemp, CModuleInfoDeRemote::m_szKeyDecFile );
	}

	//	д�� actcode
	if ( _tcslen(pstDeLicenseInfo->szActCode) )
	{
		_sntprintf( szTemp, sizeof(szTemp)-sizeof(TCHAR), _T("\"%s\""), pstDeLicenseInfo->szActCode );
		mb_pfnWritePrivateProfileString( m_szDecIniSecLicense, m_szDecIniKeyActCode, szTemp, CModuleInfoDeRemote::m_szKeyDecFile );
	}
*/

	//m_cDeFileEnc.EncryptFile( CModuleInfoDeRemote::m_szKeyDecFile, CModuleInfoDeRemote::m_szKeyFile );
	//_tunlink( CModuleInfoDeRemote::m_szKeyDecFile );
	
	//	�뿪�����	
	LeaveCriticalSection( & m_oCriSecKeyFile );

	return TRUE;
}









//////////////////////////////////////////////////////////////////////////
//	Private



BOOL CDeRemoteLicence::BuildKey()
{
	_sntprintf( m_szPrKey, sizeof(m_szPrKey)-sizeof(TCHAR), _T("%s"), _T("DeRemote") );
	memset( m_szPrKey, 0, sizeof(m_szPrKey) );

	memcpy( m_szPrKey, g_szDeRemoteLicence_PrKey, min( sizeof(g_szDeRemoteLicence_PrKey), sizeof(m_szPrKey) ) );
	delib_xorenc( m_szPrKey );

	return TRUE;
}

