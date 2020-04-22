// TProcCacheBase.h: interface for the CTDoubleBinTree class.
//
//////////////////////////////////////////////////////////////////////

#ifndef __TDOUBLEBINTREE_HEADER__
#define __TDOUBLEBINTREE_HEADER__

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000


//////////////////////////////////////////////////////////////////////////
//	CTDoubleBinTree

template< class _T >
	class CTDoubleBinTree
{
public:
	CTDoubleBinTree()
	{
		InitializeCriticalSection( & m_oCrSec );

		m_pstTree[ 0 ]	= NULL;
		m_pstTree[ 1 ]	= NULL;
		m_dwCurrent	= 0;
		m_dwCount[ 0 ]	= 0;
		m_dwCount[ 1 ]	= 0;

		m_bCleaning	= FALSE;
	}
	~CTDoubleBinTree()
	{
	}

	VOID Lock()
	{
		EnterCriticalSection( & m_oCrSec );
	}

	VOID Unlock()
	{
		LeaveCriticalSection( & m_oCrSec );
	}

	//	��ȡ�����ڵ�ָ��
	_T * GetTree()
	{
		return m_pstTree[ m_dwCurrent ];
	}

	//	���õ�һ���ڵ������
	VOID SetTreeHead( _T * pstNode )
	{
		m_pstTree[ m_dwCurrent ] = pstNode;
	}

	//	�����½ڵ�
	BOOL InsertNode( _T * pstLstNode, _T * pstNewNode, INT nCmp )
	{
		if ( NULL == pstNewNode )
		{
			return FALSE;
		}
		if ( 0 == nCmp )
		{
			return FALSE;
		}

		BOOL bRet	= FALSE;

		if ( NULL == GetTree() )
		{
			//	������һ�����
			SetTreeHead( pstNewNode );

			bRet = TRUE;
		}
		else
		{
			//	�����������
			if ( pstLstNode )
			{
				if ( nCmp > 0 )
				{
					pstLstNode->pLeft = pstNewNode;
				}
				else if ( nCmp < 0 )
				{
					pstLstNode->pRight = pstNewNode;
				}

				bRet = TRUE;
			}
		}

		if ( bRet )
		{
			//	���� ���ϵ�Ԫ�ظ���
			IncreaseCount();
		}

		return bRet;
	}

	//	�Ƿ�������������
	BOOL IsCleaning()
	{
		return m_bCleaning;
	}

	//	��ȡԪ�ظ���
	DWORD GetCount()
	{
		return m_dwCount[ m_dwCurrent ];
	}

	//	����Ԫ�ظ���
	VOID IncreaseCount( DWORD dwStep = 1 )
	{
		//	���ϵ�Ԫ�ظ��� ++
		m_dwCount[ m_dwCurrent ] += dwStep;
	}

	//
	//	������ for ProtRes
	//
	//	��������:
	//	1�������µ���ָ�뱣�浽��ʱ������
	//	2��������ָ������Ϊ NULL ���Ҽ���������
	//	3����������������
	//	4����һ���̣߳���������������������(ָ�뱣������ʱ���� pstTreeOff �� )
	BOOL CleanTree()
	{
		//
		//	��������
		//
		m_bCleaning	= TRUE;

		DWORD dwOffTreeIndex	= GetOfflineTreeIndex( m_dwCurrent );
		_T * pstTreeOff		= m_pstTree[ dwOffTreeIndex ];

		//
		//	�Ƚ����µ���ͷָ�븳Ϊ NULL������������
		//
		m_pstTree[ dwOffTreeIndex ]	= NULL;
		m_dwCount[ dwOffTreeIndex ]	= 0;

		//
		//	��������������
		//
		m_dwCurrent	= dwOffTreeIndex;

		//
		//	����һ���̣߳�������������������ʹ�õ��ڴ�
		//
		_beginthread
		(
			_threadCleanTree,
			0,
			(VOID*)pstTreeOff
		);
		
		//
		//	�������
		//
		m_bCleaning	= FALSE;

		return TRUE;
	}

private:

	DWORD GetOnlineTreeIndex( DWORD dwIndex )
	{
		return dwIndex;
	}

	DWORD GetOfflineTreeIndex( DWORD dwIndex )
	{
		return ( ( dwIndex + 1 ) % 2 );
	}

	static VOID _threadCleanTree( PVOID arglist )
	{
		if ( NULL == arglist )
		{
			return;
		}

		__try
		{
			_T * pstTree = (_T*)arglist;
			if ( pstTree )
			{
				_threadCleanTree( pstTree->pLeft );
				_threadCleanTree( pstTree->pRight );
				
				delete pstTree;
				pstTree = NULL;
			}
		}
		__except( EXCEPTION_EXECUTE_HANDLER )
		{
		}
	}

public:

private:
	CRITICAL_SECTION m_oCrSec;
	_T * m_pstTree[ 2 ];
	DWORD m_dwCurrent;
	DWORD m_dwCount[ 2 ];
	BOOL  m_bCleaning;
};





#endif // __TPROCCACHEBASE_HEADER__
