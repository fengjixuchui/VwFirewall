// MultiString.cpp: implementation of the CMultiString class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "MultiString.h"
#include <assert.h>
#include <stdlib.h>


//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CMultiString::CMultiString()
{
	m_nBufLen=0;
	m_pBuf=NULL;
}

CMultiString::~CMultiString()
{

}

/*
	�������������У�StringList��һ��char**��ָ��(ppList)
	���������������ѭ�����д���
	while(*p)	//	���һ���ַ���ָ��ΪNULL
	{
		*p����0��β���ַ�������������Խ��д���
		...
		p++;
	}
*/

/*
	��pBufָ��ĳ���ΪnBufLen��MultiString��ʽ�Ļ�����ת��ΪStringList
	���ת���ɹ����򷵻�StringList��ָ�룬String�ĸ���ͨ��pnNum����
	���ת��ʧ�ܣ��򷵻�NULL
	StringList�Լ��Ŀռ��ɺ����ĵ������ͷ�
	��ָ���ÿһ���ַ����Ŀռ�Ϊ�����pBuf����˲����ͷ�StringList
	��ÿһ���ַ�����ֻ��Ҫ�ͷ�pBuf���ɣ�����б�Ҫ�Ļ���
	��ˣ�����������Ҳ��Ӧ�����޸�ÿһ��String��ֵ��Ӧ���俴Ϊֻ����
*/
char** CMultiString::MultiStringToStringList(char *pBuf, int nBufLen, int *pnNum)
{
	if ( nBufLen < 2 )	//	�յ�MultiString(ֻ��һ��\0)
		return NULL;

	TCHAR ** ppRet		= NULL;
	INT   nCount		= 0;
	TCHAR * p = pBuf + nBufLen - 1;
	INT   nTailZero		= 0;

	do
	{
		if ( '\0' == *p )
			nTailZero ++;
		p--;
	}
	while( ('\0'==*p) && (p>pBuf) );

	assert( nTailZero > 1 );
	if((nTailZero<2) || ('\0' == *p))
		return NULL;

	p++;
	nCount=0;
	do
	{
		if(*p=='\0')
			nCount++;
		p--;
	}
	while ( p > pBuf );

	ppRet = (TCHAR**)malloc( (nCount+sizeof(TCHAR))*sizeof(TCHAR*) );
	if ( NULL == ppRet )
		return NULL;

	p = pBuf;
	for ( int i = 0; i < nCount; i++ )
	{
		ppRet[i]	= p;
		while( *p++ );
	}
	ppRet[nCount] = NULL;
	*pnNum = nCount;

	return ppRet;
}
WCHAR ** CMultiString::MultiStringToStringListW( WCHAR * pBuf, INT nBufLen, INT * pnNum )
{
	if ( nBufLen < 2 )	//	�յ�MultiString(ֻ��һ��\0)
		return NULL;
	
	WCHAR ** ppRet		= NULL;
	INT   nCount		= 0;
	WCHAR * p = pBuf + nBufLen - 1;
	INT   nTailZero		= 0;

	do
	{
		if ( '\0' == *p )
			nTailZero ++;
		p--;
	}
	while( ('\0'==*p) && (p>pBuf) );
	
	assert( nTailZero > 1 );
	if((nTailZero<2) || ('\0' == *p))
		return NULL;
	
	p++;
	nCount=0;
	do
	{
		if(*p=='\0')
			nCount++;
		p--;
	}
	while ( p > pBuf );
	
	ppRet = (WCHAR**)malloc( (nCount+sizeof(WCHAR))*sizeof(WCHAR*) );
	if ( NULL == ppRet )
		return NULL;
	
	p = pBuf;
	for ( int i = 0; i < nCount; i++ )
	{
		ppRet[i]	= p;
		while( *p++ );
	}
	ppRet[nCount] = NULL;
	*pnNum = nCount;

	return ppRet;
}

/*
	��pBufָ��ĳ���ΪnBufLen��MultiString��ʽ�Ļ�����ת��ΪStringList
	���ת��*pnNum-1����StringList�ɵ������ṩ������(ppStrList)
	���ת���ɹ����򷵻�TRUE��ͬʱͨ��pnNum����string�ĸ���
	���ת��ʧ�ܣ��򷵻�FALSE
*/
BOOL CMultiString::MultiStringToStringList(char* pBuf, int nBufLen, char** ppStrList, IN OUT int* pnNum)
{
	int nCount;
	char* p=pBuf+nBufLen-1;
	int	nTailZero;
	nTailZero=0;
	do
	{
		if(*p=='\0')
			nTailZero++;
		p--;
	}while((*p=='\0') && (p>pBuf));

	assert(nTailZero > 1);
	if((nTailZero<2) || ('\0' == *p))
		return FALSE;

	p++;
	nCount=0;
	do
	{
		if(*p=='\0')
			nCount++;
		p--;
	}while(p>pBuf);

	p=pBuf;
	for(int i=0;(i<nCount)&&(i<*pnNum-1);i++)
	{
		ppStrList[i]=p;
		while(*p++)	;
	}
	ppStrList[i]=NULL;
	*pnNum=i;

	return TRUE;
}

/*
	��ppStrListָ���StringListת��ΪMultiString��ʽ�Ļ�����
	���ת��nStrNum������StringList���������е��ַ���
	���ת���ɹ�������ָ��MultiString��ָ�룬����ͨ��pnLen����
	���ת��ʧ�ܣ��򷵻�NULL
	MultiString�Ŀռ��ɺ����������ͷ�
*/
char* CMultiString::StringListToMultiString(char **ppStrList, int nStrNum, int *pnLen)
{
	int i;
	int nLen;
	char** ppList;
	char* pRet;
	char* pCopy;
	i=0;
	nLen=0;
	ppList=ppStrList;
	while((*ppList) && (i<nStrNum))
	{
		nLen+=strlen(*ppList);
		nLen++;	//	for the last '\0'
		ppList++;
		i++;
	}
	nLen++;	//	for the last two '\0'

	pRet=(char*)malloc(nLen*sizeof(char));
	if(NULL == pRet)
		return NULL;

	pCopy=pRet;
	i=0;
	ppList=ppStrList;
	while((*ppList) && (i<nStrNum))
	{
		strcpy(pCopy,*ppList);
		pCopy+=strlen(*ppList);
		pCopy++;

		ppList++;
		i++;
	}
	*pCopy='\0';
	*pnLen=nLen;
	return pRet;
}
