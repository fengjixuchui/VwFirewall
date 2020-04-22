// DeCpuInfo.cpp: implementation of the CDeCpuInfo class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "DeCpuInfo.h"

#include <Windows.h>
#include <process.h>		// for _beginthreadex

#include "DeLib.h"
#pragma comment( lib, "DeLib.lib" )

#include "ntdll.h"


#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif


//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CDeCpuInfo::CDeCpuInfo()
{
	//m_pfnNtQuerySystemInformation = (PROCNTQSI)GetProcAddress( GetModuleHandleA("ntdll"), "NtQuerySystemInformation" );
	m_pfnNtQuerySystemInformation = NULL;

	//
	//
	//
	m_pvSystemProcessInfoBuffer	= NULL;
	m_dwSystemProcessInfoBufferSize	= 0;

	//
	//
	//
	memset( m_szProcessName, 0, sizeof(m_szProcessName) );
	m_dwProcessNameLength		= 0;
	m_dwProcessNameLengthInByte	= 0;
	m_dwRefreshInterval		= 3000;

	//
	//
	//
	m_ullLastKernelTimeSpec		= 0;
	m_ullLastUserTimeSpec		= 0;
	m_ullLastKernelTimeTotal	= 0;
	m_ullLastUserTimeTotal		= 0;


	m_dwCpuUsedSpec		= 0;
	m_dwCpuUsedTotal	= 0;

	m_hCalcTread			= NULL;

	//
	//
	//
	InitializeCriticalSection( & m_stCriticalSection );

	//if ( m_pfnNtQuerySystemInformation )
	{
		//	�°汾���·���
		//m_hCalcTread = (HANDLE)_beginthreadex
		//	(
		//		NULL,
		//		0,
		//		&_threadQueryCpuInfo,
		//		(void*)this,
		//		0,
		//		NULL
		//	);

		//	�ȶ��汾
		m_cThSleepCalcTread_StablyVer.m_hThread= (HANDLE)_beginthreadex
			(
				NULL,
				0,
				&_threadQueryCpuInfo_StablyVer,
				(void*)this,
				0,
				&m_cThSleepCalcTread_StablyVer.m_uThreadId
			);

		//	�ȶ��汾
		m_cThSleepCalcSpecTread_StablyVer.m_hThread= (HANDLE)_beginthreadex
			(
				NULL,
				0,
				&_threadQuerySpecCpuInfo_StablyVer,
				(void*)this,
				0,
				&m_cThSleepCalcSpecTread_StablyVer.m_uThreadId
			);

	}

}

CDeCpuInfo::~CDeCpuInfo()
{
	if ( m_hCalcTread )
	{
		m_cThSleepCalcTread.EndSleep();
		m_cThSleepCalcTread.EndThread( &m_hCalcTread );
	}
	if ( m_cThSleepCalcTread_StablyVer.m_hThread )
	{
		m_cThSleepCalcTread_StablyVer.EndSleep();
		m_cThSleepCalcTread_StablyVer.EndThread( &m_cThSleepCalcTread_StablyVer.m_hThread );
	}
	if ( m_cThSleepCalcSpecTread_StablyVer.m_hThread )
	{
		m_cThSleepCalcSpecTread_StablyVer.EndSleep();
		m_cThSleepCalcSpecTread_StablyVer.EndThread( &m_cThSleepCalcSpecTread_StablyVer.m_hThread );
	}

	DeleteCriticalSection( & m_stCriticalSection );
}

/**
 *	@ public
 *	������Ҫ��ȡ CPU ��Ϣ�Ľ�����Ϣ
 */
BOOL CDeCpuInfo::SetProcessInfo( LPCTSTR lpcszProcessName, DWORD dwRefreshInterval )
{
	//
	//	lpcszProcessName	- [in] ������
	//	dwRefreshInterval	- [in] ѭ�����ʱ�䣬��λ������
	//	RETURN			-
	//

	if ( NULL == lpcszProcessName || 0 == dwRefreshInterval )
	{
		return FALSE;
	}
	if ( 0 == _tcslen( lpcszProcessName ) )
	{
		return FALSE;
	}

	_sntprintf( m_szProcessName, sizeof(m_szProcessName)/sizeof(TCHAR)-sizeof(TCHAR), _T("%s"), lpcszProcessName );

	m_dwProcessNameLength		= _tcslen( m_szProcessName );
	m_dwProcessNameLengthInByte	= m_dwProcessNameLength * sizeof(TCHAR);

	m_dwRefreshInterval		= dwRefreshInterval;

	if ( m_dwRefreshInterval < 1000 )
	{
		m_dwRefreshInterval = 1000;
	}

	return TRUE;
}

DWORD CDeCpuInfo::GetCpuUsedPercentSpec()
{
	DWORD dwRet	= 0;
//	BOOL  bOddNumber;	//	�Ƿ�����

	EnterCriticalSection( & m_stCriticalSection );
	

	//INT cpu = (BYTE) ( ( ( CPUTimeDelta.QuadPart / ( ( TotalTime.QuadPart / 1000 ) ? ( TotalTime.QuadPart / 1000 ) : 1 ) ) + 5 ) / 10 );

	dwRet = m_dwCpuUsedSpec;
	if ( dwRet > 100 )
	{
		dwRet = 100;
	}
	if ( dwRet > 0 )
	{
		//bOddNumber = ( 0 != dwRet % 2 );
		//dwRet *= 2;
		//dwRet += ( bOddNumber ? 1 : 0 );
		//dwRet ++;
	}

	LeaveCriticalSection( & m_stCriticalSection );

	return dwRet;
}

DWORD CDeCpuInfo::GetCpuUsedPercentTotal()
{
	DWORD dwRet	= 0;
//	BOOL  bOddNumber;	//	�Ƿ�����
	
	EnterCriticalSection( & m_stCriticalSection );
	
	dwRet = m_dwCpuUsedTotal;
	if ( dwRet > 100 )
	{
		dwRet = 100;
	}
	if ( dwRet > 0 )
	{
		//bOddNumber = ( 0 != dwRet % 2 );
		//dwRet *= 2;
		//dwRet += ( bOddNumber ? 1 : 0 );
		//dwRet ++;
	}
	
	LeaveCriticalSection( & m_stCriticalSection );
	
	return dwRet;
}


//////////////////////////////////////////////////////////////////////////
//	Private


/**
 *	@ private
 *	�ȶ��汾�� CPU ��ȡ�취
 *	######## �� _threadQueryCpuInfo �汾��ȡ������ʱ��ʹ��
 */
unsigned __stdcall CDeCpuInfo::_threadQueryCpuInfo_StablyVer( PVOID arglist )
{
	if ( NULL == arglist )
	{
		return 1;
	}
	
	__try
	{
		CDeCpuInfo * pThis = (CDeCpuInfo*)arglist;
		if ( pThis )
		{
			pThis->QueryCpuInfo_StablyVerProc();
		}
		
		_endthreadex( 0 );
	}
	__except( EXCEPTION_EXECUTE_HANDLER )
	{
	}
	
	return 0;
}
VOID CDeCpuInfo::QueryCpuInfo_StablyVerProc()
{
	__try
	{
		while ( TRUE )
		{
			//	���� CPU ��Ϣ
			CalcCpuInfo_StablyVerProc();

			//	sleep
			Sleep( m_dwRefreshInterval );
		}
	}
	__except( EXCEPTION_EXECUTE_HANDLER )
	{
	}
}

//
VOID CDeCpuInfo::CalcCpuInfo_StablyVerProc()
{
	//
	SYSTEM_INFO siSysBaseInfo;
	STDECPUINFOSPECCPUINFO siSysTimeInfo;
	__int64  qwIdleTimeElapsed, qwKernelTimeElapsed, qwUserTimeElapsed, qwTotalTimeElapsed;

//	SYSTEM_PERFORMANCE_INFORMATION SysPerfInfo;
//	SYSTEM_TIME_INFORMATION        SysTimeInfo;
//	SYSTEM_BASIC_INFORMATION       SysBaseInfo;
//	double                         dbIdleTime;
//	double                         dbSystemTime;
//	LONG                           status;
	LARGE_INTEGER                  liOldIdleTime = {0,0};
	LARGE_INTEGER                  liOldSystemTime = {0,0};
	


	//	Copy the hardware information to the SYSTEM_INFO structure.
	GetSystemInfo( &siSysBaseInfo );
	
	// ��ȡCPU����Ŀ
	//status = m_pfnNtQuerySystemInformation( SystemBasicInformation, &SysBaseInfo, sizeof(SysBaseInfo), NULL );
	//if ( NO_ERROR != status )
	//{
	//	return;
	//}

	while( TRUE )
	{
		memset( &siSysTimeInfo, 0, sizeof(siSysTimeInfo) );
	
		//	��ȡ��ǰϵͳʱ��
		if ( GetSystemTimes( &siSysTimeInfo.ftIdleTimeStart, &siSysTimeInfo.ftKernelTimeStart, &siSysTimeInfo.ftUserTimeStart ) )
		{
			Sleep( 1000 );

			if ( GetSystemTimes( &siSysTimeInfo.ftIdleTimeEnd, &siSysTimeInfo.ftKernelTimeEnd, &siSysTimeInfo.ftUserTimeEnd ) )
			{
				qwIdleTimeElapsed	= FileTimeToQuadWord( &siSysTimeInfo.ftIdleTimeEnd ) - FileTimeToQuadWord( &siSysTimeInfo.ftIdleTimeStart );
				qwKernelTimeElapsed	= FileTimeToQuadWord( &siSysTimeInfo.ftKernelTimeEnd ) - FileTimeToQuadWord( &siSysTimeInfo.ftKernelTimeStart );
				qwUserTimeElapsed	= FileTimeToQuadWord( &siSysTimeInfo.ftUserTimeEnd ) - FileTimeToQuadWord( &siSysTimeInfo.ftUserTimeStart );
				qwTotalTimeElapsed	= qwKernelTimeElapsed + qwUserTimeElapsed;
				
				if( 0 == qwTotalTimeElapsed )
				{
					m_dwCpuUsedTotal = 0;
				}
				else
				{
					 // System Idle take 100 % of cpu :-((
					m_dwCpuUsedTotal = (DWORD)( ( qwTotalTimeElapsed - qwIdleTimeElapsed ) * 100 / qwTotalTimeElapsed );
				}	
			}
		}

		return;


/*
		if ( m_pfnNtQuerySystemInformation )
		{
			status = m_pfnNtQuerySystemInformation( SystemTimeInformation, &SysTimeInfo, sizeof(SysTimeInfo), 0 );
			if ( NO_ERROR == status )
			{
				// ��ȡCPU�Ŀ���ʱ��
				status = m_pfnNtQuerySystemInformation( SystemPerformanceInformation, &SysPerfInfo, sizeof(SysPerfInfo), NULL );
				if ( NO_ERROR == status )
				{
					if ( 0 == liOldIdleTime.QuadPart )
					{
						// �����ȡ������ʱ��
						liOldIdleTime = SysPerfInfo.liIdleTime;
						liOldSystemTime = SysTimeInfo.liKeSystemTime;
						
						Sleep( 1000 );
					}
					else
					{
						// �ڶ���ȡֵ
						dbIdleTime = Li2Double(SysPerfInfo.liIdleTime) - Li2Double(liOldIdleTime);
						dbSystemTime = Li2Double(SysTimeInfo.liKeSystemTime) - Li2Double(liOldSystemTime);
						
						// ���аٷֱ�
						dbIdleTime = dbIdleTime / dbSystemTime;
						
						// CurrentCpuUsage% = 100 - (CurrentCpuIdle * 100) / NumberOfProcessors
						//dbIdleTime = 100.0 - dbIdleTime * 100.0 / (double)SysBaseInfo.bKeNumberProcessors + 0.5;
						dbIdleTime = 100.0 - dbIdleTime * 100.0 / (double)siSysBaseInfo.dwNumberOfProcessors + 0.5;

						//	���浽ȫ�ֱ���
						m_dwCpuUsedTotal = (DWORD)dbIdleTime;

						break;
					}
				}
				else
				{
					break;
				}
			}
			else
			{
				break;
			}
		}
*/
	}
}


unsigned __stdcall CDeCpuInfo::_threadQuerySpecCpuInfo_StablyVer( PVOID arglist )
{
	if ( NULL == arglist )
	{
		return 1;
	}
	
	__try
	{
		CDeCpuInfo * pThis = (CDeCpuInfo*)arglist;
		if ( pThis )
		{
			pThis->QuerySpecCpuInfo_StablyVerProc();
		}
		
		_endthreadex( 0 );
	}
	__except( EXCEPTION_EXECUTE_HANDLER )
	{
	}
	
	return 0;
}
VOID CDeCpuInfo::QuerySpecCpuInfo_StablyVerProc()
{
	__try
	{
		while ( TRUE )
		{
			//	���� CPU ��Ϣ
			CalcSpecCpuInfo_StablyVerProc( m_szProcessName );
			
			//	sleep
			Sleep( m_dwRefreshInterval );
		}
	}
	__except( EXCEPTION_EXECUTE_HANDLER )
	{
	}
}
VOID CDeCpuInfo::CalcSpecCpuInfo_StablyVerProc( LPCTSTR lpctszAppName )
{
	HANDLE hSnapAll		= NULL;
	PROCESSENTRY32 pe	= {0};
	HANDLE hSpecProcess		= NULL;

	FILETIME ftDummy;
	//FILETIME ftKernelTimeStart, ftKernelTimeEnd;
	//FILETIME ftUserTimeStart, ftUserTimeEnd;

	STDECPUINFOSPECCPUINFO stSpecCpuInfo;
	map<DWORD,STDECPUINFOSPECCPUINFO> vcSpecCpuInfo;
	map<DWORD,STDECPUINFOSPECCPUINFO>::iterator it;
	typedef pair<DWORD,STDECPUINFOSPECCPUINFO> Pair_SpecCpuInfo;
	STDECPUINFOSPECCPUINFO * pstTempSpecCpuInfo;

	__int64  qwKernelTimeElapsed, qwUserTimeElapsed, qwTotalTimeElapsed;

	DWORD dwEnumProcessStartTime;
	DWORD dwEnumProcessUsedTime;

	if ( NULL == lpctszAppName || 0 == _tcslen( lpctszAppName ) )
	{
		return;
	}

	vcSpecCpuInfo.clear();
	dwEnumProcessStartTime	= GetTickCount();

	//
	//	Take the snapshot of all processes in system
	//
	hSnapAll = CreateToolhelp32Snapshot( TH32CS_SNAPPROCESS, 0 );
	if ( INVALID_HANDLE_VALUE != hSnapAll )
	{
		//
		//	Fill in the size of the structure before using it
		//
		pe.dwSize = sizeof(PROCESSENTRY32);

		//
		//	Work the snap of the processes, and for each process get
		//	the module file name
		//
		if ( Process32First( hSnapAll, &pe ) )
		{
			//
			//	��һ�� ��ȡ���̵��ں�ʱ����û�ʱ��
			//
			do
			{
				//	�ȽϽ��̵�����		
				if ( 0 == _tcsicmp( pe.szExeFile, lpctszAppName ) )
				{
					hSpecProcess = OpenProcess( PROCESS_QUERY_INFORMATION, false, pe.th32ProcessID );
					if ( hSpecProcess )
					{
						memset( &stSpecCpuInfo, 0, sizeof(stSpecCpuInfo) );

						stSpecCpuInfo.bQueryFirst	= FALSE;
						stSpecCpuInfo.bQuerySecond	= FALSE;

						if ( GetProcessTimes
							(
								hSpecProcess,
								&ftDummy,
								&ftDummy,
								&stSpecCpuInfo.ftKernelTimeStart,
								&stSpecCpuInfo.ftUserTimeStart
							)
						)
						{
							stSpecCpuInfo.bQueryFirst = TRUE;
							vcSpecCpuInfo.insert( Pair_SpecCpuInfo( pe.th32ProcessID, stSpecCpuInfo ) );
						}

						CloseHandle( hSpecProcess );
						hSpecProcess = NULL;
					}
				}
				
			}
			while( Process32Next( hSnapAll, &pe ) );
		}

		//
		//	����ö�ٽ������õ�ʱ��
		//	���� 2 ����˼�ǰѵڶ��λ�ȡ��ʱ��Ҳ������
		//
		dwEnumProcessUsedTime = ( GetTickCount() - dwEnumProcessStartTime ) * 2;
	
		//
		//	ֹͣ 1 ��
		//
		Sleep( 1000 - dwEnumProcessUsedTime );

		if ( Process32First( hSnapAll, &pe ) )
		{
			do
			{
				//	�ȽϽ��̵�����		
				if ( 0 == _tcsicmp( pe.szExeFile, lpctszAppName ) )
				{
					hSpecProcess = OpenProcess( PROCESS_QUERY_INFORMATION, false, pe.th32ProcessID );
					if ( hSpecProcess )
					{
						//
						//	�ڶ��� ��ȡ���̵��ں�ʱ����û�ʱ��
						//
						it = vcSpecCpuInfo.find( pe.th32ProcessID );
						if ( vcSpecCpuInfo.end() != it )
						{
							pstTempSpecCpuInfo = & it->second;
							if ( GetProcessTimes
								(
									hSpecProcess,
									&ftDummy,
									&ftDummy,
									&pstTempSpecCpuInfo->ftKernelTimeEnd,
									&pstTempSpecCpuInfo->ftUserTimeEnd
								)
							)	
							{
								pstTempSpecCpuInfo->bQuerySecond = TRUE;
							}
						}

						CloseHandle( hSpecProcess );
						hSpecProcess = NULL;
					}
				}
			}
			while( Process32Next( hSnapAll, &pe ) );
		}

		//
		//	Clean up the snapshot
		//
		CloseHandle( hSnapAll );
		hSnapAll = NULL;
	}

	qwTotalTimeElapsed = 0;

	if ( vcSpecCpuInfo.size() )
	{
		for ( it = vcSpecCpuInfo.begin(); it != vcSpecCpuInfo.end(); it ++ )
		{
			pstTempSpecCpuInfo = & it->second;
			if ( pstTempSpecCpuInfo->bQueryFirst && pstTempSpecCpuInfo->bQuerySecond )
			{
				//
				//	������
				//
				qwKernelTimeElapsed	= FileTimeToQuadWord( &pstTempSpecCpuInfo->ftKernelTimeEnd ) - FileTimeToQuadWord( &pstTempSpecCpuInfo->ftKernelTimeStart );
				qwUserTimeElapsed	= FileTimeToQuadWord( &pstTempSpecCpuInfo->ftUserTimeEnd ) - FileTimeToQuadWord( &pstTempSpecCpuInfo->ftUserTimeStart );
				qwTotalTimeElapsed	+= ( qwKernelTimeElapsed + qwUserTimeElapsed );
			}
		}

		//	1000ns 100 �� CPU ʱ��Ƭ;
		qwTotalTimeElapsed /= 1000 * 100;
	}

	//
	//	���浽��ĳ�Ա������
	//
	m_dwCpuUsedSpec = qwTotalTimeElapsed;

}