#ifndef __VWSTRUCT_HEADER__
#define __VWSTRUCT_HEADER__


#include <set>
using namespace std;

#include "VwStructBase.h"
#include "VwStructCache.h"
#include "ErrorLog.h"
#include "ProcCfg.h"
#include "VwSpiderMatchMemV.h"
#include <windows.h>
#include <stdio.h>

//#include "VwSpiderMatch.h"

/**
 *	Http Header Info ��Ϣ�洢
 */
typedef struct tagHttpHeaderInfo
{
	tagHttpHeaderInfo()
	{
		lnCfgIndex		= (-1);
		szAgent[ 0 ]		= 0;
		dwAgentLength		= 0;
		szContentType[ 0 ]	= 0;
		szContentLength[ 0 ]	= 0;
		szRange[ 0 ]		= 0;
		szRemoteAddr[ 0 ]	= 0;
		szHost[ 0 ]		= 0;
		szMethod[ 0 ]		= 0;
//		szServerPort[ 0 ]	= 0;
		szReferer[ 0 ]		= 0;
		dwRefererLength		= 0;
//		szRefererHost[ 0 ]	= 0;
		szCookie[ 0 ]		= 0;
		szFullUri[ 0 ]		= 0;
		dwFullUriLength		= 0;
		szUrlDir[ 0 ]		= 0;
		szUrlExt[ 0 ]		= 0;
		dwUrlExtLength		= 0;
		szUrlFile[ 0 ]		= 0;
	}
	LONG  lnCfgIndex;		//	STHOSTINFO �е�һ����Ա���������� SET CACHE
	TCHAR szTemp[ 128 ];		//	������ʱ��ȡ�ַ�����Ϣ�Ļ���
	TCHAR szAgent[ MAX_PATH ];	//	UserAgent : 2009-01-13 ���ȴ� 128 �� MAX_PATH
	DWORD dwAgentLength;		//	UserAgent ����
	TCHAR szContentType[ 32 ];	//	����Ҫ 32 ���ַ�����Ϊ��Content-Type: application/octet-stream �� 24 ���ֽ�
	TCHAR szContentLength[ 32 ];
	DWORD dwContentLength;
	TCHAR szRange[ 64 ];		//	2009-5-14 ���ȴ� 16 ������ 64
	DWORD dwLocalAddrValue;
//	TCHAR szLocalAddr[ 16 ];
//	DWORD dwRemoteAddrLength;
	DWORD dwRemoteAddrValue;
	TCHAR szRemoteAddr[ 16 ];
	TCHAR szHost[ 128 ];
	TCHAR szMethod[ 16 ];			//	REQUEST_METHOD
//	TCHAR szServerPort[ 6 ];		//	SERVER_PORT
	ULONG uServerPort;			//	SERVER_PORT
	TCHAR szReferer[ 520 ];			//	2009-05-25 �մ� MAX_PATH ������ 520
	DWORD dwRefererLength;			//	szReferer �ĳ���
	//TCHAR szRefererHost[ 128 ];		//
	TCHAR szCookie[ 8192 ];			//	2009-3-15 V3.1.5 �� 1024 ������ 2048
						//	2010-6-08 V3.2.2.1061 from 2048 -> 4096
						//	2010-7-11 V3.2.2.1062 from 4096 -> 8192
	TCHAR szVwRandMd5[ 128 ];		//	Vw-Vsid �ڵ�����
	TCHAR szFullUri[ 512 ];			//	URI : 2009-01-28 ���ȴ� MAX_PATH ������ 512
	TCHAR szFullUriLCase[ 512 ];		//	URI Сд�� : 2009-01-28 ���ȴ� MAX_PATH ������ 512
	DWORD dwFullUriLength;			//	the length of szFullUri
	TCHAR szUrlDir[ MAX_PATH ];		//	url dir : 2009-01-11 ���ȴ� 128 ������ MAX_PATH
	DWORD dwUrlDirLength;			//	the length of url dir
	TCHAR szUrlExt[ 16 ];			//	file ext
	DWORD dwUrlExtLength;			//	the length of szUrlExt
	TCHAR szUrlFile[ 64 ];			//	url file : 2009-01-11 ���ȴ� 32 ������ 64
						//	### ������Ȳ�������޸ģ���Ϊ SDK �ļ������������ƶ��ض�
						//	### ���Ҫ�޸ĳ��ȣ�����ͬʱ�޸� SDK �ļ��е�����

}STHTTPHEADERINFO, *PSTHTTPHEADERINFO;









typedef struct tagLmtCalc
{
	tagLmtCalc()
	{
		lnLmtSleepTime		= 0;
	//	lnLmtSpeedTimeSpent	= 0;
	//	lnLmtSpeedLastSendTime	= 0;
	//	lnLmtSpeedSendCount	= 0;

		dwTickStart		= 0;
		dwUsedTime		= 0;
		dwTotalSendData		= 0;
	}

	LONG	lnLmtSleepTime;			//	Sleep ��ʱ�䣬��Ҫ������������ֵ�������
	
	//LONG	lnLmtSpeedTimeSpent;		//	��ǰ�������������� 8192*50 ���ֽ�����Ҫ��ʱ�䣬��λ������
	//LONG	lnLmtSpeedLastSendTime;		//	�ϴη�����ʱ��
	//LONG	lnLmtSpeedSendCount;		//	��������

	DWORD	dwTickStart;
	DWORD	dwUsedTime;
	DWORD	dwTotalSendData;

}STLMTCALC, *PSTLMTCALC;



enum
{
	NOTIFY_ACTION_PREPROC_HEADERS_START	= 0,
	NOTIFY_ACTION_PREPROC_HEADERS_END,

	NOTIFY_ACTION_URL_MAP_START,
	NOTIFY_ACTION_URL_MAP_END,

	NOTIFY_ACTION_SEND_RESPONSE_START,
	NOTIFY_ACTION_SEND_RESPONSE_END,

	NOTIFY_ACTION_SEND_RAW_DATA_START,
	NOTIFY_ACTION_SEND_RAW_DATA_END,

	NOTIFY_ACTION_LOG_START,
	NOTIFY_ACTION_LOG_END,

	NOTIFY_ACTION_END_OF_REQUEST_START,
	NOTIFY_ACTION_END_OF_REQUEST_END
};
#define NOTIFY_ACTION_COUNT	12




typedef struct tagSTVWNOTIFYACTIONTIME
{
	VOID UpdateTime( DWORD dwAction )
	{
		if ( IsValidAction( dwAction ) )
		{
			dwTick = GetTickCount();
			dwArrTick[ dwAction ] = dwTick;
			GetLocalTime( &stTime[ dwAction ] );
		}
	}
	BOOL IsValidAction( DWORD dwAction )
	{
		return ( dwAction >=0 && dwAction < NOTIFY_ACTION_COUNT );
	}	

	DWORD dwTick;
	DWORD dwArrTick[ NOTIFY_ACTION_COUNT ];
	SYSTEMTIME stTime[ NOTIFY_ACTION_COUNT ];

}STVWNOTIFYACTIONTIME, *LPSTVWNOTIFYACTIONTIME;

typedef struct tagMyHttpFilterContext
{
	//
	//	����ṹ�岻�����ⲿ�� memset ֮��ĳ�ʼ������
	//
	tagMyHttpFilterContext()
	{
		lnHaspPoolIdx	= -1;
		dwRequestFlag	= REQUEST_VW_OKPASS;
	}

	DWORD   lnStartTickCount;
//	LONG	lnTickCount;		//	Current TickCount
	LONG	lnPoolNodeFlag;		//	MemPool Index
	LONG	lnCyclePoolIdx;		//	ѭ�����е� IDX ֵ
//	LONG	lnCyclePoolFlag;	//	�Ƿ�����Ϊѭ����������ʱ new �����ģ�����ǵĻ�������Ҫ free

	LONG	lnHaspPoolIdx;		//	�߳����Ƴص����� HaspPool Index
	LONG	lnCfgIdx;		//	��վ��������Ϣ�� Config index
	DWORD	dwRequestFlag;		//	������
	DWORD	dwNotificationType;	//	Notification ����
	DWORD   dwAction;
	STVWNOTIFYACTIONTIME stActionTime;

	DWORD	dwProcessId;
	TCHAR	szAppPoolName[ 128 ];
	DWORD	dwAppPoolNameCrc32;

	BOOL	bExceptResource;	//	�Ƿ����������Դ
	BOOL	bSentLastReport;

	//STCFGDATA stCfgData;
	STCFGDATA * pstCfgData;
	STLMTCALC stLmtCalc;
	STHTTPHEADERINFO stHhInfo;
	STLOGMGR * pstLogMgr;

}STMYHTTPFILTERCONTEXT, *PSTMYHTTPFILTERCONTEXT;






//
//	for socket packets
//
#define VWANITILEECHS_PACKET_TYPE_COREWORKRECORD		0x792055
#define VWANITILEECHS_PACKET_TYPE_ACTIVITYQUEUEREPORTER		0x792056
#define VWANITILEECHS_PACKET_TYPE_VWCOREMEMPOOLSTATUS		0x792057	//	VwCoreMemPoolFreeNode


typedef struct tagPacketHeader
{
	DWORD	dwHeader;		//	header flag
	DWORD	dwVersion;		//	version
	DWORD	dwDataLength;		//	size of data
	TCHAR	szAppName[ 64 ];	//	Application name

}STPACKETHEADER, *LPSTPACKETHEADER;

//
//	PacketCoreWorkRecordData
//
typedef struct tagPacketCoreWorkRecordData
{
	DWORD dwProcessId;
	TCHAR szAppPoolName[ 128 ];
	DWORD dwAppPoolNameCrc32;
	DWORD dwCoreTick;
	BOOL  bContinueWork;

}STPACKETCOREWORKRECORDDATA, *LPSTPACKETCOREWORKRECORDDATA;
typedef struct tagPacketCoreWorkRecord
{
	STPACKETHEADER stHeader;	//	header �������λ
	STPACKETCOREWORKRECORDDATA stData;

}STPACKETCOREWORKRECORD, *LPSTPACKETCOREWORKRECORD;


//
//	PacketActivityQueueReportData
//
typedef struct tagPacketActivityQueueReportData
{
	DWORD dwProcessId;
	TCHAR szAppPoolName[ 128 ];
	DWORD dwAppPoolNameCrc32;

	DWORD lnStartTickCount;
	STVWNOTIFYACTIONTIME stActionTime;
	DWORD dwAction;
	LONG  lnCyclePoolIdx;
	DWORD dwRequestFlag;
	DWORD dwNotificationType;
	BOOL  bExceptResource;
	TCHAR szAgent[ MAX_PATH ];
	DWORD dwAgentLength;
	TCHAR szContentType[ 32 ];
	DWORD dwContentLength;
	TCHAR szRange[ 64 ];
	TCHAR szRemoteAddr[ 16 ];
	TCHAR szHost[ 128 ];
	TCHAR szMethod[ 16 ];
	ULONG uServerPort;
	TCHAR szReferer[ 520 ];
	DWORD dwRefererLength;
	TCHAR szFullUri[ 512 ];
	DWORD dwFullUriLength;
	TCHAR szUrlDir[ MAX_PATH ];
	DWORD dwUrlDirLength;
	TCHAR szUrlExt[ 16 ];
	DWORD dwUrlExtLength;
	TCHAR szUrlFile[ 64 ];

}STPACKETACTIVITYQUEUEREPORTDATA, *LPSTPACKETACTIVITYQUEUEREPORTDATA;
typedef struct tagPacketActivityQueueReport
{
	STPACKETHEADER stHeader;	//	header �������λ
	STPACKETACTIVITYQUEUEREPORTDATA stData;

}STPACKETACTIVITYQUEUEREPORT, *LPSTPACKETACTIVITYQUEUEREPORT;
typedef struct tagPacketActivityQueueReportListItem
{
	BOOL bUse;
	STPACKETACTIVITYQUEUEREPORT stItem;

}STPACKETACTIVITYQUEUEREPORTLISTITEM, *LPSTPACKETACTIVITYQUEUEREPORTLISTITEM;


//
//	for VwCoreMemPoolFreeNode
//
typedef struct tagPacketVwCoreMemPoolStatusData
{
	DWORD	dwProcessId;		//	GetCurrentProcessId();
	LONG	lnCyclePoolIdx;		//	�߳����Ƴص����� HaspPool Index
	DWORD   dwAppPoolNameCrc32;
	DWORD   dwAction;

}STPACKETVWCOREMEMPOOLSTATUSDATA, *LPSTPACKETVWCOREMEMPOOLSTATUSDATA;
typedef struct tagPacketVwCoreMemPoolStatus
{
	STPACKETHEADER stHeader;	//	header �������λ
	DWORD dwValidItemCount;		//	��Ч�����
	STPACKETVWCOREMEMPOOLSTATUSDATA stData[ 128 ];

}STPACKETVWCOREMEMPOOLSTATUS, *LPSTPACKETVWCOREMEMPOOLSTATUS;


//
//	for share memory
//
#define VWANITILEECHS_MAX_TOPURL		100

typedef struct tagVirtualWallSrvTopUrl
{
	DWORD dwCountPassed;	//	�����ļ���
	DWORD dwCountDenied;	//	�ܾ��ļ���
	DWORD dwCountBlocked;	//	�����صļ���
	DWORD dwCrc32;
	DWORD dwLength;
	TCHAR szUrl[ 512 ];

}STVIRTUALWALLSRVTOPURL, *LPSTVIRTUALWALLSRVTOPURL;

typedef struct tagVirtualWallSrvShareMemory
{
	DWORD dwVersion;
	CRITICAL_SECTION oCrSecActivityListLock;
	CRITICAL_SECTION oCrSecActivitySetStatus;
	CRITICAL_SECTION oCrSecCoreWorkRecordLock;

	BOOL  bSendActivityReportToService;				//	Client �Ƿ��ͱ��������
	DWORD dwVwCfgNewActivitySetStatusTick;				//	Tick of set activity by VwCfgNew.exe
	STPACKETACTIVITYQUEUEREPORTLISTITEM ArrActivityList[ 1000 ];	//	...
	DWORD dwActivityListCount;					//	...

	STPACKETCOREWORKRECORD stCoreWorkRecord;

	//	for top url
	STVIRTUALWALLSRVTOPURL ArrTopRequestPassed[ VWANITILEECHS_MAX_TOPURL ];		//	Top ����
	DWORD dwTopRequestPassedCount;

	STVIRTUALWALLSRVTOPURL ArrTopRequestDenied[ VWANITILEECHS_MAX_TOPURL ];
	DWORD dwTopRequestDeniedCount;

	STVIRTUALWALLSRVTOPURL ArrTopRequestBlocked[ VWANITILEECHS_MAX_TOPURL ];
	DWORD dwTopRequestBlockedCount;

	STVIRTUALWALLSRVTOPURL ArrTopReferer[ VWANITILEECHS_MAX_TOPURL ];		//	Top ��Դ
	DWORD dwTopRefererCount;

}STVIRTUALWALLSRVSHAREMEMORY, *LPSTVIRTUALWALLSRVSHAREMEMORY;











typedef struct tagSiteConfig
{
	tagSiteConfig()
	{
		memset( this, 0, sizeof(tagSiteConfig) );
	}
	
	//	HTTP_FILTER_CONTEXT Pool
	//STMYHTTPFILTERCONTEXTPOOL stMyHfcPool;
	
	STIISTABLE stIis;

	//	���ص� ��չ��
	DWORD dwCountBlockExt;
	STIISBLOCKEXTTABLE stIisBlockExt[32];
	STCFGDATA stCfgDataBlockExt[32];
	
	//	���ص� Ŀ¼
	DWORD dwCountBlockDir;
	STIISBLOCKDIRSTABLE stIisBlockDir[32];
	STCFGDATA stCfgDataBlockDir[32];

	//	����� Ŀ¼
	DWORD dwCountAllowDir;
	STIISALLOWDIRSTABLE stIisAllowDir[ MAX_ALLOWDIRSCOUNT ];
	//STCFGDATA stCfgDataAllowDir[32];

	//	�������������֩��
	//STVWSPIDERMATCHTREE stIisFriendSpider;
	DWORD dwExceptedSpiderCount;
	STVWSPIDERMATCHMEMVDATA stIisFriendSpiderMemV[ MAX_FRIENDHOSTSCOUNT ];

	//	����� ����վ��
	DWORD dwCountFriendHosts;
	STIISFRIENDHOSTSTABLE stIisFriendHosts[ MAX_FRIENDHOSTSCOUNT ];
	//STCFGDATA stCfgDataFriendHosts[32];

	//	����վ��
	DWORD dwCountBlockHosts;
	STIISBLOCKHOSTSTABLE stIisBlockHosts[ MAX_BLOCKHOSTSCOUNT ];

	//	���� IP ��ַ
	DWORD dwCountBlockIp;
	STIISBLOCKIPTABLE stIisBlockIp[ MAX_BLOCKIPCOUNT ];

	STIISSECURITYTABLE stIisSecurity;
	STIISLOGABLE stIisLog;
	STIISADVANCEDTABLE stIisAdvanced;

}STSITECONFIG, *PSTSITECONFIG;


/**
 *	�����������ݽṹ
 */
typedef struct tagConfigCore
{
	tagConfigCore()
	{
		memset( this, 0, sizeof(tagConfigCore) );
		bContinueWork	= TRUE;
	}

	BOOL  bIsInit;				//	������ݽṹ���Ƿ��Ѿ���ʼ��
	BOOL  bContinueWork;			//	�Ƿ��������
	BOOL  bEnterpriseVer;			//	�Ƿ�����ҵ�汾
	BOOL  bRegUseHostSucc;			//	������� Host ע��ɹ���
	//	..
	DWORD dwDay;
	DWORD dwStartTicket;			//	��ʼ�δ�
	DWORD dwProcTimes;			//	��������Ĵ���
	DWORD dwBlockTimes;
	__int64 _n64LimitedDataTransfer;	//	���˰汾���ƴ�С
	__int64 _n64DataTransfer;		//	9223372036854775807 �ֽ� = 8589934591 G�ֽ�
	
	TCHAR szLocalAddr[ 32 ];		//	��ǰ����� IP ��ַ
	TCHAR szComputerName[ 64 ];
	TCHAR szModFile[ MAX_PATH ];		//	DLL ģ���Լ���ȫ·��
	TCHAR szModPath[ MAX_PATH ];		//	DLL ģ���Լ������ļ���
	TCHAR szDbPath[ MAX_PATH ];		//	�����ļ���·��
	TCHAR szCfgMapFile[ MAX_PATH ];		//	���������ڴ�ӳ���ļ���
	TCHAR szCoreWorkFile[ MAX_PATH ];	//	����ģ�鹤��״����¼�ļ�

	//	..
	STMAINTABLE stMain;			//	ע�������Ϣ

} STCONFIGCORE, *PSTCONFIGCORE;


typedef struct tagConfigExProcessInfo
{
	DWORD dwProcessId;
	TCHAR szAppPoolName[ 128 ];
	DWORD dwAppPoolNameCrc32;

}STCONFIGEXPROCESSINFO, *LPSTCONFIGEXPROCESSINFO;


/**
 *	��ͨ�������ݽṹ
 */
typedef struct tagConfigEx
{
	tagConfigEx()
	{
		memset( this, 0, sizeof(tagConfigEx) );
		memset( & stCfgDataInit, 0, sizeof(stCfgDataInit) );

		//	Ĭ�Ϲ����� flags
		dwFilterFlags = SF_NOTIFY_ORDER_DEFAULT | 
				SF_NOTIFY_PREPROC_HEADERS | 
				SF_NOTIFY_URL_MAP | 
				SF_NOTIFY_SEND_RESPONSE | 
				SF_NOTIFY_SEND_RAW_DATA | 
				SF_NOTIFY_END_OF_REQUEST;
	}
	~tagConfigEx()
	{
		//
		//if ( pstHostAll )
		//{
		//	delete [] pstHostAll;
		//	pstHostAll = NULL;
		//}
		//if ( pstAllSiteCfg )
		//{
		//	delete [] pstAllSiteCfg;
		//	pstAllSiteCfg = NULL;
		//}
	}

	//	..
	BOOL  bCfgLoading;			//	�Ƿ�����װ����������

	//	filter ����
	DWORD dwFilterFlags;			//	������������ flags
	TCHAR szUrlMatchHeader[ 8 ];		//	URL ƥ����ͷ�������ж���������վ��� * ƥ����㣬һ�� 6 ���ַ������� "VWXCY:"

	STHOSTINFO * pstHostAll;		//	[����] ���е� HOST �б�
	DWORD dwHostSize;			//	ռ���ڴ��С
	DWORD dwHostCount;			//	HOST ����

	//	..
	STSITECONFIG * pstAllSiteCfg;		//	[����] װ�ص�ʱ���ڴ�����ڶ���
	DWORD dwSiteCfgSize;			//	ռ���ڴ��С
	DWORD dwSiteCfgCount;			//	վ������

	//	������ʼ��
	STCFGDATA stCfgDataInit;		//	����������Ϣ��������ʼ�� STMYHTTPFILTERCONTEXT->pstCfgData����ָ�뽫ָ������
	STCONFIGEXPROCESSINFO stProcessInfo;

} STCONFIGEX, *PSTCONFIGEX;









/**
 *	�ڴ�ӳ���ļ�ͷ��Ϣ�ṹ��
 */
typedef struct tagCfgMapHeader
{
	//	for mb_stCfgCore.stMain
	DWORD dwMainTableSize;			//	main_table ռ�õ��ڴ��С sizeof( mb_stCfgCore.stMain )

	//	for tagConfigEx.pstHostAll
	DWORD dwHostSize;			//	ռ���ڴ��С
	DWORD dwHostCount;			//	HOST ����

	//	for tagConfigEx.pstAllSiteCfg
	DWORD dwSiteCfgSize;			//	ռ���ڴ��С
	DWORD dwSiteCfgCount;			//	վ������
	
} STCFGMAPHEADER, * PSTCFGMAPHEADER;


/**
 *	STRING TAB ����
 *	ʹ���� CProcBase
 */
typedef struct tagStringTab
{
	tagStringTab()
	{
		memset( this, 0, sizeof(tagStringTab) );
	}

	TCHAR szLangCode[ 16 ];

	TCHAR szResponseHeader[ MAX_URL ];
	DWORD dwResponseHeaderLength;

	TCHAR szResponseTopInfo[ MAX_URL ];
	DWORD dwResponseTopInfoLength;

	TCHAR szResponseTopInfoLogo[ MAX_URL ];
	DWORD dwResponseTopInfoLogoLength;

	TCHAR szResponseAddInfo[ MAX_URL ];
	DWORD dwResponseAddInfoLength;

	TCHAR szStringTrialVersion[ MAX_PATH ];

	TCHAR szWrnAntiLeechs[ 64 ];
	TCHAR szVrnAntiAttack[ 64 ];			//	Anti-Attack
	TCHAR szWrnAccessDenied[ 64 ];			//	Access Denied!
	TCHAR szWrnAccessDeniedBIp[ 64 ];		//	Access denied from your location.
	TCHAR szWrnAccessDeniedBHost[ 64 ];		//	Access denied from this website.

	TCHAR szWrnTooMuchConnection[ 64 ];		//	Too much connection!
	TCHAR szWrnDownloadForbidden[ 64 ];		//	The download is forbidden!
	TCHAR szWrnRequestUrlTooLong[ 64 ];		//	The request url is too long!
	TCHAR szWrnRequestUrlUnsafe[ 64 ];		//	The request url is unsafe!
	TCHAR szWrnPostContentTooLong[ 64 ];		//	The post content is too long!
	TCHAR szWrnContentSentByServerTooLong[ 64 ];	//	The content sent by server is too long!
	TCHAR szWrnTooMuchVisitToday[ 64 ];		//	Too much visiting today!

}STSTRINGTAB, *PSTSTRINGTAB;








#endif	// __VWSTRUCT_HEADER__