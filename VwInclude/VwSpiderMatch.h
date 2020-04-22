// VwSpiderMatch.h: interface for the CVwSpiderMatch class.
//
//////////////////////////////////////////////////////////////////////

#ifndef __VWSPIDERMATCH_HEADER__
#define __VWSPIDERMATCH_HEADER__

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "DeLib.h"
#pragma comment( lib, "DeLib.lib" )


/**
 *	������������֩������ֵ
 */
enum
{
	VWSPIDERMATCH_IDX_GOOGLE = 0,
	VWSPIDERMATCH_IDX_BAIDU,
	VWSPIDERMATCH_IDX_YAHOO,
	VWSPIDERMATCH_IDX_YODAO,
	VWSPIDERMATCH_IDX_SOSO,
	VWSPIDERMATCH_IDX_SOGOU,
	VWSPIDERMATCH_IDX_MSN,
	VWSPIDERMATCH_IDX_ALEXA,
	VWSPIDERMATCH_IDX_SINAWEIBO,
};


typedef struct tagVwSpiderMatchSpiderInfo
{
	tagVwSpiderMatchSpiderInfo( DWORD dwSpIndex_in, LPCTSTR lpcszName, LPCTSTR lpcszAgent )
	{
		memset( this, 0, sizeof(tagVwSpiderMatchSpiderInfo) );

		dwSpIndex = dwSpIndex_in;
		_sntprintf( szName, sizeof(szName)-sizeof(TCHAR), _T("%s"), lpcszName );
		_sntprintf( szAgent, sizeof(szAgent)-sizeof(TCHAR), _T("%s"), lpcszAgent );
	}

	DWORD dwSpIndex;
	TCHAR szName[ 64 ];
	TCHAR szAgent[ MAX_PATH ];

}STVWSPIDERMATCHSPIDERINFO, *LPSTVWSPIDERMATCHSPIDERINFO;

/**
 *	������������֩���������ű�
 *	*** ע��˳����밴������ֵ�Ķ���˳����Ϊ�������ǰ���Լ����˳�����
 */
static STVWSPIDERMATCHSPIDERINFO g_ArrVwSpiderMatchSpiderInfo[] =
{
	//	0 - GOOGLE
	//	"Googlebot" + "Googlebot-Image/1.0" + "Mediapartners-Google"
	STVWSPIDERMATCHSPIDERINFO( VWSPIDERMATCH_IDX_GOOGLE, "Google Spider", "Googlebot|Feedfetcher-Google|Mediapartners-Google|" ),

	//	1 - �ٶ�
	STVWSPIDERMATCHSPIDERINFO( VWSPIDERMATCH_IDX_BAIDU, "Baidu Spider", "Baiduspider|Baidu-Transcoder|" ),

	//	2 - YAHOO
	//	"Yahoo! Slurp China;" +  "Yahoo! Slurp;"
	STVWSPIDERMATCHSPIDERINFO( VWSPIDERMATCH_IDX_YAHOO, "Yahoo Spider", "Yahoo! Slurp|YahooSeeker|" ),

	//	3 - �е�
	//	"YodaoBot/1.0;"
	STVWSPIDERMATCHSPIDERINFO( VWSPIDERMATCH_IDX_YODAO, "Yodao Spider", "YodaoBot|" ),

	//	4 - SOSO
	STVWSPIDERMATCHSPIDERINFO( VWSPIDERMATCH_IDX_SOSO, "Soso Spider", "Sosospider|Sosoimagespider|" ),

	//	5 - Sogou
	//	"Sogou Web Sprider|Sogou web spider/4.0|Sogou Orion spider/4.0|Sogou-Test-Spider/4.0|Sogou+head+spider/3.0"
	STVWSPIDERMATCHSPIDERINFO( VWSPIDERMATCH_IDX_SOGOU, "Sogou Spider", "Sogou Web Sprider|Sogou web spider|Sogou Orion spider|Sogou-Test-Spider|Sogou+head+spider|sohu-search|" ),

	//	6 - Msn Search
	//	"msnbot-media"
	STVWSPIDERMATCHSPIDERINFO( VWSPIDERMATCH_IDX_MSN, "Msn-Search Spider", "msnbot|" ),

	//	7 - Alexa
	STVWSPIDERMATCHSPIDERINFO( VWSPIDERMATCH_IDX_ALEXA, "Alexa-Archiver Spider", "ia_archiver|" ),

	//	8 - SinaWeiboBot
	STVWSPIDERMATCHSPIDERINFO( VWSPIDERMATCH_IDX_SINAWEIBO, "SinaWeibo Bot", "SinaWeiboBot|" )
};

/**
 *	֩��ĸ���
 */
#define VWSPIDERMATCH_COUNT		( sizeof(g_ArrVwSpiderMatchSpiderInfo) / sizeof(g_ArrVwSpiderMatchSpiderInfo[0]) )
#define VWSPIDERMATCH_MAX_TREEDEEP	64	//	��������


/**
 *	@ public
 *	��ȡ��ʽ����֩������
 */
static BOOL VwSpiderMatch_IsValidSpiderIndex( DWORD dwSpIndex )
{
	return ( dwSpIndex >= 0 && dwSpIndex < VWSPIDERMATCH_COUNT );
}
static LPTSTR VwSpiderMatch_GetSpiderNameByIndex( DWORD dwSpIndex )
{
	if ( ! VwSpiderMatch_IsValidSpiderIndex( dwSpIndex ) )
	{
		return NULL;
	}

	return g_ArrVwSpiderMatchSpiderInfo[ dwSpIndex ].szName;
}
static DWORD VwSpiderMatch_GetSpiderIndexByName( LPCTSTR lpszSpiderName )
{
	if ( NULL == lpszSpiderName )
	{
		return 0;
	}

	DWORD dwRet	= 0;
	INT i;

	for ( i = 0; i < VWSPIDERMATCH_COUNT; i ++ )
	{
		if ( 0 == _tcsicmp( lpszSpiderName, g_ArrVwSpiderMatchSpiderInfo[ i ].szName ) )
		{
			dwRet = g_ArrVwSpiderMatchSpiderInfo[ i ].dwSpIndex;
			break;
		}
	}

	return dwRet;
}


/**
 *	���ݽṹ��
 */
typedef struct tagVwSpiderMatchTreeNode
{
	//	���ڵ�
	tagVwSpiderMatchTreeNode()
	{
		memset( this, 0, sizeof(tagVwSpiderMatchTreeNode) );
	}
	
	BYTE btNode[ 0x100 ];
	
}STVWSPIDERMATCHTREENODE, *LPSTVWSPIDERMATCHTREENODE;

typedef struct tagVwSpiderMatchTree
{
	//	������������
	tagVwSpiderMatchTree()
	{
		memset( this, 0, sizeof(tagVwSpiderMatchTree) );

		InitTree();
	}

	VOID InitTree()
	{
		INT   i;
		DWORD dwWordLenTemp		= 0;
		TCHAR * pszSpider		= NULL;
		TCHAR * pszHead;
		TCHAR * pszMov;

		//
		//	��ʼ������ֵ
		//
		this->bInitSucc		= FALSE;
		this->dwWordCount	= 0;
		this->dwMinWordLen	= 100000000;	//	���ƥ��ʵĳ���
		this->dwMaxWordLen	= 0;		//	�ƥ��ʵĳ���
		memset( this->stTree, 0, sizeof(this->stTree) );
		
		//
		//	��������֩������дʣ�����������Щֵ
		//
		for ( i = 0; i < VWSPIDERMATCH_COUNT; i ++ )
		{
			pszSpider = g_ArrVwSpiderMatchSpiderInfo[ i ].szAgent;
			if ( pszSpider && _tcslen(pszSpider) )
			{
				//	"Googlebot|Feedfetcher-Google|Mediapartners-Google|"
				pszHead	= pszSpider;
				while( pszHead )
				{
					pszMov = _tcsstr( pszHead, "|" );
					if ( pszMov )
					{
						dwWordLenTemp = pszMov - pszHead;
						this->dwMaxWordLen = max( this->dwMaxWordLen, dwWordLenTemp );
						this->dwMinWordLen = min( this->dwMinWordLen, dwWordLenTemp );

						pszHead = CharNext( pszMov );
					}
					else
					{
						break;
					}
				}
			}
		}

		if ( this->dwMinWordLen && this->dwMaxWordLen && this->dwMinWordLen <= this->dwMaxWordLen )
		{
			this->dwMaxWordLen = min( this->dwMaxWordLen, VWSPIDERMATCH_MAX_TREEDEEP );
			this->bInitSucc = TRUE;
		}
		else
		{
			this->dwMinWordLen = 0;
		}
	}

	//
	//	��Ա����
	//
	BOOL  bInitSucc;		//	�Ƿ��ʼ���ɹ�
	DWORD dwWordCount;		//	������ٸ��ؼ���
	DWORD dwMinWordLen;		//	���ƥ��ʵĳ���
	DWORD dwMaxWordLen;		//	�ƥ��ʵĳ���
	STVWSPIDERMATCHTREENODE stTree[ VWSPIDERMATCH_MAX_TREEDEEP ];

}STVWSPIDERMATCHTREE, *LPSTVWSPIDERMATCHTREE;




/**
 *	class of CVwSpiderMatch
 */
class CVwSpiderMatch
{
public:
	CVwSpiderMatch();
	virtual ~CVwSpiderMatch();

	BOOL InsertSpider( STVWSPIDERMATCHTREE * pstTree, DWORD dwSpiderIndex );
	BOOL IsValidTreeAndParam( STVWSPIDERMATCHTREE * pstTree, LPCTSTR lpcszText, UINT uTextLen );
	UINT PrefixMatch( STVWSPIDERMATCHTREE * pstTree, LPCTSTR lpcszText, UINT uTextLen );

};


#endif	// __VWSPIDERMATCH_HEADER__
