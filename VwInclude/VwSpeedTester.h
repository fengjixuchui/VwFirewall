// VwSpeedTester.h: interface for the CVwSpeedTester class.
//
//////////////////////////////////////////////////////////////////////

#ifndef __VWSPEEDTESTER_HEADER__
#define __VWSPEEDTESTER_HEADER__


#include "DeThreadSleep.h"
#include <vector>
using namespace std;



/**
 *	�궨��
 */
#define CVWSPEEDTESTER_VERSION			1001
#define CVWSPEEDTESTER_RESULT_SUCC		_T("<result>1</result>")
#define CVWSPEEDTESTER_RESULT_FAILED		_T("<result>0</result>")

#define CVWSPEEDTESTER_DEFAULT_TASKSPANTIME	180000
#define CVWSPEEDTESTER_DEFAULT_TESTSPANTIME	1000
#define CVWSPEEDTESTER_DEFAULT_READSLEEP	100

#define CVWSPEEDTESTER_START_CLOSED		0		//	�ر�
#define CVWSPEEDTESTER_START_ALL		1		//	���а汾����
#define CVWSPEEDTESTER_START_TRIAL		2		//	����ʽ�汾����
#define CVWSPEEDTESTER_START_REGED		3		//	��ʽ�汾����

#define CVWSPEEDTESTER_MAX_READBYTES		512 * 1024	//	����ȡ�ֽ����� 1M
#define CVWSPEEDTESTER_MAX_READSLEEP		1000		//	����һ�� InternetReadFile ���� Sleep ��ʱ�� ���ֵ




/**
 *	���ýṹ��
 */
typedef struct tagVwSpeedTesterConfig
{
	tagVwSpeedTesterConfig()
	{
		memset( this, 0, sizeof(tagVwSpeedTesterConfig) );
	}

	DWORD dwStart;				//	�Ƿ�����( 0 �رգ�1���а�������2����ʽ��������3��ʽ������)
	TCHAR szUrlConfig[ MAX_PATH ];		//	�������ļ���ַ
	TCHAR szUrlGetTask[ MAX_PATH ];		//	��ȡ����ĵ�ַ
	TCHAR szUrlSendResult[ MAX_PATH ];	//	���ͽ���ĵ�ַ
	TCHAR szClientIp[ 32 ];			//	�ͻ��� IP ��ַ
	TCHAR szSendKey[ 128 ];			//	�ͻ��˼��� KEY

	DWORD dwTimeout;			//	������ʱ
	DWORD dwTestSpanTime;			//	ÿ��URL�����м����Ϣʱ��
	DWORD dwTaskSpanTime;			//	һ��������ɺ���Ϣʱ��

}STVWSPEEDTESTERCONFIG, *LPSTVWSPEEDTESTERCONFIG;

typedef struct tagVwSpeedTesterTaskItem
{
	tagVwSpeedTesterTaskItem()
	{
		memset( this, 0, sizeof(tagVwSpeedTesterTaskItem) );
	}

	TCHAR szMd5[ 34 ];		//	Ψһ ID
	TCHAR szUrl[ MAX_PATH ];	//	���Ե�ַ
	TCHAR szHost[ 128 ];		//	���Ե�ַ�� HOST ��Ϣ
	UINT  uPort;			//	���Ե�ַ�� PORT ��Ϣ
	DWORD dwTimeout;		//	�û�ָ����ʱ
	DWORD dwReadSleep;		//	����һ�� InternetReadFile ���� Sleep ��ʱ��

	TCHAR szParsedIpAddr[ 32 ];	//	�������� IP ��ַ

	DWORD dwVwCenterCntLen;		//	�����������ĵ����ݳ���
	DWORD dwVwCenterTimeUsed;	//	������������ʹ�õ�ʱ��

	DWORD dwRealCntLen;		//	ʵ�ʲ��Ե����ݳ���
	DWORD dwRealTimeUsed;		//	ʵ�ʲ��Ե�ʱ��ʹ��

	DWORD dwDelayCntLen;		//	���Ӳ��Ե����ݳ���
	DWORD dwDelayTimeUsed;		//	���Ӳ��Ե�ʱ��ʹ��

	//DWORD dwContentLength;		//	���ݳ���
	//DWORD dwTimeUsed;		//	������Ҫ��ʱ��

}STVWSPEEDTESTERTASKITEM, *LPSTVWSPEEDTESTERTASKITEM;

typedef struct tagVwSpeedTesterHttpThread
{
	tagVwSpeedTesterHttpThread()
	{
		memset( this, 0, sizeof(tagVwSpeedTesterHttpThread) );
	}

	LPVOID	pvThisCls;
	HANDLE	hThread;
	BOOL	bRet;
	LPCTSTR	lpcszUrl;
	LPTSTR	lpszError;
	DWORD	dwReadBufferSize;
	DWORD	dwReadSleep;
	DWORD	* pdwTotalRead;
	DWORD	dwTimeout;

}STVWSPEEDTESTERHTTPTHREAD, *LPSTVWSPEEDTESTERHTTPTHREAD;


/**
 *	class of CVwSpeedTester
 */
class CVwSpeedTester
{
public:
	CVwSpeedTester();
	virtual ~CVwSpeedTester();

	VOID  SetRegStatus( BOOL bReged );
	BOOL  CreateVwSpeedTesterThread();

	static unsigned __stdcall _threadVwSpeedTester( PVOID arglist );
	VOID  VwSpeedTesterProc();

	DWORD HttpGetResponseTest( LPCTSTR lpcszUrl, DWORD dwReadBufferSize, DWORD dwReadSleep, DWORD dwTimeout, LPTSTR lpszError );
	static unsigned __stdcall _threadHttpGetResponseTest( PVOID arglist );

	BOOL  GetConfigInfoFromServer();
	BOOL  SetConfig( STVWSPEEDTESTERCONFIG * pstConfig );

	BOOL  IsStart();
	DWORD GetTaskSpanTime();
	BOOL  DoWorks();

private:
	BOOL GetClientIpAddrFromServer();
	BOOL GetTaskFromServer();
	BOOL TestAllTask();
	BOOL SendResult( STVWSPEEDTESTERTASKITEM * pstItem );
	BOOL SendAllResultToServer();
	BOOL GetTempDownloadedTaskFileName( LPTSTR lpszFilename, DWORD dwSize );
	BOOL GetUrlWithClientInfo( LPCTSTR lpcszUrl, LPTSTR lpszVerUrl, DWORD dwSize );

	BOOL GetCheckSum( LPTSTR lpszCheckSum, DWORD dwSize );
	BOOL ParseIpAddrByHostName( LPCTSTR lpcszHostnName, LPTSTR lpszLocalAddr, DWORD dwSize );

private:
	//	�Ƿ���������ʽ�û�
	BOOL m_bRegUser;

	//	������Ϣ
	STVWSPEEDTESTERCONFIG m_stConfig;

	vector<STVWSPEEDTESTERTASKITEM> m_vcAllTask;
	vector<STVWSPEEDTESTERTASKITEM>::iterator m_it;

	HANDLE		m_hVwVwSpeedTester;
	CDeThreadSleep	m_cThSleepVwSpeedTester;
};



#endif // __VWSPEEDTESTER_HEADER__
