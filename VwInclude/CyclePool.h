// CyclePool.h: interface for the CCyclePool class.
//
//////////////////////////////////////////////////////////////////////

#ifndef __CYCLEPOOL_HEADER__
#define __CYCLEPOOL_HEADER__

#include "VwStruct.h"
#include "DeThreadSleep.h"

/**
 *	const define
 */
const UINT  CONST_MAX_CYCLEPOOL_DEPTH	= 2000;	//	��������ޣ�����ѡ������
							//	2344 * 3000  = 6.7 MB
							//	2344 * 3989 = 8.9 MB

static const CHAR * CONST_FILEMAP_VW_CYCLEPOOL_HEADER	= "FILEMAP_VW_CYCLEPOOL_HEADER";
static const CHAR * CONST_FILEMAP_VW_CYCLEPOOL		= "FILEMAP_VW_CYCLEPOOL";


/**
 *	CyclePool Header �ṹ��
 */
typedef struct tagCyclePoolHeader
{
	tagCyclePoolHeader()
	{
		memset( this, 0, sizeof(tagCyclePoolHeader) );
	}

	HANDLE hFileMapHeader;	//	ͷ�����ڴ���ļ����
	HANDLE hFileMap;	//	���ݹ����ڴ���ļ����
	LONG lnRefCount;		//	ʹ�ý������ü���
	UINT uFreePos;		//	FreePos index of pool
	CRITICAL_SECTION oCriticalSectionModify;

}STCYCLEPOOLHEADER, *PSTCYCLEPOOLHEADER;

/**
 *	CyclePool Data �ṹ��
 *	[�ṹ�� STCYCLEPOOL ˵��]
 *	1��������һ����Ա������¼��ǰ�Լ�������
 *	2��������һ����Ա������¼��ǰ�Լ������ͣ���ѭ�����л����Ȳ�������ʱ new ������
 */
typedef STMYHTTPFILTERCONTEXT	STCYCLEPOOL;

/**
 *	��������ߣ��ýṹ������Ĭ���� 50 ������
 */
typedef struct tagVisitRecently
{
	tagVisitRecently()
	{
		memset( this, 0, sizeof(tagVisitRecently) );
	}

	LONG		lnTickCount;
	SYSTEMTIME	stStart;			//	����ʱ��
	TCHAR		szAgent[ 128 ];			//	Agent
	TCHAR		szUrl[ MAX_PATH ];		//	Url
	TCHAR		szRemoteAddr[ 16 ];		//	������ IP ��ַ
	TCHAR		szReferer[ MAX_PATH ];		//	��������������

}STVISITRECENTLY, *PSTVISITRECENTLY;



/**
 *	class of CCyclePool
 */
class CCyclePool
{
public:
	CCyclePool( UINT lnPoolDepth = CONST_MAX_CYCLEPOOL_DEPTH );
	virtual ~CCyclePool();

	//	Get IpList pool depth
	UINT GetPoolDepth();

	//	�Ƿ��ʼ���ɹ�
	BOOL IsInitSucc();

	//	��ȡһ���ڵ�
	BOOL GetNode( STCYCLEPOOL ** ppstNode );

	//	�ͷ�һ���ڵ�
	VOID FreeNode( STCYCLEPOOL * pstCyclePool );

	//	��̨�����߳�
	static unsigned __stdcall _threadBgWorker( PVOID arglist );
	VOID BgWorkerProc();

	//	��ȡ��� 100 ���������б�� JSON ��������Ŵ�С
	DWORD GetVisitRecentlyJsonStringLength();

	//	��ȡ��� 100 ���������б�
	BOOL GetVisitRecentlyJsonString( LPTSTR lpszVRListJson, DWORD dwSize );


private:
	VOID InitPoolNode( STCYCLEPOOL * pstPoolNode, LONG lnFlag = CONST_CYCLEPOOLFLAG_FREE );

public:
	//	..
	STCYCLEPOOLHEADER * m_pstPoolListHeader;
	STCYCLEPOOL * m_pstPoolList;

	//	������ã�������ʱ��ʹ�ã������޸ĳ� 3
	STVISITRECENTLY	m_stVisitRecently[ 1 ];

	//	�����̵߳ȴ�
	CDeThreadSleep	m_cThSleepBgWorker;

private:

	//	ѭ���صĳ���
	UINT	m_uPoolDepth;

	//	��ʶ�Ƿ��ʼ���ɹ�
	BOOL	m_bInitSucc;
	
	//	���̵�
	//CRITICAL_SECTION m_CriticalSectionModify;
	//HANDLE  m_hFileMap;

	//	ָ��ǰ�ڵ�
	//UINT	m_uCurrPos;

	//	ָ��һ�� FREE �ڵ�
	//UINT	m_uFreePos;

	//	������� �صĵ�ǰָ��
	UINT	m_uVisitRecentlyCurrPos;

	//	������� �صĳ���
	UINT	m_uVisitRecentlyPoolDepth;

};





#endif // __CYCLEPOOL_HEADER__
