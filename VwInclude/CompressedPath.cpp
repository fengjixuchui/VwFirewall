// CompressedPath.cpp: implementation of the CCompressedPath class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "CompressedPath.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CCompressedPath::CCompressedPath()
{

}

CCompressedPath::~CCompressedPath()
{

}



// ���������ļ�Ŀ¼
BOOL CCompressedPath::GetMySpecialFolder( INT nFolder, LPTSTR lpszPath, DWORD dwPSize )
{
	BOOL bRet = FALSE;
	TCHAR szFolder[ MAX_PATH ] = {0};

	switch( nFolder )
	{
	case SPPATH_SYSTEMDRIVE:	// C:  ϵͳӲ��
		{
			if ( GetWindowsDirectory( szFolder, sizeof(szFolder) ) > 0 )
			{
				szFolder[2] = 0;
				_sntprintf( lpszPath, dwPSize-sizeof(TCHAR), "%s", szFolder );
				bRet = TRUE;
			}
		}
		break;
	case SPPATH_WINDIR:		// C:\WINNT  WindowsĿ¼
		{
			if ( GetWindowsDirectory( szFolder, sizeof(szFolder) ) > 0 )
			{
				_sntprintf( lpszPath, dwPSize-sizeof(TCHAR), "%s", szFolder );
				bRet = TRUE;
			}
		}
		break;
	case SPPATH_SYSTEMROOT:		// C:\WINNT\SYSTEM32
	case SPPATH_WINSYSTEM:
		{
			if ( GetSystemDirectory( szFolder, sizeof(szFolder) ) > 0 )
			{
				_sntprintf( lpszPath, dwPSize-sizeof(TCHAR), "%s", szFolder );
				bRet = TRUE;
			}
		}
		break;
	case SPPATH_PROGRAMFILES:	// C:\Program Files
		{
			if ( SHGetSpecialFolderPath( NULL, szFolder, CSIDL_PROGRAM_FILES, FALSE ) )
			{
				_sntprintf( lpszPath, dwPSize-sizeof(TCHAR), "%s", szFolder );
				bRet = TRUE;
			}
		}
		break;
	case SPPATH_USERPROFILE:	// C:\Documents and Settings\Administrator ���� Administrator �ǵ�ǰ�û����ɱ��
		{
			if ( ExpandEnvironmentStrings("%UserProfile%", szFolder, sizeof(szFolder) ) > 0 )
			{
				_sntprintf( lpszPath, dwPSize-sizeof(TCHAR), "%s", szFolder );
				bRet = TRUE;
			}
		}
		break;
	case SPPATH_ALLUSERPROFILE:	// C:\Documents and Settings\All Users
		{
			if ( ExpandEnvironmentStrings("%AllUserProfile%", szFolder, sizeof(szFolder) ) > 0 )
			{
				_sntprintf( lpszPath, dwPSize-sizeof(TCHAR), "%s", szFolder );
				bRet = TRUE;
			}
		}
		break;
	case SPPATH_COMMON_DESKTOPDIRECTORY:
		{
			if ( SHGetSpecialFolderPath( NULL, szFolder, CSIDL_COMMON_DESKTOPDIRECTORY, FALSE ) )
			{
				_sntprintf( lpszPath, dwPSize-sizeof(TCHAR), "%s", szFolder );
				bRet = TRUE;
			}
		}
		break;
	case SPPATH_COMMON_DOCUMENTS:
		{
			if ( SHGetSpecialFolderPath( NULL, szFolder, CSIDL_COMMON_DOCUMENTS, FALSE ) )
			{
				_sntprintf( lpszPath, dwPSize-sizeof(TCHAR), "%s", szFolder );
				bRet = TRUE;
			}
		}
		break;
	case SPPATH_DESKTOPDIRECTORY:
		{
			if ( SHGetSpecialFolderPath( NULL, szFolder, CSIDL_DESKTOPDIRECTORY, FALSE ) )
			{
				_sntprintf( lpszPath, dwPSize-sizeof(TCHAR), "%s", szFolder );
				bRet = TRUE;
			}
		}
		break;
	case SPPATH_TEMP:		// C:\WINNT\TEMP
	case SPPATH_TMP:		// C:\WINNT\TEMP
		{
			if ( GetTempPath( sizeof(szFolder), szFolder ) > 0 )
			{
				_sntprintf( lpszPath, dwPSize-sizeof(TCHAR), "%s", szFolder );
				bRet = TRUE;
			}
		}
		break;
	}

	// ..
	return bRet;
}


//////////////////////////////////////////////////////////////////////////
// ��ȡ��ʽ������ļ�·������Ҫ�ǽ� Windows ·���滻�� $WINDOWSDIR$
BOOL CCompressedPath::GetCompressedPath( LPCTSTR lpszFilePathIn, LPTSTR lpszFilePathNew, DWORD dwPathNewSize, LPTSTR lpszWinDir, DWORD dwWDSize )
{
	if ( NULL == lpszFilePathIn || 0 == _tcslen(lpszFilePathIn) )
	{
		return FALSE;
	}

	BOOL  bRet = FALSE;
	TCHAR szSPFolder[ MAX_PATH ] = {0};

	// ��ȡ WINDOW Ŀ¼
	if ( lpszWinDir && dwWDSize > 0 )
		GetWindowsDirectory( lpszWinDir, dwWDSize );

	if ( !bRet && GetMySpecialFolder( SPPATH_WINSYSTEM, szSPFolder, sizeof(szSPFolder) ) )
		bRet |= LeftPathReplace( lpszFilePathIn, lpszFilePathNew, dwPathNewSize, szSPFolder, "::WinSystem::" );

	if ( !bRet && GetMySpecialFolder( SPPATH_SYSTEMROOT, szSPFolder, sizeof(szSPFolder) ) )
		bRet |= LeftPathReplace( lpszFilePathIn, lpszFilePathNew, dwPathNewSize, szSPFolder, "::SystemRoot::" );

	if ( !bRet && GetMySpecialFolder( SPPATH_WINDIR, szSPFolder, sizeof(szSPFolder) ) )
		bRet |= LeftPathReplace( lpszFilePathIn, lpszFilePathNew, dwPathNewSize, szSPFolder, "::WinDir::" );
	
	if ( !bRet && GetMySpecialFolder( SPPATH_PROGRAMFILES, szSPFolder, sizeof(szSPFolder) ) )
		bRet |= LeftPathReplace( lpszFilePathIn, lpszFilePathNew, dwPathNewSize, szSPFolder, "::ProgramFiles::" );

	if ( !bRet && GetMySpecialFolder( SPPATH_DESKTOPDIRECTORY, szSPFolder, sizeof(szSPFolder) ) )
		bRet |= LeftPathReplace( lpszFilePathIn, lpszFilePathNew, dwPathNewSize, szSPFolder, "::DesktopDirectory::" );
	
	if ( !bRet && GetMySpecialFolder( SPPATH_COMMON_DESKTOPDIRECTORY, szSPFolder, sizeof(szSPFolder) ) )
		bRet |= LeftPathReplace( lpszFilePathIn, lpszFilePathNew, dwPathNewSize, szSPFolder, "::CommonDesktopDirectory::" );
	
	if ( !bRet && GetMySpecialFolder( SPPATH_COMMON_DOCUMENTS, szSPFolder, sizeof(szSPFolder) ) )
		bRet |= LeftPathReplace( lpszFilePathIn, lpszFilePathNew, dwPathNewSize, szSPFolder, "::CommonDocuments::" );

	if ( !bRet && GetMySpecialFolder( SPPATH_USERPROFILE, szSPFolder, sizeof(szSPFolder) ) )
		bRet |= LeftPathReplace( lpszFilePathIn, lpszFilePathNew, dwPathNewSize, szSPFolder, "::UserProfile::" );
	
	if ( !bRet && GetMySpecialFolder( SPPATH_ALLUSERPROFILE, szSPFolder, sizeof(szSPFolder) ) )
		bRet |= LeftPathReplace( lpszFilePathIn, lpszFilePathNew, dwPathNewSize, szSPFolder, "::AllUserProfile::" );

	if ( !bRet && GetMySpecialFolder( SPPATH_TEMP, szSPFolder, sizeof(szSPFolder) ) )
		bRet |= LeftPathReplace( lpszFilePathIn, lpszFilePathNew, dwPathNewSize, szSPFolder, "::Temp::" );

	if ( !bRet && GetMySpecialFolder( SPPATH_TMP, szSPFolder, sizeof(szSPFolder) ) )
		bRet |= LeftPathReplace( lpszFilePathIn, lpszFilePathNew, dwPathNewSize, szSPFolder, "::Tmp::" );

	if ( !bRet && GetMySpecialFolder( SPPATH_SYSTEMDRIVE, szSPFolder, sizeof(szSPFolder) ) )
		bRet |= LeftPathReplace( lpszFilePathIn, lpszFilePathNew, dwPathNewSize, szSPFolder, "::SystemDrive::" );

	return bRet;
}

BOOL CCompressedPath::LeftPathReplace( LPCTSTR lpszSrcPath, LPTSTR lpszDstPath, DWORD dwDPSize, LPCTSTR lpszFind, LPCTSTR lpszRpWith )
{
	BOOL bRet = FALSE;
	if ( lpszSrcPath && lpszDstPath && lpszFind && lpszRpWith && _tcslen(lpszFind) > 0 )
	{
		if ( NULL == _tcsstr(lpszSrcPath,"::") )
		{
			if ( _tcslen(lpszSrcPath) > _tcslen(lpszFind) &&
				0 == _tcsnicmp( lpszFind, lpszSrcPath, _tcslen(lpszFind) ) )
			{
				bRet = TRUE;
				_sntprintf( lpszDstPath, dwDPSize-sizeof(TCHAR), "%s%s",
						lpszRpWith, (lpszSrcPath+_tcslen(lpszFind)) );
			}
		}
	}
	return bRet;
}