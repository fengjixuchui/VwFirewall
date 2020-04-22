// CVwInfoMonitorConfigFile.h: interface for the CVwInfoMonitorConfigFile class.
//
//////////////////////////////////////////////////////////////////////

#ifndef __VWINFOMONITORCONFIGFILE_HEADER__
#define __VWINFOMONITORCONFIGFILE_HEADER__

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000


#include "VwConst.h"
#include "ModuleInfo.h"
#include <vector>
using namespace std;

#include "DeLib.h"
#pragma comment( lib, "DeLib.lib" )


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
#define CVWINFOMONITORCONFIGFILE_FILE_FILTER			"VwInfoMonitor.dll"
#define CVWINFOMONITORCONFIGFILE_FILE_DELIB			"DeLib.dll"

#define CVWINFOMONITORCONFIGFILE_CONFFILE_CFG			"cfg.ini"
#define CVWINFOMONITORCONFIGFILE_CONFFILE_OUT_REP_WORDS		"out_rep_words.ini"
#define CVWINFOMONITORCONFIGFILE_CONFFILE_OUT_REP_WORDS_UTF8	"out_rep_words_utf8.ini"
#define CVWINFOMONITORCONFIGFILE_CONFFILE_OUT_BLOCK_WORDS	"out_block_words.ini"
#define CVWINFOMONITORCONFIGFILE_CONFFILE_IN_REP_WORDS		"in_rep_words.ini"
#define CVWINFOMONITORCONFIGFILE_CONFFILE_IN_BLOCK_WORDS	"in_block_words.ini"
#define CVWINFOMONITORCONFIGFILE_CONFFILE_IN_BLOCK_WORDS_ESCAPE	"in_block_words_escape.ini"
#define CVWINFOMONITORCONFIGFILE_CONFFILE_IN_BLOCK_WORDS_UTF8	"in_block_words_utf8.ini"
#define CVWINFOMONITORCONFIGFILE_CONFFILE_IN_ANTISQL_WORDS	"in_antisql_words.ini"
#define CVWINFOMONITORCONFIGFILE_CONFFILE_EXCEPTION_HOSTS	"exception_hosts.ini"
#define CVWINFOMONITORCONFIGFILE_CONFFILE_IN_BLOCK_ALERT	"in_block_alert.html"
#define CVWINFOMONITORCONFIGFILE_CONFFILE_IN_ANTISQL_ALERT	"in_antisql_alert.html"

//	�ʵ��ļ�
#define CVWINFOMONITORCONFIGFILE_DICT_OUT_REP_WORDS		"out_rep_words.dct"
#define CVWINFOMONITORCONFIGFILE_DICT_OUT_REP_WORDS_UTF8	"out_rep_words_utf8.dct"
#define CVWINFOMONITORCONFIGFILE_DICT_OUT_BLOCK_WORDS		"out_block_words.dct"
#define CVWINFOMONITORCONFIGFILE_DICT_IN_REP_WORDS		"in_rep_words.dct"
#define CVWINFOMONITORCONFIGFILE_DICT_IN_BLOCK_WORDS		"in_block_words.dct"
#define CVWINFOMONITORCONFIGFILE_DICT_IN_BLOCK_WORDS_ESCAPE	"in_block_words_escape.dct"
#define CVWINFOMONITORCONFIGFILE_DICT_IN_BLOCK_WORDS_UTF8	"in_block_words_utf8.dct"
#define CVWINFOMONITORCONFIGFILE_DICT_EXCEPTION_HOSTS		"exception_hosts.dct"

#define CVWINFOMONITORCONFIGFILE_MAX_WORDLOADCOUNT		5000	//	��һ���ļ������װ�شʻ���
#define CVWINFOMONITORCONFIGFILE_MAX_SQLMPLOADCOUNT		500	//	���SQLƥ��������

#define CVWINFOMONITORCONFIGFILE_MIN_WORDLEN			4	//	�ʵ���С����
#define CVWINFOMONITORCONFIGFILE_MAX_WORDLEN			80	//	�ʵ���󳤶�
#define CVWINFOMONITORCONFIGFILE_MAX_SQLMPLEN			100	//	SQLƥ��������󳤶�

#define CVWINFOMONITORCONFIGFILE_INI_DOMAIN_MAIN		"Main"
#define CVWINFOMONITORCONFIGFILE_INI_DOMAIN_LICENSE		"License"
#define CVWINFOMONITORCONFIGFILE_INI_DOMAIN_BLOCK		"Block"
#define CVWINFOMONITORCONFIGFILE_INI_DOMAIN_TICKINFO		"TickInfo"

#define CVWINFOMONITORCONFIGFILE_INI_KEY_START			"start"
#define CVWINFOMONITORCONFIGFILE_INI_KEY_START_WORDBLOCK	"start_wordblock"
#define CVWINFOMONITORCONFIGFILE_INI_KEY_START_ANTISQL		"start_antisql"
#define CVWINFOMONITORCONFIGFILE_INI_KEY_START_WORDFILTER	"start_wordfilter"

#define CVWINFOMONITORCONFIGFILE_INI_KEY_LOG			"log"

#define CVWINFOMONITORCONFIGFILE_INI_KEY_REGIP			"RegIp"
#define CVWINFOMONITORCONFIGFILE_INI_KEY_REGHOST		"RegHost"
#define CVWINFOMONITORCONFIGFILE_INI_KEY_REGKEY			"RegKey"
#define CVWINFOMONITORCONFIGFILE_INI_KEY_REGEXPIRETYPE		"RegExpireType"
#define CVWINFOMONITORCONFIGFILE_INI_KEY_REGCREATEDATE		"RegCreateDate"
#define CVWINFOMONITORCONFIGFILE_INI_KEY_REGEXPIREDATE		"RegExpireDate"

#define CVWINFOMONITORCONFIGFILE_INI_KEY_BLOCKPOST		"BlockPost"
#define CVWINFOMONITORCONFIGFILE_INI_KEY_BLOCKPOSTSIZE		"BlockPostSize"
#define CVWINFOMONITORCONFIGFILE_INI_KEY_BLOCKPOSTSIZECFG	"BlockPostSizeCfg"

#define CVWINFOMONITORCONFIGFILE_INI_KEY_TICKINFO_CTWT		"CTWT"		//	continue to work today
#define CVWINFOMONITORCONFIGFILE_INI_KEY_TICKINFO_TICK		"TICK"		//	tickcount


/**
 *	action
 *	ע��: Ϊ�˽�Լ�ڴ棬���� action ��ֵ���������ܺͲ��ܳ��� 255
 */
#define VWINFOM_ACTION_NONE			0	//	���κβ���
#define VWINFOM_ACTION_OUT_REPLACE		1	//	����: �滻����
#define VWINFOM_ACTION_OUT_BLOCK		2	//	����: ��ֹ��ʾ(��ʱû��)
#define VWINFOM_ACTION_IN_REPLACE		4	//	����: �滻����(��ʱû��)
#define VWINFOM_ACTION_IN_BLOCK			8	//	����: ��ֹ��ʾ
#define VWINFOM_ACTION_IN_BLOCK_POST		16	//	�����ļ��ϴ���ֹ��ʾ
#define VWINFOM_ACTION_IN_ANTISQL		32	//	�����к���Σ�� SQL ���
#define VWINFOM_ACTION_ALERT_OUT_REPLACE	64	//	����������ʾ
#define VWINFOM_ACTION_ALL			( VWINFOM_ACTION_OUT_REPLACE | VWINFOM_ACTION_IN_BLOCK )



/**
 *	struct for word
 */
typedef struct tagVwInfoMWord
{
	tagVwInfoMWord()
	{
		memset( this, 0, sizeof(tagVwInfoMWord) );
	}
	BOOL bWildcard;
	UINT uLen;
	CHAR szWord[ MAX_PATH ];

}STVWINFOMWORD, *LPSTVWINFOMWORD;


/**
 *	struct for configuration
 */
typedef struct tagVwInfoMConfigEcpHost
{
	tagVwInfoMConfigEcpHost()
	{
		memset( this, 0, sizeof(tagVwInfoMConfigEcpHost) );
	}

	TCHAR szHost[ 128 ];

}STVWINFOMCONFIGECPHOST, *LPSTVWINFOMCONFIGECPHOST;

typedef struct tagVwInfoMConfigTicketInfo
{
	BOOL bContinueWork;
	DWORD dwStartTicket;
	DWORD dwDay;
	__int64 _n64LimitedDataTransfer;	//	���˰汾���ƴ�С
	__int64 _n64DataTransfer;		//	9223372036854775807 �ֽ� = 8589934591 G�ֽ�

}STVWINFOMCONFIGTICKETINFO, *LPSTVWINFOMCONFIGTICKETINFO;

typedef struct tagVwInfoMConfig
{
	tagVwInfoMConfig()
	{
		memset( this, 0, sizeof(tagVwInfoMConfig) );
	}

	STVWINFOMCONFIGTICKETINFO stTickInfo;

	BOOL  bReged;						//	�Ƿ�ע��汾
	BOOL  bStart;						//	�Ƿ��������
	BOOL  bStartWordBlock;					//	�����ʻ�����
	BOOL  bStartAntiSQL;					//	���� SQL ����
	BOOL  bStartWordFilter;					//	�����ʻ����
	BOOL  bLog;						//	�Ƿ��¼��־

	DWORD dwFilterFlags;					//	Filter ���ֵ� Flag ��Ҫ����

	TCHAR szRegIp[ 64 ];					//	ע�� IP ��ַ
	TCHAR szRegHost[ MAX_PATH ];				//	ע�� HOST
	TCHAR szRegKey[ MAX_PATH ];				//	ע����
	DWORD dwRegExpireType;					//	��������
	TCHAR szRegCreateDate[ MAX_PATH ];			//	ע��ʱ��
	TCHAR szRegExpireDate[ MAX_PATH ];			//	����ʱ��

	BOOL  bBlockPost;					//	�Ƿ����ļ��ϴ����
	BOOL  bBlockPostSize;					//	�Ƿ����ļ��ϴ���С
	DWORD dwBlockPostSizeCfg;				//	����ļ��ϴ���С���ã���λ�ǣ����ֽ�

	LPTSTR lpszInBlockAlert_Html;				//	html in block alert �ļ��ڴ�ӳ��
	LPTSTR lpszInBlockAlert_ResHtml;			//	ϵͳ��Դ�Դ��� html in block alert �ļ��ڴ�ӳ��
	LPTSTR lpszInBlockAlert_UserHtml;			//	�û�ָ���� html in block alert �ļ��ڴ�ӳ��

	LPTSTR lpszInAntiSqlAlert_Html;				//	html in block sql alert �ļ��ڴ�ӳ��
	LPTSTR lpszInAntiSqlAlert_ResHtml;			//	ϵͳ��Դ�Դ��� html in block alert �ļ��ڴ�ӳ��
	LPTSTR lpszInAntiSqlAlert_UserHtml;			//	�û�ָ���� html in block alert �ļ��ڴ�ӳ��

	DWORD dwEcpHostWithPatternCount;			//	��ͨ����ŵ����������ĸ���
	STVWINFOMCONFIGECPHOST * pstEcpHostWithPattern;		//	ָ��һ��������������

	DWORD dwAntiSqlWordsCount;				//	Σ�� SQL ��䣨���ܴ���ͨ���������
	STVWINFOMWORD * pstAntiSqlWords;			//	Σ�� SQL ��䣨���ܴ���ͨ���������

} STVWINFOMCONFIG, *LPSTVWINFOMCONFIG;

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




/**
 *	class of CVwInfoMonitorConfigFile
 */
class CVwInfoMonitorConfigFile :
	public CModuleInfo
{
public:
	CVwInfoMonitorConfigFile();
	virtual ~CVwInfoMonitorConfigFile();

public:
	BOOL LoadConfig( STVWINFOMCONFIG * pstConfig );
	UINT LoadConfigUInt( LPCTSTR lpcszDomain, LPCTSTR lpcszKey );
	BOOL LoadConfigString( LPCTSTR lpcszDomain, LPCTSTR lpcszKey, LPTSTR lpszString, DWORD dwSize );
	BOOL SaveConfig( STVWINFOMCONFIG * pstConfig );
	BOOL SaveConfig( LPCTSTR lpcszDomain, LPCTSTR lpcszKey, LPCTSTR lpcszValue );

	BOOL LoadInBlockWords( vector<STVWINFOMWORD> & vcWordList, UINT * puWordMaxLen );
	BOOL SaveInBlockWords( vector<STVWINFOMWORD> & vcWordList, UINT uWordMaxLen );
	BOOL LoadInBlockWords_escape( vector<STVWINFOMWORD> & vcWordList, UINT * puWordMaxLen );
	BOOL SaveInBlockWords_escape( vector<STVWINFOMWORD> & vcWordList, UINT uWordMaxLen );
	BOOL LoadInBlockWords_utf8( vector<STVWINFOMWORD> & vcWordList, UINT * puWordMaxLen );
	BOOL SaveInBlockWords_utf8( vector<STVWINFOMWORD> & vcWordList, UINT uWordMaxLen );

	BOOL LoadInAntiSqlWords( vector<STVWINFOMWORD> & vcWordList, UINT * puWordMaxLen );
	BOOL SaveInAntiSqlWords( vector<STVWINFOMWORD> & vcWordList, UINT uWordMaxLen );


	BOOL LoadOutRepWords( vector<STVWINFOMWORD> & vcWordList, UINT * puWordMaxLen );
	BOOL SaveOutRepWords( vector<STVWINFOMWORD> & vcWordList, UINT uWordMaxLen );

	BOOL LoadOutRepWords_utf8( vector<STVWINFOMWORD> & vcWordList, UINT * puWordMaxLen );
	BOOL SaveOutRepWords_utf8( vector<STVWINFOMWORD> & vcWordList, UINT uWordMaxLen );


	BOOL LoadExceptionHosts( vector<STVWINFOMWORD> & vcWordList, UINT * puWordMaxLen );
	BOOL SaveExceptionHosts( vector<STVWINFOMWORD> & vcWordList, UINT uWordMaxLen );

	BOOL LoadWordsFromFile( LPCTSTR lpcszFilename, UINT uMaxWordLoadCount, vector<STVWINFOMWORD> & vcWordList, UINT * puWordMaxLen );
	BOOL LoadWildcardWordsFromFile( LPCTSTR lpcszFilename, DWORD dwMaxWordLen, BOOL bOnlyPatternWords, vector<STVWINFOMWORD> & vcWordList );
	BOOL SaveWordsFromFile( LPCTSTR lpcszFilename, vector<STVWINFOMWORD> & vcWordList, UINT uWordMaxLen );

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



#endif	//	__VWINFOMONITORCONFIGFILE_HEADER__


