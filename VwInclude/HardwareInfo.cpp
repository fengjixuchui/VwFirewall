// HardwareInfo.cpp: implementation of the CHardwareInfo class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "HardwareInfo.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CHardwareInfo::CHardwareInfo()
{

}

CHardwareInfo::~CHardwareInfo()
{

}

/**
 *	��ȡ cpuid
 */
BOOL CHardwareInfo::GetCpuId( LPTSTR lpszCPUID, DWORD dwSize )
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

#ifdef _M_IX86
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
#endif

	_sntprintf
	(
		lpszCPUID, dwSize-sizeof(TCHAR),
		"%s-%d-%08x%08x%08x%08x",
		szOEMInfo, nFamily,
		ulIdPart1, ulIdPart2, ulIdPart3, ulIdPart4
	);
	_tcslwr( lpszCPUID );

	return bException ? FALSE : TRUE;
}



/**
*	��ȡӲ�����к�
*/
BOOL CHardwareInfo::GetHDiskSerialNumber( LPTSTR lpszSerialNumber, DWORD dwSize )
{
	BOOL  bRet	= FALSE;
	TCHAR szNameBuf[ 12 ];
	DWORD dwSerialNumber;
	DWORD dwMaxLen; 
	DWORD dwFileFlag;
	TCHAR szSysNameBuf[ 10 ];
	
	bRet = ::GetVolumeInformation
		(
			"c:\\",
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