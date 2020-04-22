#include "StdAfx.h"
//#include "resource.h"
#include "DeService.h"



//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
CDeService::CDeService()
{
}
CDeService::~CDeService()
{
}

//
//	��������
//
LONG CDeService::Install( IN LPCTSTR lpcszSrvName, IN LPCTSTR lpcszSrvExeFilename, IN LPCTSTR lpcszSrvDesc, IN BOOL bInteractive /* =TRUE */ )
{
	//
	//	lpcszSrvName		- [in] ������
	//	lpcszSrvExeFilename	- [in] �����ִ���ļ���
	//	lpcszSrvDesc		- [in] ��������
	//	RETURN			- TRUE / FALSE
	//

	if ( NULL == lpcszSrvName || NULL == lpcszSrvExeFilename || NULL == lpcszSrvDesc )
	{
		return DESERVICE_ERROR_PARAM;
	}
	if ( 0 == _tcslen(lpcszSrvName) || 0 ==_tcslen(lpcszSrvExeFilename) )
	{
		return DESERVICE_ERROR_PARAM;
	}
	
	LONG lnRet = DESERVICE_ERROR_UNKNOWN;
	SC_HANDLE schSCManager;
	SC_HANDLE schService;
	DWORD dwServiceType;

	if ( bInteractive )
	{
		//	interactive to win32 logon desktop
		dwServiceType = SERVICE_WIN32_OWN_PROCESS | SERVICE_INTERACTIVE_PROCESS;
	}
	else
	{
		dwServiceType = SERVICE_WIN32_OWN_PROCESS;
	}

	schSCManager = OpenSCManager( NULL, NULL, SC_MANAGER_CREATE_SERVICE );
	if ( NULL != schSCManager )
	{
		schService = CreateService
		(
			schSCManager,			//	SCManager database
			lpcszSrvName,			//	name of service
			lpcszSrvName,			//	service name to display
			SERVICE_ALL_ACCESS,		//	desired access
			dwServiceType,			//	service type, org = "SERVICE_WIN32_OWN_PROCESS | SERVICE_INTERACTIVE_PROCESS"
			SERVICE_AUTO_START,		//	start type
			SERVICE_ERROR_NORMAL,		//	error control type
			lpcszSrvExeFilename,		//	service's binary
			NULL,				//	no load ordering group
			NULL,				//	no tag identifier
			NULL,				//	no dependencies
			NULL,				//	LocalSystem account
			NULL				//	no password
		);
		if ( NULL != schService )
		{
			lnRet = DESERVICE_ERROR_SUCC;

			//	Set Service Description
			SetDescription( lpcszSrvName, lpcszSrvDesc );

			CloseServiceHandle( schService );
		}
		else
		{
			lnRet = DESERVICE_ERROR_CREATE_SERVICE;
		}

		CloseServiceHandle( schSCManager );
	}
	else
	{
		//	�޷��򿪷������̨
		lnRet = DESERVICE_ERROR_OPEN_SC_MANAGER;
	}
	
	return lnRet;
}

//
//	��������
//
LONG CDeService::Uninstall( IN LPCTSTR lpcszSrvName )
{
	//
	//	lpcszSrvName	- [in] ������
	//	lpszError	- [out] �������
	//	RETURN		- TRUE / FALSE
	//
	
	if ( NULL == lpcszSrvName )
	{
		return DESERVICE_ERROR_PARAM;
	}
	if ( 0 == _tcslen(lpcszSrvName) )
	{
		return DESERVICE_ERROR_PARAM;
	}

	LONG lnRet = DESERVICE_ERROR_UNKNOWN;
	SC_HANDLE schSCManager = OpenSCManager( NULL, NULL, SC_MANAGER_ALL_ACCESS );
	if ( NULL != schSCManager )
	{
		SC_HANDLE schService = OpenService( schSCManager, lpcszSrvName, SERVICE_ALL_ACCESS );
		if ( NULL != schService )
		{
			if ( DeleteService( schService ) )
			{
				//	�ɹ�
				lnRet = DESERVICE_ERROR_SUCC;
			}
			else
			{
				//	ж�ط���ʧ��
				lnRet = DESERVICE_ERROR_DELETE_SERVICE;
			}

			CloseServiceHandle( schService ); 
		}
		else
		{
			//	���񲻴���
			lnRet = DESERVICE_ERROR_OPEN_SERVICE;
		}
		CloseServiceHandle( schSCManager );
	}
	else
	{
		//	�޷��򿪷������̨
		lnRet = DESERVICE_ERROR_OPEN_SC_MANAGER;
	}
	
	return lnRet;
}

//
//	��������
//
LONG CDeService::Start( IN LPCTSTR lpcszSrvName )
{
	//
	//	lpcszSrvName	- [in] ������
	//	lpszError	- [out] �������
	//	RETURN		- DESERVICE_ERROR_SUCC or ...
	//
	if ( NULL == lpcszSrvName )
	{
		return DESERVICE_ERROR_PARAM;
	}
	if ( 0 == _tcslen(lpcszSrvName) )
	{
		return DESERVICE_ERROR_PARAM;
	}

	LONG lnRet = DESERVICE_ERROR_UNKNOWN;
	SC_HANDLE scm;
	SC_HANDLE svc;
	
	scm = OpenSCManager( NULL,NULL, SC_MANAGER_CONNECT );
	if ( NULL != scm )
	{
		svc = OpenService( scm, lpcszSrvName, SERVICE_START );
		if ( NULL != svc )
		{
			if ( StartService( svc, 0, NULL ) )
			{
				lnRet = DESERVICE_ERROR_SUCC;
			}
			else
			{
				lnRet = DESERVICE_ERROR_START_SERVICE;
			}
			CloseServiceHandle( svc );
		}
		else
		{
			//	���񲻴���
			lnRet = DESERVICE_ERROR_OPEN_SERVICE;
		}
		CloseServiceHandle(scm);
	}
	else
	{
		//	�޷��򿪷������̨
		lnRet = DESERVICE_ERROR_OPEN_SC_MANAGER;
	}
	
	return lnRet;
}

//
//	��������
//
LONG CDeService::Stop( IN LPCTSTR lpcszSrvName )
{
	//
	//	lpcszSrvName	- [in] ������
	//	RETURN		- DESERVICE_ERROR_SUCC or ...
	//
	if ( NULL == lpcszSrvName )
	{
		return DESERVICE_ERROR_PARAM;
	}
	if ( 0 == _tcslen(lpcszSrvName) )
	{
		return DESERVICE_ERROR_PARAM;
	}

	LONG lnRet = DESERVICE_ERROR_UNKNOWN;
	SC_HANDLE scm;
	SC_HANDLE svc;
	SERVICE_STATUS ServiceStatus;
	
	scm = OpenSCManager( NULL, NULL, SC_MANAGER_CONNECT );
	if ( NULL != scm )
	{
		svc = OpenService( scm, lpcszSrvName, SERVICE_ALL_ACCESS );
		if ( NULL != svc )
		{
			if ( QueryServiceStatus( svc, &ServiceStatus ) )
			{
				if ( SERVICE_RUNNING == ServiceStatus.dwCurrentState )
				{
					if ( ControlService( svc, SERVICE_CONTROL_STOP, &ServiceStatus ) )
					{
						//	ֹͣ�ɹ�
						lnRet = DESERVICE_ERROR_SUCC;
					}
					else
					{
						//	ֹͣʧ��
						lnRet = DESERVICE_ERROR_STOP_SERVICE;
					}
				}
				else
				{
					//	�����Ѿ�ֹͣ��
					lnRet = DESERVICE_ERROR_SERVICE_ALREADY_STOPED;
				}
			}
			else
			{
				//	��ѯ����״̬ʧ��
				lnRet = DESERVICE_ERROR_QUERY_SERVICE_STATUS;
			}
			CloseServiceHandle( svc );
		}
		else
		{
			//	���񲻴���
			lnRet = DESERVICE_ERROR_OPEN_SERVICE;
		}
		CloseServiceHandle( scm );
	}
	else
	{
		//	�޷��򿪷������̨
		lnRet = DESERVICE_ERROR_OPEN_SC_MANAGER;
	}
	
	return lnRet;
}

//
//	��ȡ����״̬
//
LONG CDeService::GetStatus( IN LPCTSTR lpcszSrvName )
{
	//
	//	lpcszSrvName	- [in] ��������
	//	RETURN		- SERVICE_STOPPED | SERVICE_START_PENDING | SERVICE_STOP_PENDING,
	//			  SERVICE_RUNNING | SERVICE_CONTINUE_PENDING | SERVICE_PAUSE_PENDING | SERVICE_PAUSED
	//
	
	if ( NULL == lpcszSrvName )
	{
		return DESERVICE_ERROR_PARAM;
	}
	if ( 0 == _tcslen(lpcszSrvName) )
	{
		return DESERVICE_ERROR_PARAM;
	}

	LONG lnRet = DESERVICE_ERROR_UNKNOWN;
	SC_HANDLE scm;
	SC_HANDLE svc;
	SERVICE_STATUS ServiceStatus;

	scm = OpenSCManager( NULL, NULL, SC_MANAGER_CONNECT );
	if ( NULL != scm )
	{
		svc = OpenService( scm, lpcszSrvName, SERVICE_ALL_ACCESS );
		if ( NULL != svc )
		{
			if ( QueryServiceStatus( svc, &ServiceStatus ) )
			{
				lnRet = ServiceStatus.dwCurrentState;
			}
			else
			{
				lnRet = DESERVICE_ERROR_QUERY_SERVICE_STATUS;
			}

			CloseServiceHandle( svc );
		}
		else
		{
			//	���񲻴���
			lnRet = DESERVICE_ERROR_OPEN_SERVICE;
		}
		CloseServiceHandle( scm );
	}
	else
	{
		//	�޷��򿪷������̨
		lnRet = DESERVICE_ERROR_OPEN_SC_MANAGER;
	}
	
	return lnRet;
}

//
//	��ȡ������������·��
//
LONG CDeService::GetFilepath( IN LPCTSTR lpcszSrvName, OUT LPTSTR lpszSrvExeFilename, IN DWORD dwSize )
{
	//
	//	lpcszSrvName		- [in] ��������
	//	lpszSrvExeFilename	- [out] ��������������·�������ܰ�������
	//	dwSize			- [in] size of lpszSrvExeFilename
	//	RETURN			- DESERVICE_ERROR_SUCC or ...
	//
	
	if ( NULL == lpcszSrvName )
	{
		return DESERVICE_ERROR_PARAM;
	}
	if ( 0 == _tcslen(lpcszSrvName) )
	{
		return DESERVICE_ERROR_PARAM;
	}

	LONG lnRet = DESERVICE_ERROR_UNKNOWN;
	SC_HANDLE scm;
	SC_HANDLE svc;

	BOOL bQuerySucc				= FALSE;
	LPQUERY_SERVICE_CONFIG lpServiceConfig	= NULL;
	DWORD dwBytesNeeded			= 0;


	scm = OpenSCManager( NULL, NULL, SC_MANAGER_CONNECT );
	if ( NULL != scm )
	{
		svc = OpenService( scm, lpcszSrvName, SERVICE_ALL_ACCESS );
		if ( NULL != svc )
		{
			//
			//	Query Service Config
			//
			lpServiceConfig	= (LPQUERY_SERVICE_CONFIG)LocalAlloc( LPTR, 1 );
			if ( lpServiceConfig )
			{
				if ( 0 == QueryServiceConfig( svc, lpServiceConfig, 1, & dwBytesNeeded ) )
				{
					LocalFree( lpServiceConfig );
					lpServiceConfig = NULL;

					lpServiceConfig = (LPQUERY_SERVICE_CONFIG)LocalAlloc( LPTR, dwBytesNeeded );
					if ( lpServiceConfig )
					{
						if ( QueryServiceConfig( svc, lpServiceConfig, dwBytesNeeded, & dwBytesNeeded ) )
						{
							lnRet = DESERVICE_ERROR_SUCC;

							//	set return value
							_sntprintf( lpszSrvExeFilename, dwSize-sizeof(TCHAR), _T("%s"), lpServiceConfig->lpBinaryPathName );
						}
						
						LocalFree( lpServiceConfig );
						lpServiceConfig = NULL;
					}
				}
			}
			
			CloseServiceHandle( svc );
		}
		else
		{
			//	���񲻴���
			lnRet = DESERVICE_ERROR_OPEN_SERVICE;
		}
		CloseServiceHandle( scm );
	}
	else
	{
		//	�޷��򿪷������̨
		lnRet = DESERVICE_ERROR_OPEN_SC_MANAGER;
	}
	
	return lnRet;
}

//
//	�������Ƿ����
//
BOOL CDeService::IsExist( IN LPCTSTR lpcszSrvName )
{
	//
	//	lpcszSrvName	- [in] ��������
	//	RETURN		- 1 / 0
	//

	if ( NULL == lpcszSrvName )
	{
		return FALSE;
	}
	if ( 0 == _tcslen(lpcszSrvName) )
	{
		return FALSE;
	}

	BOOL bRet = FALSE;
	SC_HANDLE scm;
	SC_HANDLE svc;
	
	scm = OpenSCManager( NULL, NULL, SC_MANAGER_CONNECT );
	if ( NULL != scm )
	{
		svc = OpenService( scm, lpcszSrvName, SERVICE_ALL_ACCESS );
		if ( NULL != svc )
		{
			bRet = TRUE;
			CloseServiceHandle( svc );
		}

		CloseServiceHandle( scm );
	}

	return bRet;
}

//
//	���÷�������
//
LONG CDeService::SetDescription( IN LPCTSTR lpcszSrvName, IN LPCTSTR lpcszSrvDesc )
{
	//
	//	lpcszSrvName	- [in] ������
	//	lpcszSrvDesc	- [in] ��������
	//	RETURN		- TRUE / FALSE
	//
	if ( NULL == lpcszSrvName || NULL == lpcszSrvDesc )
	{
		return DESERVICE_ERROR_PARAM;
	}
	if ( 0 == _tcslen(lpcszSrvName) )
	{
		return DESERVICE_ERROR_PARAM;
	}

	TCHAR szKeyPath[ MAX_PATH ]		= {0};
	TCHAR szKeyValue[ MAX_PATH ]		= {0};
	DWORD dwSize				= 0;
	
	//	��ӷ����������Ϣ
	_sntprintf
	(
		szKeyPath, sizeof(szKeyPath)-sizeof(TCHAR), 
		_T("SYSTEM\\ControlSet001\\Services\\%s"), _T(lpcszSrvName)
	);
	_sntprintf( szKeyValue, sizeof(szKeyValue)-sizeof(TCHAR), _T("%s"), lpcszSrvDesc );
	dwSize = sizeof(szKeyValue);
	SHSetValue( HKEY_LOCAL_MACHINE, _T(szKeyPath), _T("Description"), REG_SZ, szKeyValue, dwSize );

	return DESERVICE_ERROR_SUCC;
}
