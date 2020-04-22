//	VwUpdate.h: interface for the CVwUpdate class.
//	Created @ 2007-09-24
//
//////////////////////////////////////////////////////////////////////

#ifndef __VWUPDATE_HEADER__
#define __VWUPDATE_HEADER__

#include <vector>
using namespace std;

#include "VwConst.h"
#include "VwFunc.h"
#include "VwHttp.h"
#include "VwZip.h"
#include "MultiString.h"
#include "ProcSecurityDesc.h"

#include "shellapi.h"
#pragma comment( lib, "shell32.lib" )


/**
 *	�����ļ�����Ҫ�ĺ�
 */
#define VWUPDATE_UPDATE_FAILED		0	//	����ʧ��
#define VWUPDATE_UPDATE_OK		1	//	���³ɹ�
#define VWUPDATE_UPDATE_RESTART		2	//	��Ҫ����

#define VWUPDATE_MOVE_FILE_OK			0
#define VWUPDATE_MOVE_FILE_ERROR			1
#define VWUPDATE_MOVE_FILE_PENDING		2

#define VWUPDATE_DELETE_FILE_OK			0
#define VWUPDATE_DELETE_FILE_ERROR		1
#define VWUPDATE_DELETE_FILE_PENDING		2

#define VWUPDATE_REGKEY_RUNONCE			"Software\\Microsoft\\Windows\\CurrentVersion\\RunOnce"
#define VWUPDATE_WININIT_FILE_NAME		"wininit.ini"
#define VWUPDATE_WININIT_RENAME_SEC		"[rename]"


/**
 *	��ȡ INI ����Ҫ�ĺ�
 */
#define VWUPDATE_SECTION_ADD		"Add"
#define VWUPDATE_SECTION_UPDATE		"update"

#define VWUPDATE_KEY_URL		"Url"
#define VWUPDATE_KEY_REG		"Reg"
#define VWUPDATE_KEY_RUN		"Run"
#define VWUPDATE_KEY_EXEC		"Exec"
#define VWUPDATE_KEY_DETAIL		"Detail"
#define VWUPDATE_KEY_LASTWORK		"lastwork"

#define VWUPDATE_EXEC_FLAG_NONE		0	//	������
#define VWUPDATE_EXEC_FLAG_EXEC		1	//	ִ��һ�� EXE ��ִ���ļ�
#define VWUPDATE_EXEC_FLAG_LOAD		2	//	��� DLL ���� LoadLibrary��Ȼ��ִ���䵼������ Exec

#define VWUPDATE_UPDATECFGINI		"UpdateCfg.ini"	//	���ظ��������ļ�


/**
 *	DLL Load �ӿڵ�������
 */
typedef BOOL(WINAPI * PFNLOAD)( LPCTSTR lpcszCmd );



/**
 *	������������
 */
typedef struct tagUpdateInfo
{
	tagUpdateInfo()
	{
		memset( this, 0, sizeof(tagUpdateInfo) );
		dwTimeout = 6*60*1000;
	}

	DWORD dwTimeout;
	HWND  hMainWnd;
	TCHAR szUpdateUrl[ MAX_PATH ];
	TCHAR szUpdateDir[ MAX_PATH ];
	TCHAR szUpdateCfgIniFile[ MAX_PATH ];
	TCHAR szUpdateIniFile[ MAX_PATH ];
	TCHAR szDstDir[ MAX_PATH ];

}STUPDATEINFO, *PSTUPDATEINFO;


/**
 *	�����б�
 */
typedef struct tagUpdateList
{
	tagUpdateList()
	{
		memset( this, 0, sizeof(tagUpdateList) );
	}
	
	TCHAR szUrl[ MAX_PATH ];
	TCHAR szVer[ 32 ];		//	INI ��ָ�����ļ������°汾��
	BOOL  bReg;
	UINT  uExec;			//	��ϸ������ĺ�
	BOOL  bDownSucc;		//	��¼�Ƿ����سɹ�

	//	..
	TCHAR szUrlFileName[ 64 ];		//	ZIP ѹ������ļ���
	TCHAR szFileName[ 64 ];		//	�ļ���
	TCHAR szDownFile[ MAX_PATH ];	//	������ʱ��ŵ��ļ�·��
	TCHAR szRealFile[ MAX_PATH ];	//	�����ļ���ʵ����·��
	TCHAR szUnZipDir[ MAX_PATH ];	//	�ļ����غ������ѹ���ļ����������ʱ��ѹĿ¼
	TCHAR szDetail[ MAX_PATH ];	//	��ϸ
	
	//	..
	TCHAR szDstFile[ MAX_PATH ];	//	Ŀ���ļ�·��
	TCHAR szDstFileVer[ 32 ];	//	Ŀ���ļ��汾
	
}STUPDATELIST, *PSTUPDATELIST;




/**
 *	������
 */
class CVwUpdate :
	public CVwHttp
{
public:
	CVwUpdate();
	virtual ~CVwUpdate();

	//	���ø���������Ϣ
	BOOL SetUpdateInfo( STUPDATEINFO * pstUpInfo );

	//	��ʼ����
	BOOL StartUpdate();
	
	//	����������ʱ�����ļ�
	VOID CleanAllTempDataFile();

private:
	//	�Ƿ�׼����
	BOOL IsReady();

	//	�ӷ�������ȡ�Ƿ���Ҫ��������Ϣ
	BOOL GetUpdateIniFromServer();

	//	���ݱ��������ļ����жϵ�ǰ�Ƿ�ʼ�����������������Ϣ
	BOOL IsWorkTime();

	//	���������ʱ��
	VOID SaveLastWorkTime();

	//	����������Ϣ
	BOOL CheckUpdateInfo();
	
	//	���� INI �ļ�
	BOOL ParseIni( LPCTSTR lpszUpdateIni );
	
	//	�������з�����ָ����ģ��
	BOOL DownloadAllModules();
	
	//	���и������з�����ָ����ģ��
	BOOL UpdateAllModules();

	//
	//	�����ļ�������ϵ��
	//
	UINT UpdateFile( LPCTSTR lpszSrc, LPCTSTR lpszDst, BOOL bForceUpdate, BOOL bReg, UINT nExecFlag );
	INT  MyMoveFile( LPCTSTR lpExistingFileName, LPCTSTR lpNewFileName, BOOL bRestartMove );
	INT  MyDeleteFile( LPCTSTR lpFileName, BOOL bBootDelete );
	BOOL IsWindowsNT();
	BOOL GetWinInitFileName( LPTSTR lpszWininitFileName, DWORD dwSize );
	BOOL AddDelOrMoveToWinInit( LPCTSTR lpExistingFileNameA, LPCTSTR lpNewFileNameA );
	BOOL IsDllCanbeRun( LPCTSTR lpszFile );
	BOOL IsDllCanbeLoad( LPCTSTR lpszFile );
	BOOL RunDllLoad( LPCTSTR lpszFile, LPCTSTR lpcszCmd );

private:
	BOOL m_bIsReady;
	STUPDATEINFO m_stUpInfo;
	vector<STUPDATELIST>		m_vcUpdateList;
	vector<STUPDATELIST>::iterator	m_vcIt;

};

#endif	// __VWUPDATE_HEADER__
