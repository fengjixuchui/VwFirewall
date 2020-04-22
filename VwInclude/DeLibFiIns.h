#ifndef __DELIBFIINS_HEADER__
#define __DELIBFIINS_HEADER__

//////////////////////////////////////////////////////////////////////////
//
//	������ʷ
//
//	1, 0, 6, 1006	- liuqixing @ 2009-01-09
//			  ���� delib_fiins_visit_first_site
//
//


#ifndef __DELIBFIINS_STRUCT__
#define __DELIBFIINS_STRUCT__

#define DELIBFIINS_AUTOACTION_NONE		0
#define DELIBFIINS_AUTOACTION_INSTALL		1
#define DELIBFIINS_AUTOACTION_UNINSTALL		2

#define DELIBFIINS_STRING_FILTER_STATUS_L	20013
#define DELIBFIINS_STRING_FILTER_STATUS_UL	20014



/**
 *	�ṹ�嶨��
 */
typedef struct tagDeLibFiInsFileList
{
	tagDeLibFiInsFileList()
	{
		memset( this, 0, sizeof(tagDeLibFiInsFileList) );
	}
	
	TCHAR szFilename[ MAX_PATH ];	//	�ļ�ȫ·��
	
}STDELIBFIINSFILELIST, *LPSTDELIBFIINSFILELIST;

typedef struct tagDeLibFiIns
{
	tagDeLibFiIns()
	{
		memset( this, 0, sizeof(tagDeLibFiIns) );
	}
	HWND  hParentDlg;
	BOOL  bAutoCloseWindow;				//	��װ������ɺ��Ƿ��Զ��رմ���
	
	TCHAR szAppDir[ MAX_PATH ];
	TCHAR szCfgDir[ MAX_PATH ];
	TCHAR szCfgFile[ MAX_PATH ];

	DWORD dwFileListCount;				//	�ļ��б����ļ��ĸ���
	STDELIBFIINSFILELIST * pstFileList;		//	�ļ��б�
	
	TCHAR szCompany[ MAX_PATH ];
	TCHAR szCompanyUrl[ MAX_PATH ];
	TCHAR szDlgHint[ MAX_PATH ];
	
	DWORD dwAutoAction;
	TCHAR szMetaPath[ MAX_PATH ];
	TCHAR szFilterName[ MAX_PATH ];
	TCHAR szFilterDesc[ MAX_PATH ];
	TCHAR szFilterDllFile[ MAX_PATH ];
	
	BOOL  bInstallExtension;			//	�Ƿ񸽴�Ҫ��װ����չ
	TCHAR szExtensionName[ MAX_PATH ];
	TCHAR szExtensionDllFile[ MAX_PATH ];
	
}STDELIBFIINS, *PSTDELIBFIINS;


#endif



/**
 *	������������
 */
HINSTANCE __stdcall delib_fiins_get_instance();
INT  __stdcall delib_fiins_load_string( UINT uID, LPTSTR lpszBuffer, DWORD dwSize );
BOOL __stdcall delib_fiins_is_iisapppool_running();

BOOL __stdcall delib_fiins_showdlg( STDELIBFIINS * pstFilter );
BOOL __stdcall delib_fiins_showminidlg( STDELIBFIINS * pstFilter );
BOOL __stdcall delib_fiins_bginstall( STDELIBFIINS * pstFilter, BOOL bOpIIS );
BOOL __stdcall delib_fiins_order_filter_in_first( STDELIBFIINS * pstFilter, BOOL bOpIIS );
BOOL __stdcall delib_fiins_apply_ext_to_childrensite( STDELIBFIINS * pstFilter );
VOID __stdcall delib_fiins_visit_first_site();
BOOL __stdcall delib_fiins_isloaded( STDELIBFIINS * pstFilter, LPTSTR lpszFilterFilePathReal, DWORD dwSize );
BOOL __stdcall delib_fiins_isextinstall( STDELIBFIINS * pstFilter );


/**
 *	��̬���õ�����������
 */
typedef HINSTANCE (WINAPI * PFN_DELIB_FIINS_GET_INSTANCE)();
typedef INT (WINAPI * PFN_DELIB_FIINS_LOAD_STRING)( UINT uID, LPTSTR lpszBuffer, DWORD dwSize );
typedef BOOL (WINAPI * PFN_DELIB_FIINS_IS_IISAPPPOOL_RUNNING)();

typedef BOOL (WINAPI * PFN_DELIB_FIINS_SHOWDLG)( STDELIBFIINS * pstFilter );
typedef BOOL (WINAPI * PFN_DELIB_FIINS_SHOWMINIDLG)( STDELIBFIINS * pstFilter );
typedef BOOL (WINAPI * PFN_DELIB_FIINS_BGINSTALL)( STDELIBFIINS * pstFilter, BOOL bOpIIS );
typedef BOOL (WINAPI * PFN_DELIB_FIINS_ORDER_FILTER_IN_FIRST)( STDELIBFIINS * pstFilter, BOOL bOpIIS );
typedef BOOL (WINAPI * PFN_DELIB_FIINS_APPLY_EXT_TO_CHILDRENSITE)( STDELIBFIINS * pstFilter );
typedef VOID (WINAPI * PFN_DELIB_FIINS_VISIT_FIRST_SITE)();
typedef BOOL (WINAPI * PFN_DELIB_FIINS_ISLOADED)( STDELIBFIINS * pstFilter, LPTSTR lpszFilterFilePathReal, DWORD dwSize );
typedef BOOL (WINAPI * PFN_DELIB_FIINS_ISEXTINSTALL)( STDELIBFIINS * pstFilter );







#endif	// __DELIBFIINS_HEADER__