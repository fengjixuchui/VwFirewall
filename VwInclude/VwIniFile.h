/*--------------------------------------------------------------------------------
 * CVwIniFile
 *
 * Update:
 *		2003-5-30 Create by liuqixing
 *		2010-5-23 Update by liuqixing
 *--------------------------------------------------------------------------------*/
#ifndef __VWINIFILE_HEADER__
#define __VWINIFILE_HEADER__

//////////////////////////////////////////////////////////////////////////////////////////
//
// �û��ӿ�˵��: �ڳ�Ա����SetVarStr��SetVarInt������,
// iType == 0 ����û��ƶ��Ĳ�����ini�ļ��в�����,���д���µı���.
// iType != 0 ����û��ƶ��Ĳ�����ini�ļ��в�����,�Ͳ�д���µı���,����ֱ�ӷ���FALSE;
//
//////////////////////////////////////////////////////////////////////////////////////////


//#include "afxtempl.h"
#include <windows.h>
#include <tchar.h>
#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <string>
#include <vector>
using namespace std;


typedef struct tagVwIniFileArray
{
	TCHAR szItem[ MAX_PATH ];
	UINT  uLength;

}STVWINIFILEARRAY, *LPSTVWINIFILEARRAY;



class CVwIniFile
{
public:
	CVwIniFile();			//���캯��
	virtual ~CVwIniFile();		//��������

private:
	CVwIniFile( const CVwIniFile & );	//���캯������
	CVwIniFile & operator = ( const CVwIniFile & );

public:
	//	��������
	virtual BOOL Create( LPCTSTR lpcszFileName );


	//	�õ�������������ֵ
	virtual INT  GetMyPrivateProfileInt( LPCTSTR lpcszSection, LPCTSTR lpcszVarName, INT nDefaultValue );
	virtual BOOL GetVarInt( LPCTSTR lpcszSection, LPCTSTR lpcszVarName, INT & nValue );
	//	�������ñ�����������ֵ
	//virtual BOOL SetVarInt( const CString &,const CString & ,const int &,const int iType = 1 );


	//	�õ������ַ�������ֵ
	virtual BOOL GetMyPrivateProfileString( LPCTSTR lpcszSection, LPCTSTR lpcszVarName, LPCTSTR lpcszDefaultValue, LPTSTR lpszRetValue, DWORD dwSize );
	virtual BOOL GetVarStr( LPCTSTR lpcszSection, LPCTSTR lpcszVarName, LPTSTR lpszReturnValue, DWORD dwSize );
	//	�������ñ����ַ�������ֵ
	//virtual BOOL SetVarStr( const CString &,const CString &, const CString &,const int iType = 1 );


	//	ɾ��ĳ�� KEY
	//virtual BOOL DelKey( const CString & strSection,const CString & strVarInfo );
	//	ɾ������ [Section]
	//virtual BOOL DelSection( const CString & strSection );


	//	��ĳ�� [Section] �е����� KEY ����һ�� KEY NAME ��������
	//virtual BOOL OrderKey( const CString & strSection, const CString & strByNewKeyName, const int & nStart );


	//	���ĳ�� [Section] �ı�������
	//virtual int SearchVarNum( const CString & );

	//	�ж�ĳ�� [Section] �Ƿ����
	//virtual BOOL IsSectionExist( const CString & strSection );

	//	��ȡĳ�� [Section] �е� nIndex �� KEY �����֣������һ�� strVarName �� nIndex=1
	//virtual BOOL GetVarName( const CString & strSection, CString & strReturnValue, const int & nIndex );


private:
	BOOL GetVar( LPCTSTR lpcszSection, LPCTSTR lpcszVarName, LPTSTR lpszReturnValue, DWORD dwSize );
	//BOOL SetVar( const CString &, const CString &, const CString &, const INT iType = 1 );
	INT  SearchLine( LPCSTR lpcszSection, LPCSTR lpcszVarName );
	//INT  SearchLine( const CString & strSection );

private:
//	vector <CString>  FileContainer;
	//CArray <CString,CString> m_caFileContainer;
	vector<STVWINIFILEARRAY> m_caFileContainer;
	vector<STVWINIFILEARRAY>::iterator m_it;
	BOOL  m_bFileExsit;
	BOOL  m_bModified;
	//CStdioFile m_cStdioFile;
	TCHAR m_szInIFileName[ MAX_PATH ];

};

#endif	// __VWINIFILE_HEADER__