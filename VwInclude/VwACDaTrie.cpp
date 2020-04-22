// VwACDaTrie.cpp: implementation of the VwACDaTrie class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "VwACDaTrie.h"

#include "shlwapi.h"
#pragma comment( lib, "shlwapi.lib" )


//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CVwACDaTrie::CVwACDaTrie()
{
	InitializeCriticalSection( & m_oCrSecTree );
	
	m_bSoftReged		= FALSE;	//	���ע��״̬
	m_bLoadingDict		= FALSE;
	
	m_uCurrTreeIndex	= 0;		//	������������ֵ

	m_pcWdaTrie		= NULL;		//	���Ϸ������
}
CVwACDaTrie::~CVwACDaTrie()
{
	DeleteCriticalSection( & m_oCrSecTree );
}


/**
 *	@ public
 *	�������ע��״̬
 */
VOID CVwACDaTrie::SetSoftRegStatus( BOOL bReged )
{
	m_bSoftReged = bReged;
}


/**
 *	@ public
 *	�����ʵ��ļ�
 */
BOOL CVwACDaTrie::BuildDict( LPCTSTR lpcszWordsFile, LPCTSTR lpcszDictFile, vector<STVWINFOMWORD> * pvcWordList /*=NULL*/ )
{
	if ( NULL == lpcszWordsFile && NULL == pvcWordList )
	{
		return FALSE;
	}
	if ( lpcszWordsFile )
	{
		if ( 0 == _tcslen( lpcszWordsFile ) || FALSE == PathFileExists( lpcszWordsFile ) )
		{
			return FALSE;
		}
	}

	if ( NULL == lpcszDictFile || 0 == _tcslen( lpcszDictFile ) )
	{
		return FALSE;
	}

	BOOL  bRet	= FALSE;
	CVwTDaTrieAc<INT> cWdaTrie;
	vector<STVWINFOMWORD> vcWordList;
	vector<STVWINFOMWORD>::iterator it;
	vector<STVWINFOMWORD> * pvcSaveWordList;
	STVWINFOMWORD stWord;
	STVWINFOMWORD * pstWord;
	FILE * fp;

	UINT uMaxWordLoadCount;
	UINT uWordMaxSize;


	//	...
	bRet = FALSE;


	try
	{
		pvcSaveWordList		= NULL;
		uMaxWordLoadCount	= 0;
		uWordMaxSize		= 0;

		if ( pvcWordList )
		{
			//
			//	����
			//	�Ӻ������������� vector ����
			//
			pvcSaveWordList = pvcWordList;

			for ( it = pvcWordList->begin(); it != pvcWordList->end(); it ++ )
			{
				pstWord = it;
				StrTrim( pstWord->szWord, " \t\r\n" );
				pstWord->uLen = _tcslen( pstWord->szWord );
				if ( pstWord->uLen > uWordMaxSize )
				{
					uWordMaxSize = pstWord->uLen;
				}
			}
		}
		else
		{
			//
			//	�Ӵ��ļ���װ��
			//

			//	��Ѱ汾���ֻ������ 6 �����˴�
			//	0 or 6
			uMaxWordLoadCount	= ( m_bSoftReged ? 0 : ( 987 - 980 - 1 ) );
			uWordMaxSize		= CVWINFOMONITORCONFIGFILE_MAX_WORDLEN;	//	DEACHASHTREE_MAX_TREE_DEEP;
			vcWordList.clear();

			//
			//	���ļ���װ�ش�
			//
			if ( CVwInfoMonitorConfigFile::LoadWordsFromFile( lpcszWordsFile, uMaxWordLoadCount, vcWordList, & uWordMaxSize ) )
			{
				pvcSaveWordList = &vcWordList;
			}
		}

		if ( pvcSaveWordList )
		{
			//
			//	��ʼ��������
			//
			if ( pvcSaveWordList->size() && uWordMaxSize )
			{
				for ( it = pvcSaveWordList->begin(); it != pvcSaveWordList->end(); it ++ )
				{
					pstWord = it;
					StrTrim( pstWord->szWord, " \t\r\n" );
					pstWord->uLen = _tcslen( pstWord->szWord );
					if ( pstWord->uLen )
					{
						//
						//	���ʲ����ڴ�������
						//
						cWdaTrie.insert( (__charwt*)pstWord->szWord );
					}
				}

				//
				//	���浽�����ļ�
				//
				bRet = cWdaTrie.saveDict( lpcszDictFile );
			}

			if ( ! bRet && 0 == pvcSaveWordList->size() )
			{
				//	û�дʣ�����һ�����ļ�
				fp = fopen( lpcszDictFile, "w" );
				if ( fp )
				{
					fclose( fp );
					fp = NULL;
				}
			}
		}
	}
	catch (...)
	{
	}

	return bRet;
}


/**
 *	@ public
 *	ת���Ѿ�����õĴʵ��ļ�
 */
BOOL CVwACDaTrie::LoadDict( LPCTSTR lpcszDictFile )
{
	if ( NULL == lpcszDictFile || FALSE == PathFileExists( lpcszDictFile ) )
	{
		return FALSE;
	}
	if ( m_bLoadingDict )
	{
		return FALSE;
	}

	BOOL bRet	= FALSE;
	CVwTDaTrieAc<INT> * poWdaTrieOff;

	//
	//	��������
	//
	//EnterCriticalSection( & m_oCrSecTree );
	m_bLoadingDict = TRUE;

	//
	//	����������װ��
	//
	poWdaTrieOff = & m_cArrWdaTrie[ GetOfflineTreeIndex() ];
	if ( poWdaTrieOff )
	{
		//
		//	װ���´ʵ�
		//	# loadDict �������Զ������ǰ���ڴ�
		//
		if ( delib_get_file_size( lpcszDictFile ) > 0 )
		{
			if ( poWdaTrieOff->loadDict( lpcszDictFile ) )
			{
				bRet	= TRUE;

				//
				//	���ո�װ�ص�����������
				//	# ������������������
				//
				PushNewTreeOnline();
			}
		}
		else
		{
			//	���Ǹ�������ֱ�������´ʿ�����
			poWdaTrieOff->clearIndex();
			poWdaTrieOff->cleanMemory();

			//
			//	���ո�װ�ص�����������
			//	# ������������������
			//
			PushNewTreeOnline();
		}
	}

	//
	//	�뿪�����
	//
	//LeaveCriticalSection( & m_oCrSecTree );
	m_bLoadingDict = FALSE;


	return bRet;
}

/**
 *	@ public
 *	ǰƥ��
 */
UINT CVwACDaTrie::PrefixMatch( LPCTSTR lpcszText )
{
	//
	//	lpcszText	- �����ַ���
	//	RETURN		- ����ǰƥ�䵽���ַ����ĳ���
	//
	
	if ( NULL == lpcszText )
	{
		return FALSE;
	}
	if ( m_bLoadingDict )
	{
		return FALSE;
	}
	if ( NULL == m_pcWdaTrie )
	{
		return FALSE;
	}
	
	//	δָ��ƥ�䳤�ȣ�ֱ�� 0 ����
	return (UINT)( m_pcWdaTrie->prefixMatch( (__charwt*)lpcszText ) );
}
UINT CVwACDaTrie::PrefixMatch( LPCTSTR lpcszText, UINT uTextLen )
{
	//
	//	lpcszText	- �����ַ���
	//	uTextLen	- �����ַ�������
	//	RETURN		- ����ǰƥ�䵽���ַ����ĳ���
	//

	if ( NULL == lpcszText )
	{
		return FALSE;
	}
	if ( m_bLoadingDict )
	{
		return FALSE;
	}
	if ( NULL == m_pcWdaTrie )
	{
		return FALSE;
	}

	return (UINT)( m_pcWdaTrie->prefixMatch( (__charwt*)lpcszText ), uTextLen );
}



//////////////////////////////////////////////////////////////////////////
//	Private




UINT CVwACDaTrie::GetOnlineTreeIndex()
{
	return m_uCurrTreeIndex;
}

UINT CVwACDaTrie::GetOfflineTreeIndex()
{
	return ( ( m_uCurrTreeIndex + 1 ) % 2 );
}

BOOL CVwACDaTrie::PushNewTreeOnline()
{
	//
	//	��ȡ��ǰ��������ֵ
	//	������ ���� ��ǰ��
	//
	m_uCurrTreeIndex = GetOfflineTreeIndex();

	//
	//	�� ��������ָ�� ֱ��ָ�� ��ǰ��ָ��
	//
	m_pcWdaTrie = & m_cArrWdaTrie[ m_uCurrTreeIndex ];
	
	return TRUE;
}