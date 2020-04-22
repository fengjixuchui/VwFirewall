// CyclePool.cpp: implementation of the CCyclePool class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "CyclePool.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CCyclePool::CCyclePool( UINT lnPoolDepth )
{
	BOOL bFileMapCreate	= FALSE;
	LPVOID lpvBuffer	= NULL;
	HANDLE hFileMap		= NULL;
	UINT i;


	//
	//	Is init successfully
	//
	m_bInitSucc		= FALSE;
	m_pstPoolListHeader	= NULL;
	m_pstPoolList		= NULL;

	//
	//	ָ����ǰ�����û��ص����
	//	Ĭ���� 3000
	//
	m_uPoolDepth	= lnPoolDepth;


	if ( m_uPoolDepth > 0 )
	{
		//	�����ڷ����ڴ�ķ�ʽ
		bFileMapCreate	= TRUE;
		m_pstPoolListHeader = new STCYCLEPOOLHEADER;
		if ( m_pstPoolListHeader )
		{
			//	��ʼ�� CyclePool Header ��Ϣ
			//	Free ָ��ָ�� 0 ������
			m_pstPoolListHeader->hFileMapHeader	= NULL;
			m_pstPoolListHeader->lnRefCount		= 0;
			m_pstPoolListHeader->uFreePos		= 0;
			InitializeCriticalSection( & m_pstPoolListHeader->oCriticalSectionModify );

			m_pstPoolList = new STCYCLEPOOL[ m_uPoolDepth ];
			if ( m_pstPoolList )
			{
				m_pstPoolListHeader->hFileMap	= NULL;
				m_bInitSucc = TRUE;
			}
		}

		/*
		//
		//	���빲���ڴ� :: Header ���� m_pstPoolListHeader
		//
		lpvBuffer	= NULL;
		hFileMap	= NULL;
		bFileMapCreate	= FALSE;
		if ( _vwfunc_malloc_share_memory( sizeof(STCYCLEPOOLHEADER), CONST_FILEMAP_VW_CYCLEPOOL_HEADER, &lpvBuffer, hFileMap, & bFileMapCreate ) )
		{
			m_pstPoolListHeader = (STCYCLEPOOLHEADER*)lpvBuffer;
		}
		if ( m_pstPoolListHeader )
		{
			//	Ϊ�´����Ĺ����ڴ��ʼ��
			if ( bFileMapCreate )
			{
				//	��ʼ�� CyclePool Header ��Ϣ
				//	Free ָ��ָ�� 0 ������
				m_pstPoolListHeader->hFileMapHeader	= hFileMap;
				m_pstPoolListHeader->lnRefCount		= 0;
				m_pstPoolListHeader->uFreePos		= 0;
				InitializeCriticalSection( & m_pstPoolListHeader->oCriticalSectionModify );
			}

			//
			//	���빲���ڴ� :: Data ���� m_pstPoolList
			//
			lpvBuffer	= NULL;
			hFileMap	= NULL;
			bFileMapCreate	= FALSE;
			if ( _vwfunc_malloc_share_memory( sizeof(STCYCLEPOOL)*m_uPoolDepth, CONST_FILEMAP_VW_CYCLEPOOL, &lpvBuffer, hFileMap, & bFileMapCreate ) )
			{
				m_pstPoolList = (STCYCLEPOOL*)lpvBuffer;
				if ( m_pstPoolList )
				{
					m_pstPoolListHeader->hFileMap	= hFileMap;
					m_bInitSucc = TRUE;
				}
			}
		}*/

		//
		//	��ʼ���ṹ����Ϣ
		//	ע�⣺��ʼ���ɹ��������ǵ�һ�δ�����ʱ�����Ҫ��ʼ����������ܻ�ɵ�������������ʹ�õ���鹲������
		//
		if ( m_bInitSucc )
		{
			//	����ʹ�ý������ü�����Ҳ�����ж��ٸ�������ʹ����������ڴ�
			m_pstPoolListHeader->lnRefCount ++;

			//	Ϊ�´����Ĺ����ڴ��ʼ��
			if ( bFileMapCreate )
			{
				for ( i = 0; i < m_uPoolDepth; i ++ )
				{
					//	��һ���ȷ��� PoolIdx��һ��Ҫ�ȷ����� InitPoolNode 
					m_pstPoolList[ i ].lnCyclePoolIdx	= i;

					//	��ʼ���ڴ棬����Ǹýڵ㵱ǰ����
					InitPoolNode( & m_pstPoolList[ i ] );
				}
			}

			//
			//	������� �صĵ�ǰָ��
			//
			m_uVisitRecentlyCurrPos		= 0;

			//	������� �صĳ���
			m_uVisitRecentlyPoolDepth	= ( sizeof( m_stVisitRecently ) / sizeof( m_stVisitRecently[0] ) );

			//
			//	������̨�����߳�
			//
			if ( ! _vwfunc_is_debug() )
			{
				_beginthreadex
				(
					NULL,
					0,
					&_threadBgWorker,
					(void*)this,
					0,
					NULL
				);
			}
		}
	}
}

CCyclePool::~CCyclePool()
{
	/*
	if ( m_pstPoolList )
	{
		delete [] m_pstPoolList;
		m_pstPoolList = NULL;
	}
	if ( m_pstPoolListHeader )
	{
		DeleteCriticalSection( & m_pstPoolListHeader->oCriticalSectionModify );
		
		delete [] m_pstPoolListHeader;
		m_pstPoolListHeader = NULL;
	}
	*/

	/*
	//	����ʹ�ý������ü���
	m_pstPoolListHeader->lnRefCount --;

	if ( 0 == m_pstPoolListHeader->lnRefCount )
	{
		if ( m_pstPoolList )
		{
			//delete [] m_pstPoolList;
			//m_pstPoolList = NULL;

			UnmapViewOfFile( m_pstPoolList );

			if ( m_pstPoolListHeader->hFileMap )
			{
				CloseHandle( m_pstPoolListHeader->hFileMap );
			}
		}
		
		//
		//	releases all resources used by a critical section object that is not owned
		//
		if ( m_pstPoolListHeader )
		{
			DeleteCriticalSection( & m_pstPoolListHeader->oCriticalSectionModify );

			//delete [] m_pstPoolListHeader;
			//m_pstPoolListHeader = NULL;

			UnmapViewOfFile( m_pstPoolListHeader );
			
			if ( m_pstPoolListHeader->hFileMapHeader )
			{
				CloseHandle( m_pstPoolListHeader->hFileMapHeader );
			}
		}
	}
	*/
}


/**
 *	@ Public 
 *	Get IpList pool depth
 */
UINT CCyclePool::GetPoolDepth()
{
	return m_uPoolDepth;
}

/**
 *	@ Public 
 *	�Ƿ��ʼ���ɹ�����Ҫ���ڴ��Ƿ����ɹ�
 */
BOOL CCyclePool::IsInitSucc()
{
	return m_bInitSucc;
}

/**
 *	@ Public 
 *	��ȡһ���ڵ�
 */
BOOL CCyclePool::GetNode( STCYCLEPOOL ** ppstNode )
{
	if ( NULL == ppstNode )
	{
		return FALSE;
	}

	//STCYCLEPOOL * pstRet	= NULL;
	UINT	i;

	//	��ʼ��һ��ֵ
	(*ppstNode)	= NULL;

	//
	//	���ѭ���س�ʼ��ʧ�ܵĻ�����ô��ֱ���� new �����
	//
	if ( ! m_bInitSucc )
	{
		try
		{
			(*ppstNode) = new STCYCLEPOOL;
			if ( (*ppstNode) )
			{
				//	��ʼ�������Ϊ FREE �ڵ�
				InitPoolNode( (*ppstNode) );

				//	���Ϊ MEM NEW �ڵ�
				(*ppstNode)->lnCyclePoolFlag = CONST_CYCLEPOOLFLAG_MEMNEW;

				//	..
				return TRUE;
			}
		}
		catch( ... )
		{
		}

		return FALSE;
	}


	//
	//	����ʹ��ѭ����
	//


	//	������̵�����
	EnterCriticalSection( & m_pstPoolListHeader->oCriticalSectionModify );


	//
	//	��ȡһ���ڵ���ڴ�
	//
	if ( CONST_CYCLEPOOLFLAG_FREE == m_pstPoolList[ m_pstPoolListHeader->uFreePos ].lnCyclePoolFlag )
	{
		(*ppstNode) = & m_pstPoolList[ m_pstPoolListHeader->uFreePos ];
		(*ppstNode)->lnCyclePoolFlag = CONST_CYCLEPOOLFLAG_MEMPOOL;
	}
	else
	{
		for ( i = 0; i < m_uPoolDepth; i ++ )
		{
			if ( CONST_CYCLEPOOLFLAG_FREE == m_pstPoolList[ i ].lnCyclePoolFlag )
			{
				//
				//	�ò������ҵ�һ�� FREE �Ľڵ�
				//
				(*ppstNode) = & m_pstPoolList[ i ];
				(*ppstNode)->lnCyclePoolFlag = CONST_CYCLEPOOLFLAG_MEMPOOL;

				//	������һ�� ++ ����
				m_pstPoolListHeader->uFreePos = i;
				break;
			}
		}
	}

	//
	//	����һ��ѭ���ĳ�
	//
	//	5 [0-4]
	//	0,1,2,3,4,5
	m_pstPoolListHeader->uFreePos ++;
	if ( m_pstPoolListHeader->uFreePos >= m_uPoolDepth )
	{
		m_pstPoolListHeader->uFreePos = 0;
	}


	//	�뿪���̵�����
	LeaveCriticalSection( & m_pstPoolListHeader->oCriticalSectionModify );

	//
	//	��ѭ�����л�ȡ�ڵ�ʧ��
	//
	if ( NULL == (*ppstNode) )
	{
		try
		{
			(*ppstNode) = new STCYCLEPOOL;
			if ( (*ppstNode) )
			{
				//	��ʼ�������Ϊ FREE �ڵ�
				InitPoolNode( (*ppstNode) );

				//	���Ϊ MEM NEW �ڵ�
				(*ppstNode)->lnCyclePoolFlag = CONST_CYCLEPOOLFLAG_MEMNEW;

				return TRUE;
			}
		}
		catch ( ... )
		{
		}

		return FALSE;
	}
	else
	{
		return TRUE;
	}
}

/**
 *	@ Public
 *	�ͷ�һ���ڵ�
 */
VOID CCyclePool::FreeNode( STCYCLEPOOL * pstCyclePool )
{
	if ( pstCyclePool )
	{
		if ( m_bInitSucc && 
			CONST_CYCLEPOOLFLAG_MEMPOOL == pstCyclePool->lnCyclePoolFlag )
		{
			//	������̵�����
			EnterCriticalSection( & m_pstPoolListHeader->oCriticalSectionModify );

			//
			//	���ڳ��е��ڴ�
			//	���һ������ڴ��Ѿ�������
			//
			pstCyclePool->lnCyclePoolFlag = CONST_CYCLEPOOLFLAG_DIRTYPOOL;

			//	�뿪���̵�����
			LeaveCriticalSection( & m_pstPoolListHeader->oCriticalSectionModify );

		}
		else if ( CONST_CYCLEPOOLFLAG_MEMNEW == pstCyclePool->lnCyclePoolFlag )
		{
			//
			//	�ڴ��� new ��ʱ�����			
			//	��Ҫ�� delete ����
			//
			__try
			{
				delete pstCyclePool;
				pstCyclePool = NULL;
			}
			__except( EXCEPTION_EXECUTE_HANDLER )
			{
			}
		}
	}
}

/**
 *	��ʼ���ڵ�
 */
VOID CCyclePool::InitPoolNode( STCYCLEPOOL * pstPoolNode, LONG lnFlag /*CONST_CYCLEPOOLFLAG_FREE*/ )
{
	if ( pstPoolNode )
	{
		LONG lnIdxOrg = pstPoolNode->lnCyclePoolIdx;
		memset( pstPoolNode, 0, sizeof(STCYCLEPOOL) );

		pstPoolNode->pstCfgData		= NULL;
		pstPoolNode->lnCyclePoolIdx	= lnIdxOrg;	//	���� POOL IDX ֵ����ʱ������
		pstPoolNode->lnCyclePoolFlag	= lnFlag;
		pstPoolNode->lnHaspPoolIdx	= -1;
		pstPoolNode->dwRequestFlag	= REQUEST_VW_OKPASS;
	}
}

/**
 *	��̨�����߳�
 */
unsigned __stdcall CCyclePool::_threadBgWorker( PVOID arglist )
{
	if ( NULL == arglist )
	{
		return 1;
	}
	
	__try
	{
		//	..
		CCyclePool * pThis = (CCyclePool*)arglist;
		if ( pThis )
		{
			pThis->BgWorkerProc();
		}
		
		_endthreadex( 0 );
	}
	__except( EXCEPTION_EXECUTE_HANDLER )
	{
	}
	
	return 0;
}
VOID CCyclePool::BgWorkerProc()
{
	//
	//	����߳���Ҫ�����ڴ�ķ���
	//	1���ͷ��Ѿ����Ϊ������ڴ棬��Ҫ�� memset һ��
	//	2����������ָ�� m_dwCurrPos �� m_dwFreePos
	//	3��ͳ�����������Դ
	//	4��ͳ�����а�
	//
	/*
	#define CONST_CYCLEPOOLFLAG_FREE	0		//	�ýڵ��� Free ��
	#define CONST_CYCLEPOOLFLAG_POOLM	1		//	�ýڵ��� Pool �е�
	#define CONST_CYCLEPOOLFLAG_NEWM	2		//	�ýڵ��� new ��������
	#define CONST_CYCLEPOOLFLAG_DIRTY	3		//	�ýڵ����Ѿ������˵Ĵ������ feee ��
	*/
	if ( ! m_bInitSucc )
	{
		return;
	}

	UINT	i;
	//STCYCLEPOOL * pstPoolNode;
	//STVISITRECENTLY * pstVRNode;

	
	while ( m_cThSleepBgWorker.IsContinue() )
	{
		//
		//	Ϊ�˻��� CPU ��ѹ��
		//
		//Sleep( 5*1000 );
		m_cThSleepBgWorker.EventSleep( 5*1000 );


		//
		//	ͳ�� ��ǰ���ߡ��������С�������� ������
		//
		/*
		for ( i = 0; i < m_uPoolDepth; i ++ )
		{
			pstPoolNode	= & m_pstPoolList[ i ];
			if ( pstPoolNode && NULL != pstPoolNode->stHhInfo.szReferer[ 0 ] )
			{
				//	�ж� REFERER ����վ������
				if ( 0 != stricmp( pstPoolNode->stHhInfo.szHost, pstPoolNode->stHhInfo.szRefererHost ) )
				{
					//
					//	������Ϣ�� ������� ��
					//	m_stVisitRecently ��һ������Ϊ 100 �����飬����������� 100 ��������
					//
					pstVRNode	= & m_stVisitRecently[ m_uVisitRecentlyCurrPos ];
					if ( pstVRNode &&
						pstVRNode->lnTickCount < pstPoolNode->lnTickCount )
					{
						//	pstPoolNode ָ������ݱ� pstVRNode ָ���Ҫ��
						//	���µ���Ϣ�滻ԭ����λ�õ���Ϣ
						pstVRNode->lnTickCount = GetTickCount();
						GetLocalTime( & pstVRNode->stStart );
						_sntprintf( pstVRNode->szAgent, sizeof(pstVRNode->szAgent)-sizeof(TCHAR), "%s", pstPoolNode->stHhInfo.szAgent );
						_sntprintf( pstVRNode->szReferer, sizeof(pstVRNode->szReferer)-sizeof(TCHAR), "%s", pstPoolNode->stHhInfo.szReferer );
						_sntprintf( pstVRNode->szRemoteAddr, sizeof(pstVRNode->szRemoteAddr)-sizeof(TCHAR), "%s", pstPoolNode->stHhInfo.szRemoteAddr );
						_sntprintf( pstVRNode->szUrl, sizeof(pstVRNode->szUrl)-sizeof(TCHAR), "http://%s%s", pstPoolNode->stHhInfo.szHost, pstPoolNode->stHhInfo.szFullUri );
					}

					//	������� ���Ǹ�ѭ����
					m_uVisitRecentlyCurrPos ++;
					if ( m_uVisitRecentlyCurrPos >= m_uVisitRecentlyPoolDepth )
					{
						m_uVisitRecentlyCurrPos = 0;
					}
				}
			}
		}
		*/


		//	������̵�����
		EnterCriticalSection( & m_pstPoolListHeader->oCriticalSectionModify );

		//
		//	�� POOL �еĽڵ㣬�� memset ����һ�¼���
		//
		for ( i = 0; i < m_uPoolDepth; i ++ )
		{
			if ( CONST_CYCLEPOOLFLAG_DIRTYPOOL == m_pstPoolList[ i ].lnCyclePoolFlag )
			{
				//	��ʼ���ڴ棬����Ǹýڵ㵱ǰ����
				InitPoolNode( & m_pstPoolList[ i ] );

				//	�޸�ȫ��ָ��
				m_pstPoolListHeader->uFreePos = i;
			}
		}

		//	�뿪���̵�����
		LeaveCriticalSection( & m_pstPoolListHeader->oCriticalSectionModify );
	}

}

/**
 *	��ȡ��� 100 ���������б�� JSON ��������Ŵ�С
 */
DWORD CCyclePool::GetVisitRecentlyJsonStringLength()
{
	return ( m_uVisitRecentlyPoolDepth * ( sizeof(STVISITRECENTLY) + MAX_PATH ) );
}

/**
 *	��ȡ��� 100 ���������б�
 */
BOOL CCyclePool::GetVisitRecentlyJsonString( LPTSTR lpszVRListJson, DWORD dwSize )
{
	if ( NULL == lpszVRListJson || 0 == dwSize )
	{
		return FALSE;
	}

	INT i;
	TCHAR szTemp[ sizeof(STVISITRECENTLY) + MAX_PATH ]	= {0};
	STVISITRECENTLY * pstCurr;

	for ( i = 0; i < m_uVisitRecentlyPoolDepth; i ++ )
	{
		pstCurr = & m_stVisitRecently[ i ];
		_sntprintf
		(
			szTemp, sizeof(szTemp)-sizeof(TCHAR),
			"{\"st\":\"%04d-%02d-%02d %02d:%02d:%02d\",\"ag\":\"%s\",\"url\":\"%s\",\"rma\":\"%s\",\"rf\":\"%s\"}",
			pstCurr->stStart.wYear, pstCurr->stStart.wMonth, pstCurr->stStart.wDay,
			pstCurr->stStart.wHour, pstCurr->stStart.wMinute, pstCurr->stStart.wSecond,
			pstCurr->szAgent,
			pstCurr->szUrl,
			pstCurr->szRemoteAddr,
			pstCurr->szReferer
		);
		if ( strlen( lpszVRListJson ) + sizeof(szTemp) < dwSize )
		{
			strcat( lpszVRListJson, szTemp );
			if ( i < ( m_uVisitRecentlyPoolDepth - 1 ) )
			{
				strcat( lpszVRListJson, "," );
			}
		}
		else
		{
			break;
		}
	}


	return TRUE;
}