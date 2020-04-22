// DeCommonLicence.h: interface for the CDeCommonLicence class.
//
//////////////////////////////////////////////////////////////////////

#ifndef __DECOMMONLICENCE_HEADER__
#define __DECOMMONLICENCE_HEADER__

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000


#include "ModuleInfo.h"
#include "DeFileEncrypt.h"
#include "DeLicence.h"



/**
 *	���߻�ȡ������ķ��ؽ��(result=)
 */
#define MCH_GET_ACTCODE_SUCC		0
#define MCH_GET_ACTCODE_ERR_UNKNOWN	-1
#define MCH_GET_ACTCODE_ERR_NETWORK	-2
#define MCH_GET_ACTCODE_ERR_MD5STR	-1001
#define MCH_GET_ACTCODE_ERR_PARAM	-1002
#define MCH_GET_ACTCODE_ERR_QUERYDB	-1003
#define MCH_GET_ACTCODE_ERR_REGCODE	-1004




/**
 *	struct
 */
typedef struct tagDeCommonLicenceCfg
{
	tagDeCommonLicenceCfg()
	{
		memset( this, 0, sizeof(tagDeCommonLicenceCfg) );
	}

	BOOL  bCreateGeneWithDiskSN;	//	�Ƿ�ʹ��Ӳ�����к���Ϊ���� gene �Ĳ���
	TCHAR szSoftName[ MAX_PATH ];	//	��Ʒ���ƣ����磺"deremote"
	TCHAR szPrKey[ MAX_PATH ];	//	��Ʒע����Կ��
	TCHAR szHttpKey[ MAX_PATH ];	//	http ͨѶԿ��
	
	TCHAR szWorkDir[ MAX_PATH ];	//	����Ŀ¼

}STDECOMMONLICENCECFG, *LPSTDECOMMONLICENCECFG;




/**
 *	class of CDeCommonLicence
 */
class CDeCommonLicence :
	public CModuleInfo,
	public CDeLicence
{
public:
	CDeCommonLicence();
	virtual ~CDeCommonLicence();

	//	��ʼ��������Ϣ
	BOOL InitCfg( STDECOMMONLICENCECFG * pstCfg );

	//	����Ƿ�����ȷ��ע�����ʽ
	BOOL IsValidLicenseString( LPCTSTR lpcszRegCode );

	//	��֤�Ƿ�����ȷ��ע����
	BOOL IsValidLicence( LPCTSTR lpcszActCode, BOOL bReLoadConfig = FALSE );
	
	//	���߻�ȡ������(ActCode)
	LONG QueryActCodeFromServer( LPCTSTR lpcszRegCode, LPTSTR lpszActCode, DWORD dwSize );
	BOOL GetQueryActCodeMd5info( LPCTSTR lpcszStr, LPTSTR lpszMd5, DWORD dwSize );
	BOOL IsValidQueryActCodeData( LPCTSTR lpcszMd5, LPCTSTR lpcszStr );
	
	BOOL ReLoadLicenseInfo();
	BOOL SaveLicenseInfo( STDELICENSEINFO * pstDeLicenseInfo, BOOL bForceWrite = FALSE );


public:
	//
	//	������������汾�� PrKey
	//
	TCHAR m_szDecDeInfoMonitorSoftName[ 32 ];
	TCHAR m_szDecDeInfoMonitorPrKey_V1_0_0_1000[ 64 ];


private:
	BOOL GetKeyFilePath( LPCTSTR lpcszSoftName, LPTSTR lpszKeyFile, DWORD dwSize );

private:
	HINSTANCE m_hDllInstance;

	BOOL  m_bInitSucc;
	STDECOMMONLICENCECFG m_stCfg;

	//	����ע����Ϣ���ļ�·��
	TCHAR m_szKeyFile[ MAX_PATH ];
	CDeFileEncrypt m_cDeFileEnc;
	CRITICAL_SECTION m_oCriSecKeyFile;

	//	���� INI �е� Sec/key ����Ϣ
	TCHAR m_szDecIniSecLicense[ 32 ];
	TCHAR m_szDecIniKeyGene[ 32 ];
	TCHAR m_szDecIniKeyRegCode[ 32 ];
	TCHAR m_szDecIniKeyActCode[ 32 ];

	//	���ܺ��ͨѶ����
	TCHAR m_szDecDefaultHttpKey[ 32 ];

};




#endif // __DECOMMONLICENCE_HEADER__
