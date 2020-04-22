// VwUpdate.cpp: implementation of the CVwUpdate class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "VwUpdate.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CVwUpdate::CVwUpdate()
{
	m_bIsReady	= FALSE;
	memset( & m_stUpInfo, 0, sizeof( m_stUpInfo ) );
}

CVwUpdate::~CVwUpdate()
{
}

/**
 *	���û����ĸ���������Ϣ
 */
BOOL CVwUpdate::SetUpdateInfo( STUPDATEINFO * pstUpInfo )
{
	if ( pstUpInfo )
	{
		m_stUpInfo = *pstUpInfo;

		if ( PathIsDirectory( m_stUpInfo.szUpdateDir ) )
		{
			//	���ظ��������ļ���������¼���ظ������
			_sntprintf
			(
				m_stUpInfo.szUpdateCfgIniFile,
				sizeof(m_stUpInfo.szUpdateCfgIniFile)-sizeof(TCHAR),
				"%s\\%s", m_stUpInfo.szUpdateDir, VWUPDATE_UPDATECFGINI
			);
		}

		m_bIsReady = TRUE;

		return TRUE;
	}
	return FALSE;
}

/**
 *	@ Public
 *	��ʼ����
 */
BOOL CVwUpdate::StartUpdate()
{
	BOOL bRet	= FALSE;

	if ( ! IsReady() )
	{
		return FALSE;
	}

	//
	//	������ʱ����Ŀ¼
	//
	CreateDirectory( m_stUpInfo.szUpdateDir, NULL );

	//
	//	�ӷ�������ȡ�Ƿ���Ҫ��������Ϣ
	//
	if ( GetUpdateIniFromServer() )
	{
		//
		//	�����Ƿ�����Ҫ����������
		//
		if ( CheckUpdateInfo() )
		{
			//
			//	��������ģ��
			//
			if ( DownloadAllModules() )
			{
				//
				//	��������ģ��
				//
				if ( UpdateAllModules() )
				{
					bRet = TRUE;
				}
			}
		}

		//	ֻҪ�ӷ�������ȡ���������ļ��ɹ����ͼ�¼һ�������ʱ�䣬���ٷ�����ѹ��
		//	һ������Ϊ 24 Сʱһ�θ���
		SaveLastWorkTime();
	}

	return bRet;
}

/**
 *	@ Public
 *	����������ʱ�����ļ�
 *	### ��ʱ�����������ʹ�ã�ԭ���ǣ���Ҫ�����ͨѶ��ȷ���Ƿ�������������ʹ����Щ ini �ļ�
 */
VOID CVwUpdate::CleanAllTempDataFile()
{
	if ( PathFileExists( m_stUpInfo.szUpdateIniFile ) )
	{
		DeleteFile( m_stUpInfo.szUpdateIniFile );
	}
}


/**
 *	@ ˽�к���
 *	�ж��Ƿ��Ѿ�׼����
 */
BOOL CVwUpdate::IsReady()
{
	return m_bIsReady;
}


/**
 *	@ ˽�к���
 *	�ӷ�������ȡ�Ƿ���Ҫ��������Ϣ
 */
BOOL CVwUpdate::GetUpdateIniFromServer()
{
	if ( ! IsWorkTime() )
	{
		return FALSE;
	}

	TCHAR szError[ MAX_PATH ]	= {0};

	//	��ɾ��ԭ�����ļ�
	DeleteFile( m_stUpInfo.szUpdateIniFile );

	//	��ʼ�����µ��ļ�
	return CVwHttp::DownloadFile
	(
		m_stUpInfo.szUpdateUrl,
		m_stUpInfo.szUpdateIniFile,
		m_stUpInfo.hMainWnd,
		TRUE,
		szError,
		m_stUpInfo.dwTimeout
	);
}

/**
 *	@ ˽�к���
 *	���ݱ��������ļ����жϵ�ǰ�Ƿ�ʼ�����������������Ϣ
 */
BOOL CVwUpdate::IsWorkTime()
{
	if ( ! PathFileExists( m_stUpInfo.szUpdateCfgIniFile ) )
	{
		//	�����ļ������ڣ��ǹ���ʱ����
		return TRUE;
	}

	BOOL bRet		= FALSE;
	SYSTEMTIME st;
	DWORD dwNowDate		= 0;
	DWORD dwRecDate		= 0;

	//	��ȡ ��ǰ����
	GetLocalTime( &st );
	dwNowDate = ( st.wMonth * 100 + st.wDay );

	//	��ȡ�ϴθ���ʱ��
	dwRecDate = (DWORD)GetPrivateProfileInt( VWUPDATE_SECTION_UPDATE, VWUPDATE_KEY_LASTWORK, 0, m_stUpInfo.szUpdateCfgIniFile );

	//	����ǽ��죬�Ͳ��ù�����
	return ( dwRecDate == dwNowDate ) ? FALSE : TRUE;	
}

/**
 *	@ ˽�к���
 *	���������ʱ��
 */
VOID CVwUpdate::SaveLastWorkTime()
{
	SYSTEMTIME st;
	DWORD dwNowDate			= 0;
	TCHAR szTemp[ MAX_PATH ]	= {0};
	
	//	��ǰ����
	GetLocalTime( &st );
	dwNowDate = ( st.wMonth * 100 + st.wDay );

	_sntprintf( szTemp, sizeof(szTemp)-sizeof(TCHAR), "%d", dwNowDate );
	WritePrivateProfileString( VWUPDATE_SECTION_UPDATE, VWUPDATE_KEY_LASTWORK, szTemp, m_stUpInfo.szUpdateCfgIniFile );
}


/**
 *	@ ˽�к���
 *	�����Ƿ��б�Ҫ��������Ϣ
 */
BOOL CVwUpdate::CheckUpdateInfo()
{
	BOOL  bRet				= FALSE;
	BOOL  nUpdateNow			= FALSE;

	if ( PathFileExists( m_stUpInfo.szUpdateIniFile ) )
	{
		nUpdateNow = (BOOL)GetPrivateProfileInt( "config", "update", 0, m_stUpInfo.szUpdateIniFile );
		if ( nUpdateNow )
		{
			//	��������б�
			if ( m_vcUpdateList.size() )
			{
				m_vcUpdateList.clear();
			}

			//
			//	���� INI ��Ϣ
			//
			if ( ParseIni( m_stUpInfo.szUpdateIniFile ) )
			{
				bRet = ( m_vcUpdateList.size() ? TRUE : FALSE );
			}
		}
	}

	return bRet;
}


/**
 *	@ ˽�к���
 *	������Ҫ���µ��ļ��б�
 */
BOOL CVwUpdate::ParseIni( LPCTSTR lpszUpdateIni )
{
	//
	//	lpszUpdateIni		- [in] INI �ļ�·��
	//	RETURN			- TRUE / FALSE
	//	Remark			- ��Ҫ���µ��ļ��б������� m_vcIniList ��
	//

	TCHAR szFileVersion[MAX_PATH]		= {0};		// xxx.dll ��
	TCHAR pszSection[ 32767 ]		= {0};		// Win95 �Ĵ�С����
	INT   nSectionLen			= 0;
	STUPDATELIST stUpdateList;

	TCHAR szError[ MAX_PATH ]		= {0};
	CMultiString mstr;
	INT   nNum				= 0;
	TCHAR ** ppList				= NULL;
	TCHAR ** p				= NULL;
	TCHAR szTargetFile[ MAX_PATH ]		= {0};
	TCHAR szFileName[ MAX_PATH ]		= {0};
	TCHAR szUrl[ MAX_PATH ]			= {0};
	STPARSEURL stParseUrl;
	TCHAR szUrlExt[ 32 ]			= {0};
	TCHAR szUrlFile[ MAX_PATH ]		= {0};
	BOOL  bGetUrlFile			= FALSE;


	//	������¶���
	if ( m_vcUpdateList.size() )
	{
		m_vcUpdateList.clear();
	}

	//	��ʼ���� INI
	nSectionLen = GetPrivateProfileSection( VWUPDATE_SECTION_ADD, pszSection, sizeof(pszSection), lpszUpdateIni );
	if ( nSectionLen <= 0 )
		return FALSE;

	ppList = mstr.MultiStringToStringList( pszSection, nSectionLen + 1, &nNum );
	if ( NULL == ppList )
	{
		return FALSE;
	}

	p = ppList;
	while( p && *p )
	{
		nNum = _stscanf( *p, "%[^=]=%s", szFileName, szFileVersion );
		if ( 2 == nNum )
		{
			memset( & stUpdateList, 0, sizeof(stUpdateList) );
			_vwfunc_replace( szFileVersion, "\"", "" );

			//	��ȡĳ���ļ�����Ϣ
			GetPrivateProfileString( szFileName, VWUPDATE_KEY_URL, "", stUpdateList.szUrl, sizeof(stUpdateList.szUrl), lpszUpdateIni );
			stUpdateList.bReg = (BOOL)GetPrivateProfileInt( szFileName, VWUPDATE_KEY_REG, 0, m_stUpInfo.szUpdateIniFile );
			stUpdateList.uExec = (UINT)GetPrivateProfileInt( szFileName, VWUPDATE_KEY_EXEC, 0, m_stUpInfo.szUpdateIniFile );

			if ( 0 == strlen( stUpdateList.szUrl ) )
			{
				continue;
			}

			//	��������سɹ� = FALSE
			stUpdateList.bDownSucc	= FALSE;

			bGetUrlFile	= FALSE;
			memset( szUrlFile, 0, sizeof( szUrlFile ) );
			if ( _vwfunc_parse_url( stUpdateList.szUrl, strlen( stUpdateList.szUrl ), & stParseUrl ) )
			{
				//	�ж�һ�� URL ָ����Ƿ���һ���ļ�����
				if ( _vwfunc_get_fileinfo_from_url( stParseUrl.szPath, szUrlExt, sizeof(szUrlExt), szUrlFile, sizeof(szUrlFile) ) )
				{
					bGetUrlFile	= TRUE;
				}
			}

			if ( ! bGetUrlFile || 0 == strlen( szUrlFile ) )
			{
				continue;
			}

			//	�����ļ���
			_sntprintf( stUpdateList.szUrlFileName, sizeof(stUpdateList.szUrlFileName)-sizeof(TCHAR), "%s", szUrlFile );

			//	���������ļ���
			_sntprintf( stUpdateList.szFileName, sizeof(stUpdateList.szFileName)-sizeof(TCHAR), "%s", szFileName );

			//	�ļ�������ʱ����·��
			_sntprintf( stUpdateList.szDownFile, sizeof(stUpdateList.szDownFile)-sizeof(TCHAR), "%s\\%s", m_stUpInfo.szUpdateDir, szUrlFile );

			//	�ļ���ѹ�����ʵ�ļ�·��
			_sntprintf( stUpdateList.szRealFile, sizeof(stUpdateList.szRealFile)-sizeof(TCHAR), "%s\\%s", m_stUpInfo.szUpdateDir, szFileName );

			//	Ŀ���ļ�������Ǵ�������Ŀ��
			_sntprintf( stUpdateList.szDstFile, sizeof(stUpdateList.szDstFile)-sizeof(TCHAR), "%s\\%s", m_stUpInfo.szDstDir, szFileName );

			//	INI ��ָ�����ļ������°汾��
			_sntprintf( stUpdateList.szVer, sizeof(stUpdateList.szVer)-sizeof(TCHAR), "%s", szFileVersion );

			//	��ϸ������Ϣ
			_vwfunc_ini_get_value_with_acpkey
			(
				m_stUpInfo.szUpdateIniFile,
				szFileName,
				VWUPDATE_KEY_DETAIL,
				stUpdateList.szDetail,
				sizeof(stUpdateList.szDetail)
			);

			//	��ȡ�����ļ� ��ǰ�汾��
			_vwfunc_get_file_version( stUpdateList.szDstFile, stUpdateList.szDstFileVer, sizeof( stUpdateList.szDstFileVer ) );

			//	�ȱȽϱ����ļ��İ汾���Ƿ���Ҫ����
			//	�����Ҫ�����Ļ�������������������Ҫ������
			if ( stricmp( stUpdateList.szVer, stUpdateList.szDstFileVer ) > 0 )
			{
				//
				//	���˸������͵������¶���
				//
				m_vcUpdateList.push_back( stUpdateList );
			}
		}
		p++;
	}

	if ( ppList )
	{
		free( ppList );
	}	

	return TRUE;
}




/**
 *	@ ˽�к���
 *	�������з�����ָ����ģ��
 */
BOOL CVwUpdate::DownloadAllModules()
{
	BOOL  bDownload			= TRUE;
	TCHAR szError[ MAX_PATH ]	= {0};
	TCHAR szTemp[ MAX_PATH ]	= {0};
	TCHAR szFileExt[ MAX_PATH ]	= {0};
	STUPDATELIST * pstUpdateList;
	CVwZip vwzip;
	LONG  lnStep;
	LONG  lnCount;


	//	���
	bDownload	= TRUE;
	lnStep		= 1;
	lnCount		= m_vcUpdateList.size();

	//
	//	�����б������д������ļ�
	//
	for ( m_vcIt = m_vcUpdateList.begin(); m_vcIt != m_vcUpdateList.end(); m_vcIt ++, lnStep ++ )
	{
		pstUpdateList = m_vcIt;

		//	���û�������ʾ��Ϣ
		//	...

		//	����ļ���������ɾ��֮
		if ( PathFileExists( pstUpdateList->szDownFile ) )
		{
			DeleteFile( pstUpdateList->szDownFile );
		}

		//	�����ļ�
		pstUpdateList->bDownSucc = CVwHttp::DownloadFile
					(
						pstUpdateList->szUrl,
						pstUpdateList->szDownFile,
						m_stUpInfo.hMainWnd,
						TRUE,
						szError,
						m_stUpInfo.dwTimeout
					);
		if ( pstUpdateList->bDownSucc )
		{
			//
			//	[ ��Ҫ ]
			//	����Լ��һ�� ZIP ������ֻ����һ���ļ����ļ��������� pstUpdateList->szRealFile
			//

			//	�ȱ�����سɹ� = FALSE
			pstUpdateList->bDownSucc = FALSE;

			_sntprintf( szFileExt, sizeof(szFileExt)-sizeof(TCHAR), "%s", PathFindExtension( pstUpdateList->szDownFile ) );
			if ( 0 == stricmp( ".zip", szFileExt ) )
			{
				//	��ѹ�ļ�����ʱ�����ļ������Ŀ¼
				if ( vwzip.UnZipFiles( pstUpdateList->szDownFile, m_stUpInfo.szUpdateDir, TRUE ) )
				{
					//	��ѹ�ɹ��󣬱�����سɹ� = TRUE
					pstUpdateList->bDownSucc = TRUE;

					//	ɾ�� ZIP ��
					DeleteFile( pstUpdateList->szDownFile );
				}
			}
		}

		//	��¼�����������������ģ�鶼���سɹ�
		bDownload &= pstUpdateList->bDownSucc;
	}

	return bDownload;
}

/**
 *	@ ˽�к���
 *	���и������з�����ָ����ģ��
 */
BOOL CVwUpdate::UpdateAllModules()
{
	BOOL bRet			= FALSE;
	CProcSecurityDesc procSdesc;
	TCHAR szError[ MAX_PATH ]	= {0};
	TCHAR szTemp[ MAX_PATH ]	= {0};
	STUPDATELIST * pstUpdateList;
	INT  nUpdateFile		= VWUPDATE_UPDATE_FAILED;
	BOOL bNeedReboot		= FALSE;	//	�Ƿ���Ҫ���������


	//
	//	���������ļ�
	//
	for ( m_vcIt = m_vcUpdateList.begin(); m_vcIt != m_vcUpdateList.end(); m_vcIt ++ )
	{
		pstUpdateList	= m_vcIt;

		//	��ʼ����һ���ļ�
		nUpdateFile = CVwUpdate::UpdateFile
			(
				pstUpdateList->szRealFile,
				pstUpdateList->szDstFile,
				FALSE,
				pstUpdateList->bReg,
				pstUpdateList->uExec
			);
		if ( VWUPDATE_UPDATE_OK == nUpdateFile )
		{
			//	���³ɹ�

			bRet	= TRUE;

			//	���ð�ȫ������
			procSdesc.SetSecurityDescForFilter( pstUpdateList->szDstFile, szError );
		}
		else if ( VWUPDATE_UPDATE_RESTART == nUpdateFile )
		{
			//	��Ҫ����
			
			bRet		= TRUE;
			bNeedReboot	= TRUE;

			//	���ð�ȫ������
			procSdesc.SetSecurityDescForFilter( pstUpdateList->szDstFile, szError );
		}
		else if ( VWUPDATE_UPDATE_FAILED == nUpdateFile )
		{
			//	����ʧ��

			bRet	= FALSE;

			//	�����Ǻ�̨���£����Ծ����ˣ�������һ��
			//break;
		}
	}

	return bRet;
}


/**
 *	ִ��һ���ļ�����
 */
UINT CVwUpdate::UpdateFile( LPCTSTR lpszSrc, LPCTSTR lpszDst, BOOL bForceUpdate, BOOL bReg, UINT uExec )
{
	//
	//	lpszSrc			- [in] Դ�ļ�
	//	lpszDst			- [in] Ŀ���ļ�
	//	bForceUpdate		- [in] �Ƿ�ǿ�Ƹ���
	//	bReg			- [in] �Ƿ���Ҫע��
	//	bRun			- [in] �Ƿ���Ҫ����
	//
	//	RETURN			- �����Ƿ�ɹ�����״̬
	//	REMARK			- �����ļ��汾���������ļ�����������ע��
	//
/*
#define VWUPDATE_EXEC_FLAG_NONE		0	//	������
#define VWUPDATE_EXEC_FLAG_EXEC		1	//	ִ��һ�� EXE ��ִ���ļ�
#define VWUPDATE_EXEC_FLAG_LOAD		2	//	�Ǹ� DLL ���� LoadLibrary��Ȼ��ִ���䵼������ Run
*/	

	assert( lpszSrc && lpszDst );

	if ( NULL == lpszSrc || NULL == lpszDst )
		return VWUPDATE_UPDATE_FAILED;
	if ( 0 == strlen( lpszSrc ) || 0 == strlen( lpszDst ) )
		return VWUPDATE_UPDATE_FAILED;
	if ( ! PathFileExists( lpszSrc ) )
		return VWUPDATE_UPDATE_FAILED;


	BOOL bIsExe			= FALSE;
	BOOL bUpdate			= FALSE;
	INT  nUpdateStatus		= VWUPDATE_UPDATE_OK;
	TCHAR szSrcVer[MAX_PATH]	= {0};
	TCHAR szDstVer[MAX_PATH]	= {0};

	LPCTSTR pdest = lpszSrc + strlen(lpszSrc) - 4*sizeof(TCHAR);
	if ( _tcsicmp( pdest, ".exe" ) == 0 )
		bIsExe = TRUE;

	if ( bForceUpdate )
	{
		bUpdate = TRUE;
	}
	else
	{
		//	����ǿ�Ƹ��µĻ�Ҫ�Ƚϰ汾��
		if ( PathFileExists( lpszDst ) )
		{
			_vwfunc_get_file_version( lpszSrc, szSrcVer, sizeof(szSrcVer) );
			_vwfunc_get_file_version( lpszDst, szDstVer, sizeof(szDstVer) );
			if ( _tcslen(szSrcVer) > 0 && _tcslen(szDstVer) > 0 )	// ��������ļ����а汾��Ϣ
			{
				//	��Դ�ļ��İ汾�Ŵ���Ŀ���ļ�ʱ����Ҫ����
				if ( _tcscmp( szSrcVer, szDstVer ) > 0 )
				{
					bUpdate = TRUE;
				}
			}
		}
		else
		{
			bUpdate = TRUE;
		}
	}
	
	if ( bUpdate )
	{
		switch( MyMoveFile( lpszSrc, lpszDst, FALSE ) )
		{
		case VWUPDATE_MOVE_FILE_ERROR:
			{
				nUpdateStatus = VWUPDATE_UPDATE_FAILED;
			}
			break;
			
		case VWUPDATE_MOVE_FILE_PENDING:
			{
				nUpdateStatus = VWUPDATE_UPDATE_RESTART;
				if ( bReg )
				{
					TCHAR szTemp[ MAX_PATH * 2 ]	= {0};
					_snprintf( szTemp, sizeof(szTemp)-sizeof(TCHAR), "regsvr32 /s %s", lpszDst );
					SHSetValue( HKEY_LOCAL_MACHINE, VWUPDATE_REGKEY_RUNONCE, lpszDst, REG_SZ, szTemp, _tcslen(szTemp) );
				}
			}
			break;
			
		default:
			{
				if ( bReg )
				{
					TCHAR szParameter[MAX_PATH]	= {0};
					_sntprintf( szParameter, sizeof(szParameter)-sizeof(TCHAR), "/s %s", lpszDst );
					ShellExecute( GetDesktopWindow(), "open", "regsvr32.exe", szParameter, NULL, SW_SHOW );
				}
				switch( uExec )
				{
				case VWUPDATE_EXEC_FLAG_EXEC:
					{
						if ( bIsExe )
						{
							// WinExec( lpszDst, SW_SHOW );
							_vwfunc_run_block_process( lpszDst );
						}
						else
						{
							if ( IsDllCanbeRun( lpszDst ) )
							{
								TCHAR szCmdLine[ MAX_PATH ] = {0};
								_sntprintf( szCmdLine, sizeof(szCmdLine)-sizeof(TCHAR), "rundll32 %s,Rundll32", lpszDst );
								// WinExec( szCmdLine, SW_SHOW );
								_vwfunc_run_block_process( szCmdLine );
							}
						}
					}
					break;
				case VWUPDATE_EXEC_FLAG_LOAD:
					{
						//	��� DLL ���� LoadLibrary��Ȼ��ִ���䵼������ Load
						//	...
						if ( IsDllCanbeLoad( lpszDst ) )
						{
							//	����֮ ...
							RunDllLoad( lpszDst, "" );
						}
					}
					break;
				}
			}
			break;
		}
	}
	
	return nUpdateStatus;
}

/**
 *	bRestartMoveǿ������������ʱ���ƶ��ļ�
 *	�ļ�·���������Ƕ�·����
 */
INT CVwUpdate::MyMoveFile( LPCTSTR lpExistingFileName, LPCTSTR lpNewFileName, BOOL bRestartMove )
{
	BOOL bRet;
	if ( !bRestartMove )
	{
		bRet = CopyFile( lpExistingFileName, lpNewFileName, FALSE );
		if ( TRUE == bRet )
		{
			MyDeleteFile( lpExistingFileName, FALSE ); 
			return VWUPDATE_MOVE_FILE_OK;
		}
	}
	
	//	WriteCnsini( lpExistingFileName, lpNewFileName );
	
	if ( IsWindowsNT() )
	{
		if ( MoveFileEx( lpExistingFileName, lpNewFileName, MOVEFILE_REPLACE_EXISTING | MOVEFILE_DELAY_UNTIL_REBOOT ) )
		{
			return VWUPDATE_MOVE_FILE_PENDING;
		}
	}
	else
	{
		if ( AddDelOrMoveToWinInit(lpExistingFileName, lpNewFileName) )
		{
			return VWUPDATE_MOVE_FILE_PENDING;
		}
	}
	return VWUPDATE_MOVE_FILE_ERROR;
}

/**
 *	����������ʱ���ɾ��
 */
INT CVwUpdate::MyDeleteFile( LPCTSTR lpFileName, BOOL bBootDelete )
{
	/*
		lpFileName	- [in] �ļ�ȫ·��
		bBootDelete	- [in] �Ƿ������������ɾ��
	*/
	BOOL bRet;
	INT  nRet;
	if ( !bBootDelete )
	{
		bRet = DeleteFile( lpFileName );
		if ( TRUE == bRet )
			return VWUPDATE_DELETE_FILE_OK;
		
		nRet = GetLastError();
	}
	else
	{
		nRet = ERROR_ACCESS_DENIED;
	}
	
	if ( ERROR_ACCESS_DENIED == nRet || ERROR_SHARING_VIOLATION == nRet )
	{
		if ( IsWindowsNT() )
		{
			MoveFileEx( lpFileName, NULL, MOVEFILE_DELAY_UNTIL_REBOOT );
			return VWUPDATE_DELETE_FILE_PENDING;
		}
		else
		{
			if ( AddDelOrMoveToWinInit( lpFileName, NULL ) )
				return VWUPDATE_DELETE_FILE_PENDING;
			return VWUPDATE_DELETE_FILE_ERROR;
		}
	}
	else if ( ERROR_FILE_NOT_FOUND == nRet )
		return TRUE;
	
	return TRUE;
}

BOOL CVwUpdate::IsWindowsNT()
{
	OSVERSIONINFO osif;
	osif.dwOSVersionInfoSize = sizeof(OSVERSIONINFO);
	GetVersionEx(&osif);
	
	if ( osif.dwPlatformId == VER_PLATFORM_WIN32_NT )
	{
		return TRUE;
	}
	else
	{
		return FALSE;
	}
}

BOOL CVwUpdate::GetWinInitFileName( LPTSTR lpszWininitFileName, DWORD dwSize )
{
	TCHAR szTemp[MAX_PATH]	= {0};
	INT   nRet		= 0;
	
	nRet = GetWindowsDirectory( szTemp, sizeof(szTemp) );
	if( 0 == nRet )
		return FALSE;
	if ( szTemp[strlen(szTemp)-sizeof(TCHAR)] != '\\' )
	{
		_tcscat( szTemp, "\\" );
	}
	
	_sntprintf( lpszWininitFileName, dwSize-sizeof(TCHAR), "%s%s", szTemp, VWUPDATE_WININIT_FILE_NAME );
	lpszWininitFileName[ dwSize-sizeof(TCHAR)] = 0;
	return TRUE;
}

BOOL CVwUpdate::AddDelOrMoveToWinInit( LPCTSTR lpExistingFileNameA, LPCTSTR lpNewFileNameA )
{
	TCHAR strWininitFileName[MAX_PATH] = {0};
	if ( FALSE == GetWinInitFileName( strWininitFileName,sizeof(strWininitFileName)) )
		return FALSE;
	
	FILE* pWininitFile;
	pWininitFile=fopen(strWininitFileName,"a+");
	if(NULL == pWininitFile)
		return FALSE;
	
	CHAR lpExistingFileName[MAX_PATH];
	CHAR lpNewFileName[MAX_PATH];
	GetShortPathName( lpExistingFileNameA, lpExistingFileName, sizeof(lpExistingFileName) );
	if( lpNewFileNameA )
	{
		int nRet = GetShortPathName( lpNewFileNameA, lpNewFileName, sizeof(lpNewFileName) );
		if( nRet == 0 || nRet > sizeof(lpNewFileName) )
		{
			memset( lpNewFileName,0,sizeof(lpNewFileName) );
			_snprintf( lpNewFileName,sizeof(lpNewFileName)-1, "%s", lpNewFileNameA );
		}
	}
	char strLineBuf[MAX_PATH*2+10];
	bool bFindRenameSection;
	bFindRenameSection=false;
	rewind(pWininitFile);
	while(NULL != (fgets(strLineBuf,sizeof(strLineBuf),pWininitFile)))
	{
		strLineBuf[sizeof(strLineBuf) -1] = '\0';
		
		if ( ! _strnicmp( VWUPDATE_WININIT_RENAME_SEC, strLineBuf, sizeof(VWUPDATE_WININIT_RENAME_SEC)-1 ) )
		{
			bFindRenameSection = true;
			break;
		}
	}
	fseek(pWininitFile,0,SEEK_END);
	if(!bFindRenameSection)
		fprintf(pWininitFile,"%s\n",VWUPDATE_WININIT_RENAME_SEC);
	if( lpNewFileNameA )
		fprintf(pWininitFile,"%s=%s\n", lpNewFileName, lpExistingFileName);
	else
	{
		int nLen = strlen(lpExistingFileName);
		if( lpExistingFileName[nLen-1] == '\\' )
		{
			lpExistingFileName[nLen-1] = '\0';
			fprintf(pWininitFile,"DIRNUL=%s\n",lpExistingFileName);
		}
		else
			fprintf(pWininitFile,"NUL=%s\n",lpExistingFileName);
	}
	
	fclose(pWininitFile);
	return TRUE;
}

/**
 *	�ж��Ƿ��ǿ������е� DLL �ļ�
 */
BOOL CVwUpdate::IsDllCanbeRun( LPCTSTR lpszFile )
{
	//
	//	lpszFile	- [in] �ļ�·��
	//	RETURN		- TRUE / FALSE

	if ( NULL == lpszFile )
		return FALSE;
	
	typedef VOID (CALLBACK* PRundll32)( HWND hMain, HINSTANCE hinst, LPTSTR lpCmdLine, int nCmdShow );

	assert( lpszFile );
	BOOL bRet = FALSE;
	HMODULE hDll = NULL;
	hDll = LoadLibrary( lpszFile );
	if( hDll )
	{
		PRundll32 Run = (PRundll32)GetProcAddress( hDll, "Rundll32" );
		if( Run )
			bRet = TRUE;
		FreeLibrary( hDll );
	}
	return bRet;
}

/**
 *	�ж��Ƿ��ǿ������е� DLL �ļ�
 */
BOOL CVwUpdate::IsDllCanbeLoad( LPCTSTR lpszFile )
{
	//
	//	lpszFile	- [in] �ļ�·��
	//	RETURN		- TRUE / FALSE

	assert( lpszFile );

	if ( NULL == lpszFile )
		return FALSE;

	BOOL bRet	= FALSE;
	HMODULE hDll	= NULL;
	PFNLOAD pfnLoad	= NULL;

	hDll = LoadLibrary( lpszFile );
	if ( hDll )
	{
		pfnLoad = (PFNLOAD)GetProcAddress( hDll, "Load" );
		if( pfnLoad )
		{
			bRet = TRUE;
		}
		FreeLibrary( hDll );
	}
	return bRet;
}

/**
 *	�ж��Ƿ��ǿ������е� DLL �ļ�
 */
BOOL CVwUpdate::RunDllLoad( LPCTSTR lpszFile, LPCTSTR lpcszCmd )
{
	//
	//	lpszFile	- [in] �ļ�·��
	//	RETURN		- TRUE / FALSE

	assert( lpszFile );

	if ( NULL == lpszFile )
		return FALSE;

	BOOL bRet	= FALSE;
	HMODULE hDll	= NULL;
	PFNLOAD pfnLoad	= NULL;

	hDll = LoadLibrary( lpszFile );
	if ( hDll )
	{
		pfnLoad = (PFNLOAD)GetProcAddress( hDll, "Load" );
		if( pfnLoad )
		{
			bRet = pfnLoad( lpcszCmd );
		}
		FreeLibrary( hDll );
	}
	return bRet;
}