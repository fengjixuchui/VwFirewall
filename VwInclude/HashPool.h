//	HashPool.h
//	LiuQiXing @ 2006-08-19
//
//////////////////////////////////////////////////////////////////////


#ifndef __HASHPOOL_HEADER__
#define __HASHPOOL_HEADER__


//#include <stdio.h>
//#include <stdlib.h>
#include "crc32.h"


/**
 *	const define
 */
const UINT  CONST_MAX_HASHPOOL_DEPTH	= 299993;		//	��������ޣ�����ѡ������
								//	999983 * 16 = 15.2 M
								//	299993 * 16 = 4.5 M

const UINT  CONST_FNV_32_PRIME		= 0x01000193UL;		//	��ϣ������ֵ
const UINT  CONST_IPADDR_NULL		= 0;

static const CHAR * CONST_FILEMAP_VW_HASHPOOL_HEADER	= "FILEMAP_VW_HASHPOOL_HEADER";
static const CHAR * CONST_FILEMAP_VW_HASHPOOL		= "FILEMAP_VW_HASHPOOL";


//	error define
//const DWORD CONST_ERROR_PARAM_HASHPOOL_DEPTH	= 0x1000;


/**
 *	HashPoolList Header �ṹ��
 */
typedef struct tagHashPoolListHeader
{
	tagHashPoolListHeader()
	{
		memset( this, 0, sizeof(tagHashPoolListHeader) );
	}

	HANDLE hFileMapHeader;	//	ͷ�����ڴ���ļ����
	HANDLE hFileMap;	//	���ݹ����ڴ���ļ����
	UINT uRefCount;		//	ʹ�ý������ü���
	CRITICAL_SECTION oCriticalSectionModify;

}STHASHPOOLLISTHEADER, *PSTHASHPOOLLISTHEADER;


/**
 *	Pool List data struct
 */
typedef struct tagHashPoolList
{
	tagHashPoolList()
	{
		dwQueryTick	= 0;
		dwIpAddr	= CONST_IPADDR_NULL;
		usThreadCount	= 0;
		usVisitCount	= 0;
		dwWakeUpTime	= 0;
	}

	DWORD   dwQueryTick;		//	ÿ�Ρ�����ĳ�� IP �ϵ��̼߳�����������ĳ�� IP �ϵ��̼߳�������Ҫ��¼�� TickCount�������ݴ������Ƴ�ʱ
	DWORD	dwIpAddr;		//	Ip Address
	USHORT	usThreadCount;		//	���ؼ���
	USHORT	usVisitCount;		//	1 �����ڵķ��ʴ���
	DWORD	dwWakeUpTime;		//	������ܾ������ﱣ����ĳ���ͻ���������ʱ���
	USHORT  usLmtDayIpLastDate;	//	������ IP ���٣�����¼ʱ�䡣��ʽ�����գ����磺1015 / 0101
	USHORT  usLmtDayIpTimes;	//	������ IP ���٣���ֵ 0 ~ 65535

}STHASHPOOLLIST, *PSTHASHPOOLLIST;





/**
 *	Hash Pool Class
 */
class CHashPool
{
public:
	//	Construction function
	CHashPool( unsigned int lnPoolDepth = CONST_MAX_HASHPOOL_DEPTH*1 );

	//	..
	virtual ~CHashPool();

public:

	BOOL MallocShareMemory( DWORD dwSize, LPCTSTR lpcszMemName, LPVOID lpvBuffer, LPBOOL lpbCreate );

	//	Get IpList pool depth
	UINT  GetPoolDepth();

	//	�Ƿ��ʼ���ɹ�
	BOOL  IsInitSucc();


	//
	//	�������ݻ�ȡ Hash Index
	//
	LONG  GetHashPoolIndex( const DWORD dwIpAddr );

	//
	//	�����������ṩ�ַ��� IP ��ַ�� DWORD ��ת��
	//
	DWORD GetIpAddrFromString( LPCSTR lpcszIpAddr );


	////////////////////////////////////////////////////////////
	//	�����߳�����
	//

	//	��ȡ����
	LONG  GetCount( LONG lnIndex );

	//	�������ݲ����Ӽ���
	BOOL  IncCount( LONG lnIndex, const DWORD dwIpAddr );

	//	���ټ���
	BOOL  DecCount( LONG lnIndex, const DWORD dwIpAddr );


	////////////////////////////////////////////////////////////
	//	�ձ����ļ����ʴ�������
	//

	//	��ȡ����
	LONG  LmtDayIpGetCount( LONG lnIndex );

	//	�������ݲ����Ӽ���
	BOOL  LmtDayIpIncCount( LONG lnIndex, const DWORD dwIpAddr );


	////////////////////////////////////////////////////////////
	//	������
	//

	//	��ȡ����
	LONG  AntiAttackGetCount( LONG lnIndex );

	//	���Ӽ���
	BOOL  AntiAttackIncCount( LONG lnIndex, const DWORD dwIpAddr );

	//	��ȡ�����ƿͻ���������ʱ���
	LONG  AntiAttackGetWakeUpTime( LONG lnIndex );

	//	���ñ����ƿͻ���������ʱ���
	VOID  AntiAttackSetWakeUpTime( LONG lnIndex, LONG lnWakeUpTime );

	//	��������̨�����߳�
	static VOID _threadAntiAttackBgWorker( PVOID pvArg );
	VOID AntiAttackBgWorkerProc();


private:

	//	��ʼ���ڵ�
	VOID InitPoolNode( STHASHPOOLLIST * pstPoolNode );

	//
	//	��� Index �Ƿ���һ��������
	//
	BOOL  IsInIndexRange( UINT uIndex );


	//	32 version for zero terminated string
	UINT  GetStringHashValue32( const char * s, UINT lnInit = CONST_FNV_32_PRIME );

	//	32 version for buffer
	UINT  GetStringHashValue32ByBuffer( const char * buf, UINT len, UINT lnInit = CONST_FNV_32_PRIME );



	//	the depth of iplist pool, you'd better set it with a "prime number".
	UINT  m_uPoolDepth;

	//	��ʶ�Ƿ��ʼ���ɹ�
	BOOL  m_bInitSucc;

	//	��ʱ
	DWORD m_dwThreadRecordTimeout;

	//	IpList data
	STHASHPOOLLISTHEADER * m_pstPoolListHeader;
	STHASHPOOLLIST * m_pstPoolList;

	//	..
	//CRITICAL_SECTION m_CriticalSectionModify;
	//HANDLE  m_hFileMap;

};



#endif	// __HASHPOOL_HEADER__





















