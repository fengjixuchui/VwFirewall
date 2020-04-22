// CVwPanDmNewConfigFile.h: interface for the CVwPanDmNewConfigFile class.
//
//////////////////////////////////////////////////////////////////////

#ifndef __VWINFOMONITORCONFIGFILE_HEADER__
#define __VWINFOMONITORCONFIGFILE_HEADER__

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000


#include "VwConst.h"
#include "ModuleInfo.h"
#include "VwPanDmNewDrvIOCtl.h"
#include "VwConstBase.h"

#include <vector>
using namespace std;

#include "DeLib.h"
#pragma comment( lib, "DeLib.lib" )


/**
 *	���������ļ�
 */
#define CVWPANDMNEWCONFIGFILE_INI_DOMAIN_MAIN		"Main"
#define CVWPANDMNEWCONFIGFILE_INI_DOMAIN_LICENSE	"License"
#define CVWPANDMNEWCONFIGFILE_INI_DOMAIN_DOMAIN		"Domain"
#define CVWPANDMNEWCONFIGFILE_INI_DOMAIN_SYSHOST	"SysHost"

#define CVWPANDMNEWCONFIGFILE_INI_KEY_START		"start"
#define CVWPANDMNEWCONFIGFILE_INI_KEY_DOMAIN		"domain"
#define CVWPANDMNEWCONFIGFILE_INI_KEY_SYSHOST		"syshost"

#define CVWPANDMNEWCONFIGFILE_CFGVALNAME_USE		"use:"
#define CVWPANDMNEWCONFIGFILE_CFGVALNAME_DM		"dm:"
#define CVWPANDMNEWCONFIGFILE_CFGVALNAME_ECPEXT		"ecpext:"
#define CVWPANDMNEWCONFIGFILE_CFGVALNAME_MAPSDD		"mapsdd:"

#define CVWPANDMNEWCONFIGFILE_INI_KEY_REGIP		"reg_ip"
#define CVWPANDMNEWCONFIGFILE_INI_KEY_REGHOST		"reg_host"
#define CVWPANDMNEWCONFIGFILE_INI_KEY_REGKEY		"reg_key"
#define CVWPANDMNEWCONFIGFILE_INI_KEY_REGPRTYPE		"reg_prtype"
#define CVWPANDMNEWCONFIGFILE_INI_KEY_REGPRTYPECS	"reg_prtypecs"
#define CVWPANDMNEWCONFIGFILE_INI_KEY_REGEXPIRETYPE	"reg_expire_type"
#define CVWPANDMNEWCONFIGFILE_INI_KEY_REGCREATEDATE	"reg_create_date"
#define CVWPANDMNEWCONFIGFILE_INI_KEY_REGEXPIREDATE	"reg_expire_date"


/**
 *	struct
 */
typedef struct tagVwPanDmNewDomain
{
	BOOL	bUse;					//	�Ƿ�����
	CHAR    szDomain[ MAX_PATH ];			//	��Ҫ��������������磺.abc.com
	USHORT  uDomainLength;				//	��Ҫ����������ĳ���
	CHAR    szExceptedSubDomain[ 32 ][ 32 ];	//	����������
	CHAR    szExceptedSubDomainString[ MAX_PATH ];	//	����������
	USHORT  uExceptedSubDomainCount;		//	�����������ĸ���
	BOOL	bMapSubDomainDir;			//	�Ƿ�ӳ��������Ŀ¼������ xing.abc.com -> abc.com/xing/

}STVWPANDMNEWDOMAIN, *LPSTVWPANDMNEWDOMAIN;

typedef struct tagVwPanDmNewRegDate
{
	tagVwPanDmNewRegDate()
	{
		memset( this, 0, sizeof(tagVwPanDmNewRegDate) );
	}

	INT nRegYear;
	INT nRegMonth;
	INT nRegDay;

	INT nExpireYear;
	INT nExpireMonth;
	INT nExpireDay;

}STVWPANDMNEWREGDATE, *LPSTVWPANDMNEWREGDATE;

typedef struct tagVwPanDmNewConfig
{
	tagVwPanDmNewConfig()
	{
		memset( this, 0, sizeof(tagVwPanDmNewConfig) );
	}

	BOOL  bReged;				//	�Ƿ�ע��汾
	BOOL  bStart;				//	�Ƿ��������

	TCHAR szRegIp[ 64 ];			//	ע�� IP ��ַ
	TCHAR szRegHost[ MAX_PATH ];		//	ע�� HOST
	TCHAR szRegKey[ MAX_PATH ];		//	ע����
	TCHAR szRegPrType[ 32 ];		//	��Ʒ����
	TCHAR szRegPrTypeChecksum[ 64 ];	//	��Ʒ���͵�У��ֵ
	DWORD dwRegExpireType;			//	��������
	TCHAR szRegCreateDate[ MAX_PATH ];	//	ע��ʱ��
	TCHAR szRegExpireDate[ MAX_PATH ];	//	����ʱ��

	DWORD dwDomainCount;			//	��ͨ����ŵ����������ĸ���
	STVWPANDMNEWDOMAIN * pstDomain;		//	ָ��һ��������������

} STVWPANDMNEWCONFIG, *LPSTVWPANDMNEWCONFIG;


//
//	qsort/bsearch compare function for the list of tagVwPanDmNewDomain
//
static int _qsortCmpFunc_tagVwPanDmNewDomain( const void * l, const void * r )
{
	return strlen( ((STVWPANDMNEWDOMAIN*)l)->szDomain ) - strlen( ((STVWPANDMNEWDOMAIN*)r)->szDomain );
}


/**
 *	class of CVwPanDmNewConfigFile
 */
class CVwPanDmNewConfigFile :
	public CModuleInfo
{
public:
	CVwPanDmNewConfigFile();
	virtual ~CVwPanDmNewConfigFile();

public:
	BOOL LoadConfig( STVWPANDMNEWCONFIG * pstConfig );
	UINT LoadConfigUInt( LPCTSTR lpcszDomain, LPCTSTR lpcszKey );
	BOOL LoadConfigString( LPCTSTR lpcszDomain, LPCTSTR lpcszKey, LPTSTR lpszString, DWORD dwSize );
	BOOL SaveConfig( LPCTSTR lpcszDomain, LPCTSTR lpcszKey, LPCTSTR lpcszValue );
	BOOL SaveConfigUInt( LPCTSTR lpcszDomain, LPCTSTR lpcszKey, UINT uValue );

private:
	BOOL InitModule();

protected:
	BOOL  m_bInitSucc;

	TCHAR m_szSysDriverDir[ MAX_PATH ];
	TCHAR m_szConfDir[ MAX_PATH ];
	TCHAR m_szLogsDir[ MAX_PATH ];

	TCHAR m_szCfgFile[ MAX_PATH ];
	TCHAR m_szKeyFile[ MAX_PATH ];
	TCHAR m_szSysDriverFile[ MAX_PATH ];
	TCHAR m_szSysDriverVersion[ MAX_PATH ];

	TCHAR m_szLocDriverFile_wnet_x86[ MAX_PATH ];
	TCHAR m_szLocDriverFile_wnet_amd64[ MAX_PATH ];

	TCHAR m_szISAPIFilterName[ 64 ];
	TCHAR m_szISAPIFilterDesc[ 64 ];
	TCHAR m_szISAPIFilterDllName[ 64 ];
	TCHAR m_szISAPIFilterFile[ MAX_PATH ];

	TCHAR m_szDllFileDeLib[ MAX_PATH ];
};



#endif	//	__VWINFOMONITORCONFIGFILE_HEADER__


