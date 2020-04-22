// DeZip.cpp: implementation of the CDeZip class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "DeZip.h"
#include <stdio.h>
#include <stdlib.h>

CDeZip::CDeZip( PSP_UNCOMPRESS_CALLBACK pfnDetailInfo ) : m_pfnCallbackDetailInfo( pfnDetailInfo )
{
}

CDeZip::~CDeZip()
{
}


/**
 *	@ Public
 *	��ѹһ�� ZIP �ļ�
 */
BOOL CDeZip::UnZipFiles( LPCSTR lpZipFile, LPCSTR lpUnZipFileDir, BOOL bUnZipSubDir, BOOL bFirstEnter )
{
	/*
		lpZipFile		- [in] Zip �ļ�·��
		lpUnZipFileDir		- [in] ��ѹ����Ŀ¼
		bUnZipSubDir		- [in/opt] �Ƿ��ѹ��Ŀ¼ 
		bFirstEnter		- [in/opt] 
	*/
	
	if ( NULL == lpZipFile || NULL == lpUnZipFileDir )
	{
		return FALSE;
	}

	BOOL bRet = TRUE;
	
	if ( MyFileExists(lpZipFile) )
	{
		//
		//	��ѹ
		//
		if ( UnZip( lpZipFile, lpUnZipFileDir ) )
		{
			if ( ! bFirstEnter )
			{
				if ( DeleteFile( lpZipFile ) )
				{
					if ( m_pfnCallbackDetailInfo )
					{
						m_pfnCallbackDetailInfo( TRUE, OPERATOR_DELETE, lpZipFile, lpUnZipFileDir, ERROR_SUCCESS );
					}
				}
				else
				{
					if ( m_pfnCallbackDetailInfo )
					{
						m_pfnCallbackDetailInfo( FALSE, OPERATOR_DELETE, lpZipFile, lpUnZipFileDir, GetLastError() );
					}
					bRet = FALSE;
				}
			}
		}
		else
		{
			bRet = FALSE;
		}
	}
	
	return bRet;
}



//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////





/**
 *	�ж��ļ��Ƿ����
 */ 
BOOL CDeZip::MyFileExists( LPCTSTR lpszFile )
{
	BOOL bRet = FALSE;
	if ( lpszFile && _tcslen(lpszFile) > 0 )
	{
		FILE * fp = fopen( lpszFile, "r" );
		if( fp )
		{
			fclose( fp );
			bRet = TRUE;
		}
	}
	return bRet;
}


/**
 *	�滻�ַ�����ָ�����ַ�Ϊ�µ��ַ�
 */
BOOL CDeZip::StrReplace( char *lpSrc, char chOld, char chNew )
{
	//
	//	lpSrc	- [in] ָ���ַ���
	//	chOld	- [in] ָ���ַ�
	//	chNew	- [in] ���滻Ϊ�����ַ�
	//

	BOOL bRet	= FALSE;
	char *p		= NULL;
	if ( lpSrc && chOld )
	{
		bRet = TRUE;
		// ..
		p = lpSrc;
		if ( NULL == chNew )
		{
			while( p && *p )
			{
				if ( *p == chOld )
				{
					memmove( p, p + 1, strlen(p + 1) + 1 );
				}
				else
					p++;
			}
		}
		else
		{
			while( p && *p )
			{
				if ( *p == chOld )
				{
					*p = chNew;
				}
				p++;
			}
		}
	}

	return bRet;
}


/**
 *	��ȫ�ش������Ŀ¼
 */
BOOL CDeZip::MultiCreateDirectory( LPCTSTR lpcszDir )
{
	//
	//	lpcszDir	- [in] ��Ҫ������Ŀ¼
	//

	BOOL bRet			= FALSE;
	TCHAR szDirTem[ MAX_URL ]	= {0};
	LPTSTR lpszMov			= NULL;

	_sntprintf( szDirTem, sizeof(szDirTem)-sizeof(TCHAR), "%s", lpcszDir );
	if ( '\\' == szDirTem[_tcslen(szDirTem)-sizeof(TCHAR)] )
	{
		szDirTem[_tcslen(szDirTem)-sizeof(TCHAR)] = 0;
		return MultiCreateDirectory( szDirTem );
	}

	// ..
	bRet = FALSE;
	while ( TRUE )
	{
		SetLastError( 0 );
		if ( CreateDirectory( szDirTem, NULL ))
		{
			//	�����ļ��гɹ���ֱ�ӷ��ؼ���
			bRet = TRUE;
			break;
		}
		else
		{
			if ( ERROR_PATH_NOT_FOUND == GetLastError() )
			{
				//	���ָ���Ŀ¼�����ڣ���ֱ��ȡ����·�����ݹ鴴��֮
				lpszMov = _tcsrchr( szDirTem, '\\' );
				if ( lpszMov )
				{
					*lpszMov = '\0';
					bRet = MultiCreateDirectory( szDirTem );
				}
			}
			else
			{
				//	������ǡ��ļ��в����ڡ��Ĵ�����ֱ����������
				break;
			}
		}
	}

	return bRet;
}


/**
 *	�⿪һ�� Zip ѹ������ָ��Ŀ¼��
 */
BOOL CDeZip::UnZip( LPCSTR lpZipFile, LPCSTR lpUnZipFileDir )
{
	//
	//	lpZipFile	- [in] Zip ѹ������ȫ·��
	//	lpUnZipFileDir	- [in] �ļ���ѹ��Ŀ¼
	//

	ZIPENTRY ze;
	BOOL bRet			= FALSE;
	INT nItemsNum			= 0;
	INT i				= 0;
	TCHAR szZipFilePath[ MAX_URL ]	= {0};

	HZIP hz = OpenZip( (PVOID)lpZipFile, 0, ZIP_FILENAME );
	if ( hz )
	{
		GetZipItem( hz, -1, &ze );
		nItemsNum = ze.index;

		for ( i = 0; i < nItemsNum; i++ )
		{
			GetZipItem( hz, i, &ze );

			if ( '\\' == lpUnZipFileDir[ _tcslen(lpUnZipFileDir) - sizeof(TCHAR) ] )
			{
				_sntprintf( szZipFilePath, sizeof(szZipFilePath)-sizeof(TCHAR), "%s%s", lpUnZipFileDir, ze.name );
			}
			else
			{
				_sntprintf( szZipFilePath, sizeof(szZipFilePath)-sizeof(TCHAR), "%s\\%s", lpUnZipFileDir, ze.name );
			}
			StrReplace( szZipFilePath, '/', '\\' );
			char * psSearch = _tcsrchr( szZipFilePath, '\\' );
			if ( psSearch )
			{
				//	��ǿ�ƽ��������ļ�·�����������Ŀ¼
				*psSearch = '\0';

				//	��������ļ���
				MultiCreateDirectory( szZipFilePath );

				//	�ٻָ��������ļ�·��
				*psSearch = '\\';
			}

			//
			//	�ж��Ƿ����ļ�
			//
			if ( '\\' != szZipFilePath[ _tcslen(szZipFilePath) - sizeof(TCHAR) ] )
			{
				//	��ѹ����������ļ���Ӳ��
				VerifyZip( i, hz, szZipFilePath );
				if ( m_pfnCallbackDetailInfo )
				{
					m_pfnCallbackDetailInfo( TRUE, OPERATOR_UNCOMPRESS, lpZipFile, szZipFilePath , ERROR_SUCCESS );
				}
			}
		}

		CloseZip( hz );
		bRet = TRUE;
	}

	return bRet;
}

/**
 *	...
 */
VOID CDeZip::VerifyZip(int nCount, HZIP hz, LPCTSTR lpszFile)
{
	if ( ! hz )
	{
		return;
	}
	if ( ! lpszFile )
	{
		return;
	}

	ZIPENTRY ze;
	INT index			= -1;
	ZRESULT zr			= 0;
	TCHAR szTargetName[ MAX_URL ]	= {0};

	memset( &ze, 0, sizeof(ze) );
	zr = FindZipItem( hz, lpszFile, TRUE, &index, &ze );
	_sntprintf( szTargetName, sizeof(szTargetName)-sizeof(TCHAR), "%s", lpszFile );
	::DeleteFile( szTargetName );
	zr = UnzipItem( hz, index, szTargetName, 0, ZIP_FILENAME );
}