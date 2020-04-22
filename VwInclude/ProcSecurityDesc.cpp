// ProcSecurityDesc.cpp: implementation of the CProcSecurityDesc class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "ProcSecurityDesc.h"

#include <stdio.h>
#include <stdlib.h>





//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CProcSecurityDesc::CProcSecurityDesc()
{

}

CProcSecurityDesc::~CProcSecurityDesc()
{

}

/**
 *	@ Public
 *	����ĳ��Ŀ¼�����ļ��İ�ȫ����Ϊ Everyone �ɶ�
 */
BOOL CProcSecurityDesc::SetSecurityDescForEveryone( LPCTSTR lpcszPath, LPTSTR pszError )
{
	return CreateEveryoneSecurityDesc( lpcszPath, GENERIC_READ|GENERIC_EXECUTE, pszError );
}

/**
 *	@ Public
 *	����ĳ��Ŀ¼�����ļ��İ�ȫ����Ϊ IIS �ɿ���
 */
BOOL CProcSecurityDesc::SetSecurityDescForFilter( LPCTSTR lpcszPath, LPTSTR pszError, BOOL bPermissionsEveryoneWritable /*= FALSE*/ )
{
	return CreateIISSecurityDesc( lpcszPath, pszError, bPermissionsEveryoneWritable );
}

/**
 *	@ Public
 *	��ȡ Everyone ��ȫ����������
 */
BOOL CProcSecurityDesc::GetEveryoneSecurityAttributesData( LPSECURITY_ATTRIBUTES lpFileMappingAttributes, LPTSTR pszError )
{
	PSID pEveryoneSID				= NULL;
	PSID pAdminSID					= NULL;
	PACL pACL					= NULL;
	PSECURITY_DESCRIPTOR pSD			= NULL;
	EXPLICIT_ACCESS ea[ EXPLICIT_ENTRIES_NUM_EVERYONE ]	= {0};		// ��ȫ��������
	SID_IDENTIFIER_AUTHORITY SIDAuthWorld		= SECURITY_WORLD_SID_AUTHORITY;
	SID_IDENTIFIER_AUTHORITY SIDAuthNT		= SECURITY_NT_AUTHORITY;
	SID_IDENTIFIER_AUTHORITY SIDAuthLocal		= SECURITY_LOCAL_SID_AUTHORITY;
//	SECURITY_ATTRIBUTES sa;
	DWORD dwErrorCode				= 0;
	BOOL bIsSuccess					= FALSE;
	DWORD dwAccessPms				= GENERIC_READ|GENERIC_EXECUTE|GENERIC_WRITE;


	lpFileMappingAttributes	= NULL;

	__try
	{
		////////////////////////////////////////////////////////////
		// ��ʼ�� ��ȫ������
		memset( &ea, 0, EXPLICIT_ENTRIES_NUM_EVERYONE * sizeof(EXPLICIT_ACCESS) );

		////////////////////////////////////////////////////////////
		// 1.1 - Everyone
		// Create a well-known SID for the Everyone group
		bIsSuccess = FALSE;
		bIsSuccess = AllocateAndInitializeSid
			(
				&SIDAuthWorld,		// ���� SID �ṹ���ַ
				1,			// Ҫ��ʼ�����漸������Ȩ���
				SECURITY_WORLD_RID,	// �����������ָ����Ҫ��ʼ���ġ���Ȩ��������µ� 7 ����������
				0, 0, 0, 0, 0, 0, 0,	// ������ʣ�µ� 7 ������Ȩ���
				&pEveryoneSID
			);
		if( FALSE == bIsSuccess )
		{
			dwErrorCode = GetLastError();
			_sntprintf( pszError, MAX_PATH-sizeof(TCHAR), _T("�޷����� Everyone �� SID������%d"), dwErrorCode );
			return FALSE;
		}

		// 1.2
		// Initialize an EXPLICIT_ACCESS structure for an ACE.
		// The ACE will allow Everyone read access to the key.

		ea[0].grfAccessPermissions	= dwAccessPms;	//GENERIC_ALL;	// �� Everyone ��дȨ��
		ea[0].grfAccessMode		= SET_ACCESS;
		ea[0].grfInheritance		= SUB_CONTAINERS_AND_OBJECTS_INHERIT;	// <*** ��� ACE
		ea[0].Trustee.TrusteeForm	= TRUSTEE_IS_SID;
		ea[0].Trustee.TrusteeType	= TRUSTEE_IS_WELL_KNOWN_GROUP;
		ea[0].Trustee.ptstrName		= (LPTSTR)pEveryoneSID;

		////////////////////////////////////////////////////////////
		// 2.1 - Administrators
		// Create a SID for the BUILTIN\Administrators group.
		bIsSuccess = FALSE;
		bIsSuccess = AllocateAndInitializeSid
			(
				&SIDAuthNT,			// ���� SID �ṹ���ַ
				2,				// Ҫ��ʼ�����漸������Ȩ���
				SECURITY_BUILTIN_DOMAIN_RID,	// ����ָ����Ҫ��ʼ���ĵ� 1 ������Ȩ���
				DOMAIN_ALIAS_RID_ADMINS,	// ����ָ����Ҫ��ʼ���ĵ� 2 ������Ȩ���
				0, 0, 0, 0, 0, 0,		// ������ʣ�µ� 6 ������Ȩ���
				&pAdminSID
			);
		if ( FALSE == bIsSuccess )
		{
			dwErrorCode = GetLastError();
			_sntprintf( pszError, MAX_PATH-sizeof(TCHAR), _T("�޷����� Administrators �� SID������%d"), dwErrorCode );
			return FALSE;
		}

		// 2.2
		// Initialize an EXPLICIT_ACCESS structure for an ACE.
		// The ACE will allow the Administrators group full access to the key.

		ea[1].grfAccessPermissions	= SPECIFIC_RIGHTS_ALL | STANDARD_RIGHTS_ALL;
		ea[1].grfAccessMode		= SET_ACCESS;
		ea[1].grfInheritance		= SUB_CONTAINERS_AND_OBJECTS_INHERIT;	// <*** ��� ACE
		ea[1].Trustee.TrusteeForm	= TRUSTEE_IS_SID;
		ea[1].Trustee.TrusteeType	= TRUSTEE_IS_GROUP;
		ea[1].Trustee.ptstrName		= (LPTSTR)pAdminSID;


		////////////////////////////////////////////////////////////
		// Create a new ACL that contains the new ACEs.
		if ( ERROR_SUCCESS != SetEntriesInAcl( EXPLICIT_ENTRIES_NUM_EVERYONE, ea, NULL, &pACL ) )
		{
			dwErrorCode = GetLastError();
			_sntprintf( pszError, MAX_PATH-sizeof(TCHAR), _T("�޷������� ACL������%d"), dwErrorCode );
			return FALSE;
		}

		// Initialize a security descriptor.  

		pSD = (PSECURITY_DESCRIPTOR)LocalAlloc( LPTR, SECURITY_DESCRIPTOR_MIN_LENGTH );
		if ( pSD == NULL )
		{
			dwErrorCode = GetLastError();
			_sntprintf( pszError, MAX_PATH-sizeof(TCHAR), _T("�޷�Ϊ��ȫ�����������ڴ�ռ䣬����%d"), dwErrorCode );
			return FALSE;
		}

		if ( ! InitializeSecurityDescriptor( pSD, SECURITY_DESCRIPTOR_REVISION ) )
		{
			dwErrorCode = GetLastError();
			_sntprintf( pszError, MAX_PATH-sizeof(TCHAR), _T("�޷���ʼ����ȫ������������%d"), dwErrorCode );
			return FALSE;
		}

		// Add the ACL to the security descriptor.
		bIsSuccess = FALSE;
		bIsSuccess = SetSecurityDescriptorDacl
			(
				pSD,
				TRUE,		// fDaclPresent flag
				pACL,
				FALSE		// not a default DACL
			);
		if ( FALSE == bIsSuccess )
		{
			dwErrorCode = GetLastError();
			_sntprintf( pszError, MAX_PATH-sizeof(TCHAR), _T("�޷��� ACL ��ӵ���ȫ�������У�����%d"), dwErrorCode );
			return FALSE;
		}

		// Initialize a security attributes structure.
		lpFileMappingAttributes->nLength		= sizeof(SECURITY_ATTRIBUTES);
		lpFileMappingAttributes->lpSecurityDescriptor	= pSD;
		lpFileMappingAttributes->bInheritHandle		= FALSE;
	}
	__finally
	{
		if ( pEveryoneSID )
			FreeSid( pEveryoneSID );

		if ( pAdminSID )
			FreeSid( pAdminSID );

		if ( pACL )
			LocalFree( pACL );

		if ( pSD )
			LocalFree(pSD);
	}


	return TRUE;
}

/**
 *	��� Local System + Administrator Ȩ��
 */
BOOL CProcSecurityDesc::GetLocalSystemSecurityAttributesData( LPSECURITY_ATTRIBUTES lpFileMappingAttributes, LPTSTR pszError )
{
	PSID pSystemSID					= NULL;
	PSID pAdminSID					= NULL;
	PACL pACL					= NULL;
	PSECURITY_DESCRIPTOR pSD			= NULL;
	EXPLICIT_ACCESS ea[ 2 ]				= {0};		// ��ȫ��������
//	SID_IDENTIFIER_AUTHORITY SIDAuthWorld		= SECURITY_WORLD_SID_AUTHORITY;
	SID_IDENTIFIER_AUTHORITY SIDAuthNT		= SECURITY_NT_AUTHORITY;
	SID_IDENTIFIER_AUTHORITY SIDAuthLocal		= SECURITY_LOCAL_SID_AUTHORITY;
//	SECURITY_ATTRIBUTES sa;
	DWORD dwErrorCode				= 0;
	BOOL bIsSuccess					= FALSE;
	DWORD dwAccessPms				= GENERIC_READ|GENERIC_EXECUTE|GENERIC_WRITE;


	lpFileMappingAttributes	= NULL;

	__try
	{
		////////////////////////////////////////////////////////////
		// ��ʼ�� ��ȫ������
		memset( &ea, 0, 2 * sizeof(EXPLICIT_ACCESS) );

		////////////////////////////////////////////////////////////
		// 1.1 - System
		bIsSuccess = FALSE;
		bIsSuccess &= AllocateAndInitializeSid
			(
				&SIDAuthNT,			//	This structure provides the top-level identifier authority value to set in the SID ���� SID �ṹ���ַ
				1,				//	Ҫ��ʼ�����漸������Ȩ���
				SECURITY_LOCAL_SYSTEM_RID,	//	�����������ָ����Ҫ��ʼ���ġ���Ȩ��������µ� 7 ����������
				0, 0, 0, 0, 0, 0, 0,		//	������ʣ�µ� 7 ������Ȩ���
				&pSystemSID
			);
		if( FALSE == bIsSuccess )
		{
			dwErrorCode = GetLastError();
			_sntprintf( pszError, MAX_PATH-sizeof(TCHAR), _T("�޷����� System �� SID������%d"), dwErrorCode );
			return FALSE;
		}

		// 1.2
		// Initialize an EXPLICIT_ACCESS structure for an ACE.
		// The ACE will allow Everyone read access to the key.

	//	ea[0].grfAccessPermissions	= dwAccessPms;	//GENERIC_ALL;	// �� Everyone ��дȨ��
	//	ea[0].grfAccessMode		= SET_ACCESS;
	//	ea[0].grfInheritance		= SUB_CONTAINERS_AND_OBJECTS_INHERIT;	// <*** ��� ACE
	//	ea[0].Trustee.TrusteeForm	= TRUSTEE_IS_SID;
	//	ea[0].Trustee.TrusteeType	= TRUSTEE_IS_WELL_KNOWN_GROUP;
	//	ea[0].Trustee.ptstrName		= (LPTSTR)pSystemSID;


		ea[0].grfAccessPermissions	= SPECIFIC_RIGHTS_ALL | STANDARD_RIGHTS_ALL;	// GENERIC_ALL;	// �� SYSTEM ��дȨ��
		ea[0].grfAccessMode		= SET_ACCESS;
		ea[0].grfInheritance		= SUB_CONTAINERS_AND_OBJECTS_INHERIT;	// <*** ��� ACE
		ea[0].Trustee.TrusteeForm	= TRUSTEE_IS_SID;
		ea[0].Trustee.TrusteeType	= TRUSTEE_IS_WELL_KNOWN_GROUP;
		ea[0].Trustee.ptstrName		= (LPTSTR)pSystemSID;


		////////////////////////////////////////////////////////////
		// 2.1 - Administrators
		// Create a SID for the BUILTIN\Administrators group.
		bIsSuccess = FALSE;
		bIsSuccess = AllocateAndInitializeSid
			(
				&SIDAuthNT,			// ���� SID �ṹ���ַ
				2,				// Ҫ��ʼ�����漸������Ȩ���
				SECURITY_BUILTIN_DOMAIN_RID,	// ����ָ����Ҫ��ʼ���ĵ� 1 ������Ȩ���
				DOMAIN_ALIAS_RID_ADMINS,	// ����ָ����Ҫ��ʼ���ĵ� 2 ������Ȩ���
				0, 0, 0, 0, 0, 0,		// ������ʣ�µ� 6 ������Ȩ���
				&pAdminSID
			);
		if ( FALSE == bIsSuccess )
		{
			dwErrorCode = GetLastError();
			_sntprintf( pszError, MAX_PATH-sizeof(TCHAR), _T("�޷����� Administrators �� SID������%d"), dwErrorCode );
			return FALSE;
		}

		// 2.2
		// Initialize an EXPLICIT_ACCESS structure for an ACE.
		// The ACE will allow the Administrators group full access to the key.

		ea[1].grfAccessPermissions	= SPECIFIC_RIGHTS_ALL | STANDARD_RIGHTS_ALL;
		ea[1].grfAccessMode		= SET_ACCESS;
		ea[1].grfInheritance		= SUB_CONTAINERS_AND_OBJECTS_INHERIT;	// <*** ��� ACE
		ea[1].Trustee.TrusteeForm	= TRUSTEE_IS_SID;
		ea[1].Trustee.TrusteeType	= TRUSTEE_IS_GROUP;
		ea[1].Trustee.ptstrName		= (LPTSTR)pAdminSID;


		////////////////////////////////////////////////////////////
		// Create a new ACL that contains the new ACEs.
		if ( ERROR_SUCCESS != SetEntriesInAcl( EXPLICIT_ENTRIES_NUM_EVERYONE, ea, NULL, &pACL ) )
		{
			dwErrorCode = GetLastError();
			_sntprintf( pszError, MAX_PATH-sizeof(TCHAR), _T("�޷������� ACL������%d"), dwErrorCode );
			return FALSE;
		}

		// Initialize a security descriptor.  

		pSD = (PSECURITY_DESCRIPTOR)LocalAlloc( LPTR, SECURITY_DESCRIPTOR_MIN_LENGTH );
		if ( pSD == NULL )
		{
			dwErrorCode = GetLastError();
			_sntprintf( pszError, MAX_PATH-sizeof(TCHAR), _T("�޷�Ϊ��ȫ�����������ڴ�ռ䣬����%d"), dwErrorCode );
			return FALSE;
		}

		if ( ! InitializeSecurityDescriptor( pSD, SECURITY_DESCRIPTOR_REVISION ) )
		{
			dwErrorCode = GetLastError();
			_sntprintf( pszError, MAX_PATH-sizeof(TCHAR), _T("�޷���ʼ����ȫ������������%d"), dwErrorCode );
			return FALSE;
		}

		// Add the ACL to the security descriptor.
		bIsSuccess = FALSE;
		bIsSuccess = SetSecurityDescriptorDacl
			(
				pSD,
				TRUE,		// fDaclPresent flag
				pACL,
				FALSE		// not a default DACL
			);
		if ( FALSE == bIsSuccess )
		{
			dwErrorCode = GetLastError();
			_sntprintf( pszError, MAX_PATH-sizeof(TCHAR), _T("�޷��� ACL ��ӵ���ȫ�������У�����%d"), dwErrorCode );
			return FALSE;
		}

		// Initialize a security attributes structure.
		lpFileMappingAttributes->nLength		= sizeof(SECURITY_ATTRIBUTES);
		lpFileMappingAttributes->lpSecurityDescriptor	= pSD;
		lpFileMappingAttributes->bInheritHandle		= FALSE;
	}
	__finally
	{
		if ( pSystemSID )
		{
			FreeSid( pSystemSID );
			pSystemSID = NULL;
		}

		if ( pAdminSID )
		{
			FreeSid( pAdminSID );
			pAdminSID = NULL;
		}

		if ( pACL )
		{
			LocalFree( pACL );
			pACL = NULL;
		}

		if ( pSD )
		{
			LocalFree( pSD );
			pSD = NULL;
		}
	}

	return TRUE;
}



//////////////////////////////////////////////////////////////////////////
//
// ���� NTFS ���� EveryOne ���Ե�Ŀ¼
//
BOOL CProcSecurityDesc::CreateEveryoneSecurityDesc( LPCTSTR lpszPath, DWORD dwAccessPms, LPTSTR pszError )
{
	////////////////////////////////////////
	// ���½���Ŀ¼���ϰ�ȫ����
	PSID pEveryoneSID				= NULL;
	PSID pAdminSID					= NULL;
	PACL pACL					= NULL;
	PSECURITY_DESCRIPTOR pSD			= NULL;
	EXPLICIT_ACCESS ea[ EXPLICIT_ENTRIES_NUM_EVERYONE ]	= {0};		// ��ȫ��������
	SID_IDENTIFIER_AUTHORITY SIDAuthWorld		= SECURITY_WORLD_SID_AUTHORITY;
	SID_IDENTIFIER_AUTHORITY SIDAuthNT		= SECURITY_NT_AUTHORITY;
	SID_IDENTIFIER_AUTHORITY SIDAuthLocal		= SECURITY_LOCAL_SID_AUTHORITY;
	SECURITY_ATTRIBUTES sa;
	DWORD dwErrorCode				= 0;
	BOOL bIsSuccess					= FALSE;


	__try
	{

		////////////////////////////////////////////////////////////
		// ��ʼ�� ��ȫ������
		memset( &ea, 0, EXPLICIT_ENTRIES_NUM_EVERYONE * sizeof(EXPLICIT_ACCESS) );


		////////////////////////////////////////////////////////////
		// 1.1 - Everyone
		// Create a well-known SID for the Everyone group
		bIsSuccess = FALSE;
		bIsSuccess = AllocateAndInitializeSid
			(
				&SIDAuthWorld,		// ���� SID �ṹ���ַ
				1,			// Ҫ��ʼ�����漸������Ȩ���
				SECURITY_WORLD_RID,	// �����������ָ����Ҫ��ʼ���ġ���Ȩ��������µ� 7 ����������
				0, 0, 0, 0, 0, 0, 0,	// ������ʣ�µ� 7 ������Ȩ���
				&pEveryoneSID
			);
		if( FALSE == bIsSuccess )
		{
			dwErrorCode = GetLastError();
			_sntprintf( pszError, MAX_PATH-sizeof(TCHAR), _T("�޷����� Everyone �� SID������%d"), dwErrorCode );
			return FALSE;
		}

		// 1.2
		// Initialize an EXPLICIT_ACCESS structure for an ACE.
		// The ACE will allow Everyone read access to the key.

		ea[0].grfAccessPermissions	= dwAccessPms;	//GENERIC_ALL;	// �� Everyone ��дȨ��
		ea[0].grfAccessMode		= SET_ACCESS;
		ea[0].grfInheritance		= SUB_CONTAINERS_AND_OBJECTS_INHERIT;	// <*** ��� ACE
		ea[0].Trustee.TrusteeForm	= TRUSTEE_IS_SID;
		ea[0].Trustee.TrusteeType	= TRUSTEE_IS_WELL_KNOWN_GROUP;
		ea[0].Trustee.ptstrName		= (LPTSTR)pEveryoneSID;


		////////////////////////////////////////////////////////////
		// 2.1 - Administrators
		// Create a SID for the BUILTIN\Administrators group.
		bIsSuccess = FALSE;
		bIsSuccess = AllocateAndInitializeSid
			(
				&SIDAuthNT,			// ���� SID �ṹ���ַ
				2,				// Ҫ��ʼ�����漸������Ȩ���
				SECURITY_BUILTIN_DOMAIN_RID,	// ����ָ����Ҫ��ʼ���ĵ� 1 ������Ȩ���
				DOMAIN_ALIAS_RID_ADMINS,	// ����ָ����Ҫ��ʼ���ĵ� 2 ������Ȩ���
				0, 0, 0, 0, 0, 0,		// ������ʣ�µ� 6 ������Ȩ���
				&pAdminSID
			);
		if ( FALSE == bIsSuccess ) {
			dwErrorCode = GetLastError();
			_sntprintf( pszError, MAX_PATH-sizeof(TCHAR), _T("�޷����� Administrators �� SID������%d"), dwErrorCode );
			return FALSE;
		}

		// 2.2
		// Initialize an EXPLICIT_ACCESS structure for an ACE.
		// The ACE will allow the Administrators group full access to the key.

		ea[1].grfAccessPermissions	= SPECIFIC_RIGHTS_ALL | STANDARD_RIGHTS_ALL;
		ea[1].grfAccessMode		= SET_ACCESS;
		ea[1].grfInheritance		= SUB_CONTAINERS_AND_OBJECTS_INHERIT;	// <*** ��� ACE
		ea[1].Trustee.TrusteeForm	= TRUSTEE_IS_SID;
		ea[1].Trustee.TrusteeType	= TRUSTEE_IS_GROUP;
		ea[1].Trustee.ptstrName		= (LPTSTR)pAdminSID;



		////////////////////////////////////////////////////////////
		// Create a new ACL that contains the new ACEs.
		if ( ERROR_SUCCESS != SetEntriesInAcl( EXPLICIT_ENTRIES_NUM_EVERYONE, ea, NULL, &pACL ) )
		{
			dwErrorCode = GetLastError();
			_sntprintf( pszError, MAX_PATH-sizeof(TCHAR), _T("�޷������� ACL������%d"), dwErrorCode );
			return FALSE;
		}

		// Initialize a security descriptor.  

		pSD = (PSECURITY_DESCRIPTOR)LocalAlloc( LPTR, SECURITY_DESCRIPTOR_MIN_LENGTH );
		if ( pSD == NULL ) {
			dwErrorCode = GetLastError();
			_sntprintf( pszError, MAX_PATH-sizeof(TCHAR), _T("�޷�Ϊ��ȫ�����������ڴ�ռ䣬����%d"), dwErrorCode );
			return FALSE;
		}

		if ( ! InitializeSecurityDescriptor( pSD, SECURITY_DESCRIPTOR_REVISION ) ) {
			dwErrorCode = GetLastError();
			_sntprintf( pszError, MAX_PATH-sizeof(TCHAR), _T("�޷���ʼ����ȫ������������%d"), dwErrorCode );
			return FALSE;
		}

		// Add the ACL to the security descriptor.
		bIsSuccess = FALSE;
		bIsSuccess = SetSecurityDescriptorDacl(
				pSD,
				TRUE,		// fDaclPresent flag
				pACL,
				FALSE );	// not a default DACL
		if ( FALSE == bIsSuccess ) {
			dwErrorCode = GetLastError();
			_sntprintf( pszError, MAX_PATH-sizeof(TCHAR), _T("�޷��� ACL ��ӵ���ȫ�������У�����%d"), dwErrorCode );
			return FALSE;
		}

		// Initialize a security attributes structure.

		sa.nLength		= sizeof (SECURITY_ATTRIBUTES);
		sa.lpSecurityDescriptor	= pSD;
		sa.bInheritHandle	= FALSE;


		dwErrorCode = 0;
		if ( FALSE == SetFileSecurity( lpszPath, DACL_SECURITY_INFORMATION, pSD ) ) {
			dwErrorCode = GetLastError();
			_sntprintf( pszError, MAX_PATH-sizeof(TCHAR), _T("�޷�������Ҳ�޷��޸ġ�%s���İ�ȫ���ԣ�����%d"), lpszPath, dwErrorCode );
			return FALSE;
		}

	}
	__finally
	{
		if ( pEveryoneSID )
			FreeSid( pEveryoneSID );

		if ( pAdminSID )
			FreeSid( pAdminSID );

		if ( pACL )
			LocalFree( pACL );

		if ( pSD )
			LocalFree(pSD);
	}


	return TRUE;

}


/**
*	Get Windows system type
*/
ENUMDEWINDOWSSYSTYPE CProcSecurityDesc::GetShellSysType()
{
	ENUMDEWINDOWSSYSTYPE ShellType;
	DWORD dwWinVer;
	//OSVERSIONINFO * osvi;
	OSVERSIONINFOEX stOsVerEx;
	TCHAR szTemp[ MAX_PATH ];
	
	ShellType = _DEOS_UNKNOWN;
	
	__try
	{
		ZeroMemory( & stOsVerEx, sizeof(stOsVerEx) );
		
		dwWinVer = GetVersion();
		if ( dwWinVer < 0x80000000 )
		{
			// NT
			ShellType = _DEOS_WINDOWS_NT3;
			stOsVerEx.dwOSVersionInfoSize = sizeof(OSVERSIONINFOEX);
			if ( GetVersionEx( (OSVERSIONINFO*)&stOsVerEx ) )
			{
				_sntprintf( szTemp, sizeof(szTemp)/sizeof(TCHAR)-1,
					_T("stOsVerEx.dwMajorVersion=%d, stOsVerEx.dwMinorVersion=%d"),
					stOsVerEx.dwMajorVersion,
					stOsVerEx.dwMinorVersion );
				MessageBox( NULL, szTemp, NULL, NULL );

				if ( 4L == stOsVerEx.dwMajorVersion )
				{
					ShellType = _DEOS_WINDOWS_NT4;
				}
				else if ( 5L == stOsVerEx.dwMajorVersion && 0L == stOsVerEx.dwMinorVersion )
				{
					ShellType = _DEOS_WINDOWS_2000;
				}
				else if ( 5L == stOsVerEx.dwMajorVersion && 1L == stOsVerEx.dwMinorVersion )
				{
					ShellType = _DEOS_WINDOWS_XP;
				}
				else if ( 5L == stOsVerEx.dwMajorVersion && 2L == stOsVerEx.dwMinorVersion )
				{
					ShellType = _DEOS_WINDOWS_2003;
				}
				else if ( 6L == stOsVerEx.dwMajorVersion && 1L == stOsVerEx.dwMinorVersion )
				{
					if ( VER_NT_WORKSTATION == stOsVerEx.wProductType )
					{
						//	Windows 7
						//	OSVERSIONINFOEX.wProductType == VER_NT_WORKSTATION
						ShellType = _DEOS_WINDOWS_7;
					}
					else
					{
						//	Windows Server 2008 R2
						//	OSVERSIONINFOEX.wProductType != VER_NT_WORKSTATION
						ShellType = _DEOS_WINDOWS_2008_R2;
					}
				}
				else if ( 6L == stOsVerEx.dwMajorVersion && 0L == stOsVerEx.dwMinorVersion )
				{
					if ( VER_NT_WORKSTATION == stOsVerEx.wProductType )
					{
						//	Windows Vista
						//	OSVERSIONINFOEX.wProductType == VER_NT_WORKSTATION
						ShellType = _DEOS_WINDOWS_VISTA;
					}
					else
					{
						//	Windows Server 2008
						//	OSVERSIONINFOEX.wProductType != VER_NT_WORKSTATION
						ShellType = _DEOS_WINDOWS_2008;
					}
				}
			}
		}
		else if ( LOBYTE(LOWORD(dwWinVer)) < 4 )
		{
			ShellType = _DEOS_WINDOWS_32S;
		}
		else
		{
			ShellType = _DEOS_WINDOWS_95;
			stOsVerEx.dwOSVersionInfoSize = sizeof(OSVERSIONINFO);
			if ( GetVersionEx( (OSVERSIONINFO*) &stOsVerEx ) )
			{
				if ( 4L == stOsVerEx.dwMajorVersion && 10L == stOsVerEx.dwMinorVersion )
				{
					ShellType = _DEOS_WINDOWS_98;
				}
				else if ( 4L == stOsVerEx.dwMajorVersion && 90L == stOsVerEx.dwMinorVersion )
				{
					ShellType = _DEOS_WINDOWS_ME;
				}
			}
		}
	}
	__except( EXCEPTION_EXECUTE_HANDLER )
	{
	}
	
	return ShellType;
}
//////////////////////////////////////////////////////////////////////////
//
// ���� NTFS ���� IIS �������Ե�Ŀ¼
//
/*
 * ------------------------------------------------------------
 *  ��������ǵĵط���
 *  ACE �ļ̳�������ѡ�����£�
 * ------------------------------------------------------------
 *	OBJECT_INHERIT_ACE			- ���ļ��м��ļ�
 *	CONTAINER_INHERIT_ACE			- ���ļ��м����ļ���
 *	NO_PROPAGATE_INHERIT_ACE		- ֻ�и��ļ���
 *	INHERIT_ONLY_ACE			- ֻ���ļ�
 *	INHERITED_ACE
 *	VALID_INHERIT_FLAGS
 *	SUB_CONTAINERS_ONLY_INHERIT		- ֻ�����ļ���
 *	SUB_OBJECTS_ONLY_INHERIT		- ֻ�����ļ�
 *	SUB_CONTAINERS_AND_OBJECTS_INHERIT	- ���ļ��У����ļ��м��ļ�
 */
BOOL CProcSecurityDesc::CreateIISSecurityDesc( LPCTSTR lpcszPath, LPTSTR pszError, BOOL bPermissionsEveryoneWritable /*= FALSE*/ )
{
	//
	//	lpcszPath	- [in]  Pointer to a character null-termined string containing the file path
	//	dwAccessPms	- [in]  Specifies the access to file, for example : GENERIC_READ|GENERIC_EXECUTE|GENERIC_WRITE
	//	pszError	- [out] Pointer to a character null-termined string to receive the error information.
	//

	if ( NULL == lpcszPath || NULL == pszError )
	{
		return FALSE;
	}

	////////////////////////////////////////
	// ���½���Ŀ¼���ϰ�ȫ����
	BOOL  bRet			= FALSE;
	ENUMDEWINDOWSSYSTYPE emOsType;
	PACL pNewACL			= NULL;
	PSECURITY_DESCRIPTOR pSD	= NULL;
	EXPLICIT_ACCESS ea[ EXPLICIT_ENTRIES_NUM_FILTER ]	= {0};		// ��ȫ��������
	SECURITY_ATTRIBUTES sa;
	DWORD dwErrorCode		= 0;
	SECURITY_INFORMATION si		= OWNER_SECURITY_INFORMATION | GROUP_SECURITY_INFORMATION | DACL_SECURITY_INFORMATION | SACL_SECURITY_INFORMATION;


	// Ϊ IIS5 ���� EXPLICIT_ACCESS
	emOsType = delib_get_shellsystype();
	if ( _DEOS_WINDOWS_2003 == emOsType )
	{
		BuildEaFor_Iis60_2003( lpcszPath, ea, sizeof(ea)/sizeof(ea[0]), bPermissionsEveryoneWritable );
	}
	else if ( _DEOS_WINDOWS_2008 == emOsType || _DEOS_WINDOWS_2008_R2 == emOsType || _DEOS_WINDOWS_VISTA == emOsType || _DEOS_WINDOWS_7 == emOsType )
	{
		BuildEaFor_Iis70_2008( lpcszPath, ea, sizeof(ea)/sizeof(ea[0]), bPermissionsEveryoneWritable );
	}
	else
	{
		BuildEaFor_Iis50_2000( lpcszPath, ea, sizeof(ea)/sizeof(ea[0]), bPermissionsEveryoneWritable );
	}


	////////////////////////////////////////////////////////////
	// Create a new ACL that contains the new ACEs.
	// The SetEntriesInAcl function creates a new access control list (ACL) by merging new access control or audit control information into an existing ACL structure.			
	if ( ERROR_SUCCESS == SetEntriesInAcl( EXPLICIT_ENTRIES_NUM_FILTER, ea, NULL, &pNewACL ) )
	{
		// Initialize a security descriptor.
		pSD = (PSECURITY_DESCRIPTOR)LocalAlloc( LPTR, SECURITY_DESCRIPTOR_MIN_LENGTH );
		if ( pSD )
		{
			if ( InitializeSecurityDescriptor( pSD, SECURITY_DESCRIPTOR_REVISION ) )
			{
				// Add the ACL to the security descriptor.
				if ( SetSecurityDescriptorDacl( pSD, TRUE, pNewACL, FALSE ) )
				{
					// Initialize a security attributes structure.
					sa.nLength		= sizeof(sa);
					sa.lpSecurityDescriptor	= pSD;
					sa.bInheritHandle	= FALSE;

					dwErrorCode = 0;
					if ( SetFileSecurity( lpcszPath, DACL_SECURITY_INFORMATION, pSD ) )
					{
						bRet = TRUE;
					}
					else
					{
						dwErrorCode = GetLastError();
						_sntprintf( pszError, MAX_PATH-sizeof(TCHAR), _T("�޷�������Ҳ�޷��޸ġ�%s���İ�ȫ���ԣ�����%d"), lpcszPath, dwErrorCode );
					}
				}
				else
				{
					dwErrorCode = GetLastError();
					_sntprintf( pszError, MAX_PATH-sizeof(TCHAR), _T("�޷��� ACL ��ӵ���ȫ�������У�����%d"), dwErrorCode );
				}
			}
			else
			{
				dwErrorCode = GetLastError();
				_sntprintf( pszError, MAX_PATH-sizeof(TCHAR), _T("�޷���ʼ����ȫ������������%d"), dwErrorCode );
			}

			// ..
			LocalFree(pSD);
		}
		else
		{
			dwErrorCode = GetLastError();
			_sntprintf( pszError, MAX_PATH-sizeof(TCHAR), _T("�޷�Ϊ��ȫ�����������ڴ�ռ䣬����%d"), dwErrorCode );
		}

		// ..
		if ( pNewACL )
			LocalFree( pNewACL );
	}
	else
	{
		dwErrorCode = GetLastError();
		_sntprintf( pszError, MAX_PATH-sizeof(TCHAR), _T("�޷������� ACL������%d"), dwErrorCode );
	}

	// ..
	return bRet;
}



//////////////////////////////////////////////////////////////////////////
// Ϊ IIS 5.0/Win2000 ���� EXPLICIT_ACCESS
// 
BOOL CProcSecurityDesc::BuildEaFor_Iis50_2000( LPCTSTR lpcszPath, EXPLICIT_ACCESS * pea, DWORD dwEaCount, BOOL bPermissionsEveryoneWritable /*= FALSE*/ )
{
	if ( NULL == pea || EXPLICIT_ENTRIES_NUM_FILTER != dwEaCount )
	{
		return FALSE;
	}

	BOOL bAllocateSucc	= TRUE;
	SID_IDENTIFIER_AUTHORITY SIDAuth	= SECURITY_WORLD_SID_AUTHORITY;
	SID_IDENTIFIER_AUTHORITY SIDAuthNT	= SECURITY_NT_AUTHORITY;
	SID_IDENTIFIER_AUTHORITY SIDAuthLocal	= SECURITY_LOCAL_SID_AUTHORITY;
	SID_IDENTIFIER_AUTHORITY SIDAuthWorld	= SECURITY_WORLD_SID_AUTHORITY;
	PSID pSystemSID				= NULL;
	PSID pAdminSID				= NULL;
	PSID pPwUsrSID				= NULL;
	PSID pEveryoneSID			= NULL;


	memset( pea, 0, dwEaCount * sizeof(EXPLICIT_ACCESS) );

	////////////////////////////////////////////////////////////
	//	4.1 - SYSTEM
	//	Create a well-known SID for the SYSTEM
	bAllocateSucc &= AllocateAndInitializeSid
		(
			&SIDAuthNT,			// This structure provides the top-level identifier authority value to set in the SID ���� SID �ṹ���ַ
			1,				// Ҫ��ʼ�����漸������Ȩ���
			SECURITY_LOCAL_SYSTEM_RID,	// �����������ָ����Ҫ��ʼ���ġ���Ȩ��������µ� 7 ����������
			0, 0, 0, 0, 0, 0, 0,		// ������ʣ�µ� 7 ������Ȩ���
			&pSystemSID
		);
	if ( bAllocateSucc )
	{
		// Initialize an EXPLICIT_ACCESS structure for an ACE.
		// The ACE will allow SYSTEM read access to the key.
		pea[0].grfAccessPermissions	= SPECIFIC_RIGHTS_ALL | STANDARD_RIGHTS_ALL;	// GENERIC_ALL;	// �� SYSTEM ��дȨ��
		pea[0].grfAccessMode		= SET_ACCESS;
		pea[0].grfInheritance		= SUB_CONTAINERS_AND_OBJECTS_INHERIT;	// <*** ��� ACE
		pea[0].Trustee.TrusteeForm	= TRUSTEE_IS_SID;
		pea[0].Trustee.TrusteeType	= TRUSTEE_IS_WELL_KNOWN_GROUP;
		pea[0].Trustee.ptstrName	= (LPTSTR)pSystemSID;

		////////////////////////////////////////////////////////////
		//	4.2 - Administrators
		//	Create a SID for the BUILTIN\Administrators group.
		bAllocateSucc &= AllocateAndInitializeSid
			(
				&SIDAuthNT,			// ���� SID �ṹ���ַ
				2,				// Ҫ��ʼ�����漸������Ȩ���
				SECURITY_BUILTIN_DOMAIN_RID,	// ����ָ����Ҫ��ʼ���ĵ� 1 ������Ȩ���
				DOMAIN_ALIAS_RID_ADMINS,		// ����ָ����Ҫ��ʼ���ĵ� 2 ������Ȩ���
				0, 0, 0, 0, 0, 0,		// ������ʣ�µ� 6 ������Ȩ���
				&pAdminSID
			);
		if ( bAllocateSucc )
		{
			//	Initialize an EXPLICIT_ACCESS structure for an ACE.
			//	The ACE will allow the Administrators group full access to the key.
			pea[1].grfAccessPermissions	= SPECIFIC_RIGHTS_ALL | STANDARD_RIGHTS_ALL;	// dwAccessPms
			pea[1].grfAccessMode		= SET_ACCESS;
			pea[1].grfInheritance		= SUB_CONTAINERS_AND_OBJECTS_INHERIT;	// <*** ��� ACE
			pea[1].Trustee.TrusteeForm	= TRUSTEE_IS_SID;
			pea[1].Trustee.TrusteeType	= TRUSTEE_IS_GROUP;
			pea[1].Trustee.ptstrName	= (LPTSTR)pAdminSID;

			////////////////////////////////////////////////////////////
			//	4.3 - PowerUser
			//	Create a SID for the BUILTIN\Administrators group.
			bAllocateSucc &= AllocateAndInitializeSid
				(
					&SIDAuthNT,			// ���� SID �ṹ���ַ
					2,				// Ҫ��ʼ�����漸������Ȩ���
					SECURITY_BUILTIN_DOMAIN_RID,	// ����ָ����Ҫ��ʼ���ĵ� 1 ������Ȩ���
					DOMAIN_ALIAS_RID_POWER_USERS,	// ����ָ����Ҫ��ʼ���ĵ� 2 ������Ȩ���
					0, 0, 0, 0, 0, 0,		// ������ʣ�µ� 6 ������Ȩ���
					&pPwUsrSID
				);
			if ( bAllocateSucc )
			{
				// Initialize an EXPLICIT_ACCESS structure for an ACE.
				// The ACE will allow the Administrators group full access to the key.
				pea[2].grfAccessPermissions	= STANDARD_RIGHTS_READ;	// dwAccessPms
				pea[2].grfAccessMode		= SET_ACCESS;
				pea[2].grfInheritance		= SUB_CONTAINERS_AND_OBJECTS_INHERIT;	// <*** ��� ACE
				pea[2].Trustee.TrusteeForm	= TRUSTEE_IS_SID;
				pea[2].Trustee.TrusteeType	= TRUSTEE_IS_GROUP;
				pea[2].Trustee.ptstrName	= (LPTSTR)pPwUsrSID;

				// ..
				//FreeSid( pIisWpgSID );

				////////////////////////////////////////////////////////////
				//	4.3	Everyone
				//
				bAllocateSucc &= AllocateAndInitializeSid
					(
						&SIDAuthWorld,		// ���� SID �ṹ���ַ
						1,			// Ҫ��ʼ�����漸������Ȩ���
						SECURITY_WORLD_RID,	// �����������ָ����Ҫ��ʼ���ġ���Ȩ��������µ� 7 ����������
						0, 0, 0, 0, 0, 0, 0,	// ������ʣ�µ� 7 ������Ȩ���
						&pEveryoneSID
					);
				if ( bAllocateSucc )
				{
					pea[3].grfAccessPermissions	= GENERIC_READ|GENERIC_EXECUTE;	// �� Everyone ��дȨ��
					pea[3].grfAccessMode		= SET_ACCESS;
					pea[3].grfInheritance		= SUB_CONTAINERS_AND_OBJECTS_INHERIT;	// <*** ��� ACE
					pea[3].Trustee.TrusteeForm	= TRUSTEE_IS_SID;
					pea[3].Trustee.TrusteeType	= TRUSTEE_IS_WELL_KNOWN_GROUP;
					pea[3].Trustee.ptstrName	= (LPTSTR)pEveryoneSID;
				}
			}
		}

		// ..
		//FreeSid( pSystemSID );
	}

	// ..
	return bAllocateSucc;
}

//////////////////////////////////////////////////////////////////////////
// Ϊ IIS 7.0/Win2008/2008R2 ���� EXPLICIT_ACCESS
//
BOOL CProcSecurityDesc::BuildEaFor_Iis70_2008( LPCTSTR lpcszPath, EXPLICIT_ACCESS * pea, DWORD dwEaCount, BOOL bPermissionsEveryoneWritable /*= FALSE*/ )
{
	if ( NULL == pea || EXPLICIT_ENTRIES_NUM_FILTER != dwEaCount )
	{
		return FALSE;
	}

	BOOL bAllocateSucc	= TRUE;
	SID_IDENTIFIER_AUTHORITY SIDAuth	= SECURITY_WORLD_SID_AUTHORITY;
	SID_IDENTIFIER_AUTHORITY SIDAuthNT	= SECURITY_NT_AUTHORITY;
	SID_IDENTIFIER_AUTHORITY SIDAuthLocal	= SECURITY_LOCAL_SID_AUTHORITY;
	SID_IDENTIFIER_AUTHORITY SIDAuthWorld	= SECURITY_WORLD_SID_AUTHORITY;
	PSID pSystemSID				= NULL;
	PSID pAdminSID				= NULL;
	PSID pPwUsrSID				= NULL;
	PSID pEveryoneSID			= NULL;


	memset( pea, 0, dwEaCount * sizeof(EXPLICIT_ACCESS) );

	////////////////////////////////////////////////////////////
	//	4.1 - SYSTEM
	//	Create a well-known SID for the SYSTEM
	bAllocateSucc &= AllocateAndInitializeSid
		(
			&SIDAuthNT,			// This structure provides the top-level identifier authority value to set in the SID ���� SID �ṹ���ַ
			1,				// Ҫ��ʼ�����漸������Ȩ���
			SECURITY_LOCAL_SYSTEM_RID,	// �����������ָ����Ҫ��ʼ���ġ���Ȩ��������µ� 7 ����������
			0, 0, 0, 0, 0, 0, 0,		// ������ʣ�µ� 7 ������Ȩ���
			&pSystemSID
		);
	if ( bAllocateSucc )
	{
		// Initialize an EXPLICIT_ACCESS structure for an ACE.
		// The ACE will allow SYSTEM read access to the key.
		pea[0].grfAccessPermissions	= SPECIFIC_RIGHTS_ALL | STANDARD_RIGHTS_ALL;	// GENERIC_ALL;	// �� SYSTEM ��дȨ��
		pea[0].grfAccessMode		= SET_ACCESS;
		pea[0].grfInheritance		= SUB_CONTAINERS_AND_OBJECTS_INHERIT;	// <*** ��� ACE
		pea[0].Trustee.TrusteeForm	= TRUSTEE_IS_SID;
		pea[0].Trustee.TrusteeType	= TRUSTEE_IS_WELL_KNOWN_GROUP;
		pea[0].Trustee.ptstrName	= (LPTSTR)pSystemSID;

		////////////////////////////////////////////////////////////
		//	4.2 - Administrators
		//	Create a SID for the BUILTIN\Administrators group.
		bAllocateSucc &= AllocateAndInitializeSid
			(
				&SIDAuthNT,			// ���� SID �ṹ���ַ
				2,				// Ҫ��ʼ�����漸������Ȩ���
				SECURITY_BUILTIN_DOMAIN_RID,	// ����ָ����Ҫ��ʼ���ĵ� 1 ������Ȩ���
				DOMAIN_ALIAS_RID_ADMINS,		// ����ָ����Ҫ��ʼ���ĵ� 2 ������Ȩ���
				0, 0, 0, 0, 0, 0,		// ������ʣ�µ� 6 ������Ȩ���
				&pAdminSID
			);
		if ( bAllocateSucc )
		{
			//	Initialize an EXPLICIT_ACCESS structure for an ACE.
			//	The ACE will allow the Administrators group full access to the key.
			pea[1].grfAccessPermissions	= SPECIFIC_RIGHTS_ALL | STANDARD_RIGHTS_ALL;	// dwAccessPms
			pea[1].grfAccessMode		= SET_ACCESS;
			pea[1].grfInheritance		= SUB_CONTAINERS_AND_OBJECTS_INHERIT;	// <*** ��� ACE
			pea[1].Trustee.TrusteeForm	= TRUSTEE_IS_SID;
			pea[1].Trustee.TrusteeType	= TRUSTEE_IS_GROUP;
			pea[1].Trustee.ptstrName	= (LPTSTR)pAdminSID;

			////////////////////////////////////////////////////////////
			//	4.3 - PowerUser
			//	Create a SID for the BUILTIN\Administrators group.
			bAllocateSucc &= AllocateAndInitializeSid
				(
					&SIDAuthNT,			// ���� SID �ṹ���ַ
					2,				// Ҫ��ʼ�����漸������Ȩ���
					SECURITY_BUILTIN_DOMAIN_RID,	// ����ָ����Ҫ��ʼ���ĵ� 1 ������Ȩ���
					DOMAIN_ALIAS_RID_POWER_USERS,	// ����ָ����Ҫ��ʼ���ĵ� 2 ������Ȩ���
					0, 0, 0, 0, 0, 0,		// ������ʣ�µ� 6 ������Ȩ���
					&pPwUsrSID
				);
			if ( bAllocateSucc )
			{
				// Initialize an EXPLICIT_ACCESS structure for an ACE.
				// The ACE will allow the Administrators group full access to the key.
				pea[2].grfAccessPermissions	= STANDARD_RIGHTS_READ;	// dwAccessPms
				pea[2].grfAccessMode		= SET_ACCESS;
				pea[2].grfInheritance		= SUB_CONTAINERS_AND_OBJECTS_INHERIT;	// <*** ��� ACE
				pea[2].Trustee.TrusteeForm	= TRUSTEE_IS_SID;
				pea[2].Trustee.TrusteeType	= TRUSTEE_IS_GROUP;
				pea[2].Trustee.ptstrName	= (LPTSTR)pPwUsrSID;

				// ..
				//FreeSid( pIisWpgSID );

				////////////////////////////////////////////////////////////
				//	4.3	Everyone
				//
				bAllocateSucc &= AllocateAndInitializeSid
					(
						&SIDAuthWorld,		// ���� SID �ṹ���ַ
						1,			// Ҫ��ʼ�����漸������Ȩ���
						SECURITY_WORLD_RID,	// �����������ָ����Ҫ��ʼ���ġ���Ȩ��������µ� 7 ����������
						0, 0, 0, 0, 0, 0, 0,	// ������ʣ�µ� 7 ������Ȩ���
						&pEveryoneSID
					);
				if ( bAllocateSucc )
				{
					if ( bPermissionsEveryoneWritable )
					{
						pea[3].grfAccessPermissions	= GENERIC_READ|GENERIC_WRITE|GENERIC_EXECUTE;	// �� Everyone ��дȨ��
					}
					else
					{
						pea[3].grfAccessPermissions	= GENERIC_READ|GENERIC_EXECUTE;		// �� Everyone ��дȨ��
					}
					pea[3].grfAccessMode		= SET_ACCESS;
					pea[3].grfInheritance		= SUB_CONTAINERS_AND_OBJECTS_INHERIT;	// <*** ��� ACE
					pea[3].Trustee.TrusteeForm	= TRUSTEE_IS_SID;
					pea[3].Trustee.TrusteeType	= TRUSTEE_IS_WELL_KNOWN_GROUP;
					pea[3].Trustee.ptstrName	= (LPTSTR)pEveryoneSID;
				}
			}
		}

		// ..
		//FreeSid( pSystemSID );
	}

	// ..
	return bAllocateSucc;
}

//////////////////////////////////////////////////////////////////////////
// Ϊ IIS 6.0/Win2003 ���� EXPLICIT_ACCESS
//
BOOL CProcSecurityDesc::BuildEaFor_Iis60_2003( LPCTSTR lpcszPath, EXPLICIT_ACCESS * pea, DWORD dwEaCount, BOOL bPermissionsEveryoneWritable /*= FALSE*/ )
{
	if ( NULL == pea || EXPLICIT_ENTRIES_NUM_FILTER != dwEaCount )
	{
		return FALSE;
	}

	BOOL bAllocateSucc			= TRUE;
	SID_IDENTIFIER_AUTHORITY SIDAuth	= SECURITY_WORLD_SID_AUTHORITY;
	SID_IDENTIFIER_AUTHORITY SIDAuthNT	= SECURITY_NT_AUTHORITY;
	SID_IDENTIFIER_AUTHORITY SIDAuthLocal	= SECURITY_LOCAL_SID_AUTHORITY;
	SID_IDENTIFIER_AUTHORITY SIDAuthWorld	= SECURITY_WORLD_SID_AUTHORITY;
	PSID pSystemSID				= NULL;
	PSID pAdminSID				= NULL;
	PSID pIisWpgSID				= NULL;
	PSID pEveryoneSID			= NULL;

	memset( pea, 0, dwEaCount * sizeof(EXPLICIT_ACCESS) );
	
	////////////////////////////////////////////////////////////
	//	4.1 - SYSTEM
	//	Create a well-known SID for the SYSTEM
	bAllocateSucc &= AllocateAndInitializeSid
		(
			&SIDAuthNT,			// This structure provides the top-level identifier authority value to set in the SID ���� SID �ṹ���ַ
			1,				// Ҫ��ʼ�����漸������Ȩ���
			SECURITY_LOCAL_SYSTEM_RID,	// �����������ָ����Ҫ��ʼ���ġ���Ȩ��������µ� 7 ����������
			0, 0, 0, 0, 0, 0, 0,		// ������ʣ�µ� 7 ������Ȩ���
			&pSystemSID
		);
	if ( bAllocateSucc )
	{
		//	Initialize an EXPLICIT_ACCESS structure for an ACE.
		//	The ACE will allow SYSTEM read access to the key.
		pea[0].grfAccessPermissions	= SPECIFIC_RIGHTS_ALL | STANDARD_RIGHTS_ALL;	// GENERIC_ALL;	// �� SYSTEM ��дȨ��
		pea[0].grfAccessMode		= SET_ACCESS;
		pea[0].grfInheritance		= SUB_CONTAINERS_AND_OBJECTS_INHERIT;	// <*** ��� ACE
		pea[0].Trustee.TrusteeForm	= TRUSTEE_IS_SID;
		pea[0].Trustee.TrusteeType	= TRUSTEE_IS_WELL_KNOWN_GROUP;
		pea[0].Trustee.ptstrName	= (LPTSTR)pSystemSID;


		////////////////////////////////////////////////////////////
		//	4.2 - Administrators
		//	Create a SID for the BUILTIN\Administrators group.
		bAllocateSucc &= AllocateAndInitializeSid
			(
				&SIDAuthNT,			// ���� SID �ṹ���ַ
				2,				// Ҫ��ʼ�����漸������Ȩ���
				SECURITY_BUILTIN_DOMAIN_RID,	// ����ָ����Ҫ��ʼ���ĵ� 1 ������Ȩ���
				DOMAIN_ALIAS_RID_ADMINS,	// ����ָ����Ҫ��ʼ���ĵ� 2 ������Ȩ���
				0, 0, 0, 0, 0, 0,		// ������ʣ�µ� 6 ������Ȩ���
				&pAdminSID
			);
		if ( bAllocateSucc )
		{
			//	Initialize an EXPLICIT_ACCESS structure for an ACE.
			//	The ACE will allow the Administrators group full access to the key.
			pea[1].grfAccessPermissions	= SPECIFIC_RIGHTS_ALL | STANDARD_RIGHTS_ALL;	// dwAccessPms
			pea[1].grfAccessMode		= SET_ACCESS;
			pea[1].grfInheritance		= SUB_CONTAINERS_AND_OBJECTS_INHERIT;	// <*** ��� ACE
			pea[1].Trustee.TrusteeForm	= TRUSTEE_IS_SID;
			pea[1].Trustee.TrusteeType	= TRUSTEE_IS_GROUP;
			pea[1].Trustee.ptstrName	= (LPTSTR)pAdminSID;


			////////////////////////////////////////////////////////////
			//	4.3 - IIS_WPG
			//	"S-1-5-x-1000"
			//	"S-1-5-21-3338309497-422766316-1218862070-1018"
			//	ConvertStringSidToSid( "S-1-5-32-1000", &pIisWpgSID )
			if ( SUCCEEDED( GetSIDFromName( "IIS_WPG", &pIisWpgSID ) ) )
			{
				//	Initialize an EXPLICIT_ACCESS structure for an ACE.
				//	The ACE will allow the Administrators group full access to the key.
				pea[2].grfAccessPermissions	= SPECIFIC_RIGHTS_ALL | STANDARD_RIGHTS_ALL;	// dwAccessPms
				pea[2].grfAccessMode		= SET_ACCESS;
				pea[2].grfInheritance		= SUB_CONTAINERS_AND_OBJECTS_INHERIT;	// <*** ��� ACE
				pea[2].Trustee.TrusteeForm	= TRUSTEE_IS_SID;
				pea[2].Trustee.TrusteeType	= TRUSTEE_IS_GROUP;
				pea[2].Trustee.ptstrName	= (LPTSTR)pIisWpgSID;
				//FreeSid( pIisWpgSID );

				////////////////////////////////////////////////////////////
				//	4.3	Everyone
				//
				bAllocateSucc &= AllocateAndInitializeSid
				(
					&SIDAuthWorld,		// ���� SID �ṹ���ַ
					1,			// Ҫ��ʼ�����漸������Ȩ���
					SECURITY_WORLD_RID,	// �����������ָ����Ҫ��ʼ���ġ���Ȩ��������µ� 7 ����������
					0, 0, 0, 0, 0, 0, 0,	// ������ʣ�µ� 7 ������Ȩ���
					&pEveryoneSID
				);
				if ( bAllocateSucc )
				{
					pea[3].grfAccessPermissions	= GENERIC_READ|GENERIC_EXECUTE;	// �� Everyone ��дȨ��
					pea[3].grfAccessMode		= SET_ACCESS;
					pea[3].grfInheritance		= SUB_CONTAINERS_AND_OBJECTS_INHERIT;	// <*** ��� ACE
					pea[3].Trustee.TrusteeForm	= TRUSTEE_IS_SID;
					pea[3].Trustee.TrusteeType	= TRUSTEE_IS_WELL_KNOWN_GROUP;
					pea[3].Trustee.ptstrName	= (LPTSTR)pEveryoneSID;
				}
			}
		}
		
		// ..
		//FreeSid( pSystemSID );
	}
	
	// ..
	return bAllocateSucc;
}



//////////////////////////////////////////////////////////////////////////
// ��ȡָ���û����� SID
HRESULT CProcSecurityDesc::GetSIDFromName( LPCSTR pwszUserName, PSID * ppSid )
{
	// Translate the user name into a SID
	if( NULL == ppSid )
	{
		return( E_INVALIDARG );
	}
	
	HRESULT hr		= S_OK;
	DWORD dwRetVal		= 0;
	SID_NAME_USE SidNameUse;
	DWORD cbSid		= 0;
	DWORD cbDomainName	= 0;
	LPSTR wstrDomainName	= NULL;
	BOOL fRetVal		= FALSE;
	
	do
	{
		if ( ! LookupAccountName( NULL, pwszUserName, NULL, &cbSid, NULL, &cbDomainName, &SidNameUse ) )
		{
			dwRetVal = GetLastError();
			if ( ERROR_INSUFFICIENT_BUFFER != dwRetVal )
			{
				hr = HRESULT_FROM_WIN32( dwRetVal );
				break;
			}
		}
		
		*ppSid = (SID *)LocalAlloc( LPTR, cbSid );
		if ( NULL == *ppSid )
		{
			hr = E_OUTOFMEMORY;
			break;
		}
		
		wstrDomainName = new CHAR[ cbDomainName ];
		if ( NULL == wstrDomainName )
		{
			hr = E_OUTOFMEMORY;
			break;
		}
		
		fRetVal = LookupAccountName( NULL, pwszUserName, *ppSid, &cbSid, wstrDomainName, &cbDomainName, &SidNameUse );
		if ( ! fRetVal )
		{
			dwRetVal = GetLastError();
			hr = HRESULT_FROM_WIN32( dwRetVal );
			break;
		}
	}
	while( FALSE );
	
	if ( NULL != wstrDomainName )
	{
		delete [] wstrDomainName;
	}
	
	if ( FAILED( hr ) && ( NULL != *ppSid ) )
	{
		LocalFree( *ppSid );
		*ppSid = NULL;
	}
	
	return ( hr );
}