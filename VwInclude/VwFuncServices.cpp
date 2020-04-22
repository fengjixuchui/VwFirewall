// VwFuncServices.cpp: implementation of the CVwFunc class.
//
//////////////////////////////////////////////////////////////////////

#include "StdAfx.h"
#include "VwFuncServices.h"
#include <stdio.h>
#include <stdlib.h>

//#include "shlwapi.h"
//#pragma comment( lib, "shlwapi.lib" )



/**
 *	@ ���� ȫ�ֺ���
 *	��� IIS �����Ƿ����
 *	������ʷ��
 *		2007-12-30 �����Ǵ���
 *
 **/
BOOL _vwfunc_enum_services( vector<ENUM_SERVICE_STATUS> & vcAllServices )
{
	BOOL bRet	= FALSE;
	SC_HANDLE hHandle;

	ENUM_SERVICE_STATUS service;
	DWORD dwBytesNeeded		= 0;
	DWORD dwServicesCount		= 0;
	DWORD dwResumedHandle		= 0;
	DWORD dwServiceType		= SERVICE_WIN32;	//	ֻ��ȡ WIN32 �ķ��� | SERVICE_DRIVER;
	BOOL bResult			= FALSE;
	TCHAR szW3Svc[ 64 ]		= { "W3SVC" };
	TCHAR szSrvNameTmp[ 64 ]	= { 0 };
	UINT  i;
	DWORD dwArrLen;
	ENUM_SERVICE_STATUS stService;
	ENUM_SERVICE_STATUS * pArrServices	= NULL;

	hHandle = OpenSCManager( NULL, NULL, SC_MANAGER_ALL_ACCESS );
	if ( NULL == hHandle )
	{
		return FALSE;
	}

	//	...
	vcAllServices.clear();

	//	���ҷ����б�
	bResult = EnumServicesStatus
	(
		hHandle,
		dwServiceType, 
		SERVICE_STATE_ALL,
		&service, sizeof(ENUM_SERVICE_STATUS),
		&dwBytesNeeded, &dwServicesCount, &dwResumedHandle
	);
	if ( 0 == bResult && hHandle )
	{
		//	��ȡӦ��Ҫ���ڴ�ռ�
		if ( ERROR_MORE_DATA == GetLastError() ) 
		{
			//dwArrLen	= dwBytesNeeded / sizeof( ENUM_SERVICE_STATUS );
			dwArrLen	= sizeof( ENUM_SERVICE_STATUS ) + dwBytesNeeded;
			pArrServices	= new ENUM_SERVICE_STATUS[ dwArrLen ];

			if ( pArrServices )
			{
				bRet	= TRUE;

				//	��ȡ�����б�
				EnumServicesStatus
				(
					hHandle, 
					dwServiceType,
					SERVICE_STATE_ALL, 
					pArrServices, dwBytesNeeded,
					&dwBytesNeeded, &dwServicesCount, &dwResumedHandle
				);

				//	��ȡÿ���б�
				for ( i = 0; i < min( dwArrLen, dwServicesCount ); i++ ) 
				{
					stService = *( pArrServices + i );
					vcAllServices.push_back( stService );
				}

				delete [] pArrServices;
				pArrServices = NULL;
			}
		}
		CloseServiceHandle( hHandle );
	}

	return bRet;
}


/**
 *	@ ���� ȫ�ֺ���
 *	��� IIS �����Ƿ����
 *	������ʷ��
 *		2006-7-30 �����Ǵ���
 *
 **/
BOOL _vwfunc_isexist_w3svc( BOOL bRunning )
{
	//
	//	bRunning	- [in] Ҫ�� IIS �Ƿ�Ϊ���е�״̬
	//	����ֵ		- TRUE/FALSE
	//

	BOOL bRet		= FALSE;
	INT  i = 0;
	SC_HANDLE scm;
	SC_HANDLE svc;
	SERVICE_STATUS stServiceStatus;

	scm = OpenSCManager( NULL, NULL, SC_MANAGER_CONNECT );
	if ( scm )
	{
		svc = OpenService( scm, "W3svc", SERVICE_ALL_ACCESS );
		if ( svc )
		{
			if ( bRunning )
			{
				QueryServiceStatus( svc, &stServiceStatus );
				bRet = ( SERVICE_RUNNING == stServiceStatus.dwCurrentState );
			}
			else
			{
				bRet = TRUE;
			}
			CloseServiceHandle( svc );
		}
		
		CloseServiceHandle( scm );
	}

	return bRet;
}

/**
 *	@ ���� ȫ�ֺ���
 *	ֹͣ IIS ����
 *	������ʷ��
 *		2006-7-30 �����Ǵ���
 *
 **/
BOOL _vwfunc_stop_w3svc()
{
	/*
		����ֵ		- TRUE/FALSE
	*/
	
	BOOL bRet		= FALSE;
	BOOL bStopedSucc	= FALSE;
	INT  i = 0;
	SC_HANDLE scm;
	SC_HANDLE svc;
	SERVICE_STATUS stServiceStatus;

	scm = OpenSCManager( NULL, NULL, SC_MANAGER_CONNECT );
	if ( scm )
	{
		svc = OpenService( scm, "W3svc", SERVICE_ALL_ACCESS );
		if ( svc )
		{
			QueryServiceStatus( svc, &stServiceStatus );
			if ( SERVICE_RUNNING == stServiceStatus.dwCurrentState )
			{
				ControlService( svc, SERVICE_CONTROL_STOP, &stServiceStatus );
			}
			for ( i = 0; i < 120; i ++ )
			{
				QueryServiceStatus( svc, &stServiceStatus );
				if ( SERVICE_STOPPED == stServiceStatus.dwCurrentState )
				{
					bStopedSucc = TRUE;
					break;
				}
				Sleep( 2000 );
			}
			CloseServiceHandle( svc );
		}
		
		CloseServiceHandle( scm );
	}
	else
	{
		//MessageBox( "Unable to open Service Control Manager", "Warning", MB_ICONERROR );
	}

	return bStopedSucc;
}


/**
 *	@ ���� ȫ�ֺ���
 *	���� IIS ����
 *	������ʷ��
 *		2006-7-30 �����Ǵ���
 *
 **/
BOOL _vwfunc_start_w3svc()
{
	/*
		����ֵ		- TRUE/FALSE
	*/

	BOOL bRet		= FALSE;
	BOOL bStartSucc		= FALSE;
	INT  i = 0;
	SC_HANDLE scm;
	SC_HANDLE svc;
	SERVICE_STATUS stServiceStatus;

	scm = OpenSCManager( NULL, NULL, SC_MANAGER_CONNECT );
	if ( scm )
	{
		svc = OpenService( scm, "W3svc", SERVICE_ALL_ACCESS );
		if ( svc )
		{
			StartService( svc, 0, NULL );
			// ..
			for ( i = 0; i < 120; i ++ )
			{
				QueryServiceStatus( svc, &stServiceStatus );
				if ( SERVICE_RUNNING == stServiceStatus.dwCurrentState )
				{
					bStartSucc = TRUE;
					break;
				}
				Sleep( 2000 );
			}
			CloseServiceHandle( svc );
		}
		
		CloseServiceHandle(scm);
	}
	else
	{
		//	MessageBox( "Unable to open Service Control Manager", "Warning", MB_ICONERROR );
	}

	return bStartSucc;
}

/**
 *	@ ���� ȫ�ֺ���
 *	���� IIS ����
 *	������ʷ��
 *		2006-7-30 �����Ǵ���
 *
 **/
BOOL _vwfunc_restart_w3svc()
{
	/*
		����ֵ		- TRUE/FALSE
	*/

	if ( _vwfunc_stop_w3svc() )
	{
		return _vwfunc_start_w3svc();
	}

	return FALSE;
}
