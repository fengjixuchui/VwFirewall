// DeThreadSleep.cpp: implementation of the CDeThreadSleep class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "DeThreadSleep.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CDeThreadSleep::CDeThreadSleep()
{
	//	��ʼ�� public �߳̾��
	m_hThread	= NULL;
	m_uThreadId	= 0;

	//	���� event
	m_hSleepEvent = CreateEvent( NULL, TRUE, FALSE, NULL );

	//	���õȴ�״̬
	SetThreadWait( TRUE );

	//	�Ƿ����ִ���߳�
	m_bIsContinue	= TRUE;
}

CDeThreadSleep::~CDeThreadSleep()
{
	if ( m_hSleepEvent )
	{
		CloseHandle( m_hSleepEvent );
		m_hSleepEvent = NULL;
	}
}

/**
 *	@ public
 *	�������̵߳ȴ�
 */
VOID CDeThreadSleep::SetThreadWait( BOOL bWait )
{
	if ( bWait )
	{
		//	sets to nonsignaled.
		ResetEvent( m_hSleepEvent );
	}
	else
	{
		//	sets to signaled.
		SetEvent( m_hSleepEvent );
	}
}

/**
 *	@ public
 *	�����ȴ�
 */
VOID CDeThreadSleep::EndSleep()
{
	//	ֹͣ�ȴ�
	SetThreadWait( FALSE );

	//	���ý�����־
	m_bIsContinue = FALSE;
}

/**
 *	@ public
 *	Event �ȴ�
 */
BOOL CDeThreadSleep::EventSleep( DWORD dwMilliseconds )
{
	//
	//	dwMilliseconds	- [in] Ҫ Sleep ��ʱ�䣬��λ����
	//			       ������� INFINITE ����һֱ�ȴ�
	//	RETURN		- TRUE  ������ Sleep �� dwMilliseconds ����ʱ��
	//			  FALSE ��������Ϊ m_hSleepEvent ������Ϊ signaled ״̬����������ԭ��
	//

	if ( WAIT_TIMEOUT == WaitForSingleObject( m_hSleepEvent, dwMilliseconds ) )
	{
		return TRUE;
	}
	else
	{
		return FALSE;
	}
}

/**
 *	@ public
 *	�����߳�
 */
BOOL CDeThreadSleep::EndThread( HANDLE * phThread /*= NULL*/ )
{
	BOOL bRet		= FALSE;
	HANDLE * phThisThread	= NULL;


	__try
	{
		if ( phThread )
		{
			phThisThread = phThread;
		}
		else
		{
			phThisThread = &m_hThread;
		}

		if ( phThisThread && (*phThisThread) )
		{
			TerminateThread( (*phThisThread), 0 );
			
			while ( WAIT_TIMEOUT == WaitForSingleObject( (*phThisThread), 1000 ) )
			{
				TerminateThread( (*phThisThread), 0 );
			}

			CloseHandle( (*phThisThread) );
			(*phThisThread) = NULL;

			bRet = TRUE;
		}
	}
	__except( EXCEPTION_EXECUTE_HANDLER )
	{
	}

	return bRet;
}

/**
 *	@ public
 *	��ѯ�Ƿ�����߳�
 */
BOOL CDeThreadSleep::IsContinue()
{
	return m_bIsContinue;
}

/**
 *	@ public
 *	��ѯ�߳��Ƿ���������
 */
BOOL CDeThreadSleep::IsRunning()
{
	BOOL  bRet;
	DWORD dwCode;

	if ( NULL == m_hThread )
	{
		return FALSE;
	}

	bRet	= FALSE;
	dwCode	= 0;

	//	...
	::GetExitCodeThread( m_hThread, &dwCode );

	if ( STILL_ACTIVE == dwCode )
	{
		bRet = TRUE;
	}

	return bRet;
}



//////////////////////////////////////////////////////////////////////////
//	Private
