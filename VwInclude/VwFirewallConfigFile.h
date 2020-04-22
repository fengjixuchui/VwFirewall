// CVwFirewallConfigFile.h: interface for the CVwFirewallConfigFile class.
//
//////////////////////////////////////////////////////////////////////

#ifndef __VWDMFIREWALLCONFIGFILE_HEADER__
#define __VWDMFIREWALLCONFIGFILE_HEADER__

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000


#include "VwConst.h"
#include "ModuleInfo.h"
#include "VwFirewallDrvIOCtl.h"
#include "VwConstBase.h"

#include <vector>
using namespace std;

#include "DeLib.h"
#pragma comment( lib, "DeLib.lib" )


/**
 *	���������ļ�
 */
#define CVWFIREWALLCONFIGFILE_INI_DOMAIN_MAIN			"Main"
#define CVWFIREWALLCONFIGFILE_INI_DOMAIN_LICENSE		"License"
#define CVWFIREWALLCONFIGFILE_INI_DOMAIN_DOMAIN			"Domain"
#define CVWFIREWALLCONFIGFILE_INI_DOMAIN_RDPCLIENTNAME		"RdpClientName"
#define CVWFIREWALLCONFIGFILE_INI_DOMAIN_PROTECTEDDIR		"ProtectedDir"
#define CVWFIREWALLCONFIGFILE_INI_DOMAIN_EXCEPTEDPROCESS	"ExceptedProcess"
#define CVWFIREWALLCONFIGFILE_INI_DOMAIN_SYSHOST		"SysHost"
#define CVWFIREWALLCONFIGFILE_INI_DOMAIN_APPINFO		"AppInfo"

#define CVWFIREWALLCONFIGFILE_INI_KEY_START			"start"
#define CVWFIREWALLCONFIGFILE_INI_KEY_STARTFILESAFE		"startfilesafe"
#define CVWFIREWALLCONFIGFILE_INI_KEY_STARTPRDP			"startprdp"
#define CVWFIREWALLCONFIGFILE_INI_KEY_STARTPDOMAIN		"startpdomain"
#define CVWFIREWALLCONFIGFILE_INI_KEY_DOMAIN			"domain"
#define CVWFIREWALLCONFIGFILE_INI_KEY_SYSHOST			"syshost"
#define CVWFIREWALLCONFIGFILE_INI_KEY_RDP_CLIENTNAME		"rdpclientname"
#define CVWFIREWALLCONFIGFILE_INI_KEY_PROTECTEDDIR		"protecteddir"
#define CVWFIREWALLCONFIGFILE_INI_KEY_EXCEPTEDPROCESS		"exceptedprocess"
#define CVWFIREWALLCONFIGFILE_INI_KEY_APP_INSDIR		"app_insdir"
#define CVWFIREWALLCONFIGFILE_INI_KEY_APP_LOGDIR		"app_logdir"

#define CVWFIREWALLCONFIGFILE_CFGVALNAME_USE			"use:"
#define CVWFIREWALLCONFIGFILE_CFGVALNAME_ITEM			"item:"
#define CVWFIREWALLCONFIGFILE_CFGVALNAME_DM			"dm:"
#define CVWFIREWALLCONFIGFILE_CFGVALNAME_WCD			"wcd:"		//	Wildcard

#define CVWFIREWALLCONFIGFILE_CFGVALNAME_DIR			"dir:"		//	for [ProtectedDir]
#define CVWFIREWALLCONFIGFILE_CFGVALNAME_TREE			"tree:"		//	for [ProtectedDir]
#define CVWFIREWALLCONFIGFILE_CFGVALNAME_ALLEXT			"allext:"	//	for [ProtectedDir]
#define CVWFIREWALLCONFIGFILE_CFGVALNAME_PROTEXT		"protext:"	//	for [ProtectedDir]
#define CVWFIREWALLCONFIGFILE_CFGVALNAME_ECPEXT			"ecpext:"	//	for [ProtectedDir]

#define CVWFIREWALLCONFIGFILE_INI_KEY_REGIP			"reg_ip"
#define CVWFIREWALLCONFIGFILE_INI_KEY_REGHOST			"reg_host"
#define CVWFIREWALLCONFIGFILE_INI_KEY_REGKEY			"reg_key"
#define CVWFIREWALLCONFIGFILE_INI_KEY_REGPRTYPE			"reg_prtype"
#define CVWFIREWALLCONFIGFILE_INI_KEY_REGPRTYPECS		"reg_prtypecs"
#define CVWFIREWALLCONFIGFILE_INI_KEY_REGEXPIRETYPE		"reg_expire_type"
#define CVWFIREWALLCONFIGFILE_INI_KEY_REGCREATEDATE		"reg_create_date"
#define CVWFIREWALLCONFIGFILE_INI_KEY_REGEXPIREDATE		"reg_expire_date"


/**
 *	struct
 */
typedef struct tagVwFirewallDomain
{
	BOOL	bUse;					//	�Ƿ�����
	CHAR    szDomain[ MAX_PATH ];			//	��Ҫ��������������磺.abc.com
	USHORT  uDomainLength;				//	��Ҫ����������ĳ���
	BOOL	bWildcard;				//	�Ƿ��Ƿ�����

}STVWFIREWALLDOMAIN, *LPVWFIREWALLDOMAIN;

typedef struct tagRdpClientName
{
	BOOLEAN bUse;
	TCHAR   szClientName[ 16 ];
	USHORT  uLength;

}STRDPCLIENTNAME, *LPSTRDPCLIENTNAME;

typedef struct tagProtectedDir
{
	BOOL   bUse;					//	�Ƿ�����
	TCHAR  szDir[ MAX_PATH ];			//	������Ŀ¼
	BOOL   bProtSubDir;				//	�Ƿ񱣻�����Ŀ¼	
	BOOL   bProtAllExt;				//	�Ƿ�Ҫ�������е���չ����TRUE �ǣ�FALSE ���ǣ��������� uszProtectedExt ��ָ����
	TCHAR  szExceptedExt[ 1024 ];			//	

}STPROTECTEDDIR, *LPSTPROTECTEDDIR;

typedef struct tagExceptedProcess
{
	BOOL   bUse;					//	�Ƿ�����
	TCHAR  szPath[ MAX_PATH ];			//	������̵�·��

}STEXCEPTEDPROCESS, *LPSTEXCEPTEDPROCESS;

typedef struct tagVwFirewallRegDate
{
	tagVwFirewallRegDate()
	{
		memset( this, 0, sizeof(tagVwFirewallRegDate) );
	}

	INT nRegYear;
	INT nRegMonth;
	INT nRegDay;

	INT nExpireYear;
	INT nExpireMonth;
	INT nExpireDay;

}STVWFIREWALLREGDATE, *LPSTVWFIREWALLREGDATE;

typedef struct tagVwFirewallConfig
{
	tagVwFirewallConfig()
	{
		memset( this, 0, sizeof(tagVwFirewallConfig) );
	}

	BOOL  bReged;				//	�Ƿ�ע��汾
	BOOL  bStart;				//	�Ƿ���������ǽ
	BOOL  bStartProtectFile;		//	����������
	BOOL  bStartProtectDomain;		//	��������������
	BOOL  bStartProtectRdp;			//	�������� RDP

	TCHAR szRegIp[ 64 ];			//	ע�� IP ��ַ
	TCHAR szRegHost[ MAX_PATH ];		//	ע�� HOST
	TCHAR szRegKey[ MAX_PATH ];		//	ע����
	TCHAR szRegPrType[ 32 ];		//	��Ʒ����
	TCHAR szRegPrTypeChecksum[ 64 ];	//	��Ʒ���͵�У��ֵ
	DWORD dwRegExpireType;			//	��������
	TCHAR szRegCreateDate[ MAX_PATH ];	//	ע��ʱ��
	TCHAR szRegExpireDate[ MAX_PATH ];	//	����ʱ��

	DWORD dwDomainCount;			//	��ͨ����ŵ����������ĸ���
	STVWFIREWALLDOMAIN * pstDomain;		//	ָ��һ��������������

} STVWFIREWALLCONFIG, *LPSTVWFIREWALLCONFIG;


//
//	qsort/bsearch compare function for the list of tagVwDmFirewallDomain
//
static int _qsortCmpFunc_tagVwDmFirewallDomain( const void * l, const void * r )
{
	return strlen( ((STVWFIREWALLDOMAIN*)l)->szDomain ) - strlen( ((STVWFIREWALLDOMAIN*)r)->szDomain );
}


/**
 *	class of CVwFirewallConfigFile
 */
class CVwFirewallConfigFile :
	public CModuleInfo
{
public:
	CVwFirewallConfigFile();
	virtual ~CVwFirewallConfigFile();

public:
	BOOL LoadConfig( STVWFIREWALLCONFIG * pstConfig );

	UINT LoadConfigUInt( LPCTSTR lpcszDomain, LPCTSTR lpcszKey, LPCTSTR lpcszFilename = NULL );
	BOOL LoadConfigString( LPCTSTR lpcszDomain, LPCTSTR lpcszKey, LPTSTR lpszString, DWORD dwSize, LPCTSTR lpcszFilename = NULL );
	BOOL SaveConfig( LPCTSTR lpcszDomain, LPCTSTR lpcszKey, LPCTSTR lpcszValue, LPCTSTR lpcszFilename = NULL );
	BOOL SaveConfigUInt( LPCTSTR lpcszDomain, LPCTSTR lpcszKey, UINT uValue, LPCTSTR lpcszFilename = NULL );

private:
	BOOL InitModule();

public:
	BOOL  m_bInitSucc;

	TCHAR m_szSysDriverDir[ MAX_PATH ];
	TCHAR m_szConfDir[ MAX_PATH ];
	TCHAR m_szLogsDir[ MAX_PATH ];
	TCHAR m_szScriptDir[ MAX_PATH ];
	TCHAR m_szBackupDir[ MAX_PATH ];
	TCHAR m_szBackupObjectDir[ MAX_PATH ];
	TCHAR m_szBackupServiceDir[ MAX_PATH ];

	TCHAR m_szCfgFile[ MAX_PATH ];
	TCHAR m_szKeyFile[ MAX_PATH ];
	TCHAR m_szSysDriverFile[ MAX_PATH ];
	TCHAR m_szSysDriverVersion[ MAX_PATH ];

	TCHAR m_szSys_Wshom_ocx[ MAX_PATH ];


	TCHAR m_szLocDriverFile_wnet_x86[ MAX_PATH ];
	TCHAR m_szLocDriverFile_wnet_amd64[ MAX_PATH ];

	TCHAR m_szISAPIFilterName[ 64 ];
	TCHAR m_szISAPIFilterDesc[ 64 ];
	TCHAR m_szISAPIFilterDllName[ 64 ];
	TCHAR m_szISAPIFilterFile[ MAX_PATH ];

	TCHAR m_szDllFileDeLib[ MAX_PATH ];

	TCHAR m_szCfgAclsFileData[ MAX_PATH ];
	TCHAR m_szCfgAclsFolderData[ MAX_PATH ];
	TCHAR m_szCfgAclsAntiVirusData[ MAX_PATH ];
	TCHAR m_szCfgServiceData[ MAX_PATH ];
	TCHAR m_szCfgObjectData[ MAX_PATH ];
};



#endif	//	__VWDMFIREWALLCONFIGFILE_HEADER__


