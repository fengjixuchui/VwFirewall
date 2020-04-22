// DeACHashTree.cpp: implementation of the CDeACHashTree class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "DeACHashTree.h"

#include "shlwapi.h"
#pragma comment( lib, "shlwapi.lib" )



//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CDeACHashTree::CDeACHashTree()
{
	InitializeCriticalSection( & m_oCrSecTree );

	m_bSoftReged		= FALSE;	//	���ע��״̬
	m_bTreeSwitching	= FALSE;

	m_uCurrTreeIndex	= 0;
	m_uCurrTreeDeep		= 0;

	m_pArrTree[ 0 ]		= NULL;
	m_pArrTree[ 1 ]		= NULL;
	m_pstTreeOn		= m_pArrTree[ 0 ];	//	���Ϸ������
}

CDeACHashTree::~CDeACHashTree()
{
	DeleteCriticalSection( & m_oCrSecTree );
}

/**
 *	@ public
 *	�������ע��״̬
 */
VOID CDeACHashTree::SetSoftRegStatus( BOOL bReged )
{
	m_bSoftReged = bReged;
}

BOOL CDeACHashTree::BuildTree( LPCTSTR lpcszFilename )
{
	if ( NULL == lpcszFilename || ( ! PathFileExists( lpcszFilename ) ) )
	{
		return FALSE;
	}

	BOOL  bRet			= FALSE;
	FILE * fp			= NULL;
	vector<STVWINFOMWORD> vcWordList;
	vector<STVWINFOMWORD>::iterator it;
	STVWINFOMWORD stWord;
	STVWINFOMWORD * pstWord;

	UINT uMaxWordLoadCount	= 0;
	UINT uWordMaxSize	= 0;
	
	UINT i;
	STDEACHASHTREENODE * pstNewTree;

	UINT uDeep;
	UINT uAddStep;
	UCHAR ucCharL, ucCharH;
	WORD wPosVal;

	//	���ļ���װ�ش�
	vcWordList.clear();

	if ( m_bSoftReged )
	{
		uMaxWordLoadCount = 0;
	}
	else
	{
		uMaxWordLoadCount = ( 987 - 980 - 1 );	//	6
	}
	uWordMaxSize		= CVWINFOMONITORCONFIGFILE_MAX_WORDLEN;	//DEACHASHTREE_MAX_TREE_DEEP;
	CVwInfoMonitorConfigFile::LoadWordsFromFile( lpcszFilename, uMaxWordLoadCount, vcWordList, & uWordMaxSize );

	try
	{
		//	��ʼ��������
		if ( vcWordList.size() && uWordMaxSize )
		{
			pstNewTree = new STDEACHASHTREENODE[ uWordMaxSize + 1 ];
			if ( pstNewTree )
			{
				memset( pstNewTree, 0, ( sizeof(STDEACHASHTREENODE) ) * ( uWordMaxSize + 1 ) );

				for ( it = vcWordList.begin(); it != vcWordList.end(); it ++ )
				{
					pstWord = it;

					i	= 0;
					uDeep	= 0;

					while( i < pstWord->uLen )
					{
						//	Ĭ��ÿ�� +1
						uAddStep = 1;
						ucCharH = (UCHAR)pstWord->szWord[ i ];
						if ( ucCharH > 0x80 && i < ( pstWord->uLen - 1 ) )
						{
							//	����ȡ����λ������Ҫ +2
							uAddStep = 2;
							ucCharL  = (UCHAR)pstWord->szWord[ ( i + 1 ) ];
							wPosVal	 = MAKEWORD( ucCharL, ucCharH );
						}
						else
						{
							wPosVal	 = ucCharH;
						}

						pstNewTree[ uDeep ].btNode[ wPosVal ]	= 1;

						i += uAddStep;
						uDeep ++;
					}
				}

				//
				//	���� build ����������
				//
				PushNewTreeOnline( pstNewTree, uWordMaxSize );
			}
		}
	}
	catch (...)
	{
	}

	return bRet;
}

UINT CDeACHashTree::GetOnlineTreeIndex()
{
	return m_uCurrTreeIndex;
}

UINT CDeACHashTree::GetOfflineTreeIndex()
{
	return ( ( m_uCurrTreeIndex + 1 ) % 2 );
}

BOOL CDeACHashTree::PushNewTreeOnline( STDEACHASHTREENODE * pstNewTree, UINT uTreeDeep )
{
	if ( NULL == pstNewTree || 0 == uTreeDeep )
	{
		return FALSE;
	}

	//	��������
	//EnterCriticalSection( & m_oCrSecTree );
	m_bTreeSwitching = TRUE;


	//	������ߵ����ı��
	UINT uOfflineTreeIndex = GetOfflineTreeIndex();

	//	��ɾ���������� tree ������
	if ( m_pArrTree[ uOfflineTreeIndex ] )
	{
		delete [] m_pArrTree[ uOfflineTreeIndex ];
		m_pArrTree[ uOfflineTreeIndex ] = NULL;
	}

	//
	//	���� Tree �����ݽṹָ��ָ����������
	//
	m_pArrTree[ uOfflineTreeIndex ]	= pstNewTree;

	//
	//	���࣬�����µ� Tree ��������
	//
	m_pstTreeOn		= m_pArrTree[ uOfflineTreeIndex ];
	m_uCurrTreeIndex	= uOfflineTreeIndex;
	m_uCurrTreeDeep		= uTreeDeep;


	//	�뿪�����
	//LeaveCriticalSection( & m_oCrSecTree );
	m_bTreeSwitching = FALSE;

	return TRUE;
}

UINT CDeACHashTree::PrefixMatch( LPCTSTR lpcszText, UINT uTextLen )
{
	//
	//	����ƥ�䵽�ĳ���
	//

	if ( NULL == lpcszText || 0 == uTextLen )
	{
		return 0;
	}
	if ( m_bTreeSwitching )
	{
		//	��������������������ڽ��ӹ�������ֱ�ӷ���
		return 0;
	}
	if ( NULL == m_pstTreeOn )
	{
		return 0;
	}

	UINT i;
	UINT nMax;

	UINT uDeep;
	UINT uAddStep;
	UCHAR ucCharL, ucCharH;
	WORD  wPosVal;


	//	��������
	//EnterCriticalSection( & m_oCrSecTree );


	//	ɨ���������
	nMax	= min( ( m_uCurrTreeDeep + 1 ), uTextLen );
	i	= 0;
	uDeep	= 0;

	while( i < nMax )
	{
		//	Ĭ��ÿ�� +1
		uAddStep = 1;
		ucCharH  = (UCHAR)lpcszText[ i ];
		if ( ucCharH > 0x80 && i < ( nMax - 1 ) )
		{
			//	����ȡ����λ������Ҫ +2
			uAddStep = 2;
			ucCharL  = (UCHAR)lpcszText[ ( i + 1 ) ];
			wPosVal	 = MAKEWORD( ucCharL, ucCharH );
		}
		else
		{
			wPosVal	 = ucCharH;
		}

		if ( 0 == m_pstTreeOn[ uDeep ].btNode[ wPosVal ]  )
		{
			break;
		}

		//	�ƶ�ָ��λ��
		i += uAddStep;

		//	����ɨ�����
		uDeep ++;
	}


	//	�뿪�����
	//LeaveCriticalSection( & m_oCrSecTree );


	return i;
}




//////////////////////////////////////////////////////////////////////////
//	Private

