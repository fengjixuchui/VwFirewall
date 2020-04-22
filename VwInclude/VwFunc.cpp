// VwFunc.cpp: implementation of the CVwFunc class.
//
//////////////////////////////////////////////////////////////////////

#include "StdAfx.h"
#include "VwFunc.h"
#include <stdio.h>
#include <stdlib.h>

#include "shlwapi.h"
#pragma comment( lib, "shlwapi.lib" )


/**
 *	���� CfgMap �ļ�	
 */
BOOL _vwfunc_make_cfg_map_file_byrundll( LPCTSTR lpcszVwDllFile )
{
	if ( NULL == lpcszVwDllFile )
		return FALSE;
	
	BOOL  bRet			= FALSE;
	TCHAR szSystemDir[ MAX_PATH ]	= {0};
	TCHAR szShortPath[ MAX_PATH ]	= {0};
	TCHAR szCmdLine[ 1024 ]		= {0};
	
	if ( GetSystemDirectory( szSystemDir, sizeof(szSystemDir) ) )
	{
		if ( GetShortPathName( lpcszVwDllFile, szShortPath, sizeof(szShortPath) ) )
		{
			_sntprintf( szCmdLine, sizeof(szCmdLine)-sizeof(TCHAR), "%s\\Rundll32.exe %s,RundllMakeCfgMap", szSystemDir, szShortPath );
			bRet = _vwfunc_run_block_process( szCmdLine );
		}
	}
	
	return bRet;
}
BOOL _vwfunc_make_cfg_map_file_bystdcall( LPCTSTR lpcszVwDllFile )
{
	BOOL bRet			= FALSE;
	HINSTANCE hLibrary		= NULL;
	PFNMAKECFGMAP pfnMakeCfgMap;

	hLibrary = LoadLibrary( lpcszVwDllFile );
	if ( hLibrary )
	{
		pfnMakeCfgMap = (PFNMAKECFGMAP)GetProcAddress( hLibrary, "MakeCfgMap" );
		if ( pfnMakeCfgMap )
		{
			bRet	= TRUE;

			pfnMakeCfgMap();
		}

		//	Free ��ʱ��Ƚ��鷳
		//FreeLibrary( hLibrary );
	}

	return bRet;
}



/**
 *	�ж� VwCfgNew �Ƿ���������
 */
BOOL _vwfunc_is_vwcfgnew_running()
{
	//	..
	BOOL   bVwCfgNewExist	= FALSE;
	HANDLE hMutex = CreateMutex( NULL, TRUE, MUTEX_SHARE_VWCFGNEW_EXE );
	if ( hMutex )
	{
		if ( GetLastError() == ERROR_ALREADY_EXISTS )
		{
			//	�Ѿ���һ��Ӧ�ó�����������
			bVwCfgNewExist = TRUE;
		}
		CloseHandle( hMutex );
	}

	return bVwCfgNewExist;
}