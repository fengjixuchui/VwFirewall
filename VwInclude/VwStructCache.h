#ifndef __VWSTRUCTCACHE_HEADER__
#define __VWSTRUCTCACHE_HEADER__



/**
 *	struct
 *	CfgIdx cache �ṹ��
 */
typedef struct tagCacheCfgIdx
{
	tagCacheCfgIdx()
	{
		uServerPort		= 0;
		dwLocalAddrValue	= 0;
		szHost[ 0 ]		= 0;
		pLeft			= NULL;
		pRight			= NULL;
	}
	int operator - ( const tagCacheCfgIdx & r )
	{
		if ( uServerPort == r.uServerPort )
		{
			if ( dwLocalAddrValue == r.dwLocalAddrValue )
			{
				return _tcsicmp( szHost, r.szHost );
			}
			else
			{
				return ( dwLocalAddrValue - r.dwLocalAddrValue );
			}
		}
		else
		{
			return ( uServerPort - r.uServerPort );
		}
	}

	LONG  lnCfgIndex;		//	STHOSTINFO �е�һ����Ա���������� SET CACHE
	ULONG uServerPort;
	TCHAR szHost[ 128 ];
	DWORD dwLocalAddrValue;

	tagCacheCfgIdx * pLeft;		//	��ڵ�
	tagCacheCfgIdx * pRight;	//	�ֽڵ�

}STCACHECFGIDX, *PSTCACHECFGIDX;



/**
 *	config data
 */
typedef struct tagCfgData
{
	tagCfgData()
	{
		memset( this, 0, sizeof(tagCfgData) );
		lpszLmtPlayer	= NULL;
		lpszReplaceFile	= NULL;
	}
	LONG	lnConfig;		//	�Ƿ�����1-Deny all HTTP requests, 2-anti-leechs, 4-limit playonly, 8-limit thread, 16-limit speed
	LONG	lnLmtThread;		//	�����߳�
	LONG	lnLmtSpeed;		//	�����ٶ�
	LONG	lnLmtDayIp;		//	���Ʊ�������Դ�շ��ʴ���	

	LONG	lnAction;		//	��������1-Send warning messages��ʾ���;�����Ϣ, 2-��ʾʹ���ļ��滻	
	LPSTR	lpszLmtPlayer;		//	����չ������ʹ���ĸ����������š�MP ��ʾ Window Media Player��RP ��ʾ Real Player��
	LPSTR	lpszReplaceFile;	//	����չ���ľ����滻�ļ�·��

}STCFGDATA, *PSTCFGDATA;


/**
 *	�����жϺ��� IsProtectResource �ȵ� Set Cache �洢
 */
typedef struct tagCacheRes
{
	tagCacheRes()
	{
		lnFlag		= (-1);
		pstCfgData	= NULL;
		lnCfgIndex	= (-1);
		dwFullUriCrc32	= 0;

		pLeft		= NULL;
		pRight		= NULL;
	}
	int operator - ( const tagCacheRes & r )
	{
		if ( lnCfgIndex == r.lnCfgIndex )
		{
			return ( dwFullUriCrc32 - r.dwFullUriCrc32 );
		}
		else
		{
			return ( lnCfgIndex - r.lnCfgIndex );
		}
	}

	LONG  lnFlag;			//	�� Set ����: ��ǲ���
	LONG  lnReturn;			//	�� Set ����: ��ǲ���
	STCFGDATA * pstCfgData;		//	�� Set ����: ���ò���ָ��

	LONG  lnCfgIndex;		//	STHOSTINFO �е�һ����Ա���������� set cache ����
	DWORD dwFullUriCrc32;		//	���� set cache ����
	//DWORD dwRefererHostCrc32;	//	�û�������ж� 2009-05-26 �ϳ� RefererHost ������ֱ���� Referer �ж�
	DWORD dwRefererCrc32;		//	�û�������ж�

	tagCacheRes * pLeft;		//	��ڵ�
	tagCacheRes * pRight;		//	�ֽڵ�

}STCACHERES, *PSTCACHERES;

//	less compare function for ProtectedRes
class __stcmp_less_tagCachePrtRes
{
public:	
	bool operator()( const STCACHERES & l, const STCACHERES & r ) const
	{
		if ( l.lnCfgIndex == r.lnCfgIndex )
		{
			return ( l.dwFullUriCrc32 < r.dwFullUriCrc32 ) ? true : false;
		}
		else
		{
			return ( l.lnCfgIndex < r.lnCfgIndex ) ? true : false;
		}
	}
};

//	less compare function for ExceptionRes
class __stcmp_less_tagCacheEcpRes
{
public:
	bool operator()( const STCACHERES & l, const STCACHERES & r ) const
	{
		if ( l.lnCfgIndex == r.lnCfgIndex )
		{
			if ( l.dwFullUriCrc32 == r.dwFullUriCrc32 )
			{
				return ( l.dwRefererCrc32 < r.dwRefererCrc32 ) ? true : false;
			}
			else
			{
				return ( l.dwFullUriCrc32 < r.dwFullUriCrc32 ) ? true : false;
			}
		}
		else
		{
			return ( l.lnCfgIndex < r.lnCfgIndex ) ? true : false;
		}
	}
};




#endif	// __VWSTRUCTCACHE_HEADER__