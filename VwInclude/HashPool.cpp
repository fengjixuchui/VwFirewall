#include "StdAfx.h"
#include "HashPool.h"




CHashPool::CHashPool( unsigned int lnPoolDepth /* CONST_MAX_HASHPOOL_DEPTH*3 */ )
{
	BOOL bFileMapCreate	= FALSE;
	LPVOID lpvBuffer	= NULL;
	HANDLE hFileMap		= NULL;
	UINT i;


	//
	//	Is init successfully
	//
	m_bInitSucc		= FALSE;
	m_dwThreadRecordTimeout	= 5*60*1000;	//	�̼߳�����ʱ 5*60 ��
	m_pstPoolListHeader	= NULL;
	m_pstPoolList		= NULL;

	//
	//	Special the depth of the pool by user
	//
	m_uPoolDepth	= lnPoolDepth;

	if ( m_uPoolDepth > 0 )
	{
		//	�����ڷ����ڴ�ķ�ʽ
		bFileMapCreate	= TRUE;
		m_pstPoolListHeader = new STHASHPOOLLISTHEADER;
		if ( m_pstPoolListHeader )
		{
			//	��ʼ�� CyclePool Header ��Ϣ
			//	Free ָ��ָ�� 0 ������
			m_pstPoolListHeader->hFileMapHeader	= NULL;
			m_pstPoolListHeader->uRefCount		= 0;
			InitializeCriticalSection( & m_pstPoolListHeader->oCriticalSectionModify );

			m_pstPoolList = new STHASHPOOLLIST[ m_uPoolDepth ];
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
		if ( _vwfunc_malloc_share_memory( sizeof(STHASHPOOLLISTHEADER), CONST_FILEMAP_VW_HASHPOOL_HEADER, &lpvBuffer, hFileMap, & bFileMapCreate ) )
		{
			m_pstPoolListHeader = (STHASHPOOLLISTHEADER*)lpvBuffer;
		}
		if ( m_pstPoolListHeader )
		{
			//	Ϊ�´����Ĺ����ڴ��ʼ��
			if ( bFileMapCreate )
			{
				m_pstPoolListHeader->hFileMapHeader	= hFileMap;
				m_pstPoolListHeader->uRefCount		= 0;
				InitializeCriticalSection( & m_pstPoolListHeader->oCriticalSectionModify );
			}
		
			//
			//	���빲���ڴ� :: m_pstPoolList
			//
			lpvBuffer	= NULL;
			hFileMap	= NULL;
			bFileMapCreate	= FALSE;
			if ( _vwfunc_malloc_share_memory( sizeof(STHASHPOOLLIST)*m_uPoolDepth, CONST_FILEMAP_VW_HASHPOOL, &lpvBuffer, hFileMap, & bFileMapCreate ) )
			{
				m_pstPoolList = (STHASHPOOLLIST*)lpvBuffer;
				if ( m_pstPoolList )
				{
					m_pstPoolListHeader->hFileMap	= hFileMap;
					m_bInitSucc = TRUE;
				}
			}
		}
		*/

		//
		//	��ʼ���ṹ����Ϣ
		//	ע�⣺��ʼ���ɹ��������ǵ�һ�δ�����ʱ�����Ҫ��ʼ����������ܻ�ɵ�������������ʹ�õ���鹲������
		//
		if ( m_bInitSucc )
		{
			//	����ʹ�ý������ü�����Ҳ�����ж��ٸ�������ʹ����������ڴ�
			m_pstPoolListHeader->uRefCount ++;

			//	Ϊ��һ�δ��������Ĺ����ڴ��ʼ��
			if ( bFileMapCreate )
			{
				for ( i = 0; i < m_uPoolDepth; i ++ )
				{
					//	��ʼ���ڵ�
					InitPoolNode( & m_pstPoolList[ i ] );
				}
			}

			//
			//	������������̨�߳�
			//	ע�⣺
			//		2007-10-17 By LQX :: 3.0.9.1048 �汾�Ժ����ʱ�����ù��ܣ����������������߳�
			//
			//if ( ! _vwfunc_is_debug() )
			//{
			//	_beginthread( _threadAntiAttackBgWorker, 0, (void*)this );
			//}
		}
	}
}

CHashPool::~CHashPool()
{
	//	��Ϊ�ǹ����ڴ棬���Բ����ͷ�
/*
	//	����ʹ�ý������ü���
	m_pstPoolListHeader->uRefCount --;

	if ( 0 == m_pstPoolListHeader->uRefCount )
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
UINT CHashPool::GetPoolDepth()
{
	return m_uPoolDepth;
}

/**
 *	@ Public 
 *	�Ƿ��ʼ���ɹ�����Ҫ���ڴ��Ƿ����ɹ�
 */
BOOL CHashPool::IsInitSucc()
{
	return m_bInitSucc;
}


/**
 *	@ Public 
 *	[string hash]
 *	Get hashed pool index	
 */
LONG CHashPool::GetHashPoolIndex( const DWORD dwIpAddr )
{
	//
	//	dwIpAddr	- [in] ip address
	//
	//	RETURN
	//			- LONG type hash value if successfully
	//			  otherwise -1
	//

	if ( ! m_bInitSucc )
	{
		return -1;
	}

	return ( dwIpAddr % ( m_uPoolDepth + 1 ) );
}

/**
 *	@ Public 
 *	Get special item count	
 */
LONG CHashPool::GetCount( LONG lnIndex )
{
	//
	//	lnIndex		- [in] hash pool index
	//
	//	RETURN		-
	//			  the count of special pool indexed by uIndex if successfulyy
	//			  otherwise -1
	//

	if ( ! m_bInitSucc )
	{
		return -1;
	}
	if ( ! IsInIndexRange( lnIndex ) )
	{
		return -1;
	}

	if ( m_pstPoolList[ lnIndex ].dwQueryTick > 0 &&
		GetTickCount() - m_pstPoolList[ lnIndex ].dwQueryTick > m_dwThreadRecordTimeout )
	{
		//
		//	��ʱ�ˣ�������������Ϊ0
		//
		EnterCriticalSection( & m_pstPoolListHeader->oCriticalSectionModify );
		m_pstPoolList[ lnIndex ].usThreadCount = 0;
		LeaveCriticalSection( & m_pstPoolListHeader->oCriticalSectionModify );

		return 0;
	}

	//	��¼ QueryTick
	EnterCriticalSection( & m_pstPoolListHeader->oCriticalSectionModify );
	m_pstPoolList[ lnIndex ].dwQueryTick = GetTickCount();
	LeaveCriticalSection( & m_pstPoolListHeader->oCriticalSectionModify );

	//	..
	return m_pstPoolList[ lnIndex ].usThreadCount;
}

/**
 *	@ Public 
 *	Increase the count and Save the data
 */
BOOL CHashPool::IncCount( LONG lnIndex, const DWORD dwIpAddr )
{
	//
	//	lnIndex		- [in] hash pool index
	//	dwIpAddr	- [in] Ip Address
	//
	//	RETURN		- TRUE/FALSE
	//
	
	if ( ! m_bInitSucc )
	{
		return FALSE;
	}
	if ( ! IsInIndexRange( lnIndex ) )
	{
		return FALSE;
	}

	if ( 0 == m_pstPoolList[ lnIndex ].usThreadCount )
	{
		//	�µ���

		EnterCriticalSection( & m_pstPoolListHeader->oCriticalSectionModify );

		m_pstPoolList[ lnIndex ].dwIpAddr	= dwIpAddr;
		m_pstPoolList[ lnIndex ].usThreadCount	= 1;
		m_pstPoolList[ lnIndex ].dwQueryTick	= GetTickCount();

		LeaveCriticalSection( & m_pstPoolListHeader->oCriticalSectionModify );

		return TRUE;
	}
	else if ( dwIpAddr == m_pstPoolList[ lnIndex ].dwIpAddr )
	{
		//	�Ѿ����ڵ���

		EnterCriticalSection( & m_pstPoolListHeader->oCriticalSectionModify );

		m_pstPoolList[ lnIndex ].usThreadCount ++;
		m_pstPoolList[ lnIndex ].dwQueryTick = GetTickCount();

		LeaveCriticalSection( & m_pstPoolListHeader->oCriticalSectionModify );

		return TRUE;
	}
	else
	{
		//	��ͻ���������֮
		//	..

		return TRUE;
	}
}

/**
 *	@ Public 
 *	Decrease the count
 */
BOOL CHashPool::DecCount( LONG lnIndex, const DWORD dwIpAddr )
{
	//
	//	uIndex		- [in] hash pool index
	//	dwIpAddr	- [in] Ip Address 
	//
	//	RETURN		-
	//			  the count of special pool indexed by uIndex
	//

	if ( ! m_bInitSucc )
	{
		return FALSE;
	}
	if ( ! IsInIndexRange( lnIndex ) )
	{
		return FALSE;
	}

	BOOL bRet	= FALSE;

	if ( dwIpAddr == m_pstPoolList[ lnIndex ].dwIpAddr )
	{
		//	���ҵ�����
		if ( m_pstPoolList[ lnIndex ].usThreadCount > 0 )
		{
			bRet = TRUE;

			EnterCriticalSection( & m_pstPoolListHeader->oCriticalSectionModify );

			m_pstPoolList[ lnIndex ].usThreadCount --;
			m_pstPoolList[ lnIndex ].dwQueryTick = GetTickCount();

			LeaveCriticalSection( & m_pstPoolListHeader->oCriticalSectionModify );
		}
	}
	else
	{
		//	�����ǳ�ͻ������������壬���֮
		//	..

	}

	return bRet;
}


/**
 *	@ Public 
 *	Decrease the count
 */
DWORD CHashPool::GetIpAddrFromString( LPCSTR lpcszIpAddr )
{
	if ( NULL == lpcszIpAddr || 0 == lpcszIpAddr[ 0 ] )
		return 0;

	//	INADDR_NONE �������һ���Ϸ��� IP ��ַ
	return inet_addr( lpcszIpAddr );
}






/**
 *	@ Public 
 *	�ձ����ļ����ʴ������� :: ��ȡ����	
 */
LONG CHashPool::LmtDayIpGetCount( LONG lnIndex )
{
	//
	//	lnIndex		- [in] hash pool index
	//
	//	RETURN		-
	//			  the count of special pool indexed by uIndex if successfulyy
	//			  otherwise -1
	//
	
	if ( ! m_bInitSucc )
	{
		return -1;
	}
	if ( ! IsInIndexRange( lnIndex ) )
	{
		return -1;
	}
	
	//	..
	return m_pstPoolList[ lnIndex ].usLmtDayIpTimes;
}

/**
 *	@ Public 
 *	�ձ����ļ����ʴ������� :: �������ݲ����Ӽ���	
 */
BOOL CHashPool::LmtDayIpIncCount( LONG lnIndex, const DWORD dwIpAddr )
{
	//
	//	lnIndex		- [in] hash pool index
	//	dwIpAddr	- [in] Ip Address
	//
	//	RETURN		- TRUE/FALSE
	//
	
	if ( ! m_bInitSucc )
	{
		return FALSE;
	}
	if ( ! IsInIndexRange( lnIndex ) )
	{
		return FALSE;
	}

	SYSTEMTIME st;
	USHORT usLastDate;
	
	GetLocalTime( & st );
	usLastDate = ( st.wMonth * 100 + st.wDay );

	if ( 0 == m_pstPoolList[ lnIndex ].usLmtDayIpTimes )
	{
		//	�µ���

		EnterCriticalSection( & m_pstPoolListHeader->oCriticalSectionModify );

		m_pstPoolList[ lnIndex ].dwIpAddr		= dwIpAddr;
		m_pstPoolList[ lnIndex ].usLmtDayIpLastDate	= usLastDate;
		m_pstPoolList[ lnIndex ].usLmtDayIpTimes	= 1;

		LeaveCriticalSection( & m_pstPoolListHeader->oCriticalSectionModify );

		return TRUE;
	}
	else if ( dwIpAddr == m_pstPoolList[ lnIndex ].dwIpAddr )
	{
		//	�Ѿ����ڵ���

		EnterCriticalSection( & m_pstPoolListHeader->oCriticalSectionModify );

		if ( usLastDate != m_pstPoolList[ lnIndex ].usLmtDayIpLastDate )
		{
			//	�ֹ���һ��
			m_pstPoolList[ lnIndex ].usLmtDayIpLastDate	= usLastDate;
			m_pstPoolList[ lnIndex ].usLmtDayIpTimes	= 1;
		}
		else
		{
			m_pstPoolList[ lnIndex ].usLmtDayIpTimes ++;
		}

		LeaveCriticalSection( & m_pstPoolListHeader->oCriticalSectionModify );

		return TRUE;
	}
	else
	{
		//	��ͻ���������֮
		//	..

		return TRUE;
	}
}






/**
 *	@ Public 
 *	Get special item count	
 */
LONG CHashPool::AntiAttackGetCount( LONG lnIndex )
{
	//
	//	lnIndex		- [in] hash pool index
	//
	//	RETURN		-
	//			  the count of special pool indexed by uIndex if successfulyy
	//			  otherwise -1
	//

	if ( ! m_bInitSucc )
	{
		return -1;
	}
	if ( ! IsInIndexRange( lnIndex ) )
	{
		return -1;
	}

	//	..
	return m_pstPoolList[ lnIndex ].usVisitCount;
}

/**
 *	@ Public 
 *	Increase the count and Save the data
 */
BOOL CHashPool::AntiAttackIncCount( LONG lnIndex, const DWORD dwIpAddr )
{
	//
	//	lnIndex		- [in] hash pool index
	//	dwIpAddr	- [in] Ip Address
	//
	//	RETURN		- TRUE/FALSE
	//
	
	if ( ! m_bInitSucc )
	{
		return FALSE;
	}
	if ( ! IsInIndexRange( lnIndex ) )
	{
		return FALSE;
	}

	if ( 0 == m_pstPoolList[ lnIndex ].usVisitCount )
	{
		//	�µ���
		if ( CONST_IPADDR_NULL == m_pstPoolList[ lnIndex ].dwIpAddr )
		{
			m_pstPoolList[ lnIndex ].dwIpAddr = dwIpAddr;
		}
		m_pstPoolList[ lnIndex ].usVisitCount = 1;

		return TRUE;
	}
	else if ( dwIpAddr == m_pstPoolList[ lnIndex ].dwIpAddr )
	{
		//	�Ѿ����ڵ���
		m_pstPoolList[ lnIndex ].usVisitCount ++;

		return TRUE;
	}
	else
	{
		//	��ͻ���������֮
		//	..

		return TRUE;
	}
}

/**
 *	��ȡ �����ƿͻ���������ʱ���
 */
LONG CHashPool::AntiAttackGetWakeUpTime( LONG lnIndex )
{
	//
	//	lnIndex		- [in] hash pool index
	//	RETURN		- ���������ƿͻ���������ʱ���
	//			  ������ʱ��㲻Ϊ 0 �Ļ���ô����Ϊ�ÿͻ����Ǳ����Ƶ�
	//			  otherwise -1
	//

	if ( ! m_bInitSucc )
	{
		return -1;
	}
	if ( ! IsInIndexRange( lnIndex ) )
	{
		return -1;
	}

	//	..
	return m_pstPoolList[ lnIndex ].dwWakeUpTime;
}

/**
 *	���� �����ƿͻ���������ʱ���
 */
VOID CHashPool::AntiAttackSetWakeUpTime( LONG lnIndex, LONG lnWakeUpTime )
{
	//
	//	lnIndex		- [in] hash pool index
	//	lnWakeUpTime	- [in] ָ�������ƿͻ���������ʱ��㣬һ���� GetTickCount() + X����
	//	RETURN		- VOID
	//

	if ( ! m_bInitSucc )
	{
		return;
	}
	if ( ! IsInIndexRange( lnIndex ) )
	{
		return;
	}

	//	..
	m_pstPoolList[ lnIndex ].dwWakeUpTime = lnWakeUpTime;
}

/**
 *	��������̨�����߳�
 */
VOID CHashPool::_threadAntiAttackBgWorker( PVOID pvArg )
{
	if ( NULL == pvArg )
		return;

	//	..
	CHashPool * pThis = (CHashPool*)pvArg;
	if ( pThis )
	{
		pThis->AntiAttackBgWorkerProc();
	}
}
VOID CHashPool::AntiAttackBgWorkerProc()
{
	//
	//	ʵ�ʲ������������ص� usVisitCount = 0; �����Ҫ 0.031 ~ 0.062 ��
	//

	UINT	i;
	
	while ( TRUE )
	{
		//
		//	1 ������һ��
		//
		Sleep( 60*1000 );

		//
		//	�ͷ�����һ�����ڵļ�ʱΪ 0
		//	Ϊ���ٶȺ�Ч����ʱ�����ǻ�������
		//
		for ( i = 0; i < m_uPoolDepth; i ++ )
		{
			m_pstPoolList[ i ].usVisitCount	= 0;
		}

		if ( _vwfunc_is_debug() )
		{
			break;
		}
	}
}





////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////





/**
 *	@ Public 
 *	Check if the index of hash pool is in range	
 */
BOOL CHashPool::IsInIndexRange( UINT uIndex )
{
	//
	//	uIndex		- [in] hash pool index
	//	RETURN		- TRUE/FALSE

	if ( uIndex >= 0 && uIndex < m_uPoolDepth )
	{
		return TRUE;
	}
	else
	{
		return FALSE;
	}
}



/**
 *	@ Private
 *	��ʼ���ڵ�
 */
VOID CHashPool::InitPoolNode( STHASHPOOLLIST * pstPoolNode )
{
	if ( pstPoolNode )
	{
		pstPoolNode->dwQueryTick	= 0;
		pstPoolNode->dwIpAddr		= CONST_IPADDR_NULL;
		pstPoolNode->usThreadCount	= 0;
		pstPoolNode->usVisitCount	= 0;
		pstPoolNode->dwWakeUpTime	= 0;
	}
}



/**
 *	@ Private ��ʱδʹ�õĺ���
 *	[string hash]
 *	32 version for zero terminated string
 */
UINT CHashPool::GetStringHashValue32( const char * s, UINT lnInit /*CONST_FNV_32_PRIME*/ )
{
	//
	//	s	- [in] pointer to a null-terminated string 
	//	lnInit	- [in] init frime value
	//
	//	RETURN
	//		UINT type hash value
	//

	UINT hval;

	for ( hval = lnInit; *s; s++ )
	{
		//	multiply by the 32 bit FNV magic prime mod 2^32
		hval *= CONST_FNV_32_PRIME;

		//	xor the bottom with the current octet
		hval ^= (UINT)*s;
	}

	return hval;
}

/**
 *	@ Private ��ʱδʹ�õĺ���
 *	[string hash]
 *	32 version for buffer
 */
UINT CHashPool::GetStringHashValue32ByBuffer( const char * buf, UINT len, UINT lnInit /*CONST_FNV_32_PRIME*/ )
{
	//
	//	buf	- [in] pointer to a buffer
	//	len	- [in] length of buffer that need to hash
	//	lnInit	- [in] init frime value
	//
	//	RETURN
	//		UINT type hash value
	//


	//	initial hash value
	UINT hval;

	for ( hval = CONST_FNV_32_PRIME; len > 0; --len )
	{
		hval *= CONST_FNV_32_PRIME;
		hval ^= (UINT)(*buf);
		++ buf;
	}

	return hval;
}





























