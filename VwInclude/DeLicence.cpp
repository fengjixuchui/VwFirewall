// DeLicence.cpp: implementation of the CDeLicence class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "DeLicence.h"

#include "shlwapi.h"
#pragma comment( lib, "shlwapi.lib" )

#include <nb30.h>
#pragma comment( lib, "netapi32.lib" )

#include "DeLib.h"
#pragma comment( lib, "DeLib.lib" )

#include "md5.h"


/**
 *	�ṹ�嶨��
 */
typedef struct tagAstat
{
	ADAPTER_STATUS adapt;
	NAME_BUFFER NameBuff[ 30 ];
}STASTAT, * PSTASTAT;

//	"c:\\"
static TCHAR g_szDeLicence_CDisk[] =
{
	-100, -59, -93, -93, 0
};

//	"%s|%s|%s"
static TCHAR g_szDeLicence_fmt1[] =
{
	-38, -116, -125, -38, -116, -125, -38, -116, 0
};

//	"%s-%s"
static TCHAR g_szDeLicence_FmtRegCode[] =
{
	-38, -116, -46, -38, -116, 0
};

//	"%s|%s"
static TCHAR g_szDeLicence_FmtActCode[] =
{
	-38, -116, -125, -38, -116, 0
};

//	"%02x-%02x-%02x-%02x-%02x-%02x"
static TCHAR g_szDeLicence_FmtMacAddress[] =
{
	-38, -49, -51, -121, -46, -38, -49, -51, -121, -46, -38, -49, -51, -121, -46, -38, -49, -51, -121, -46, -38,
	-49, -51, -121, -46, -38, -49, -51, -121, 0
};

//	"%s-%d-%08x%08x%08x%08x"
static TCHAR g_szDeLicence_FmtCpuId[] =
{
	-38, -116, -46, -38, -101, -46, -38, -49, -57, -121, -38, -49, -57, -121, -38, -49, -57, -121, -38, -49, -57,
	-121, 0
};


//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CDeLicence::CDeLicence()
{
	memset( & mb_stLcInfo, 0, sizeof(mb_stLcInfo) );

	//	Ĭ����ʹ��Ӳ�����к� ��Ϊ���� gene �Ĳ���
	m_bCreateGeneWithDiskSN	= TRUE;

	//
	//	���� fmt
	//
	memset( m_szDecCDisk, 0, sizeof(m_szDecCDisk) );
	memcpy( m_szDecCDisk, g_szDeLicence_CDisk, min( sizeof(g_szDeLicence_CDisk), sizeof(m_szDecCDisk) ) );
	delib_xorenc( m_szDecCDisk );

	memset( m_szDecFmt1, 0, sizeof(m_szDecFmt1) );
	memcpy( m_szDecFmt1, g_szDeLicence_fmt1, min( sizeof(g_szDeLicence_fmt1), sizeof(m_szDecFmt1) ) );
	delib_xorenc( m_szDecFmt1 );

	memset( m_szDecFmtRegCode, 0, sizeof(m_szDecFmtRegCode) );
	memcpy( m_szDecFmtRegCode, g_szDeLicence_FmtRegCode, min( sizeof(g_szDeLicence_FmtRegCode), sizeof(m_szDecFmtRegCode) ) );
	delib_xorenc( m_szDecFmtRegCode );

	memset( m_szDecFmtActCode, 0, sizeof(m_szDecFmtActCode) );
	memcpy( m_szDecFmtActCode, g_szDeLicence_FmtActCode, min( sizeof(g_szDeLicence_FmtActCode), sizeof(m_szDecFmtActCode) ) );
	delib_xorenc( m_szDecFmtActCode );

	memset( m_szDecFmtMacAddress, 0, sizeof(m_szDecFmtMacAddress) );
	memcpy( m_szDecFmtMacAddress, g_szDeLicence_FmtMacAddress, min( sizeof(g_szDeLicence_FmtMacAddress), sizeof(m_szDecFmtMacAddress) ) );
	delib_xorenc( m_szDecFmtMacAddress );

	memset( m_szDecFmtCpuId, 0, sizeof(m_szDecFmtCpuId) );
	memcpy( m_szDecFmtCpuId, g_szDeLicence_FmtCpuId, min( sizeof(g_szDeLicence_FmtCpuId), sizeof(m_szDecFmtCpuId) ) );
	delib_xorenc( m_szDecFmtCpuId );


//	memset( mb_stLcInfo.szKey, 0, sizeof(mb_stLcInfo.szKey) );
//	memset( mb_stLcInfo.szGene, 0, sizeof(mb_stLcInfo.szGene) );

//	memset( mb_szRegCode, 0, sizeof(mb_szRegCode) );
//	memset( mb_stLcInfo.szActCode, 0, sizeof(mb_stLcInfo.szActCode) );
}
CDeLicence::~CDeLicence()
{
}




//////////////////////////////////////////////////////////////////////////
//	protected


/**
 *	@ protected
 *	��ʼ������
 */
BOOL CDeLicence::InitData( IN HINSTANCE hCallerInstance, IN LPCTSTR lpcszKey, IN BOOL bCreateGeneWithDiskSN )
{
	memset( mb_stLcInfo.szKey, 0, sizeof(mb_stLcInfo.szKey) );
	if ( lpcszKey )
	{
		_sntprintf( mb_stLcInfo.szKey, sizeof(mb_stLcInfo.szKey)-sizeof(TCHAR), _T("%s"), lpcszKey );
	}

	//	�Ƿ�ʹ��Ӳ�����к� ��Ϊ���� GENE �Ĳ���
	m_bCreateGeneWithDiskSN	= bCreateGeneWithDiskSN;

	//	get gene
	GetGene( mb_stLcInfo.szGene, sizeof(mb_stLcInfo.szGene) );
	_tcsupr( mb_stLcInfo.szGene );

	return TRUE;
}

/**
 *	@ protected
 *	�Ƿ�����ȷ�����
 *	��֤ ActCode �Ƿ���ȷ
 */
BOOL CDeLicence::IsValidLicence( LPCTSTR lpszActCode )
{
	//
	//	lpszActCode	- [in] ���֤��
	//	RETURN		- TRUE / FALSE
	//

	BOOL  bRet				= FALSE;
	TCHAR szGene[ MAX_PATH ]		= {0};
	TCHAR szActCodeInput[ MAX_PATH ]	= {0};
	TCHAR szActCodeCorrect[ MAX_PATH ]	= {0};

	_sntprintf
	(
		szActCodeInput,
		sizeof(szActCodeInput)-sizeof(TCHAR),
		_T("%s"),
		lpszActCode ? lpszActCode : mb_stLcInfo.szActCode
	);

	if ( _tcslen(mb_stLcInfo.szGene) )
	{
		_sntprintf( szGene, sizeof(szGene)-sizeof(TCHAR), _T("%s"), mb_stLcInfo.szGene );

		if ( GetActCode( szGene, szActCodeCorrect, sizeof(szActCodeCorrect) ) )
		{
			_tcslwr( szActCodeCorrect );
			_tcslwr( szActCodeInput );
			bRet = IsEqualString( szActCodeCorrect, szActCodeInput );
		}
	}

	return bRet;
}



//////////////////////////////////////////////////////////////////////////
//	Public


/**
 *	@ public
 *	����һ��������������
 */
BOOL CDeLicence::GetGene( OUT LPTSTR lpszGene, IN DWORD dwSize )
{
	//
	//	lpszGene	- [out] ���ؼ������ӣ�32 λ�ַ� MD5
	//	dwSize		- [in]  ���ػ������Ĵ�С��һ��Ϊһ������ 32 �Ļ�����
	//	RETURN
	//	�㷨��
	//		md5( m_szKey + NetworkCard Mac + DiskSerial )
	//

	TCHAR szHDiskSN[ MAX_PATH ]	= {0};
	TCHAR szCPUID[ MAX_PATH ]	= {0};
	TCHAR szSrc[ MAX_PATH ]		= {0};

	//	��ȡ cpuid
	GetCpuId( szCPUID, sizeof(szCPUID) );

	//	��ȡӲ�����к�
	if ( m_bCreateGeneWithDiskSN )
	{
		GetHDiskSerialNumber( szHDiskSN, sizeof(szHDiskSN) );
	}
	else
	{
		//	����Ȩ�޵����⣬����ʹ��Ӳ�����к���Ϊ�������������к�
		memset( szHDiskSN, 0, sizeof(szHDiskSN) );
	}

	//	����Դ��
	//	key + cpuid + HDiskSN
	_sntprintf( szSrc, sizeof(szSrc)-sizeof(TCHAR), m_szDecFmt1, mb_stLcInfo.szKey, szCPUID, szHDiskSN );
	_tcslwr( szSrc );

	//	..
	return GetStringMd5( szSrc, lpszGene, dwSize, 0 );
}

/**
 *	@ public
 *	��ȡ regcode
 */
BOOL CDeLicence::GetRegCode( IN LPCTSTR lpcszGene, OUT LPTSTR lpszRegCode, IN DWORD dwSize )
{
	//
	//	lpcszGene	- [in]  �������ӣ�һ��Ϊ NULL
	//	lpszRegCode	- [out] �������֤��
	//	dwSize		- [in]  ���ػ������Ĵ�С
	//	RETURN
	//	�㷨��		md5( m_szKey + "-" + Gene )
	//

	BOOL  bRet			= FALSE;
	BOOL  bGetGene			= FALSE;
	TCHAR szGene[ MAX_PATH ]	= {0};
	TCHAR szSrc[ MAX_PATH ]		= {0};
	TCHAR szMd5[ 64 ]		= {0};
	
	if ( lpcszGene )
	{
		bGetGene = TRUE;
		_sntprintf( szGene, sizeof(szGene)-sizeof(TCHAR), _T("%s"), lpcszGene );
	}
	else if ( _tcslen(mb_stLcInfo.szGene) )
	{
		bGetGene = TRUE;
		_sntprintf( szGene, sizeof(szGene)-sizeof(TCHAR), _T("%s"), mb_stLcInfo.szGene );
	}
	
	if ( bGetGene )
	{
		//	regcode �㷨
		//	key - gene = "%s-%s"
		_sntprintf( szSrc, sizeof(szSrc)-sizeof(TCHAR), m_szDecFmtRegCode, mb_stLcInfo.szKey, szGene );
		StrTrim( szSrc, _T("\r\n\t ") );
		_tcslwr( szSrc );

		if ( GetStringMd5( szSrc, szMd5, sizeof(szMd5), 0 ) )
		{
			bRet = GetFormatLicenceString( szMd5, lpszRegCode, dwSize );
		}
	}
	
	return bRet;
}


/**
 *	@ public
 *	��ȡ actcode
 */
BOOL CDeLicence::GetActCode( IN LPCTSTR lpcszGene, OUT LPTSTR lpszActCode, IN DWORD dwSize )
{
	//
	//	lpcszGene	- [in]  �������ӣ�һ��Ϊ NULL
	//	lpszActCode	- [out] �������֤��
	//	dwSize		- [in]  ���ػ������Ĵ�С
	//	RETURN
	//	�㷨��		md5( m_szKey + "|" + Gene )
	//

	BOOL  bRet			= FALSE;
	BOOL  bGetGene			= FALSE;
	TCHAR szGene[ MAX_PATH ]	= {0};
	TCHAR szSrc[ MAX_PATH ]		= {0};
	TCHAR szMd5[ 64 ]		= {0};

	if ( lpcszGene )
	{
		bGetGene = TRUE;
		_sntprintf( szGene, sizeof(szGene)-sizeof(TCHAR), _T("%s"), lpcszGene );
	}
	else if ( _tcslen(mb_stLcInfo.szGene) )
	{
		bGetGene = TRUE;
		_sntprintf( szGene, sizeof(szGene)-sizeof(TCHAR), _T("%s"), mb_stLcInfo.szGene );
	}

	if ( bGetGene )
	{
		//	actcode �㷨
		//	key | gene = "%s|%s"
		_sntprintf( szSrc, sizeof(szSrc)-sizeof(TCHAR), m_szDecFmtActCode, mb_stLcInfo.szKey, szGene );
		StrTrim( szSrc, _T("\r\n\t ") );
		_tcslwr( szSrc );

		if ( GetStringMd5( szSrc, szMd5, sizeof(szMd5), 0 ) )
		{
			bRet = GetFormatLicenceString( szMd5, lpszActCode, dwSize );
		}
	}

	return bRet;
}

/**
 *	��ȡ MAC ��ַ
 */
BOOL CDeLicence::GetMacAddress( LPCTSTR lpcszNetBiosName, LPTSTR lpszMacAddress, DWORD dwSize )
{
	//
	//	lpcszNetBiosName	- [in]
	//	lpszMacAddress		- [out] 
	//	dwSize			- [in]
	//	RETURN			- TRUE / FALSE
	//

	if ( NULL == lpcszNetBiosName || 0 == strlen( lpcszNetBiosName ) )
	{
		return FALSE;
	}
	if ( NULL == lpszMacAddress || 0 == dwSize )
	{
		return FALSE;
	}
	
	BOOL bRet		= FALSE;
	TCHAR szBiosName[ 64 ]	= {0};
	STASTAT Adapter;
	NCB ncb;
	UCHAR uRetCode;

	//	copy bios name
	_sntprintf( szBiosName, sizeof(szBiosName)-sizeof(TCHAR), "%s", lpcszNetBiosName );
	
	memset( & ncb, 0, sizeof(ncb) );
	ncb.ncb_command		= NCBRESET;
	ncb.ncb_lana_num	= 0;

	uRetCode = Netbios( &ncb );
	if ( 0 == uRetCode )
	{
		memset( & ncb, 0, sizeof(ncb) );
		ncb.ncb_command		= NCBASTAT;
		ncb.ncb_lana_num	= 0;
		
		//	��д
		_tcsupr( szBiosName );
		
		FillMemory( ncb.ncb_callname, NCBNAMSZ - 1, 0x20 );
		_sntprintf( (LPTSTR)ncb.ncb_callname, sizeof(ncb.ncb_callname)-sizeof(TCHAR), "%s", szBiosName );
		
		ncb.ncb_callname[ strlen( szBiosName ) ] = 0x20;
		ncb.ncb_callname[ NCBNAMSZ ] = 0x0;
		
		ncb.ncb_buffer = (unsigned char *) &Adapter;
		ncb.ncb_length = sizeof(Adapter);
		
		uRetCode = Netbios( &ncb );
		if ( 0 == uRetCode )
		{
			bRet = TRUE;
			_sntprintf
			(
				lpszMacAddress,
				dwSize-sizeof(TCHAR),
				m_szDecFmtMacAddress,		//	"%02x-%02x-%02x-%02x-%02x-%02x"
				Adapter.adapt.adapter_address[0],
				Adapter.adapt.adapter_address[1],
				Adapter.adapt.adapter_address[2],
				Adapter.adapt.adapter_address[3],
				Adapter.adapt.adapter_address[4],
				Adapter.adapt.adapter_address[5]
			);
		}
	}
	
	return bRet;
}

/**
 *	��ȡӲ�����к�
 */
BOOL CDeLicence::GetHDiskSerialNumber( LPTSTR lpszSerialNumber, DWORD dwSize )
{
	BOOL  bRet	= FALSE;
	TCHAR szNameBuf[ 12 ];
	DWORD dwSerialNumber;
	DWORD dwMaxLen; 
	DWORD dwFileFlag;
	TCHAR szSysNameBuf[ 10 ];

	bRet = ::GetVolumeInformation
	(
		m_szDecCDisk,	//	"c:\\"
		szNameBuf,
		sizeof(szNameBuf), 
		& dwSerialNumber, 
		& dwMaxLen,
		& dwFileFlag,
		szSysNameBuf,
		sizeof(szSysNameBuf)
	);
	if ( bRet )
	{
		_sntprintf( lpszSerialNumber, dwSize-sizeof(TCHAR), _T("%x"), dwSerialNumber );
	}
	
	return bRet;
}

/**
 *	��ȡ cpuid
 */
BOOL CDeLicence::GetCpuId( LPTSTR lpszCPUID, DWORD dwSize )
{
	//
	//	lpszCPUID	- [out] cpuid ����ֵ
	//	dwSize		- [in]  ����ֵ��������С
	//	RETURN		- TRUE / FALSE
	//
	//	��ע��
	//		_asm cpuid ָ����Ա�һ�� CPU ʶ��
	//			- Intel 486 ���ϵ� CPU
	//			- Cyrix M1 ���ϵ� CPU
	//			- AMD Am486 ���ϵ� CPU
	//

	if ( NULL == lpszCPUID || 0 == dwSize )
	{
		return FALSE;
	}

	BOOL  bException	= FALSE;
	TCHAR szOEMInfo[ 13 ]	= {0};
	INT nFamily;
	INT nEAXValue, nEBXValue, nECXValue, nEDXValue;
	INT nALValue;
	ULONG ulIdPart1	= 0;
	ULONG ulIdPart2	= 0;
	ULONG ulIdPart3	= 0;
	ULONG ulIdPart4	= 0;

	__try
	{
		//	get OEM info
		//
		//	(1)�ж�CPU���̣����� EAX = 0���ٵ��� cpuid
		//
		_asm
		{
			mov	eax,	0
			cpuid
			mov	DWORD PTR szOEMInfo[0],	ebx
			mov	DWORD PTR szOEMInfo[4],	edx
			mov	DWORD PTR szOEMInfo[8],	ecx
			mov	BYTE PTR szOEMInfo[12],	0
		}

		//
		//	get family number
		//
		//	(2)CPU �����Ǽ� 86���Ƿ�֧�� MMX
		//	���� EAX = 1���ٵ��� cpuid
		//	EAX �� 8 �� 11 λ�ͱ����Ǽ� 86
		//	3 - 386
		//	4 - i486
		//	5 - Pentium
		//	6 - Pentium Pro Pentium II
		//	2 - Dual Processors
		//	EDX �ĵ� 0 λ: ���� FPU
		//	EDX �ĵ� 23 λ: CPU �Ƿ�֧�� IA MMX������Ҫ��������������� 57 ��������ָ��� 
		//	�����һλ�ɣ�����͵��ſ�Windows�ġ��ó���ִ���˷Ƿ�ָ������رա��� ��
		//
		//	(3)ר�ż���Ƿ� P6 �ܹ�
		//	���� EAX = 1���ٵ��� CPUID
		//	��� AL = 1������ Pentium Pro �� Pentium II
		//
		//	(4)ר�ż�� AMD �� CPU ��Ϣ
		//	���� EAX = 80000001H���ٵ��� CPUID
		//	��� EAX = 51H���� AMD K5
		//	��� EAX = 66H���� K6
		//	K7 ��ʲô��־��ֻ�еȴ���õ� K7 ��о��˵�ˡ�
		//	EDX �� 0 λ: �Ƿ��� FPU(����ģ�˭�ù�û FPU �� K5,K6?)
		//	EDX �� 23 λ��CPU �Ƿ�֧�� MMX
		//
		_asm
		{
			mov	eax,	1
			cpuid
			mov	nEAXValue,	eax
			mov	nEBXValue,	ebx
			mov	nECXValue,	ecx
			mov	nEDXValue,	edx
		}
		nFamily	= ( 0xf00 & nEAXValue ) >> 8;

		_asm
		{
			mov	eax,	2
			cpuid
			mov	nALValue,	eax
		}
		
		//
		//	get cpu id
		//
		//	CPU �����к���һ�� 96bit �Ĵ���ʾ����ʽ�������� 6 �� WORD ֵ��XXXX-XXXX-XXXX-XXX-XXXX-XXXX��
		//	WORD��16��bit�������ݣ�������unsigned shortģ�⣺
		//	typedef unsigned short WORD;
		//	������к���Ҫ�������裬������ eax = 1 �����������ص� eax �д洢���кŵĸ����� WORD��
		//	�� eax = 3 ������������ ecx �� edx ���ӵ�λ����λ��˳��洢ǰ 4 �� WORD ���ɵõ� cpuid
		//
		_asm
		{
			mov	eax,	01h
			xor	edx,	edx
			cpuid
			mov	ulIdPart1,	edx
			mov	ulIdPart2,	eax
		}	
		_asm
		{
			mov	eax,	03h
			xor	ecx,	ecx
			xor	edx,	edx
			cpuid
			mov	ulIdPart3,	edx
			mov	ulIdPart4,	ecx
		}
	}
	__except( EXCEPTION_EXECUTE_HANDLER )
	{
		bException = TRUE;
	}

	_sntprintf
	(
		lpszCPUID, dwSize-sizeof(TCHAR),
		m_szDecFmtCpuId,	//	"%s-%d-%08x%08x%08x%08x"
		szOEMInfo, nFamily,
		ulIdPart1, ulIdPart2, ulIdPart3, ulIdPart4
	);
	_tcslwr( lpszCPUID );

	return bException ? FALSE : TRUE;
}


/**
 *	��ȡһ���ַ����� MD5 ֵ(32λ�ַ���)
 */
BOOL CDeLicence::GetStringMd5( LPCTSTR lpszString, LPTSTR lpszMd5, DWORD dwSize, DWORD dwSpecStringLen /* = 0 */ )
{
	//
	//	lpszString	- [in]  �������ַ���
	//	lpszMd5		- [out] ���� MD5 ֵ�ַ���������
	//	dwSize		- [in]  ���� MD5 ֵ�ַ�������
	//	dwSpecStringLen	- [in]  ָ������ lpszString ��ͷ��ʼ�Ķ��ٸ��ֽ�
	//	RETURN		- TRUE / FALSE
	//

	if ( NULL == lpszString || NULL == lpszMd5 )
		return FALSE;
	if ( dwSize < 32 )
		return FALSE;
	
	MD5_CTX	m_md5;
	unsigned char szEncrypt[16];
	unsigned char c;
	INT i			= 0;

	memset( szEncrypt, 0, sizeof(szEncrypt) );
	if ( dwSpecStringLen > 0 )
	{
		m_md5.MD5Update( (unsigned char *)lpszString, dwSpecStringLen );
	}
	else
	{
		m_md5.MD5Update( (unsigned char *)lpszString, _tcslen(lpszString) );
	}
	m_md5.MD5Final( szEncrypt );
	
	//	...
	for ( i = 0; i < 16; i++ )
	{
		c = szEncrypt[i] / 16;
		lpszMd5[i*2]	= ( c < 10 ) ? ( '0' + c ) : ( 'a' + c - 10 );
		c = szEncrypt[i] % 16;
		lpszMd5[i*2+1]	= ( c < 10 ) ? ( '0' + c ) : ( 'a' + c - 10 );
	}

	//	..
	lpszMd5[ min( 32, dwSize-sizeof(TCHAR) ) ] = 0;
	
	return TRUE;
}

/**
 *	��ȡָ�����Ƶ� Cookie Value
 */
BOOL CDeLicence::GetCookieValue( LPCTSTR lpszCookieString, LPCTSTR lpszCookieName, LPTSTR lpszValue, DWORD dwSize )
{
	//
	//	lpszCookieString	- [in]  ���� COOKIE ���ַ���
	//	lpszCookieName		- [in]  COOKIE ����
	//	lpszValue		- [out] ����ֵ
	//	dwSize			- [in]  ����ֵ��������С
	//	RETURN			- TRUE / FALSE
	//
	
	if ( NULL == lpszCookieString || NULL == lpszCookieName || NULL == lpszValue )
	{
		return FALSE;
	}
	if ( NULL == lpszCookieString[0] || NULL == lpszCookieName[0] )
	{
		return FALSE;
	}
	
	BOOL	bRet		= FALSE;
	LPTSTR	lpszHead	= NULL;
	LPTSTR	lpszMove	= NULL;
	LONG	uValueLen	= 0;
	
	lpszMove = _tcsstr( lpszCookieString, lpszCookieName );
	if ( lpszMove )
	{
		lpszHead = lpszMove + _tcslen(lpszCookieName);
		if ( lpszHead )
		{
			//
			//	�������ݵ�����ֵ������
			//
			memcpy( lpszValue, lpszHead, dwSize-sizeof(TCHAR) );
			
			//
			//	����β��
			//
			lpszMove = strpbrk( lpszValue, _T(" ;\r\n") );
			if ( lpszMove )
			{
				*lpszMove = 0;
			}
			
			bRet = TRUE;
		}
	}
	return bRet;
}


/**
 *	�ַ����Ƚ�
 */
BOOL CDeLicence::IsEqualString( LPCTSTR lpcszStr1, LPCTSTR lpcszStr2 )
{
	BOOL bRet	= FALSE;
	INT  i, nLen1, nLen2;

	if ( lpcszStr1 && lpcszStr2 )
	{
		nLen1	= _tcslen(lpcszStr1);
		nLen2	= _tcslen(lpcszStr2);
		if ( nLen1 == nLen2 )
		{
			bRet = TRUE;
			for ( i = 0; i < nLen1; i ++ )
			{
				if ( lpcszStr1[ i ] != lpcszStr2[ i ] )
				{
					bRet = FALSE;
					break;
				}
			}
		}
	}

	return bRet;
}











//////////////////////////////////////////////////////////////////////////
//	Private




/**
 *	��ȡ��ʽ�����ע������Ϣ
 */
BOOL CDeLicence::GetFormatLicenceString( LPCTSTR lpcszInput, LPTSTR lpszLicenceString, DWORD dwSize )
{
	if ( NULL == lpcszInput || 32 != _tcslen(lpcszInput) )
	{
		return FALSE;
	}
	if ( NULL == lpszLicenceString || dwSize <= 35 )
	{
		return FALSE;
	}

	//	...
	_sntprintf
	(
		lpszLicenceString, dwSize-sizeof(TCHAR),
		_T("%.8s-%.8s-%.8s-%.8s"),
		lpcszInput+0,    lpcszInput+8,
		lpcszInput+16,   lpcszInput+24
	);
	return TRUE;
}


