/*
 ************************************************************
 *
 *	TCyclePool.h
 *
 *	CLASS NAME:
 *		Desafe Ascii Code Hash Tree
 *		�ṩ�ڴ��ģ����
 *		* ֧�ֺ�̨�߳��Զ��ͷ�
 *
 *	AUTHOR:
 *		������ liuqixing@gmail.com
 *
 *	HISTORY:
 *		2008-10-15 ������ liuqixing@gmail.com ���
 *
 ************************************************************
 */

#ifndef __TCYCLEPOOL_HEADER__
#define __TCYCLEPOOL_HEADER__


#include "process.h"
#include "DeThreadSleep.h"


#include "DeLib.h"
#pragma comment( lib, "DeLib.lib" )



/**
 *	const define
 */


/**
 *	���� CyclePool ��ǳ���
 */
#define CTCYCLEPOOL_MAX_POOL_DEPTH	2000		//	�ص�Ĭ�����

#define CTCYCLEPOOL_POOLFLAG_FREE	0		//	�ýڵ��� Free ��
#define CTCYCLEPOOL_POOLFLAG_MEMPOOL	1		//	�ýڵ��� Pool �е�
#define CTCYCLEPOOL_POOLFLAG_MEMNEW	2		//	�ýڵ��� new ��������
#define CTCYCLEPOOL_POOLFLAG_DIRTYPOOL	3		//	�ýڵ����Ѿ������ˣ�POOL ���ʹ������ feee ��


/**
 *	�ṹ�嶨��
 */
typedef struct tagTCyclePool
{
	//
	//	����ṹ�岻�����ⲿ�� memset ֮��ĳ�ʼ������
	//
	tagTCyclePool()
	{
	}

	DWORD	dwFreeNodeCallTick;	//	���� FreeNode �� Tick
	LONG	lnPoolIdx;		//	ѭ�����е� index ֵ
	LONG	lnPoolFlag;		//	�Ƿ�����Ϊѭ����������ʱ new �����ģ�����ǵĻ�������Ҫ free

	LPVOID	lpvData;		//	�����ڴ�ָ��

	tagTCyclePool * pNext;		//	ָ����һ���ڵ㣬�Ӷ�����һ����״
	BOOL	bLastNode;		//	�Ƿ������һ���ڵ�

}STTCYCLEPOOL, *LPSTTCYCLEPOOL;



//////////////////////////////////////////////////////////////////////////
//	CTCyclePool

template< class _T, UINT t_uPoolDepth = CTCYCLEPOOL_MAX_POOL_DEPTH >
	class CTCyclePool
{
public:
	CTCyclePool()
	{
		InitializeCriticalSection( & m_oCrSec );
		m_cThSleepBgWorker.m_hThread	= NULL;

		//
		//	init
		//
		m_uPoolDepth	= 0;
		m_bInitSucc	= InitTemplate( t_uPoolDepth );

		if ( m_bInitSucc && m_cThSleepBgWorker.m_hThread )
		{
			ResumeThread( m_cThSleepBgWorker.m_hThread );
		}
	}
	~CTCyclePool()
	{
		DeleteCriticalSection( & m_oCrSec );

		//
		//	�ȴ��߳̽���
		//
		StopBgWorkerThread();
	}

	VOID Lock()
	{
		EnterCriticalSection( & m_oCrSec );
	}
	VOID Unlock()
	{
		LeaveCriticalSection( & m_oCrSec );
	}

	//	Get IpList pool depth
	UINT GetPoolDepth()
	{
		return m_uPoolDepth;
	}

	//	�Ƿ��ʼ���ɹ�
	BOOL IsInitSucc()
	{
		return m_bInitSucc;
	}

	//	��ϵͳ���������ڴ�
	BOOL GetNewNodeFromOS( STTCYCLEPOOL ** ppstNode )
	{
		if ( NULL == ppstNode )
		{
			return FALSE;
		}

		try
		{
			(*ppstNode) = new STTCYCLEPOOL;
			if ( (*ppstNode) )
			{
				(*ppstNode)->lpvData	= new _T;
				if ( (*ppstNode)->lpvData )
				{
					//	��ʼ�������Ϊ FREE �ڵ�
					InitPoolNode( (_T*)( (*ppstNode)->lpvData ) );

					//	���Ϊ MEM NEW �ڵ�
					(*ppstNode)->lnPoolFlag = CTCYCLEPOOL_POOLFLAG_MEMNEW;

					//	..
					return TRUE;
				}
			}
		}
		catch( ... )
		{
		}
		
		return FALSE;
	}

	BOOL GetNodeByPoolIdx( LONG lnPoolIdx, STTCYCLEPOOL ** ppstNode )
	{
		if ( lnPoolIdx < 0 || lnPoolIdx > m_uPoolDepth )
		{
			return FALSE;
		}
		if ( NULL == ppstNode )
		{
			return FALSE;
		}

		BOOL bRet;

		//	...
		bRet = FALSE;

		if ( m_pstPoolList )
		{
			bRet = TRUE;
			(*ppstNode) = & ( m_pstPoolList[ lnPoolIdx ] );
		}

		return bRet;
	}

	//	��ȡһ���ڵ�
	BOOL GetNode( STTCYCLEPOOL ** ppstNode )
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
			return GetNewNodeFromOS( ppstNode );
		}


		//
		//	����ʹ��ѭ����
		//


		//	������̵�����
		Lock();


		//
		//	��ȡһ���ڵ���ڴ�
		//
		if ( CTCYCLEPOOL_POOLFLAG_FREE == m_pstPoolListFreeNode->lnPoolFlag )
		{
			(*ppstNode) = m_pstPoolListFreeNode;
			(*ppstNode)->lnPoolFlag = CTCYCLEPOOL_POOLFLAG_MEMPOOL;
		}
		else
		{
			//
			//	�ӵ�ǰ FreeNode �ڵ㿪ʼתһȦ
			//
			for ( i = 0; i < m_uPoolDepth; i ++ )
			{
				m_pstPoolListFreeNode	= m_pstPoolListFreeNode->pNext;

				if ( CTCYCLEPOOL_POOLFLAG_FREE == m_pstPoolListFreeNode->lnPoolFlag )
				{
					//
					//	�ò������ҵ�һ�� FREE �Ľڵ�
					//
					(*ppstNode) = m_pstPoolListFreeNode;
					(*ppstNode)->lnPoolFlag = CTCYCLEPOOL_POOLFLAG_MEMPOOL;

					//	������һ�� ++ ����
					m_pstPoolListFreeNode	= m_pstPoolListFreeNode->pNext;
					break;
				}
			}
		}

		//	�뿪���̵�����
		Unlock();

		//
		//	��ѭ�����л�ȡ�ڵ�ʧ��
		//
		if ( NULL == (*ppstNode) )
		{
			return GetNewNodeFromOS( ppstNode );
		}
		else
		{
			return TRUE;
		}
	}
	
	//	�ͷ�һ���ڵ�
	VOID FreeNode( STTCYCLEPOOL * pstNode )
	{
		if ( pstNode )
		{
			if ( m_bInitSucc && CTCYCLEPOOL_POOLFLAG_MEMPOOL == pstNode->lnPoolFlag )
			{
				//	������̵�����
				Lock();

				//
				//	���ڳ��е��ڴ�
				//	���һ������ڴ��Ѿ�������
				//
				pstNode->dwFreeNodeCallTick = GetTickCount();
				pstNode->lnPoolFlag = CTCYCLEPOOL_POOLFLAG_DIRTYPOOL;

				//	�뿪���̵�����
				Unlock();
			}
			else if ( CTCYCLEPOOL_POOLFLAG_MEMNEW == pstNode->lnPoolFlag )
			{
				//
				//	�ڴ��� new ��ʱ�����			
				//	��Ҫ�� delete ����
				//
				__try
				{
					delete pstNode->lpvData;
					pstNode->lpvData = NULL;

					delete pstNode;
					pstNode = NULL;
				}
				__except( EXCEPTION_EXECUTE_HANDLER )
				{
				}
			}
		}
	}


private:

	//
	//	@ private
	//	��ʼ��ģ��
	//
	BOOL InitTemplate( UINT uPoolDepth )
	{
		if ( uPoolDepth < 3 )
		{
			return FALSE;
		}

		BOOL bRet	= FALSE;
		UINT i;

		//
		//
		//
		bRet			= FALSE;
		m_ptDataList		= NULL;
		m_pstPoolList		= NULL;

		//
		//	ָ����ǰ�����û��ص����
		//	Ĭ���� 2000
		//
		m_uPoolDepth	= uPoolDepth;

		//
		//	���������ݵ��ڴ�
		//
		try
		{
			m_ptDataList = new _T[ m_uPoolDepth ];
			if ( m_ptDataList )
			{
				//	��ʼ���ڴ�
				memset( m_ptDataList, 0, m_uPoolDepth*sizeof(_T) );

				//
				//	�������Ӧ�����Ĺ���ڵ�
				//
				m_pstPoolList = new STTCYCLEPOOL[ m_uPoolDepth ];
				if ( m_pstPoolList )
				{
					bRet	= TRUE;

					//	Ĭ�Ͻ� free node ָ���һ���ڵ�
					m_pstPoolListFreeNode	= m_pstPoolList;


					//	��ʼ���ڴ�
					memset( m_pstPoolList, 0, m_uPoolDepth*sizeof(STTCYCLEPOOL) );

					//
					//	�����ݽڵ� link ������ڵ�
					//
					for ( i = 0; i < m_uPoolDepth; i ++ )
					{
						//	���ݽڵ����ӵ�����ڵ���
						m_pstPoolList[ i ].lpvData	= &m_ptDataList[ i ];
						m_pstPoolList[ i ].lnPoolIdx	= i;

						//	���ɻ�
						if ( i < ( m_uPoolDepth - 1 ) )
						{
							//	ֱ��ָ����һ������
							m_pstPoolList[ i ].bLastNode	= FALSE;
							m_pstPoolList[ i ].pNext	= & m_pstPoolList[ i + 1 ];
						}
						else
						{
							//	�����һ���ڵ�� next ���ӵ���һ�����Ӷ����ɱպϵĻ�
							m_pstPoolList[ i ].bLastNode	= TRUE;
							m_pstPoolList[ i ].pNext	= & m_pstPoolList[ 0 ];
						}
					}

					//
					//	������̨�����߳�
					//
					//if ( ! delib_is_debug() )
					{
						CreateBgWorkerThread();
					}
				}
			}
		}
		catch ( ... )
		{
		}

		return bRet;
	}

	VOID InitPoolNode( _T * pstPoolNode, LONG lnFlag = CTCYCLEPOOL_POOLFLAG_FREE )
	{
		if ( pstPoolNode )
		{
			memset( pstPoolNode, 0, sizeof(_T) );
		}
	}

	//
	//	������̨�����߳�
	//
	VOID CreateBgWorkerThread()
	{
		m_cThSleepBgWorker.m_hThread = (HANDLE)_beginthreadex
			(
				NULL,
				0,
				&_threadBgWorker,
				(void*)this,
				CREATE_SUSPENDED,
				&m_cThSleepBgWorker.m_uThreadId
			);
	}

	//
	//	ֹͣ�߳�
	//
	VOID StopBgWorkerThread()
	{
		__try
		{
			m_cThSleepBgWorker.EndSleep();
			m_cThSleepBgWorker.EndThread( &m_cThSleepBgWorker.m_hThread );
		}
		__except( EXCEPTION_EXECUTE_HANDLER )
		{
		}
	}

	//
	//	��̨�����߳�
	//
	static unsigned __stdcall _threadBgWorker( PVOID arglist )
	{
		if ( NULL == arglist )
		{
			return 1;
		}
		
		__try
		{
			CTCyclePool * pThis = (CTCyclePool*)arglist;
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
	VOID BgWorkerProc()
	{
		//
		//	����߳���Ҫ�����ڴ�ķ���
		//	1���ͷ��Ѿ����Ϊ������ڴ棬��Ҫ�� memset һ�£������� lnPoolFlag = CTCYCLEPOOL_POOLFLAG_FREE
		//	2����������ָ�� m_dwCurrPos �� m_dwFreePos
		//

		STTCYCLEPOOL * pstNode;
		STMYHTTPFILTERCONTEXT * pMyHfc;

		while ( m_cThSleepBgWorker.IsContinue() )
		{
			//
			//	Ϊ�˻��� CPU ��ѹ��
			//
			m_cThSleepBgWorker.EventSleep( 5*1000 );

			if ( ! m_bInitSucc )
			{
				continue;
			}

			//	...
			Lock();

			//
			//	�� POOL �еĽڵ㣬�� memset ����һ�¼���
			//
			pstNode	= m_pstPoolList;
			while( pstNode )
			{
				//
				//	��������ڴ��ʼ��һ��
				//
				if ( CTCYCLEPOOL_POOLFLAG_DIRTYPOOL == pstNode->lnPoolFlag )
				{
					pMyHfc = (STMYHTTPFILTERCONTEXT*)pstNode->lpvData;
					if ( pMyHfc->bSentLastReport ||
						GetTickCount() - pMyHfc->stActionTime.dwTick > 3000 )
					{
						//
						//	�Ѿ����ͱ��棬���߳�ʱ 5 ���ͳͳ����
						//

						//TCHAR szTemp[ MAX_PATH ];
						//_sntprintf( szTemp, sizeof(szTemp)/sizeof(TCHAR)-1, "@@@Free Node[%d] memory!", pstNode->lnPoolIdx );
						//OutputDebugString( szTemp );

						//	zero memory
						memset( pstNode->lpvData, 0, sizeof(_T) );

						//	set flag to free
						pstNode->lnPoolFlag = CTCYCLEPOOL_POOLFLAG_FREE;
					}
				}

				//
				//	��������һ���ڵ㣬��ֱ������ѭ��
				//
				if ( pstNode->bLastNode )
				{
					break;
				}

				pstNode = pstNode->pNext;
			}

			//	...
			Unlock();
		}
	}


public:
	//	���ݳص�ָ��
	_T * m_ptDataList;

	//	����ص�ָ��
	STTCYCLEPOOL * m_pstPoolList;
	STTCYCLEPOOL * m_pstPoolListFreeNode;

	//	�����̵߳ȴ�
	CDeThreadSleep	m_cThSleepBgWorker;

private:
	//	ѭ���صĳ���
	UINT	m_uPoolDepth;

	//	��ʶ�Ƿ��ʼ���ɹ�
	BOOL	m_bInitSucc;

	UINT m_uFreePos;		//	FreePos index of pool
	CRITICAL_SECTION m_oCrSec;
};







#endif	// __TCYCLEPOOL_HEADER__


