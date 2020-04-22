// VwConstAntileechs.h: define all const value for the project
//

#ifndef __VWCONSTANTILEECHS_HEADER__
#define __VWCONSTANTILEECHS_HEADER__

#include <math.h>
#include <httpfilt.h>
#include "VwConstBase.h"




#define VIRTUALWALL_DEBUG_VER	0

//////////////////////////////////////////////////////////////////////////
#define alerti( msg ) MessageBox( msg, "Information", MB_ICONINFORMATION )
#define alertw( msg ) MessageBox( msg, "Warning", MB_ICONWARNING )
#define alerte( msg ) MessageBox( msg, "Error", MB_ICONERROR )

#define MUTEX_SHARE_VWCFGNEW_EXE		_T("Global\\VwCfgNew_EXE")
//#define MUTEX_SHARE_VWCFGNEW_EXE		_T("VwCfgNew_EXE")
#define MUTEX_SHARE_RUNDLLMAKECFGMAP		_T("RundllMakeCfgMap_Dll")
#define VWNAMEDPIPE_COREWORK			_T("VwNamedPipeCoreWork")
#define VWNAMEDPIPE_LOADCONFIG			_T("VwNamedPipeLoadConfig")
#define VIRTUALWALL_SHAREMM_COREWORK		_T("VirtualWallShareMMCoreWork")

//////////////////////////////////////////////////////////////////////////
//	pipe message
#define VWPIPE_MSG_READ_ENTVER			"read_entver"
#define VWPIPE_MSG_READ_CONTINUEWORK		"read_continuework"
#define VWPIPE_MSG_READ_CORETICK		"read_coretick"
#define VWPIPE_MSG_READ_LOADCFGTICK		"read_loadcfgtick"
#define VWPIPE_MSG_CALL_RELOADCONFIG		"call_reloadconfig"

//////////////////////////////////////////////////////////////////////////
//	socket config
#define VWSKS_VWANTILEECHS_IPADDR			"127.0.0.1"
#define VWSKS_VWANTILEECHS_PORT				20055
#define VWSKS_VWANTILEECHS_APP_ACTIVITYQUEUEREPORTER	"ActivityQueueReporter"
#define VWSKS_VWANTILEECHS_APP_VWCOREWORK		"VwCoreWork"

#define VWSKS_VWCOREWORK_IPADDR			"127.0.0.1"
#define VWSKS_VWCOREWORK_PORT			20055
#define VWSKS_VWCOREWORK_MSG_READ_CCW		"read_core_continuework"

#define VWSKS_ACTIVITYQUEUEREPORTER_IPADDR	"127.0.0.1"
#define VWSKS_ACTIVITYQUEUEREPORTER_PORT	20056



//////////////////////////////////////////////////////////////////////////
//	for global
#define MAX_URL				1024	// max URL length
#define MAX_SINGLEURLCHARSLEN		16
#define MAX_HOSTCOUNT			1000	//	Host �������
#define MAX_SITECOUNT			500	//	Site �������
#define MAX_ALLOWDIRSCOUNT		512	//	����Ŀ¼�������
#define MAX_FRIENDHOSTSCOUNT		512	//	����վ���������
#define MAX_BLOCKHOSTSCOUNT		512	//	����վ���������
#define MAX_BLOCKIPCOUNT		1024	//	���� IP �������

#define MAX_CACHESIZE_PROTECTEDRES	10000
#define MAX_CACHESIZE_EXCEPTIONRES	10000
#define MAX_CACHESIZE_CFGIDX		10000


//////////////////////////////////////////////////////////////////////////
typedef unsigned (WINAPI *PBEGINTHREADEX_THREADFUNC)(LPVOID lpThreadParameter);
typedef unsigned *PBEGINTHREADEX_THREADID;


//////////////////////////////////////////////////////////////////////////
//
#define FILTERDESC_VIRTUALWALL		"VirtualWall Filter"
#define FILTERDESC_VWCLIENT		"VirtualWall filter's client"
#define FILTERDESC_VIRTUALHTML		"VirtualHtml SEO Filter"
#define FILTERDESC_VWPANDOMAIN		"VirtualWall Pan-Domain Filter"

/**
 *	���� Filter Flags
 */
#define VIRTUALWALL_FILTER_FLAGS_DEFAULT		SF_NOTIFY_ORDER_HIGH | \
							SF_NOTIFY_PREPROC_HEADERS | \
							SF_NOTIFY_URL_MAP | \
							SF_NOTIFY_SEND_RESPONSE | \
							SF_NOTIFY_SEND_RAW_DATA | \
							SF_NOTIFY_END_OF_REQUEST

#define VIRTUALWALL_FILTER_FLAGS_SPEED_LIMITLESS	SF_NOTIFY_ORDER_HIGH | \
							SF_NOTIFY_PREPROC_HEADERS | \
							SF_NOTIFY_URL_MAP | \
							SF_NOTIFY_SEND_RESPONSE | \
							SF_NOTIFY_END_OF_REQUEST

//////////////////////////////////////////////////////////////////////////
//
#define CONST_DIR_LOGS			"Logs"


//////////////////////////////////////////////////////////////////////////
//	for VirtualWall core module
#define FLAG_MRDS			"/mrds-"
#define EXT_HTML			".htm"
#define MAX_PARAM			512
//#define MAX_URL			1024
#define MAX_EXTLEN			10
//#define MAX_ALERTFILE			16
//#define DEF_BLOCK_EXT			".mdb|.exe|.rar|.zip"
//#define DEF_ALLOW_DIRS		"/|/images"
//#define DEF_BLOCK_DIRS		""
//#define DEF_BLOCK_TIMES		1000
//#define DEF_PUBVSIDUSECOOKIE		1
//#define DEF_PUBVSIDKEY		"vsid"
//#define DEF_PUBVSIDKEYTIME		0	// 2.1.6.1037 �Ժ�汾�Ͳ��ٹ����û�ʹ��ʱ�������ˣ�����Ĭ��ֵ 6000 ��Ϊ 0
//#define DEF_PLAYERMPEXTS		""	// 2.1.7.1038 �Ժ�Ĭ��Ϊ�� ".wma|.wmv|.wav|.asf|.avi|.aiff|.au|.mp2|.mp3|.mp4|.mpg|.mpeg|.mpe|.mpv2|.mp2v|.mpa|.mid"
//#define DEF_PLAYERRPEXTS		""	// 2.1.7.1038 �Ժ�Ĭ��Ϊ�� ".rm|.ram|.rmvb|.rpm|.rt|.rp|.smi|.smil|.ra"

#define AGENT_MEDIAPLAYER		"nsplayer"		//	NSPlayer/10.0.0.3646 WMFSDK/10.0
#define AGENT_MEDIAPLAYER2		"media-player"		//	Windows-Media-Player/10.00.00.3802
#define AGENT_MEDIAPLAYER3		"wmplayer"		//	WMPlayer/11.0.5721.5251
#define AGENT_REALPLAYER		"realmedia"		//	RMA/1.0 (compatible; RealMedia)

#define VSIDCOOKIE_NAME			"virtualwall=vsid="	//	Vsid Cookie ͷ
#define ITEM_VSID			"vsid"
#define HTTPHEADER_VWVERIFY		"Vw-Verify"		//	http header
#define HTTPHEADER_VWVERIFY_CO		"Vw-Verify:"		//	http header
#define HTTPHEADER_RANGE		"Range"			//	http header
#define HTTPHEADER_RANGE_CO		"Range:"		//	http header

#define HTTPURI_SYS_VW_HTML		"/_sys_vw.html"		//	"/_sys_vw.html"
#define HTTPURI_SYS_VW_HTML_LEN		13			//	length of "/_sys_vw.html"
#define HTTPURI_SYS_VW_VHTML		"/_sys_vw.vhtml"		//	"/_sys_vw.html"
#define HTTPURI_SYS_VW_VHTML_LEN	14			//	length of "/_sys_vw.html"

#define VSID_ALIAS_PARAMNAME		"/vsid-"		//	URL �� "?vsid=" �ı��������������ȼۣ���Ҫ��Ϊ��ֹ���ع������� .EXE �ļ�������չ������
								//	"http://www.vidun.com/a.exe?vsid=1234567890"
								//	"http://www.vidun.com/vsid-1234567890/a.exe"

#define VWREALNAME_ALIAS_PARAMNAME	"/vwrealname-"		//	URL �� "//vwrealname-��ʵ�ļ���"
#define VWREALNAME_ALIAS_PARAMNAME_LEN	12			//	��������


#define REQUEST_VW_OKPASS		(DWORD)0			//	�Ź���ǰ����
#define REQUEST_VW_DENY			(DWORD)pow( 2, 0 )		//	�ܾ�����
#define REQUEST_VW_BLOCK		(DWORD)pow( 2, 1 )		//	��ֹ��ǰ����
#define REQUEST_VW_LIMITTHREAD		(DWORD)pow( 2, 2 )		//	�߳�����
#define REQUEST_VW_LIMITSPEED		(DWORD)pow( 2, 3 )		//	�ٶ�����
#define REQUEST_VW_LIMITPLAYONLY	(DWORD)pow( 2, 4 )		//	ֻ�����߲��Ŷ��������ص�����
#define REQUEST_VW_LIMITURLLEN		(DWORD)pow( 2, 5 )		//	���� URL ����
#define REQUEST_VW_LIMITURLCHRS		(DWORD)pow( 2, 6 )		//	���� URL �е��ַ���
#define REQUEST_VW_LIMITCONTENTPOST	(DWORD)pow( 2, 7 )		//	���� POST ���ݵĳ���
#define REQUEST_VW_LIMITCONTENTSEND	(DWORD)pow( 2, 8 )		//	���Ʒ���η��͸��ͻ��˵����ݳ���
#define REQUEST_VW_ANTIATTACK		(DWORD)pow( 2, 9 )		//	������
#define REQUEST_VW_LIMITDAYIP		(DWORD)pow( 2, 10 )		//	�ձ�������Դ���ʴ�������
#define REQUEST_VW_LIMITHOST		(DWORD)pow( 2, 11 )		//	���Ʒ��ʵ� HOST
#define REQUEST_VW_LIMITIP		(DWORD)pow( 2, 12 )		//	���Ʒ��ʵ� IP ��ַ
#define REQUEST_VW_CACHECONTROL_NOCACHE	(DWORD)pow( 2, 13 )		//	��ֹ�ͻ��˻���
#define REQUEST_VW_DISP_VHTML		(DWORD)pow( 2, 14 )		//	��ʾ "/_sys_vw.vhtml"

#define CONST_BWTYPE_BLACK		1		//	���������ڰ�����-������
#define CONST_BWTYPE_WHITE		2		//	���������ڰ�����-������


/**
 *	���� CyclePool ��ǳ���
 */
#define CONST_CYCLEPOOLFLAG_FREE	0		//	�ýڵ��� Free ��
#define CONST_CYCLEPOOLFLAG_MEMPOOL	1		//	�ýڵ��� Pool �е�
#define CONST_CYCLEPOOLFLAG_MEMNEW	2		//	�ýڵ��� new ��������
#define CONST_CYCLEPOOLFLAG_DIRTYPOOL	3		//	�ýڵ����Ѿ������ˣ�POOL ���ʹ������ feee ��


/**
 *	��ʱ���غ궨��
 */
#define CONST_HOUR_00_01		(DWORD)pow( 2, 0 )
#define CONST_HOUR_01_02		(DWORD)pow( 2, 1 )
#define CONST_HOUR_02_03		(DWORD)pow( 2, 2 )
#define CONST_HOUR_03_04		(DWORD)pow( 2, 3 )
#define CONST_HOUR_04_05		(DWORD)pow( 2, 4 )
#define CONST_HOUR_05_06		(DWORD)pow( 2, 5 )
#define CONST_HOUR_06_07		(DWORD)pow( 2, 6 )
#define CONST_HOUR_07_08		(DWORD)pow( 2, 7 )
#define CONST_HOUR_08_09		(DWORD)pow( 2, 8 )
#define CONST_HOUR_09_10		(DWORD)pow( 2, 9 )
#define CONST_HOUR_10_11		(DWORD)pow( 2, 10 )
#define CONST_HOUR_11_12		(DWORD)pow( 2, 11 )
#define CONST_HOUR_12_13		(DWORD)pow( 2, 12 )
#define CONST_HOUR_13_14		(DWORD)pow( 2, 13 )
#define CONST_HOUR_14_15		(DWORD)pow( 2, 14 )
#define CONST_HOUR_15_16		(DWORD)pow( 2, 15 )
#define CONST_HOUR_16_17		(DWORD)pow( 2, 16 )
#define CONST_HOUR_17_18		(DWORD)pow( 2, 17 )
#define CONST_HOUR_18_19		(DWORD)pow( 2, 18 )
#define CONST_HOUR_19_20		(DWORD)pow( 2, 19 )
#define CONST_HOUR_20_21		(DWORD)pow( 2, 20 )
#define CONST_HOUR_21_22		(DWORD)pow( 2, 21 )
#define CONST_HOUR_22_23		(DWORD)pow( 2, 22 )
#define CONST_HOUR_23_00		(DWORD)pow( 2, 23 )




//////////////////////////////////////////////////////////////////////////
//	for socket
//static const VERSION_MAJOR		= 1;
//static const VERSION_MINOR		= 1;

static const DWORD INSERT_NEW_ITEM_FIRST	= 1000000;


//////////////////////////////////////////////////////////////////////////
//#define TREE_PARAM_ROOT			0
//#define TREE_PARAM_LICENSE		1
//#define TREE_PARAM_SETTINGS		2
//#define TREE_PARAM_UPDATE		3
//#define TREE_PARAM_WEBSITES		4
#define WIN_SUB_VIRTUALWALL		(-1)
#define WIN_SUB_LICENSE			(-2)
#define WIN_SUB_UPDATE			(-3)
#define WIN_SUB_SETTING			(-4)
#define WIN_SUB_GLOBALWEBSITES		(-5)
#define WIN_SUB_GLOBALACTIVITY		(-6)






//////////////////////////////////////////////////////////////////////////

#define CONFIG_DENYALLHTTP			1	//	Deny all HTTP requests
#define CONFIG_ANTILEECHS			2	//	anti-leechs
#define CONFIG_LIMITPLAYONLY			4	//	limit playonly
#define CONFIG_LIMITDOWNLOADINGTHREADS		8	//	limit threads
#define CONFIG_LIMITDOWNLOADINGSPEED		16	//	limit speed
#define CONFIG_ALLOW_REDOWNLOAD			32	//	Allow re-download from dropped connection after IP changed
#define CONFIG_LIMITDAYIP			64	//	limit dayip
#define CONFIG_CACHECONTROL_NOCACHE		128	//	Cache-Control no-cache
#define CONFIG_BLOCK_EMPTYREFERER		256	//	Block the request without referer
#define CONFIG_ALL				CONFIG_DENYALLHTTP | \
						CONFIG_ANTILEECHS | \
						CONFIG_LIMITPLAYONLY | \
						CONFIG_LIMITDOWNLOADINGTHREADS | \
						CONFIG_LIMITDOWNLOADINGSPEED | \
						CONFIG_ALLOW_REDOWNLOAD | \
						CONFIG_LIMITDAYIP | \
						CONFIG_CACHECONTROL_NOCACHE | \
						CONFIG_BLOCK_EMPTYREFERER


#define ACTION_SENDMESSAGES			1	//	Send warning messages
#define ACTION_REPLACEFILE			2	//	Replace file

#define CONFIG_LIMITPLAYONLY_MP			"MP"
#define CONFIG_LIMITPLAYONLY_RP			"RP"

//////////////////////////////////////////////////////////////////////////
//
#define CONST_FILE_EXTENSION_IMAGES		".gif.jpg.jepg.png.bmp."


// ----------------------------------------------------------------------
//	for cfg.mdb -> table [iis_advanced_table]
#define IAV_WM_MODEL_NORMAL			1	//	
#define IAV_WM_MODEL_EXPERT			2	//
#define IAV_WM_MODEL_HTMLFILE			3	//

// ----------------------------------------------------------------------
//	for cfg.mdb -> table [iis_log_table]
#define ILG_LOGTIMEPERIOD_HOURLY		0
#define ILG_LOGTIMEPERIOD_DAILY			1
#define ILG_LOGTIMEPERIOD_WEEKLY		2
#define ILG_LOGTIMEPERIOD_MONTHLY		3

// ----------------------------------------------------------------------
//	for cfg.mdb -> table [iis_security_table]
#define ISC_DISGUISE_AS_APACHE			1


// ----------------------------------------------------------------------
//	for cfg.mdb -> default value

#define PUBVSIDKEY_TYPE_NORMAL		0	//	��ͨ��Ĭ�Ϸ�ʽ
#define PUBVSIDKEY_TYPE_RANDOM		1	//	����仯
#define PUBVSIDKEY_TYPE_FILENAME	2	//	���ļ�����Կ�׼���

#define DEF_PUBVSIDUSECOOKIE		1
#define DEF_PUBVSIDUSEBROWSERIN		0
#define DEF_PUBVSIDKEY			"vsid"
#define DEF_PUBVSIDKEYTIME		0	//	2.1.6.1037 �Ժ�汾�Ͳ��ٹ����û�ʹ��ʱ�������ˣ�����Ĭ��ֵ 6000 ��Ϊ 0
#define DEF_PUBKEYTYPE			PUBVSIDKEY_TYPE_NORMAL

#define DEF_PLAYERMPEXTS		""	//	2.1.7.1038 �Ժ�Ĭ��Ϊ�� ".wma|.wmv|.wav|.asf|.avi|.aiff|.au|.mp2|.mp3|.mp4|.mpg|.mpeg|.mpe|.mpv2|.mp2v|.mpa|.mid"
#define DEF_PLAYERRPEXTS		""	//	2.1.7.1038 �Ժ�Ĭ��Ϊ�� ".rm|.ram|.rmvb|.rpm|.rt|.rp|.smi|.smil|.ra"

#define DEF_ISC_LMT_URLCHARS_CFG	"..|./|\\|'|:|;|(|)|<|>"
#define DEF_ISC_ENABLE_ANTATT		0			//	Ĭ��ֵ���Ƿ�ʼ����ǽ
#define DEF_ISC_ANTATT_MAXVISIT		1000			//	Ĭ��ֵ��ÿ�����������һ���ͻ��˷��ʵĴ���
#define DEF_ISC_ANTATT_SLEEP		5			//	Ĭ��ֵ���ܵ��������ܾ�����೤ʱ�䣨��λ���ӣ�
#define DEF_ISC_ANTATT_DISABLED_PROXY	0			//	Ĭ��ֵ���Ƿ��ֹͨ���������

#define DEF_IAV_WM_DELAY_TIME		10
#define DEF_IAV_WM_REDIRECT_URL		"http://www.vidun.com/"
#define DEF_IAV_WM_SITENAME		"VirtualWall"

#define BLOCK_TYPE_NORMAL		0
#define BLOCK_TYPE_SUPER		1


#define CONST_CONFIG_DBFILE		"cfg.mdb"
#define CONST_CONFIG_CHGFILE		"cfg.chg"
#define CONST_CONFIG_MAPFILE		"cfg.map"
#define CONST_CORE_WORKFILE		"corework.ini"
#define CONST_VWDIYHEADER_DAT		"VwDiyHeader.dat"
#define CONST_STATUSBARAD_INIFILE	"VwStatusbarAd.ini"


#define CONST_CFGMAP_HEADER		"---VIRTUALWALL-CFGMAP COPYRIGHT(C) FANGDAOLIAN.COM---"		//	�ڴ�ӳ���ļ�ͷ����ʶ
#define CONST_CFGMAP_SPLIT		"-VW->-VW->-VW->-VW->-VW->-VW->-VW->-VW->-VW->-VW-"		//	�ڴ�ӳ���ļ����ݷָ�
#define CONST_CFGMAP_FOOTER		"----------VW----------CFGMAP-END----------VW----------"	//	�ڴ�ӳ���ļ���β

#define DIYHEADER_SOFT_VER		1000	//	Diy Header version


// ----------------------------------------------------------------------
//	for ini file

#define DOMAIN_MAIN			"Main"
#define DOMAIN_BLOCK			"Block"

#define KEY_MAIN_START			"Start"
#define KEY_BLOCK_PUBVSIDKEY		"PubVsidKey"
#define KEY_BLOCK_PUBVSIDKEYTIME	"PubVsidKeyTime"
#define KEY_BLOCK_PUBKEYTYPE		"PubKeyType"

#define KEY_MAIN_USEPROXY		"UseProxy"
#define KEY_MAIN_PROXY_VARIABLE		"ProxyVariable"

// ----------------------------------------------------------------------
//	Timer
#define VWTIMER_MAINFRM_STATUSBAR_AD	0x1000





#endif	//	__VWCONSTBASE_HEADER__