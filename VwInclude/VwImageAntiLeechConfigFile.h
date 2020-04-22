// CVwImageAntiLeechConfigFile.h: interface for the CVwImageAntiLeechConfigFile class.
//
//////////////////////////////////////////////////////////////////////

#ifndef __VWIMAGEANTILEECHCONFIGFILE_HEADER__
#define __VWIMAGEANTILEECHCONFIGFILE_HEADER__

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000


#include "VwConst.h"
#include "ModuleInfo.h"



#include "DeLib.h"
#pragma comment( lib, "DeLib.lib" )

#include "DeLibDrv.h"
#pragma comment( lib, "DeLibDrv.lib" )

#include <vector>
#include <set>
using namespace std;




/**
 *	������/��չ�������Լ�����
 */
#define VWINFOM_FILTER_NAME		"VwInfoM"
#define VWINFOM_FILTER_DESC		"VirtualWall.VwInfoMonitor"

#define VWINFOM_EXTENSION_NAME		"VwInfoMExt"
#define VWINFOM_EXTENSION_DESC		"VirtualWall.VwInfoMonitor.Extension"


/**
 *	���������ļ�
 */
#define CVWIMAGEANTILEECHCONFIGFILE_FILE_FILTER				"VwInfoMonitor.dll"
#define CVWIMAGEANTILEECHCONFIGFILE_FILE_DELIB				"DeLib.dll"

#define CVWIMAGEANTILEECHCONFIGFILE_CONFFILE_CFG			"cfg.ini"


#define CVWIMAGEANTILEECHCONFIGFILE_MAX_WORDLOADCOUNT		5000	//	��һ���ļ������װ�شʻ���
#define CVWIMAGEANTILEECHCONFIGFILE_MAX_SQLMPLOADCOUNT		500	//	���SQLƥ��������

#define CVWIMAGEANTILEECHCONFIGFILE_MIN_WORDLEN			4	//	�ʵ���С����
#define CVWIMAGEANTILEECHCONFIGFILE_MAX_WORDLEN			80	//	�ʵ���󳤶�
#define CVWIMAGEANTILEECHCONFIGFILE_MAX_SQLMPLEN		100	//	SQLƥ��������󳤶�

#define CVWIMAGEANTILEECHCONFIGFILE_INI_DOMAIN_MAIN		"Main"
#define CVWIMAGEANTILEECHCONFIGFILE_INI_DOMAIN_LICENSE		"License"
#define CVWIMAGEANTILEECHCONFIGFILE_INI_DOMAIN_TICKINFO		"TickInfo"
#define CVWIMAGEANTILEECHCONFIGFILE_INI_DOMAIN_HOST		"Host"
#define CVWIMAGEANTILEECHCONFIGFILE_INI_DOMAIN_SITE		"Site"
#define CVWIMAGEANTILEECHCONFIGFILE_INI_DOMAIN_ITEMLIST		"ItemList"

#define CVWIMAGEANTILEECHCONFIGFILE_INI_KEY_START		"start"
#define CVWIMAGEANTILEECHCONFIGFILE_INI_KEY_LOG			"log"

#define CVWIMAGEANTILEECHCONFIGFILE_INI_KEY_REGIP		"RegIp"
#define CVWIMAGEANTILEECHCONFIGFILE_INI_KEY_REGHOST		"RegHost"
#define CVWIMAGEANTILEECHCONFIGFILE_INI_KEY_REGKEY		"RegKey"
#define CVWIMAGEANTILEECHCONFIGFILE_INI_KEY_REGEXPIRETYPE	"RegExpireType"
#define CVWIMAGEANTILEECHCONFIGFILE_INI_KEY_REGCREATEDATE	"RegCreateDate"
#define CVWIMAGEANTILEECHCONFIGFILE_INI_KEY_REGEXPIREDATE	"RegExpireDate"

#define CVWIMAGEANTILEECHCONFIGFILE_INI_KEY_HOST		"Host"
#define CVWIMAGEANTILEECHCONFIGFILE_INI_KEY_SITE		"Site"
#define CVWIMAGEANTILEECHCONFIGFILE_INI_KEY_ITEMNAME		"ItemName"
#define CVWIMAGEANTILEECHCONFIGFILE_INI_KEY_ITEMURL		"ItemUrl"
#define CVWIMAGEANTILEECHCONFIGFILE_INI_KEY_ITEMID		"ItemID"

#define CVWIMAGEANTILEECHCONFIGFILE_INI_KEY_JPG			"Jpg"
#define CVWIMAGEANTILEECHCONFIGFILE_INI_KEY_JPEG		"Jpeg"
#define CVWIMAGEANTILEECHCONFIGFILE_INI_KEY_GIF			"Gif"
#define CVWIMAGEANTILEECHCONFIGFILE_INI_KEY_PNG			"Png"
#define CVWIMAGEANTILEECHCONFIGFILE_INI_KEY_BMP			"Bmp"


#define CVWIMAGEANTILEECHCONFIGFILE_INI_KEY_TICKINFO_CTWT	"CTWT"		//	continue to work today
#define CVWIMAGEANTILEECHCONFIGFILE_INI_KEY_TICKINFO_TICK	"TICK"		//	tickcount

#define CVWIMAGEANTILEECHCONFIGFILE_CFGVALNAME_USE		"use:"
#define CVWIMAGEANTILEECHCONFIGFILE_CFGVALNAME_HST		"hst:"
#define CVWIMAGEANTILEECHCONFIGFILE_CFGVALNAME_PRTEXT		"prtext:"
#define CVWIMAGEANTILEECHCONFIGFILE_CFGVALNAME_ITEMC		"itemc:"
#define CVWIMAGEANTILEECHCONFIGFILE_CFGVALNAME_ISREP		"isrep:"
#define CVWIMAGEANTILEECHCONFIGFILE_CFGVALNAME_REPFILE		"repfile:"


/**
 *	Protected File Types
 */
enum
{
	VWIMAGEANTILEECH_FILETYPE_COUNT	= 5,
	VWIMAGEANTILEECH_FILETYPE_NONE	= 0,
	VWIMAGEANTILEECH_FILETYPE_JPG	= 1,
	VWIMAGEANTILEECH_FILETYPE_JPEG	= 2,
	VWIMAGEANTILEECH_FILETYPE_GIF	= 4,
	VWIMAGEANTILEECH_FILETYPE_PNG	= 8,
	VWIMAGEANTILEECH_FILETYPE_BMP	= 16,
};


/**
 *	action
 *	ע��: Ϊ�˽�Լ�ڴ棬���� action ��ֵ���������ܺͲ��ܳ��� 255
 */
#define VWINFOM_ACTION_NONE			0	//	���κβ���
#define VWINFOM_ACTION_PROTECTED		1
#define VWINFOM_ACTION_OUT_REPLACE		1	//	����: �滻����
#define VWINFOM_ACTION_OUT_BLOCK		2	//	����: ��ֹ��ʾ(��ʱû��)
#define VWINFOM_ACTION_IN_REPLACE		4	//	����: �滻����(��ʱû��)
#define VWINFOM_ACTION_IN_BLOCK			8	//	����: ��ֹ��ʾ
#define VWINFOM_ACTION_IN_BLOCK_POST		16	//	�����ļ��ϴ���ֹ��ʾ
#define VWINFOM_ACTION_IN_ANTISQL		32	//	�����к���Σ�� SQL ���
#define VWINFOM_ACTION_ALERT_OUT_REPLACE	64	//	����������ʾ
#define VWINFOM_ACTION_ALL			( VWINFOM_ACTION_OUT_REPLACE | VWINFOM_ACTION_IN_BLOCK )


typedef struct tagVwImageAntiLeechReplace
{
	tagVwImageAntiLeechReplace()
	{
		memset( this, 0, sizeof(tagVwImageAntiLeechReplace) );
	}
	DWORD dwType;
	BOOL  bIsReplace;
	BOOL  bFileExist;
	TCHAR szReplaceFile[ MAX_PATH ];

}STVWIMAGEANTILEECHREPLACE, *LPSTVWIMAGEANTILEECHREPLACE;

typedef struct tagVwImageAntiLeechHost
{
	tagVwImageAntiLeechHost()
	{
		memset( this, 0, sizeof(tagVwImageAntiLeechHost) );
	}
	
	BOOL  bUse;
	BOOL  bWildcard;
	TCHAR szHost[ 128 ];
	DWORD dwPrtExt;

}STVWIMAGEANTILEECHHOST, *LPVWIMAGEANTILEECHHOST;

typedef struct tagVwImageAntiLeechHostList
{
	tagVwImageAntiLeechHostList()
	{
		memset( this, 0, sizeof(tagVwImageAntiLeechHostList) );
	}

	STVWIMAGEANTILEECHHOST stHost;
	STVWIMAGEANTILEECHREPLACE ArrReplaceFiles[ VWIMAGEANTILEECH_FILETYPE_COUNT ];

}STVWIMAGEANTILEECHHOSTLIST, *LPSTVWIMAGEANTILEECHHOSTLIST;

typedef struct tagVwImageAntiLeechItem
{
	tagVwImageAntiLeechItem()
	{
		memset( this, 0, sizeof(tagVwImageAntiLeechItem) );
	}

	TCHAR szItemID[ 32 ];
	TCHAR szItemUrl[ MAX_PATH ];
	TCHAR szItemName[ MAX_PATH ];

}STVWIMAGEANTILEECHITEM, *LPSTVWIMAGEANTILEECHITEM;

typedef struct tagVwImageAntiLeechItemID
{
	tagVwImageAntiLeechItemID()
	{
		memset( this, 0, sizeof(tagVwImageAntiLeechItemID) );
	}

	TCHAR szItemID[ 32 ];

}STVWIMAGEANTILEECHITEMID, *LPSTVWIMAGEANTILEECHITEMID;

struct __lessCmpFunc_tagVwImageAntiLeechItemID
{
	bool operator()( const STVWIMAGEANTILEECHITEMID & l, const STVWIMAGEANTILEECHITEMID & r ) const
	{
		return ( _tcsicmp( l.szItemID, r.szItemID ) < 0 ? TRUE : FALSE );
	}
};
typedef set<STVWIMAGEANTILEECHITEMID,__lessCmpFunc_tagVwImageAntiLeechItemID> _SET_ITEMID;
typedef set<STVWIMAGEANTILEECHITEMID,__lessCmpFunc_tagVwImageAntiLeechItemID>::iterator _SET_ITEMID_IT;

typedef struct tagVwImageAntiLeechSite
{
	tagVwImageAntiLeechSite()
	{
		Initalize();
	}
	VOID Initalize()
	{
		bUse		= FALSE;
		dwItemCount	= 0;
		memset( szHost, 0, sizeof(szHost) );
		setItemIDList.clear();
	}

	BOOL  bUse;
	TCHAR szHost[ MAX_PATH ];
	DWORD dwItemCount;

	set<STVWIMAGEANTILEECHITEMID,__lessCmpFunc_tagVwImageAntiLeechItemID> setItemIDList;
	set<STVWIMAGEANTILEECHITEMID,__lessCmpFunc_tagVwImageAntiLeechItemID>::iterator itItemIDList;

}STVWIMAGEANTILEECHSITE, *LPSTVWIMAGEANTILEECHSITE;


typedef struct tagVwImageAntiLeechConfigTicketInfo
{
	BOOL bContinueWork;
	DWORD dwStartTicket;
	DWORD dwDay;
	__int64 _n64LimitedDataTransfer;	//	���˰汾���ƴ�С
	__int64 _n64DataTransfer;		//	9223372036854775807 �ֽ� = 8589934591 G�ֽ�

}STVWIMAGEANTILEECHCONFIGTICKETINFO, *LPSTVWIMAGEANTILEECHCONFIGTICKETINFO;

typedef struct tagVwImageAntiLeechConfig
{
	tagVwImageAntiLeechConfig()
	{
		memset( this, 0, sizeof(tagVwImageAntiLeechConfig) );
	}

	STVWIMAGEANTILEECHCONFIGTICKETINFO stTickInfo;

	BOOL  bReged;						//	�Ƿ�ע��汾
	BOOL  bStart;						//	�Ƿ��������
	BOOL  bLog;						//	�Ƿ��¼��־

	DWORD dwFilterFlags;					//	Filter ���ֵ� Flag ��Ҫ����

	TCHAR szRegIp[ 64 ];					//	ע�� IP ��ַ
	TCHAR szRegHost[ MAX_PATH ];				//	ע�� HOST
	TCHAR szRegKey[ MAX_PATH ];				//	ע����
	DWORD dwRegExpireType;					//	��������
	TCHAR szRegCreateDate[ MAX_PATH ];			//	ע��ʱ��
	TCHAR szRegExpireDate[ MAX_PATH ];			//	����ʱ��



} STVWIMAGEANTILEECHCONFIG, *LPSTVWIMAGEANTILEECHCONFIG;

typedef struct tagVwImageAntiLeechConfigData
{
	tagVwImageAntiLeechConfigData()
	{
		Initialize();
	}
	~tagVwImageAntiLeechConfigData()
	{
		DeleteCriticalSection( & sm_oCrSecTree );
	}
	VOID Initialize()
	{
		InitializeCriticalSection( & sm_oCrSecTree );
		
		sm_uCurrTreeIndex = 0;
		
		vcProtectedHostList[ 0 ].clear();
		vcProtectedHostList[ 1 ].clear();
		pvcCurrentProtectedHostList = & vcProtectedHostList[ 0 ];
		pvcOfflineProtectedHostList = & vcProtectedHostList[ 1 ];
		
		vcExceptedSite[ 0 ].clear();
		vcExceptedSite[ 1 ].clear();
		pvcCurrentExceptedSite = & vcExceptedSite[ 0 ];
		pvcOfflineExceptedSite = & vcExceptedSite[ 1 ];
	}
	UINT GetOnlineTreeIndex()
	{
		return sm_uCurrTreeIndex;
	}
	UINT GetOfflineTreeIndex()
	{
		return ( ( sm_uCurrTreeIndex + 1 ) % 2 );
	}
	vector<STVWIMAGEANTILEECHHOSTLIST> * GetProtectedHostList( BOOL bOnline )
	{
		vector<STVWIMAGEANTILEECHHOSTLIST> * pvcRet;

		//	...
		EnterCriticalSection( & sm_oCrSecTree );

		pvcRet = bOnline ? pvcCurrentProtectedHostList : pvcOfflineProtectedHostList;

		//	...
		LeaveCriticalSection( & sm_oCrSecTree );

		return pvcRet;
	}
	vector<STVWIMAGEANTILEECHSITE> * GetExceptedSite( BOOL bOnline )
	{
		vector<STVWIMAGEANTILEECHSITE> * pvcRet;

		//	...
		EnterCriticalSection( & sm_oCrSecTree );
		
		pvcRet = bOnline ? pvcCurrentExceptedSite : pvcOfflineExceptedSite;
		
		//	...
		LeaveCriticalSection( & sm_oCrSecTree );

		return pvcRet;
	}
	VOID SwitchTrees()
	{
		//	...
		EnterCriticalSection( & sm_oCrSecTree );

		//
		//	��ȡ��ǰ��������ֵ
		//	����������ֵ -> ���� -> ��ǰ������������ֵ
		//
		sm_uCurrTreeIndex = GetOfflineTreeIndex();

		//
		//	�� ��������ָ�� ֱ��ָ�� ��ǰ��ָ��
		//
		pvcCurrentProtectedHostList = & vcProtectedHostList[ sm_uCurrTreeIndex ];
		pvcOfflineProtectedHostList = & vcProtectedHostList[ GetOfflineTreeIndex() ];

		pvcCurrentExceptedSite = & vcExceptedSite[ sm_uCurrTreeIndex ];
		pvcOfflineExceptedSite = & vcExceptedSite[ GetOfflineTreeIndex() ];

		//	...
		LeaveCriticalSection( & sm_oCrSecTree );
	}

	//	Index of tree
	UINT sm_uCurrTreeIndex;
	CRITICAL_SECTION sm_oCrSecTree;


	//	�ܱ����� HOST �б�
	vector<STVWIMAGEANTILEECHHOSTLIST> vcProtectedHostList[ 2 ];
	vector<STVWIMAGEANTILEECHHOSTLIST> * pvcCurrentProtectedHostList;
	vector<STVWIMAGEANTILEECHHOSTLIST> * pvcOfflineProtectedHostList;

	//	���������
	vector<STVWIMAGEANTILEECHSITE> vcExceptedSite[ 2 ];
	vector<STVWIMAGEANTILEECHSITE> * pvcCurrentExceptedSite;
	vector<STVWIMAGEANTILEECHSITE> * pvcOfflineExceptedSite;

}STVWIMAGEANTILEECHCONFIGDATA, *LPSTVWIMAGEANTILEECHCONFIGDATA;


typedef struct tagVwInfoMDictFile
{
	//	����
	TCHAR szOutRepWordsFile[ MAX_PATH ];
	TCHAR szOutRepWordsFile_utf8[ MAX_PATH ];
	TCHAR szOutBlockWordsFile[ MAX_PATH ];		//	�ݲ�ʹ��

	//	����
	TCHAR szInRepWordsFile[ MAX_PATH ];		//	�ݲ�ʹ��
	TCHAR szInBlockWordsFile[ MAX_PATH ];		//	������˴�
	TCHAR szInBlockWordsFile_escape[ MAX_PATH ];	//	������˴� escape �����汾
	TCHAR szInBlockWordsFile_utf8[ MAX_PATH ];	//	������˴� utf8 �汾
	TCHAR szInBlockSqlWordsFile[ MAX_PATH ];	//	�� SQL ע��

	//	��������
	TCHAR szExceptionHostsFile[ MAX_PATH ];

	TCHAR szInBlockAlertFile[ MAX_PATH ];		//	���뾯���ļ�
	TCHAR szInBlockSqlAlertFile[ MAX_PATH ];	//	��SQLע�뾯���ļ�


	//
	//	�ʵ��ļ�
	//

	//	����
	TCHAR szOutRepWordsDict[ MAX_PATH ];
	TCHAR szOutRepWordsDict_utf8[ MAX_PATH ];
	TCHAR szOutBlockWordsDict[ MAX_PATH ];	//	�ݲ�ʹ��
	
	//	����
	TCHAR szInRepWordsDict[ MAX_PATH ];		//	�ݲ�ʹ��
	TCHAR szInBlockWordsDict[ MAX_PATH ];		//	������˴�
	TCHAR szInBlockWordsDict_escape[ MAX_PATH ];	//	������˴� escape �����汾
	TCHAR szInBlockWordsDict_utf8[ MAX_PATH ];	//	������˴� utf-8 �汾

	//	��������
	TCHAR szExceptionHostsDict[ MAX_PATH ];

}STVWINFOMDICTFILE, *LPSTVWINFOMDICTFILE;



//
//	qsort/bsearch compare function for the list of tagVwImageAntiLeechHost
//
static int _qsortCmpFunc_tagVwImageAntiLeechHost( const void * l, const void * r )
{
	if ( ((STVWIMAGEANTILEECHHOST*)l)->bUse > ((STVWIMAGEANTILEECHHOST*)r)->bUse )
	{
		return 1;
	}
	else if ( ((STVWIMAGEANTILEECHHOST*)l)->bUse == ((STVWIMAGEANTILEECHHOST*)r)->bUse )
	{
		return _tcsicmp( ((STVWIMAGEANTILEECHHOST*)r)->szHost, ((STVWIMAGEANTILEECHHOST*)l)->szHost );
	}
	else
	{
		return -2;
	}
	//return strlen( ((STVWIMAGEANTILEECHHOST*)l)->szDomain ) - strlen( ((STVWIMAGEANTILEECHHOST*)r)->szDomain );
}




/**
 *	class of CVwImageAntiLeechConfigFile
 */
class CVwImageAntiLeechConfigFile :
	public CModuleInfo
{
public:
	CVwImageAntiLeechConfigFile();
	virtual ~CVwImageAntiLeechConfigFile();

public:
	BOOL LoadConfig( STVWIMAGEANTILEECHCONFIG * pstConfig );
	UINT LoadConfigUInt( LPCTSTR lpcszDomain, LPCTSTR lpcszKey, LPCTSTR lpcszCfgFile = NULL );
	BOOL LoadConfigString( LPCTSTR lpcszDomain, LPCTSTR lpcszKey, LPTSTR lpszString, DWORD dwSize, LPCTSTR lpcszCfgFile = NULL );
	BOOL SaveConfig( STVWIMAGEANTILEECHCONFIG * pstConfig );
	BOOL SaveConfig( LPCTSTR lpcszDomain, LPCTSTR lpcszKey, LPCTSTR lpcszValue );

	BOOL GetHostDetailIniDomainNameByHost( LPCTSTR lpcszHost, LPTSTR lpszDomainName, DWORD dwSize );
	BOOL GetReplaceInfoByHostAndFiletype( LPCTSTR lpcszHost, LPCTSTR lpcszFiletype, STVWIMAGEANTILEECHREPLACE * pstReplace );
	BOOL SaveReplaceInfoByHostAndFiletype( LPCTSTR lpcszHost, LPCTSTR lpcszFiletype, STVWIMAGEANTILEECHREPLACE * pstReplace );
	BOOL GetItemInfoFilenameByHost( LPCTSTR lpcszHost, LPTSTR lpszFilename, DWORD dwSize );
	BOOL ReadAllExceptedSitesFromConfig( vector<STVWIMAGEANTILEECHSITE> * pvcSiteList );

private:
	BOOL InitModule();

protected:
	BOOL  m_bInitSucc;

	TCHAR m_szConfDir[ MAX_PATH ];
	TCHAR m_szLogsDir[ MAX_PATH ];


	TCHAR m_szFilterDllFile[ MAX_PATH ];
	TCHAR m_szDeLibDllFile[ MAX_PATH ];
	TCHAR m_szFilterVersion[ MAX_PATH ];


	TCHAR m_szCfgFile[ MAX_PATH ];
	
	STVWINFOMDICTFILE m_stDictFile;
	
	//
	//	�����ļ�
	//

};



#endif	//	__VWIMAGEANTILEECHCONFIGFILE_HEADER__


