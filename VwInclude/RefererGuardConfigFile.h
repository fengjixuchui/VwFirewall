// CRefererGuardConfigFile.h: interface for the CRefererGuardConfigFile class.
//
//////////////////////////////////////////////////////////////////////

#ifndef __REFERERGUARDCONFIGFILE_HEADER__
#define __REFERERGUARDCONFIGFILE_HEADER__

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000


#include "VwConst.h"
#include "ModuleInfo.h"
#include <vector>
using namespace std;

#include "TDoubleBinTree.h"

#include "DeLib.h"
#pragma comment( lib, "DeLib.lib" )


/**
 *	������/��չ�������Լ�����
 */
#define REFERERGUARD_FILTER_NAME		"RefererGuard"
#define REFERERGUARD_FILTER_DESC		"RefererGuard.Main.Filter"

#define REFGCLIENT_FILTER_NAME			"RefgClient"
#define REFGCLIENT_FILTER_DESC			"RefererGuard.RefgClient.Filter"

#define REFGCLIENT_URLPARAM_REFGCID		"_refgcid"
#define REFGCLIENT_URLPARAM_REFGCHECKSUM	"_refgchecksum"



/**
 *	���������ļ�
 */
#define CREFERERGUARDCONFIGFILE_FILE_FILTER_MAIN	"RefererGuard.dll"
#define CREFERERGUARDCONFIGFILE_FILE_FILTER_CLIENT	"RefgClient.dll"
#define CREFERERGUARDCONFIGFILE_FILE_DELIB		"DeLib.dll"
#define CREFERERGUARDCONFIGFILE_FILE_DELIBDRV		"DeLibDrv.dll"
#define CREFERERGUARDCONFIGFILE_FILE_DELIBFIINS		"DeLibFiIns.dll"

#define CREFERERGUARDCONFIGFILE_CONFFILE_CFG		"cfg.ini"
#define CREFERERGUARDCONFIGFILE_CONFFILE_TICK		"tick.ini"
#define CREFERERGUARDCONFIGFILE_CONFFILE_ECPHOST	"ecphost.ini"
#define CREFERERGUARDCONFIGFILE_CONFFILE_BLACKLIST	"blacklist.ini"


#define CREFERERGUARDCONFIGFILE_INI_DOMAIN_MAIN			"Main"
#define CREFERERGUARDCONFIGFILE_INI_DOMAIN_LICENSE		"License"
#define CREFERERGUARDCONFIGFILE_INI_DOMAIN_TICKINFO		"TickInfo"
#define CREFERERGUARDCONFIGFILE_INI_DOMAIN_PROTECTEDHOSTS	"ProtectedHosts"
#define CREFERERGUARDCONFIGFILE_INI_DOMAIN_EXCEPTEDHOSTS	"ExceptedHosts"


#define CREFERERGUARDCONFIGFILE_INI_KEY_START			"start"
#define CREFERERGUARDCONFIGFILE_INI_KEY_LOG			"log"
#define CREFERERGUARDCONFIGFILE_INI_KEY_USEFIREWALL		"usefirewall"
#define CREFERERGUARDCONFIGFILE_INI_KEY_ENABLEECPHOSTS		"enableecphosts"
#define CREFERERGUARDCONFIGFILE_INI_KEY_PUBKEY			"pubkey"

#define CREFERERGUARDCONFIGFILE_INI_KEY_DENY_EMPTY_REFERER		"deny_empty_referer"
#define CREFERERGUARDCONFIGFILE_INI_KEY_DENY_EMPTY_REFERER_IS_RDTO	"deny_empty_referer_is_rdto"
#define CREFERERGUARDCONFIGFILE_INI_KEY_DENY_EMPTY_REFERER_RDTOWAIT	"deny_empty_referer_rdtowait"
#define CREFERERGUARDCONFIGFILE_INI_KEY_DENY_EMPTY_REFERER_RDTOURL	"deny_empty_referer_rdtourl"

#define CREFERERGUARDCONFIGFILE_INI_KEY_DENY_ALL_OTHER_REFERER		"deny_all_other_referer"
#define CREFERERGUARDCONFIGFILE_INI_KEY_DENY_ALL_OTHER_REFERER_IS_RDTO	"deny_all_other_referer_is_rdto"
#define CREFERERGUARDCONFIGFILE_INI_KEY_DENY_ALL_OTHER_REFERER_RDTOWAIT	"deny_all_other_referer_rdtowait"
#define CREFERERGUARDCONFIGFILE_INI_KEY_DENY_ALL_OTHER_REFERER_RDTOURL	"deny_all_other_referer_rdtourl"

#define CREFERERGUARDCONFIGFILE_INI_KEY_DENY_BLACKLIST			"deny_blacklist"
#define CREFERERGUARDCONFIGFILE_INI_KEY_DENY_BLACKLIST_IS_RDTO		"deny_blacklist_is_rdto"
#define CREFERERGUARDCONFIGFILE_INI_KEY_DENY_BLACKLIST_RDTOWAIT		"deny_blacklist_rdtowait"
#define CREFERERGUARDCONFIGFILE_INI_KEY_DENY_BLACKLIST_RDTOURL		"deny_blacklist_rdtourl"


#define CREFERERGUARDCONFIGFILE_INI_KEY_REGIP				"RegIp"
#define CREFERERGUARDCONFIGFILE_INI_KEY_REGHOST				"RegHost"
#define CREFERERGUARDCONFIGFILE_INI_KEY_REGKEY				"RegKey"
#define CREFERERGUARDCONFIGFILE_INI_KEY_REGEXPIRETYPE			"RegExpireType"
#define CREFERERGUARDCONFIGFILE_INI_KEY_REGCREATEDATE			"RegCreateDate"
#define CREFERERGUARDCONFIGFILE_INI_KEY_REGEXPIREDATE			"RegExpireDate"
#define CREFERERGUARDCONFIGFILE_INI_KEY_REGPRTYPE			"RegPrType"
#define CREFERERGUARDCONFIGFILE_INI_KEY_REGPRTYPECHECKSUM		"RegPrTypeChecksum"

#define CREFERERGUARDCONFIGFILE_INI_KEY_TICKINFO_CTWT			"CTWT"		//	continue to work today
#define CREFERERGUARDCONFIGFILE_INI_KEY_TICKINFO_TICK			"TICK"		//	tickcount


/**
 *	action
 *	ע��: Ϊ�˽�Լ�ڴ棬���� action ��ֵ���������ܺͲ��ܳ��� 255
 */
#define REFERERGUARD_ACTION_NONE			0	//	���κβ���
#define REFERERGUARD_ACTION_START			1	//	Start flag
#define REFERERGUARD_ACTION_SET_MFSID_COOKIE		2	//	���� MFSID Cookie
#define REFERERGUARD_ACTION_DENY_EMPTY_REFERER		4	//	�ܾ����п���Դ������
#define REFERERGUARD_ACTION_DENY_UNKNOWN_REFERER	8	//	�ܾ�����������Դ������
#define REFERERGUARD_ACTION_DENY_BLACKLIST		16	//	�ܾ��������е���Դ������
#define REFERERGUARD_ACTION_ALL				( REFERERGUARD_ACTION_START )



/**
 *	struct for word
 */
typedef struct tagRefererGuardWord
{
	tagRefererGuardWord()
	{
		memset( this, 0, sizeof(tagRefererGuardWord) );
	}
	BOOL bWildcard;
	UINT uLen;
	CHAR szWord[ MAX_PATH ];
	
}STREFERERGUARDWORD, *LPSTREFERERGUARDWORD;

/**
*	struct for configuration
*/
typedef struct tagRefererGuardHost
{
	tagRefererGuardHost()
	{
		memset( this, 0, sizeof(tagRefererGuardHost) );
	}
	BOOL  bUse;
	BOOL  bWildcard;
	
	TCHAR szHost[ 128 ];
	UINT  uHostLen;

	TCHAR szExceptedPage[ 64 ];
	UINT  uEcpPageLen;

	tagRefererGuardHost * pLeft;	//	��ڵ�
	tagRefererGuardHost * pRight;	//	�ֽڵ�

}STREFERERGUARDHOST, *LPSTREFERERGUARDHOST;

typedef struct tagRefererGuardConfigTicketInfo
{
	BOOL bContinueWork;
	DWORD dwStartTicket;
	DWORD dwDay;
	__int64 _n64LimitedDataTransfer;	//	���˰汾���ƴ�С
	__int64 _n64DataTransfer;		//	9223372036854775807 �ֽ� = 8589934591 G�ֽ�

}STREFERERGUARDCONFIGTICKETINFO, *LPSTREFERERGUARDCONFIGTICKETINFO;

typedef struct tagRefererGuardConfig
{
	tagRefererGuardConfig()
	{
		memset( this, 0, sizeof(tagRefererGuardConfig) );
	}

	STREFERERGUARDCONFIGTICKETINFO stTickInfo;

	BOOL  bReged;					//	�Ƿ�ע��汾
	BOOL  bValidPrType;				//	�Ƿ���ȷ�� PRTYPE
	UINT  uMaxHostCount;				//	��� HOST ����
	UINT  uMaxBlacklistCount;			//	������������

	BOOL  bStart;					//	�Ƿ��������
	BOOL  bLog;					//	�Ƿ��¼��־
	BOOL  bUseFirewall;				//	��̨�����������ڷ���ǽ����
	BOOL  bEnableEcpHosts;				//	�Ƿ����������������

	BOOL  bDenyEmptyReferer;			//	�ܾ����п���Դ����
	BOOL  bDenyEmptyRefererIsRdTo;
	DWORD dwDenyEmptyRefererRdToWait;
	TCHAR szDenyEmptyRefererRdToUrl[ MAX_PATH ];	//	..................ת��� URL
	
	BOOL  bDenyAllOtherReferer;			//	�ܾ����в�����Դ������
	BOOL  bDenyAllOtherRefererIsRdTo;
	DWORD dwDenyAllOtherRefererRdToWait;
	TCHAR szDenyAllOtherRefererRdToUrl[ MAX_PATH ];	//	..................ת��� URL
	
	BOOL  bDenyBlackList;				//	�ܾ��������е���Դ������
	BOOL  bDenyBlackListIsRdTo;
	DWORD dwDenyBlackListRdToWait;
	TCHAR szDenyBlackListRdToUrl[ MAX_PATH ];	//	..................ת��� URL


	DWORD dwFilterFlags;			//	Filter ���ֵ� Flag ��Ҫ����


	TCHAR szPubKey[ 64 ];			//	public key

	TCHAR szRegIp[ 64 ];			//	ע�� IP ��ַ
	TCHAR szRegHost[ MAX_PATH ];		//	ע�� HOST
	TCHAR szRegKey[ MAX_PATH ];		//	ע����
	DWORD dwRegExpireType;			//	��������
	TCHAR szRegCreateDate[ MAX_PATH ];	//	ע��ʱ��
	TCHAR szRegExpireDate[ MAX_PATH ];	//	����ʱ��
	TCHAR szRegPrType[ 32 ];		//	��Ʒ���ͣ�<vwprtype>pslnormal</vwprtype>
	TCHAR szRegPrTypeChecksum[ 64 ];	//	��Ʒ����У��ֵ�� <vwprtypecs>a76e024d907205c9199184d63763475a</vwprtypecs>

	vector<STREFERERGUARDHOST> vcProtectedHosts;
	vector<STREFERERGUARDHOST>::iterator itPH;

	vector<STREFERERGUARDHOST> vcExceptedHosts;
	vector<STREFERERGUARDHOST>::iterator itEH;
	
#ifdef APPINSTANCE_REFERERGUARDCFG
	//	for RefererGuardCfg
	vector<STREFERERGUARDWORD> vcBlackListWithPattern;
	vector<STREFERERGUARDWORD> vcBlackList;
	UINT uBlackListWordMaxLen;
#endif

} STREFERERGUARDCONFIG, *LPSTREFERERGUARDCONFIG;

typedef struct tagRefererGuardConfigData
{
	tagRefererGuardConfigData()
	{
		dwBlackListWithPatternCount	= 0;
		pstBlackListWithPattern		= NULL;
	}

	CTDoubleBinTree<STREFERERGUARDHOST> btreeBlackList;
	DWORD dwBlackListWithPatternCount;
	STREFERERGUARDHOST * pstBlackListWithPattern;

}STREFERERGUARDCONFIGDATA, *LPSTREFERERGUARDCONFIGDATA;


//
//	qsort/bsearch compare function for the list of tagRefererGuardHost
//
static int _qsortCmpFunc_tagRefererGuardHost( const void * l, const void * r )
{
	return strlen( ((STREFERERGUARDHOST*)l)->szHost ) - strlen( ((STREFERERGUARDHOST*)r)->szHost );
}



/**
 *	class of CRefererGuardConfigFile
 */
class CRefererGuardConfigFile :
	public CModuleInfo
{
public:
	CRefererGuardConfigFile();
	virtual ~CRefererGuardConfigFile();

public:
	BOOL LoadConfig( STREFERERGUARDCONFIG * pstConfig, LPCTSTR lpcszFilename = NULL );
	UINT LoadConfigUInt( LPCTSTR lpcszDomain, LPCTSTR lpcszKey, LPCTSTR lpcszFilename = NULL );
	BOOL LoadConfigString( LPCTSTR lpcszDomain, LPCTSTR lpcszKey, LPTSTR lpszString, DWORD dwSize, LPCTSTR lpcszFilename = NULL );
	BOOL SaveConfig( STREFERERGUARDCONFIG * pstConfig, LPCTSTR lpcszFilename = NULL );
	BOOL SaveConfig( LPCTSTR lpcszDomain, LPCTSTR lpcszKey, LPCTSTR lpcszValue, LPCTSTR lpcszFilename = NULL );
	BOOL SaveConfig( LPCTSTR lpcszDomain, LPCTSTR lpcszKey, UINT uValue, LPCTSTR lpcszFilename = NULL );

	BOOL LoadWordsFromFile( LPCTSTR lpcszFilename, BOOL bWithPatternWords, vector<STREFERERGUARDWORD> & vcWordList, UINT * puWordMaxLen );
	BOOL DeleteAllWordsInFile( LPCTSTR lpcszFilename );
	BOOL SaveWordsToFile( LPCTSTR lpcszFilename, vector<STREFERERGUARDWORD> & vcWordList, UINT uWordMaxLen );

	//
	//	...
	//
	BOOL LoadProtectedHosts( vector<STREFERERGUARDHOST> & vcProtectedHosts, LPCTSTR lpcszFilename = NULL, DWORD dwMaxHostCount = 0, BOOL bLoadUsedOnly = FALSE );
	BOOL SaveProtectedHosts( vector<STREFERERGUARDHOST> & vcProtectedHosts, LPCTSTR lpcszFilename = NULL );
	
	BOOL LoadExceptedHosts( vector<STREFERERGUARDHOST> & vcExceptedHosts, LPCTSTR lpcszFilename = NULL );
	BOOL SaveExceptedHosts( vector<STREFERERGUARDHOST> & vcExceptedHosts, LPCTSTR lpcszFilename = NULL );

private:
	BOOL InitModule();

protected:
	BOOL  m_bInitSucc;

	//	...
	TCHAR m_szLogsDir[ MAX_PATH ];

	//	...
	TCHAR m_szDeLibDllFile[ MAX_PATH ];
	TCHAR m_szDeLibDrvDllFile[ MAX_PATH ];
	TCHAR m_szDeLibFiInsDllFile[ MAX_PATH ];

	TCHAR m_szFilterMainDllFile[ MAX_PATH ];
	TCHAR m_szFilterClientDllFile[ MAX_PATH ];
	TCHAR m_szFilterMainVersion[ MAX_PATH ];
	TCHAR m_szFilterClientVersion[ MAX_PATH ];

	//	...
	TCHAR m_szCfgFileNameOnly[ MAX_PATH ];
	TCHAR m_szCfgFile[ MAX_PATH ];
	TCHAR m_szTickFile[ MAX_PATH ];
	TCHAR m_szExceptionHostsFile[ MAX_PATH ];
	TCHAR m_szBlackListFile[ MAX_PATH ];

};



#endif	// __REFERERGUARDCONFIGFILE_HEADER__


