/*
 ************************************************************
 *
 *	DeACHashTree.h
 *
 *	CLASS NAME:
 *		Desafe Ascii Code Hash Tree
 *		�ṩ����ǰƥ����ҵ��㷨
 *		* ����֧�� Ascii ����
 *		* ���ʺ�С�ʹʿ⣬����ÿ���ʶ��̵ܶ����
 *
 *	AUTHOR:
 *		������ liuqixing@gmail.com
 *
 *	HISTORY:
 *		2008-09-01 ������ liuqixing@gmail.com ���
 *
 ************************************************************
 */

#ifndef __DEACHASHTREE_HEADER__
#define __DEACHASHTREE_HEADER__

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include <windows.h>
#include <tchar.h>
#include <stdio.h>
#include <stdlib.h>

#include "VwInfoMonitorConfigFile.h"

#include <vector>
using namespace std;


/**
 *	define
 */
#define DEACHASHTREE_GETCHARSEX( cChar )	( cChar >= 0 ? 1 : 0 )
#define DEACHASHTREE_MAKEINT( low, high )	( ( (CHAR)(low) ) + ( ( (CHAR)(high) ) << 8 ) )

//	���ʳ���(�����ֽڼ��㣬һ������ 2 ���ֽ�)
//#define DEACHASHTREE_MAX_TREE_DEEP		CVWINFOMONITORCONFIGFILE_MAX_WORDLEN



/**
 *	struct
 */
typedef struct tagDeACHashTreeNode
{
	tagDeACHashTreeNode()
	{
		memset( this, 0, sizeof(tagDeACHashTreeNode) );
	}

	BYTE btNode[ 0x10000 ];	//	���� 0 ~ 0xFFFF ��λ��

}STDEACHASHTREENODE, *PSTDEACHASHTREENODE;

/*
typedef struct tagDeACHashTreeWord
{
	tagDeACHashTreeWord()
	{
		memset( this, 0, sizeof(tagDeACHashTreeWord) );
	}

	UINT uLen;
	CHAR szWord[ MAX_PATH ];

}STDEACHASHTREEWORD, *PSTDEACHASHTREEWORD;
*/




/**
 *	class of CDeACHashTree
 */
class CDeACHashTree :
	public CVwInfoMonitorConfigFile
{
public:
	CDeACHashTree();
	virtual ~CDeACHashTree();

public:
	VOID SetSoftRegStatus( BOOL bReged );
	BOOL BuildTree( LPCTSTR lpcszFilename );
	UINT GetOnlineTreeIndex();
	UINT GetOfflineTreeIndex();
	BOOL PushNewTreeOnline( STDEACHASHTREENODE * pstNewTree, UINT uTreeDeep );

	UINT PrefixMatch( LPCTSTR lpcszText, UINT uTextLen );


private:
	BOOL m_bSoftReged;			//	m_bSoftReged

	CRITICAL_SECTION m_oCrSecTree;

	BOOL m_bTreeSwitching;			//	�Ƿ����� �û� �����������µ���
	UINT m_uCurrTreeIndex;			//	0 or 1 ��ʾ��ǰ���ڹ�������
	UINT m_uCurrTreeDeep;			//	��ǰ�������
	STDEACHASHTREENODE * m_pArrTree[ 2 ];	//	����ʵ�ʵ���
	STDEACHASHTREENODE * m_pstTreeOn;	//	ָ�����ڹ�������
};




#endif // __DEACHASHTREE_HEADER__
