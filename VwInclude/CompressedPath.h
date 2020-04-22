// CompressedPath.h: interface for the CompressedPath class.
//
//////////////////////////////////////////////////////////////////////

#ifndef __CCOMPRESSEDPATH_HEADER__
#define __CCOMPRESSEDPATH_HEADER__


#include "shlobj.h"
#pragma comment( lib, "shell32.lib" )

//////////////////////////////////////////////////////////////////////////
#define SPPATH_SYSTEMDRIVE			0x101	// C:  ϵͳӲ��
#define SPPATH_WINDIR				0x102	// C:\WINNT	 WindowsĿ¼
#define SPPATH_SYSTEMROOT			0x103	// C:\WINNT	 ϵͳ����Ŀ¼
#define SPPATH_WINSYSTEM			0x104	// C:\WINNT\SYSTEM32
#define SPPATH_PROGRAMFILES			0x105	// C:\Program Files
#define SPPATH_USERPROFILE			0x106	// C:\Documents and Settings\Administrator ���� Administrator �ǵ�ǰ�û�username���ɱ��
#define SPPATH_ALLUSERPROFILE			0x107	// C:\Documents and Settings\All Users
#define SPPATH_COMMON_DESKTOPDIRECTORY		0x108	// C:\Documents and Settings\All Users\Desktop
#define SPPATH_COMMON_DOCUMENTS			0x109	// C:\Documents and Settings\All Users\Documents
#define SPPATH_DESKTOPDIRECTORY			0x110	// C:\Documents and Settings\username\Desktop
#define SPPATH_TEMP				0x111	// C:\WINNT\TEMP
#define SPPATH_TMP				0x112	// C:\WINNT\TEMP


class CCompressedPath
{
public:
	CCompressedPath();
	virtual ~CCompressedPath();

	BOOL GetCompressedPath( LPCTSTR lpszFilePathIn, LPTSTR lpszFilePathNew, DWORD dwPathNewSize, LPTSTR lpszWinDir, DWORD dwWDSize );

private:
	BOOL GetMySpecialFolder( INT nFolder, LPTSTR lpszPath, DWORD dwPSize );
	BOOL LeftPathReplace( LPCTSTR lpszSrcPath, LPTSTR lpszDstPath, DWORD dwDPSize, LPCTSTR lpszFind, LPCTSTR lpszRpWith );

};



#endif // __CCOMPRESSEDPATH_HEADER__
