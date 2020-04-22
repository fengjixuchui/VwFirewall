#ifndef __VWCONST_HEADER__
#define __VWCONST_HEADER__


// VwConst.h: define all const value for the project
//
//////////////////////////////////////////////////////////////////////



/**
 *	���ܵ��ַ�������ͷ�ļ�
 */
#include "VwEnCodeString.h"
#include "VwConstBase.h"





// ----------------------------------------------------------------------
//	Ϊ�˷�ֹ�ƽ�Ķ�̬װ��

//	Kernel32.dll
typedef UINT (WINAPI * PFNGETPRIVATEPROFILEINTA)( LPCTSTR lpAppName, LPCTSTR lpKeyName, INT nDefault, LPCTSTR lpFileName );
typedef DWORD (WINAPI * PFNGETPRIVATEPROFILESTRINGA)( LPCTSTR lpAppName, LPCTSTR lpKeyName, LPCTSTR lpDefault, LPTSTR lpReturnedString, DWORD nSize, LPCTSTR lpFileName );
typedef BOOL (WINAPI * PFNWRITEPRIVATEPROFILESTRINGA)( LPCTSTR lpAppName, LPCTSTR lpKeyName, LPCTSTR lpString, LPCTSTR lpFileName );

//	�����ڴ����ƶ���
static const CHAR * CONST_FILEMAP_VW_COREINFO		= "FILEMAP_VW_COREINFO";


/**
 *	Ĭ��ҳԤ����
 */
struct STDEFAULTPAGE
{
	LPTSTR	pszPage;	//	ҳ������
	DWORD	dwLen;		//	ҳ�����Ƶ��ַ�������
};
static const STDEFAULTPAGE g_pArrPage[] = 
{
	{ (LPTSTR)"/block.",	(DWORD)_tcslen("/block.")	},
	{ (LPTSTR)"/default.",	(DWORD)_tcslen("/default.")	},
	{ (LPTSTR)"/index.",	(DWORD)_tcslen("/index.")	},
};

//
//	��������һ�����������ʴ���
//
static const UINT g_ArrAntAttMaxVisit[] =
{
	500, 1000, 2000, 3000, 5000, 7000, 10000, 20000, 30000
};
//
//	���������ܵ���������Ϣ�೤ʱ��
//
static const UINT g_ArrAntAttSleep[] =
{
	1, 3, 5, 10, 20, 30, 60,	//	��ʾ�ܾ���ʱ��
	28800				//	28800 = 60*24*20 ���ӣ�Ҳ���� 20 �죻��ʾ���þܾ�
};

//
//	VwInfoM����ֹ�ļ��ϴ��������ļ��ߴ��б�(��λ���ֽ�)
//
static const UINT g_ArrVwInfoMBlockUploadFileSizeCfg[] =
{
	1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 15, 18, 20, 25, 30, 40, 50, 60, 70, 80, 100, 1000, 2000, 3000, 4000
};



typedef struct tagLogInfo
{
	tagLogInfo()
	{
		memset( this, 0, sizeof(tagLogInfo) );
	}
	TCHAR szTime[ 32 ];
	TCHAR szType[ 32 ];
	TCHAR szUser[ 32 ];
	TCHAR szLeechs[ 1024 ];
	TCHAR szResource[ 1024 ];

}STLOGINFO, *PSTLOGINFO;

/*
	using in FormLeftTree
*/
typedef struct tagIdContainer
{
	tagIdContainer()
	{
		memset( this, 0, sizeof(tagIdContainer) );
	}
	LONG  lnIndex;		//	index
	TCHAR cType;		//	IIS = 'i', SERVU = 'u', MEDIA SERVER = 'm'
	TCHAR szId[ 32 ];	//	SITEID
	TCHAR szName[ MAX_PATH ];

}STIDCONTAINER, *PSTIDCONTAINER;


typedef struct tagDiyHeader
{
	tagDiyHeader()
	{
		memset( this, 0, sizeof(tagDiyHeader) );
	}
	DWORD	dwDhSoftVer;
	TCHAR	szCheckSum[ 64 ];
	TCHAR	szHeader[ 512 ];
	
}STDIYHEADER, *PSTDIYHEADER;



#endif	//	__VWCONST_HEADER__