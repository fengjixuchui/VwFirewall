// MetaOpIISHeader.h: interface for the CAdsiOpIIS class.
//
//////////////////////////////////////////////////////////////////////
#ifndef __METAOPIISHEADER_HEADER__
#define __METAOPIISHEADER_HEADER__

//	for iis
#include <iadmw.h>
#include <iiscnfg.h>
#include <iwamreg.h>

//	for stl
#include <ctime>
#include <memory>
#include <string>
#include <bitset>
#include <fstream>
#include <vector>
#include <map>
#include <algorithm>
using namespace std;



/**
 *	��������
 */
#define METAOPIIS_MD_TIMEOUT		20							//��METABASE������ʱ����
#define METAOPIIS_KEYNAME_LEN		16
#define METAOPIIS_BUFFER_SIZE		1024							

//	����FLAG��־
//#define FLAG_NORMAL			0
//#define FLAG_NEW			1
//#define FLAG_DELETE			2
//#define FLAG_MODIFY			3


///////////////////////////////////////////////////////////////////////////
//	����������
#define ERROR_CREATE_DIR			1001		//	�½�һ��Ŀ¼ʧ��
#define ERROR_DELETE_DIR			1002		//	ɾ��һ��Ŀ¼ʧ��

#define ERROR_ADD_FTPUSER			1003		//	����һ��FTP�û�ʧ��
#define ERROR_DELETE_FTPUSER			1004		//	ɾ��һ��FTP�û�ʧ��
#define ERROR_ENABLE_FTPUSER			1005		//	����һ��FTP�û��ʻ�ʧ��
#define ERROR_DISABLE_FTPUSER			1006		//	����һ��FTP�û��ʻ�ʧ��
#define ERROR_MODIFY_FTPUSER			1007		//	�޸�һ��FTP�ʻ���Ϣʧ��

#define ERROR_CREATE_KEY			1010		//	�½�һ������ʧ��
#define ERROR_MODIFY_SBINDING			1011		//	�޸İ���Ϣʧ��
#define ERROR_MODIFY_SCOMMENT			1012		//	�޸�վ��˵��ʧ��
#define ERROR_MODIFY_SSTATE			1013		//	�޸�վ��״̬ʧ��
#define ERROR_MODIFY_MAXCONNECTION		1014		//	�޸����������ʧ��
#define ERROR_MODIFY_MAINDIR			1015		//	�޸�վ����Ŀ¼ʧ��
#define ERROR_MODIFY_MDIRACCESS			1016		//	�޸���Ŀ¼����ʧ��
#define ERROR_STATE_COMMAND			1017		//	����վ��ʧ��
#define ERROR_MODIFY_DEFAULTDOC			1018		//	�޸�Ĭ���ĵ�ʧ��		
#define ERROR_CREATE_APP			1019		//	����Ӧ�ó���ʧ��
#define ERROR_USE_LOGS				1020		//	���û�������־ʧ��
#define ERROR_NEW_LOGFILES			1021		//	�޸�����־ʱ����ʧ��

#define ERROR_NOEXIST_KEY			1101		//	����������������
#define ERROR_INVALID_SB			1102		//	�����İ����ݲ��ϸ�


#define FILE_VERSION_COMMENT	"IIS backup tools V4.15 Copyright (c) 1999-2008 ViDun.Com"
#define METAOPIIS_DEFAULTDOC	"default.htm,default.html,default.asp,index.htm,index.html,index.asp"


//	AppPool key define
//#define MD_APPPOOL_STATE				9027	//	2001
#define MD_APPPOOL_PERIODICRESTARTTIME			9001	//	2002
#define MD_APPPOOL_PERIODICRESTARTREQUESTS		9002	//	2003
#define MD_APPPOOL_PERIODICRESTARTSCHEDULE		9020	//	2004
#define MD_APPPOOL_PERIODICRESTARTMEMORY		9024	//	2005
#define MD_APPPOOL_PERIODICRESTARTPRIVATEMEMORY		9038	//	2006
#define MD_APPPOOL_IDLETIMEOUT				9005	//	2007
#define MD_APPPOOL_APPPOOLQUEUELENGTH			9017	//	2008
#define MD_APPPOOL_CPULIMIT				9023	//	2009
#define MD_APPPOOL_CPURESETINTERVAL			2144	//	2010
#define MD_APPPOOL_CPUACTION				9022	//	2011
#define MD_APPPOOL_MAXPROCESSES				9003	//	2012
#define MD_APPPOOL_PINGINGENABLED			9004	//	2013	
#define MD_APPPOOL_PINGINTERVAL				9013	//	2014
#define MD_APPPOOL_RAPIDFAILPROTECTION			9006	//	2015
#define MD_APPPOOL_RAPIDFAILPROTECTIONMAXCRASHES	9030	//	2016
#define MD_APPPOOL_RAPIDFAILPROTECTIONINTERVAL		9029	//	2017
#define MD_APPPOOL_STARTUPTIMELIMIT			9011	//	2018
#define MD_APPPOOL_SHUTDOWNTIMELIMIT			9012	//	2018



/**
 *	�ṹ�嶨��
 */
typedef struct tagMetaKeyName
{
	tagMetaKeyName()
	{
		memset( this, 0, sizeof(tagMetaKeyName) );
	}
	WCHAR wszKeyName[ METADATA_MAX_NAME_LEN ];

}STMETAKEYNAME, *PSTMETAKEYNAME;

typedef struct tagMetaVirtualDir
{
	//	����Ŀ¼����
	tagMetaVirtualDir()
	{
		memset( this, 0, sizeof(tagMetaVirtualDir) );
	}
	
	DWORD dwFlag;						//	������־	
	
	TCHAR szSiteKeyName[ METADATA_MAX_NAME_LEN ];		//	����վ�����
	TCHAR szKeyName_VirDir[ METADATA_MAX_NAME_LEN ];	//	����Ŀ¼����
	TCHAR szPath_VirDir[ MAX_PATH];				//	����Ŀ¼·��������HTTP�ض�λ·��
	DWORD dwAccessFlag;					//	����Ŀ¼��������
	
}STMETAVIRTUALDIR, *PSTMETAVIRTUALDIR;

typedef struct tagMetaSiteInfo
{
	//	վ����Ϣ
	tagMetaSiteInfo()
	{
		memset( this, 0, sizeof(tagMetaSiteInfo) );
		lnMaxBindwidth	= -1;
	}

	DWORD dwOpFlag;					//	��վ��ı�־,�����ķ�ʽ

	TCHAR szKeyName[ METADATA_MAX_NAME_LEN ];	//	��վ����METABASE�еļ���
	TCHAR szServerComment[ MAX_PATH ];		//	վ��˵��(����)
	TCHAR szAppPoolId[ MAX_PATH ];			//	AppPoolId
	DWORD dwServerState;				//	վ�����״̬
	DWORD dwAllowKeepAlive;				//	���� Http ����
	DWORD dwConnectionTimeout;			//	���ӳ�ʱ
	LONG  lnMaxConnection;				//	������������������������
	LONG  lnMaxBindwidth;				//	����������

	TCHAR szBindings[ METAOPIIS_BUFFER_SIZE ];	//	����Ϣ

	LONG  lnLogType;				//	�Ƿ�������־�Լ�������־��ʽ 0-������ 1-���ò�ʹ��W3C��ʽ
	LONG  lnLogFileLocaltimeRollover;		//	��־��¼��ʹ�ñ���ʱ��
	LONG  lnLogFilePeriod;				//	��־��¼���ƻ�
	LONG  lnLogFileTruncateSize;			//	��־��¼���ļ���С
	LONG  lnLogExtFileFlags;			//	��־��¼���߼�ѡ��
	TCHAR szLogFileDirectory[ MAX_PATH ];		//	��־��¼��Ŀ¼
	TCHAR szLogPluginClsid[ MAX_PATH ];		//	��־��¼��PluginClsid

	TCHAR szServerMDir[ MAX_PATH ];			//	վ����Ŀ¼
	DWORD dwMDirAccessFlag;				//	��Ŀ¼����������
	LONG  lnDirBrowseFlags;				//	��Ŀ¼��Ŀ¼�������
	DWORD dwDontLog;				//	��Ŀ¼����¼����
	DWORD dwContentIndexed;				//	��Ŀ¼��������Դ
	TCHAR szDefaultDoc[ METAOPIIS_BUFFER_SIZE ];	//	Ĭ���ĵ�
	TCHAR szHttpExpires[ METADATA_MAX_NAME_LEN ];	//	Httpͷ >> �������ݹ���
	TCHAR szHttpErrors[ METAOPIIS_BUFFER_SIZE ];	//	�Զ������
	LONG  lnEnableDocFooter;			//	�ĵ� >> �����ĵ�ҳ��
	TCHAR szDefaultDocFooter[ MAX_PATH ];		//	�ĵ� >> �ĵ�ҳ���ļ�·��

	INT   nStatus;					//	վ��״̬

}STMETASITEINFO, *PSTMETASITEINFO;

typedef struct tagMetaAppPoolInfo
{
	//	AppPool �ṹ
	tagMetaAppPoolInfo()
	{
		memset( this, 0, sizeof(tagMetaAppPoolInfo) );
	}

	TCHAR szKeyName[ METADATA_MAX_NAME_LEN ];	//	��վ����METABASE�еļ���
	//TCHAR szPoolName[ MAX_PATH ];
	DWORD dwAppPoolState;			//	2001 

	DWORD dwPeriodicRestartTime;					//	2002 ���� >> ���չ�������(����)
	DWORD dwPeriodicRestartRequests;				//	2003 ���� >> ���չ�������(������Ŀ)
	TCHAR szPeriodicRestartSchedule[ METAOPIIS_BUFFER_SIZE ];	//	2004 ���� >> ������ʱ����չ�������
	DWORD dwPeriodicRestartMemory;		//	2005 ���� >> �ڴ���� >> ��������ڴ�(��)
	DWORD dwPeriodicRestartPrivateMemory;	//	2006 ���� >> �ڴ���� >> ���ʹ�õ��ڴ�(��)

	DWORD dwIdleTimeout;			//	2007 ���� >> ���г�ʱ >> �ڿ��д˶ι���ʵ����رչ�������(����)
	DWORD dwAppPoolQueueLength;		//	2008 ���� >> ����������� >> ���������������Ϊ(�������)
	DWORD dwCPULimit;			//	2009 ���� >> ���� CPU ���� >> ��� CPU ʹ����(�ٷֱ�)
	DWORD dwCPUResetInterval;		//	2010 ���� >> ���� CPU ���� >> ˢ�� CPU ʹ����(����)
	DWORD dwCPUAction;			//	2011 ���� >> ���� CPU ���� >> CPU ʹ���ʳ������ʹ����ʱִ�еĲ���
	DWORD dwMaxProcesses;			//	2012 ���� >> Web԰ >> �����������

	DWORD dwPingingEnabled;			//	2013 ����״�� >> ���� Ping
	DWORD dwPingInterval;			//	2014 ����״�� >> ���� Ping >> ÿ������ʱ�� Ping �����߳�(��)
	DWORD dwRapidFailProtection;		//	2015 ����״�� >> ���ÿ���ʧ�ܱ���
	DWORD dwRapidFailProtectionMaxCrashes;	//	2016 ����״�� >> ���ÿ���ʧ�ܱ��� >> ʧ����
	DWORD dwRapidFailProtectionInterval;	//	2017 ����״�� >> ���ÿ���ʧ�ܱ��� >> ʱ���(����)
	
	DWORD dwStartupTimeLimit;		//	2018 ����״�� >> ����ʱ������ >> �������̱���������ʱ���ڿ�ʼ(��)
	DWORD dwShutdownTimeLimit;		//	2019 ����״�� >> �ر�ʱ������ >> �������̱���������ʱ���ڹر�(��)

}STMETAAPPPOOLINFO, *PSTMETAAPPPOOLINFO;


typedef struct tagMetaAllData
{
	//	���� IIS ������
	tagMetaAllData()
	{
		memset( this, 0, sizeof(tagMetaAllData) );
	}

	vector<STMETASITEINFO> vcSite;
	vector<STMETAAPPPOOLINFO> vcAppPool;

}STMETAALLDATA, *PSTMETAALLDATA;


typedef struct tagMetaOperRecord
{
	//	������¼�ṹ
	WCHAR wszKeyPath[ MAX_PATH ];		//	����·��
	union					//	��������
	{
		WCHAR wszKeyName[ METAOPIIS_KEYNAME_LEN ];
		METADATA_RECORD metaRecord;
	};

}STMETAOPERRECORD, *PSTMETAOPERRECORD;






#endif	//	__METAOPIISHEADER_HEADER__