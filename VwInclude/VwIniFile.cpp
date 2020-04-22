// VwIniFile.cpp : implementation file
//


#include "stdafx.h"
#include "VwIniFile.h"

#include "Shlwapi.h"
#pragma comment( lib, "Shlwapi.lib" )



CVwIniFile::CVwIniFile()
{
	memset( m_szInIFileName, 0, sizeof(m_szInIFileName) );

	//	�ļ��Ƿ����
	m_bFileExsit	= FALSE;

	//	�ļ������Ƿ��޸�
	m_bModified	= FALSE;
}

CVwIniFile::~CVwIniFile()
{
	FILE * fp;
	STVWINIFILEARRAY * pstLine;

	if ( m_bModified )
	{
		//	���ݱ��޸Ĺ�
		if ( m_bFileExsit )
		{
			fp = fopen( m_szInIFileName, "w" );
			if ( fp )
			{
				for ( m_it = m_caFileContainer.begin(); m_it != m_caFileContainer.end(); m_it ++ )
				{
					pstLine = m_it;	
					if ( pstLine )
					{
						fwrite( pstLine->szItem, sizeof(TCHAR), pstLine->uLength, fp );
						fwrite( _T("\r\n"), 1, 2, fp );
					}
				}
				fclose( fp );
				fp = NULL;
			}
		}
	}

	if ( m_caFileContainer.size() )
	{
		m_caFileContainer.clear();
	}
}


BOOL CVwIniFile::Create( LPCTSTR lpcszFileName )
{
	FILE * fp;
	STVWINIFILEARRAY stLine;

	if ( NULL == lpcszFileName )
	{
		return FALSE;
	}

	m_bFileExsit		= FALSE;
	_sntprintf( m_szInIFileName, sizeof(m_szInIFileName)/sizeof(TCHAR)-1, _T("%s"), lpcszFileName );

	if ( PathFileExists( m_szInIFileName ) )
	{
		if ( m_caFileContainer.size() )
		{
			m_caFileContainer.clear();
		}
	
		fp = fopen( m_szInIFileName, "r" );
		if ( fp )
		{
			while ( ! feof( fp ) )
			{
				memset( &stLine, 0, sizeof(stLine) );
				if ( fgets( stLine.szItem, sizeof(stLine.szItem), fp ) )
				{
					stLine.uLength = _tcslen(stLine.szItem);
					m_caFileContainer.push_back( stLine );
				}
			}

			fclose( fp );
			fp = NULL;
		}

		m_bFileExsit = ( m_caFileContainer.size() > 0 );
	}

	return m_bFileExsit;
}

INT CVwIniFile::GetMyPrivateProfileInt( LPCTSTR lpcszSection, LPCTSTR lpcszVarName, INT nDefaultValue )
{
	INT nRet;

	if ( NULL == lpcszSection || NULL == lpcszVarName )
	{
		return nDefaultValue;
	}

	nRet = 0;
	if ( ! GetVarInt( lpcszSection, lpcszVarName, nRet ) )
	{
		nRet = nDefaultValue;
	}

	return nRet;
}
BOOL CVwIniFile::GetVarInt( LPCTSTR lpcszSection, LPCTSTR lpcszVarName, INT & nValue )
{
	BOOL bRet;
	TCHAR szReturnVar[ MAX_PATH ];

	if ( NULL == lpcszSection || NULL == lpcszVarName )
	{
		return FALSE;
	}

	bRet	= FALSE;
	nValue	= 0;

	if ( GetVar( lpcszSection, lpcszVarName, szReturnVar, sizeof(szReturnVar) ) )
	{
		StrTrim( szReturnVar, _T("\r\n\t\" ") );

		bRet	= TRUE;
		nValue	= atoi( szReturnVar );
	}

	return bRet;
}
/*
BOOL CVwIniFile::SetVarInt( const CString & strSection, const CString & strVarName, const int & iValue, const INT iType )
{
	BOOL bRet;
	CString strVar;

	//	�����޸ı��
	m_bModified = TRUE;

	strVar.Format( _T("%d"), iValue );
	bRet = SetVar( strSection, strVarName, strVar, iType );
	if ( bRet )
	{
		//	�����޸ı��
		m_bModified = TRUE;
	}

	return bRet;
}
*/

BOOL CVwIniFile::GetMyPrivateProfileString( LPCTSTR lpcszSection, LPCTSTR lpcszVarName, LPCTSTR lpcszDefaultValue, LPTSTR lpszRetValue, DWORD dwSize )
{
	BOOL bRet;

	if ( NULL == lpcszSection || NULL == lpcszVarName || NULL == lpszRetValue || 0 == dwSize )
	{
		return FALSE;
	}

	bRet = FALSE;

	if ( GetVarStr( lpcszSection, lpcszVarName, lpszRetValue, dwSize ) )
	{
		bRet = TRUE;
	}
	else
	{
		if ( lpcszDefaultValue )
		{
			_sntprintf( lpszRetValue, dwSize/sizeof(TCHAR)-1, _T("%s"), lpcszDefaultValue );
		}	
	}

	return bRet;
}
BOOL CVwIniFile::GetVarStr( LPCTSTR lpcszSection, LPCTSTR lpcszVarName, LPTSTR lpszReturnValue, DWORD dwSize )
{
	BOOL bRet;

	if ( NULL == lpcszSection || NULL == lpcszVarName || NULL == lpszReturnValue || 0 == dwSize )
	{
		return FALSE;
	}

	bRet = GetVar( lpcszSection, lpcszVarName, lpszReturnValue, dwSize );
	if ( bRet )
	{
		StrTrim( lpszReturnValue, _T("\r\b\t\" ") );
	}

	return bRet;
}
/*
BOOL CVwIniFile::SetVarStr( const CString & strSection, const CString & strVarName, const CString & strValue, const INT iType)
{
	BOOL bRet;

	bRet = SetVar( strSection, strVarName, strValue, iType );
	if ( bRet )
	{
		//	�����޸ı��
		m_bModified = TRUE;
	}

	return bRet;
}
*/
/*
//	ɾ��ĳ�� KEY
BOOL CVwIniFile::DelKey( const CString & strSection, const CString & strVarInfo )
{
	if ( FALSE == m_bFileExsit || 0 == m_caFileContainer.GetSize() )
	{
		//	һ�����ļ���ֱ�ӷ���
		return FALSE;
	}

	BOOL bRet;
	int i = 0;
	int iFileLines = m_caFileContainer.GetSize();
	CString strTVarInfo;
	CString	strValue;
	CString strSectionList;
	CString strParam;
	int nSlash;

	//	...
	bRet = FALSE;

	strTVarInfo = strVarInfo;
	strTVarInfo.TrimLeft();
	strTVarInfo.TrimRight();
	strTVarInfo.MakeLower();

	while( i < iFileLines )
	{
		strValue = m_caFileContainer.GetAt(i++);
		strValue.TrimLeft();
		
		if ( strValue.Find( _T("[") ) >= 0 && 
			strValue.Find(strSection) >= 0 )
		{
			while ( i < iFileLines )
			{
				strSectionList = m_caFileContainer[i++];
				strSectionList.TrimLeft();
				strSectionList.TrimRight();
				strSectionList.MakeLower();
				
				//if ( strSectionList.Find( _T("//") ) >= 0 )
				if ( _T(";") == strSectionList.Left(1) )
				{
					//	�ҵ�ע����
					continue;
				}	

				if ( strSectionList.Find( strTVarInfo ) >= 0 )
				{
					//	�ҵ���ָ���� KEY ���ڵ���
					strParam = strSectionList;
					strParam.TrimLeft();
					strParam.MakeLower();
					
					nSlash = strParam.Find( _T("user") );
					if ( 0 == nSlash )
					{
						bRet = TRUE;
						m_caFileContainer.RemoveAt( i - 1, 1 );
						break;
					}
				}
			}//end of while
		}
	
		if ( bRet )
		{
			break;
		}	
	}

	if ( bRet )
	{
		//	�����޸ı��
		m_bModified = TRUE;	
	}	

	return bRet;
}

//	ɾ������ [Section]
BOOL CVwIniFile::DelSection( const CString & strSection )
{
	BOOL bRet;
	INT i;
	INT iFileLines;
	CString	strValue;
	CString TestSectionName;

	if ( FALSE == m_bFileExsit || 0 == m_caFileContainer.GetSize() )
	{
		//	һ�����ļ���ֱ�ӷ���
		return FALSE;
	}


	bRet		= FALSE;
	i		= 0;
	iFileLines	= m_caFileContainer.GetSize();

	//for(pInterator;pInterator != m_caFileContainer.end();++pInterator)
	//{
	while ( i < iFileLines )
	{
		strValue = m_caFileContainer.GetAt( i++ );
		strValue.TrimLeft();

		if ( strValue.Find( _T("[") ) >= 0 &&
			strValue.Find( strSection ) >= 0 )
		{
			TestSectionName = "";
			TestSectionName.Format( _T("[%s]"), strSection );
			TestSectionName.MakeLower();
			strValue.TrimLeft();
			strValue.TrimRight();
			strValue.MakeLower();
	
			if ( TestSectionName == strValue )
			{
				//	��������˵��������ҵ���ָ����Ϣ
				bRet = TRUE;
				m_caFileContainer.RemoveAt( i - 1, SearchVarNum( strSection ) + 1 );
			}
			else
			{
				//	����ҵ��� [Section] ����ȫ����ָ���� Section��ֱ�ӷ���
			}

			break;
		}
	}

	if ( bRet )
	{
		//	�����޸ı��
		m_bModified = TRUE;
	}

	return bRet;
}

//	��ĳ�� [Section] �е����� KEY ����һ�� KEY NAME ��������
BOOL CVwIniFile::OrderKey( const CString & strSection, const CString & strByNewKeyName, const int & nStart )
{
	int i;
	int iFileLines;
	int nStartLine;
	int nVarKeyNum;
	int nEndLine;

	if ( FALSE == m_bFileExsit || 0 == m_caFileContainer.GetSize() )
	{
		//	һ�����ļ���ֱ�ӷ���
		return FALSE;
	}

	i = 0;
	iFileLines	= m_caFileContainer.GetSize();
	nStartLine	= SearchLine( strSection );	//	�ҵ���ʼ��λ��
	nVarKeyNum	= SearchVarNum( strSection );	//	�ҵ�ĳ�� [Section] �µ� KEY ����
	nEndLine	= nStartLine + nVarKeyNum;	//	�ҵ�����λ��

	if ( nEndLine > iFileLines )
	{
		return FALSE;
	}
	if ( 0 == nVarKeyNum )
	{
		//	��� Domain1 ����û���κ� KEY��������ֱ�ӷ��� TRUE
		return TRUE;
	}


	int nNewStart	= nStart;
	int nSlash	= 0;
	CString strValue, strKeyVal, strNewLine;

	for ( i = nStartLine; i < nEndLine; i ++ )
	{
		strValue	= m_caFileContainer.GetAt( i );
		nSlash		= strValue.Find( _T("=") );
		strKeyVal	= strValue.Mid( nSlash + 1 );
		strNewLine.Format( _T("%s%d=%s"), strByNewKeyName, nNewStart++, strKeyVal );
		m_caFileContainer.SetAt( i, strNewLine );
	}

	//	�����޸ı��
	m_bModified = TRUE;

	return TRUE;
	//˵����nStartLine �Ǳ�ʾ�� 1 ��ʼ���ļ��У��� m_caFileContainer �Ǵ� 0 ��ʼ���ļ���
	//���ԣ�for �Ĳ��� i=nStartLine�������� nStartLine+1
	//		nStartLine + nVarKeyNum = nEndLine ���������һ��
}
*/
/*
//	���� ĳ�� [Section] �� KEY ������
INT CVwIniFile::SearchVarNum( const CString & strSection )
{
	CString strTSection;
	int i;
	int iFileLines;
	CString strValue;
	int VarNum;
	CString strSectionList;

	if ( FALSE == m_bFileExsit || m_caFileContainer.GetSize() <= 0 )
	{
		return 0;
	}

	strTSection = strSection;
	strTSection.TrimLeft();
	strTSection.TrimRight();
	strTSection.MakeLower();

	i = 0;
	iFileLines = m_caFileContainer.GetSize();
	
	while ( i < iFileLines )
	{
		strValue = m_caFileContainer[ i++ ];
		strValue.TrimLeft();
		strValue.TrimRight();
		strValue.MakeLower();
	
		if ( strValue.Find( _T("[") ) >=0 &&
			strValue.Find( strTSection, 1 ) >= 0 )
		{
			//	�ҵ�ָ�� [Section]
			VarNum	= 0;
			while( i < iFileLines )
			{
				strSectionList = m_caFileContainer[ i++ ];
				
				strSectionList.TrimLeft();
				strSectionList.TrimRight();

				//if ( strSectionList.Find( _T("//") ) >= 0 )
				if ( _T(";") == strSectionList.Left(1) )
				{	
					//	�ҵ�ע����
					continue;
				}
				
				if ( strSectionList.Find( _T("["), 0 ) >= 0 )
				{
					//	����һ���������,Ѱ��ʧ��
					return VarNum;
				}

				strSectionList.TrimRight();
				if ( ! strSectionList.IsEmpty() )
				{	
					VarNum++;
				}	
			}
			return VarNum;
		}
	}

	return 0;
}

BOOL CVwIniFile::IsSectionExist( const CString & strSection )
{
	BOOL bRet;
	CString strTSection;
	int i;
	int iFileLines;
	CString strValue;
	CString TestSectionName;

	if ( FALSE == m_bFileExsit || m_caFileContainer.GetSize() <=0 )
	{	
		return FALSE;
	}	

	strTSection = strSection;
	strTSection.TrimLeft();
	strTSection.TrimRight();
	strTSection.MakeLower();

	bRet		= FALSE;
	i		= 0;
	iFileLines	= m_caFileContainer.GetSize();

	while ( i < iFileLines )
	{
		strValue = m_caFileContainer[ i++ ];
		strValue.TrimLeft();
		strValue.TrimRight();
		strValue.MakeLower();

		if ( strValue.Find( _T("[") ) >=0 &&
			strValue.Find( strTSection, 1 ) >= 0 )
		{
			//	�ҵ�ָ�� [Section]
			TestSectionName = "";
			TestSectionName.Format( _T("[%s]"), strSection );
			TestSectionName.MakeLower();

			if ( TestSectionName == strValue )
			{
				bRet = TRUE;
			}
			else
			{
				//	����ҵ��� [Section] ����ȫ����ָ���� Section��ֱ�ӷ���
			}

			break;
		}
	}

	return bRet;
}

//	��ȡĳ�� [Section] �е� nIndex �� KEY �����֣������һ�� strVarName �� nIndex=1
BOOL CVwIniFile::GetVarName( const CString & strSection, CString & strReturnValue, const int & nIndex = 1 )
{
	BOOL bRet;
	INT  nLine;
	CString	strParam;
	INT nSlash;

	if ( FALSE == m_bFileExsit || m_caFileContainer.GetSize() <= 0 )
	{
		//	�ļ������ڻ���Ϊ��
		return FALSE;
	}
	if ( nIndex < 1 )
	{
		return FALSE;
	}

	bRet	= FALSE;
	nLine	= SearchLine(strSection);
	if ( nLine > 0 )
	{
		bRet = TRUE;

		strParam = m_caFileContainer[ nLine + ( nIndex - 1 ) ];
		strParam.TrimLeft();
		nSlash		= strParam.Find( _T("=") );
		strReturnValue	= strParam.Mid( 0, nSlash );
	}

	return bRet;
}
*/





//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
//	Private




/**
 *	@ Private
 *	��ȡֵ
 */
BOOL CVwIniFile::GetVar( LPCTSTR lpcszSection, LPCTSTR lpcszVarName, LPTSTR lpszReturnValue, DWORD dwSize )
{
	if ( NULL == lpcszSection || NULL == lpcszVarName || NULL == lpszReturnValue || 0 == dwSize )
	{
		return FALSE;
	}	
	if ( FALSE == m_bFileExsit || 0 == m_caFileContainer.size() )
	{
		//	�ļ������ڻ���Ϊ��
		return FALSE;
	}

	BOOL bRet;
	INT  nLineIndex;
	STVWINIFILEARRAY stItem;
	TCHAR * pszEqualMark;

	bRet		= FALSE;	
	nLineIndex	= SearchLine( lpcszSection, lpcszVarName );
	if ( nLineIndex > 0 )
	{
		stItem = m_caFileContainer.at( nLineIndex - 1 );
		pszEqualMark = _tcschr( stItem.szItem, '=' );
		if ( pszEqualMark )
		{
			//	..
			bRet = TRUE;

			//	..
			_sntprintf( lpszReturnValue, dwSize/sizeof(TCHAR)-1, _T("%s"), pszEqualMark );
			StrTrim( lpszReturnValue, _T("\r\n\t\"= ") );
		}
	}

	return bRet;
}

/**
 *	@ Private
 *	����ֵ
 */
/*
BOOL CVwIniFile::SetVar( const CString & strSection, const CString & strVarName, const CString & strVar, const int iType )
{
	if ( FALSE == m_bFileExsit )
	{
		return FALSE;
	}

	INT i;
	INT iFileLines;
	CString	strValue;
	CString strSectionList;
	CString strParam;
	int iPre;
	CString strBehind;

	i		= 0;
	iFileLines	= m_caFileContainer.GetSize();

	if ( 0 == iFileLines )
	{
		//	һ�����ļ�������µ�����
		m_caFileContainer.Add( _T("[") + strSection + _T("]") );
		m_caFileContainer.Add( strVarName + _T("=") + strVar );
		return TRUE;
	}
	else
	{
		while ( i < iFileLines )
		{
			strValue = m_caFileContainer.GetAt( i++ );
			strValue.TrimLeft();
			strValue.TrimRight();
		
			if ( ( strValue.Find( _T("[") ) >= 0 ) && ( strValue.Find( strSection ) >= 0 ) )
			{
				while( i < iFileLines )
				{
					strSectionList	= m_caFileContainer[i++];
					strSectionList.TrimLeft();
					strSectionList.TrimRight();

					//if ( strSectionList.Find( _T("//") ) >= 0 )
					if ( _T(";") == strSectionList.Left(1) )
					{
						//	�ҵ�ע����
						//	TODO: ����������Ǻܺã���Ҫ��һ������
						continue;
					}

					if ( strSectionList.Find( strVarName ) >= 0 )
					{
						//	�ҵ�
						strParam = strVarName + _T("=") + strVar;
						//m_caFileContainer.SetAt(i-1,strParam);
						m_caFileContainer[ i - 1 ] = strParam;
						return TRUE;
					}

					if ( strSectionList.Find( _T("[") , 0 ) >= 0 )
					{
						//	��ԭ���ļ���SECTION��,û����Ӧ�ı�����Ҫ��Ӷ���,���������,�±߻��б��section
						//	����������������,���Ȱѵ�ǰ����ֵ��������ƶ�,Ȼ���ڵ�ǰλ�ü����³��ֵ���ֵ
						if ( 0 != iType )
						{	
							return FALSE;
						}

						m_caFileContainer.Add( strParam );
						iPre = m_caFileContainer.GetSize() - 1;
						while ( iPre >= i )
						{
							strBehind = m_caFileContainer[ iPre - 1 ];
							m_caFileContainer[ iPre ] = strBehind;
							iPre --;
						}

						strParam = strVarName + _T("=") + strVar;
						m_caFileContainer.SetAt( i - 1, strParam );
						return TRUE;
					}

					if ( i == iFileLines && 0 == iType )
					{
						m_caFileContainer.Add( strVarName + _T("=") + strVar );
						return TRUE;
					}
				}
			}
		}

		if ( 0 == iType )
		{
			m_caFileContainer.Add( _T("[") + strSection + _T("]") );
			m_caFileContainer.Add( strVarName + _T("=") + strVar );
		}
	}	

	return TRUE;
}
*/

INT CVwIniFile::SearchLine( LPCSTR lpcszSection, LPCSTR lpcszVarName )
{
	TCHAR szTSection[ 128 ];
	TCHAR szVarName[ 128 ];
	INT   nTSectionLen;
	INT   nVarNameLen;
	int i;
	int iFileLines;
	STVWINIFILEARRAY stItem;
	STVWINIFILEARRAY stSectionList;

	if ( NULL == lpcszSection || NULL == lpcszVarName )
	{
		return -1;
	}
	if ( FALSE == m_bFileExsit || 0 == m_caFileContainer.size() )
	{
		//	�ļ������ڻ���Ϊ��
		return -1;
	}

	_sntprintf( szTSection, sizeof(szTSection)/sizeof(TCHAR)-1, _T("[%s]"), lpcszSection );
	StrTrim( szTSection, _T("\r\n\t\b ") );
	_tcslwr( szTSection );
	nTSectionLen = _tcslen( szTSection );

	_sntprintf( szVarName, sizeof(szVarName)/sizeof(TCHAR)-1, _T("%s"), lpcszVarName );
	StrTrim( szVarName, _T("\r\n\t\b ") );
	_tcslwr( szVarName );
	nVarNameLen = _tcslen( szVarName );

	i = 0;
	iFileLines = m_caFileContainer.size();

	while( i < iFileLines )
	{
		stItem = m_caFileContainer.at( i ++ );
		StrTrim( stItem.szItem, _T("\r\n\t\b ") );
		_tcslwr( stItem.szItem );
		stItem.uLength = _tcslen( stItem.szItem );

		if ( stItem.uLength == nTSectionLen &&
			0 == _tcsnicmp( stItem.szItem, szTSection, nTSectionLen ) )
		{
			while( i < iFileLines )
			{
				stSectionList = m_caFileContainer.at( i ++ );
				StrTrim( stSectionList.szItem, _T("\r\n\t\b ") );
				_tcslwr( stSectionList.szItem );
				stSectionList.uLength = _tcslen( stSectionList.szItem );

				if ( 0 == _tcsnicmp( _T(";"), stSectionList.szItem, 1 ) )
				{
					//	�ҵ�ע����
					continue;
				}
				if ( 0 == _tcsnicmp( stSectionList.szItem, _T("["), 1 ) )
				{
					//	����һ���������,Ѱ��ʧ��
					return -2;
				}

				if ( stSectionList.uLength > nVarNameLen &&
					0 == _tcsnicmp( stSectionList.szItem, szVarName, nVarNameLen ) )
				{
					//	�ҵ�
					//	�˷���ֵ����һ�� i=1
					return i;
				}
			}
		}
	}

	return -1;
}

/*
int CVwIniFile::SearchLine( const CString & strSection )
{
	CString strTSection = strSection;
	int i;
	int iFileLines;
	CString strValue;
	CString TestSectionName;

	if ( FALSE == m_bFileExsit || m_caFileContainer.GetSize() <= 0 )
	{
		//	�ļ������ڻ���Ϊ��
		return -1;
	}

	strTSection.TrimLeft();
	strTSection.TrimRight();
	strTSection.MakeLower();

	i = 0;
	iFileLines = m_caFileContainer.GetSize();
	
	while( i < iFileLines )
	{
		strValue = m_caFileContainer[i++];
		strValue.TrimLeft();
		strValue.TrimRight();
		strValue.MakeLower();
		
		if ( strValue.Find( _T("[") ) >=0 &&
			strValue.Find( strTSection, 1 ) >= 0 )
		{
			TestSectionName = "";
			TestSectionName.Format( _T("[%s]"), strTSection );
			TestSectionName.MakeLower();

			if ( TestSectionName != strValue )
			{
				//	����ҵ��� [Section] ����ȫ����ָ���� Section��ֱ�ӷ��� -1
				return -1;
			}

			//	�˷���ֵ����һ�� i=1
			return i;
		}
	}

	return -1;
}
*/