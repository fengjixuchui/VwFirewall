// DePoolErrorLog.cpp: implementation of the DePoolErrorLog class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "DePoolErrorLog.h"
#include "process.h"


//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CDePoolErrorLog::CDePoolErrorLog()
{
	InitializeCriticalSection( & m_oCrSecPool );

	//
	//	ɾ�������е�����Ԫ��
	//
	m_arrDataPool.RemoveAll();


	//
	//	���� �ؼ�ع����߳�
	//
	_beginthreadex
	(
		NULL,
		0,
		&_threadPoolWorker,
		(void*)this,
		0,
		NULL
	);
}

CDePoolErrorLog::~CDePoolErrorLog()
{
	DeleteCriticalSection( & m_oCrSecPool );
}


/**
 *	@ Public
 *	��ʽ��д��־
 */
BOOL CDePoolErrorLog::AsyncWriteErrorLog( DWORD dwErrorCode, LPCTSTR lpszFmt, ... )
{
	BOOL  bRet			= FALSE;
	INT   nSize			= 0;
	STDEPOOLERRORLOGDATA stData;
	va_list args;

	__try
	{
		//	dwErrorCode
		stData.dwErrorCode	= dwErrorCode;

		//	build error desc
		va_start( args, lpszFmt );
		nSize = _vsnprintf( stData.szErrorDesc, sizeof(stData.szErrorDesc)-sizeof(TCHAR), lpszFmt, args );
		va_end( args );

		//
		//	��������
		//
		EnterCriticalSection( & m_oCrSecPool );

		//
		//	�����ݱ��浽����
		//
		bRet = m_arrDataPool.Add( stData );

		//
		//	�뿪�����
		//
		LeaveCriticalSection( & m_oCrSecPool );

	}
	__except( EXCEPTION_EXECUTE_HANDLER )
	{
		bRet = FALSE;
	}

	return bRet;
}



/**
 *	�ļ��仯���
 */
unsigned __stdcall CDePoolErrorLog::_threadPoolWorker( PVOID arglist )
{
	if ( NULL == arglist )
	{
		return 1;
	}

	__try
	{
		CDePoolErrorLog * pThis = (CDePoolErrorLog*)(arglist);
		if ( pThis )
		{
			pThis->PoolWorkerProc();
		}

		_endthreadex( 0 );
	}
	__except( EXCEPTION_EXECUTE_HANDLER )
	{
	}
	
	return 0;

}
VOID CDePoolErrorLog::PoolWorkerProc()
{
	INT i;

	while( TRUE )
	{
		//
		//	��������
		//
		EnterCriticalSection( & m_oCrSecPool );


		//
		//	ѭ�����У�ȡ����־����ʼд�����
		//
		for ( i = 0; i < m_arrDataPool.GetSize(); i++ )
		{
			CDeErrorLog::WriteErrorLog( m_arrDataPool[ i ].dwErrorCode, m_arrDataPool[ i ].szErrorDesc );
		}

		//
		//	ɾ�������е�����Ԫ��
		//
		m_arrDataPool.RemoveAll();


		//
		//	�뿪�����
		//
		LeaveCriticalSection( & m_oCrSecPool );


		//
		//	��Ϣһ���
		//
		Sleep( 10*1000 );
	}
}