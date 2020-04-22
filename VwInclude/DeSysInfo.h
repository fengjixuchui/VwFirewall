// DeSysInfo.h: interface for the CDeSysInfo class.
//
//////////////////////////////////////////////////////////////////////

#ifndef __DESYSINFO_HEADER__
#define __DESYSINFO_HEADER__

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000


#include "ntdll.h"






/**
 *	struct of system performance
 */
typedef struct _DE_SYSTEM_PERFORMANCE_INFORMATION
{
	LARGE_INTEGER liIdleTime;			//	CPU����ʱ�䣻
	LARGE_INTEGER liReadTransferCount;		//	I/O��������Ŀ��
	LARGE_INTEGER liWriteTransferCount;		//	I/Oд������Ŀ��
	LARGE_INTEGER liOtherTransferCount;		//	I/O����������Ŀ��
	ULONG ulReadOperationCount;			//	I/O��������Ŀ��
	ULONG ulWriteOperationCount;			//	I/Oд������Ŀ��
	ULONG ulOtherOperationCount;			//	I/O��������������Ŀ��
	ULONG ulAvailablePages;				//	�ɻ�õ�ҳ��Ŀ��
	ULONG ulTotalCommittedPages;			//	�ܹ��ύҳ��Ŀ��
	ULONG ulTotalCommitLimit;			//	���ύҳ��Ŀ��
	ULONG ulPeakCommitment;				//	ҳ�ύ��ֵ��
	ULONG ulPageFaults;				//	ҳ������Ŀ��
	ULONG ulWriteCopyFaults;			//	Copy-On-Write������Ŀ��
	ULONG ulTransitionFaults;			//	��ҳ������Ŀ��
	ULONG ulReserved1;
	ULONG ulDemandZeroFaults;			//	����0��������
	ULONG ulPagesRead;				//	��ҳ��Ŀ��
	ULONG ulPageReadIos;				//	��ҳI/O��������
	ULONG ulReserved2[2];
	ULONG ulPagefilePagesWritten;			//	��дҳ�ļ�ҳ����
	ULONG ulPagefilePageWriteIos;			//	��дҳ�ļ���������
	ULONG ulMappedFilePagesWritten;			//	��дӳ���ļ�ҳ����
	ULONG ulMappedFileWriteIos;			//	��дӳ���ļ���������
	ULONG ulPagedPoolUsage;				//	��ҳ��ʹ�ã�
	ULONG ulNonPagedPoolUsage;			//	�Ƿ�ҳ��ʹ�ã�
	ULONG ulPagedPoolAllocs;			//	��ҳ�ط��������
	ULONG ulPagedPoolFrees;				//	��ҳ���ͷ������
	ULONG ulNonPagedPoolAllocs;			//	�Ƿ�ҳ�ط��������
	ULONG ulNonPagedPoolFress;			//	�Ƿ�ҳ���ͷ������
	ULONG ulTotalFreeSystemPtes;			//	ϵͳҳ�����ͷ�������
	ULONG ulSystemCodePage;				//	����ϵͳ����ҳ����
	ULONG ulTotalSystemDriverPages;			//	�ɷ�ҳ��������ҳ����
	ULONG ulTotalSystemCodePages;			//	����ϵͳ����ҳ������
	ULONG ulSmallNonPagedLookasideListAllocateHits;	//	С�Ƿ�ҳ�����б���������
	ULONG ulSmallPagedLookasideListAllocateHits;	//	С��ҳ�����б���������
	ULONG ulReserved3;
	ULONG ulMmSystemCachePage;			//	ϵͳ����ҳ����
	ULONG ulPagedPoolPage;				//	��ҳ��ҳ����
	ULONG ulSystemDriverPage;			//	�ɷ�ҳ����ҳ����
	ULONG ulFastReadNoWait;				//	�첽���ٶ���Ŀ��
	ULONG ulFastReadWait;				//	ͬ�����ٶ���Ŀ��
	ULONG ulFastReadResourceMiss;			//	���ٶ���Դ��ͻ����
	ULONG ulFastReadNotPossible;			//	���ٶ�ʧ������
	ULONG ulFastMdlReadNoWait;			//	�첽MDL���ٶ���Ŀ��
	ULONG ulFastMdlReadWait;			//	ͬ��MDL���ٶ���Ŀ��
	ULONG ulFastMdlReadResourceMiss;		//	MDL����Դ��ͻ����
	ULONG ulFastMdlReadNotPossible;			//	MDL��ʧ������
	ULONG ulMapDataNoWait;				//	�첽ӳ�����ݴ�����
	ULONG ulMapDataWait;				//	ͬ��ӳ�����ݴ�����
	ULONG ulMapDataNoWaitMiss;			//	�첽ӳ�����ݳ�ͻ������
	ULONG ulMapDataWaitMiss;			//	ͬ��ӳ�����ݳ�ͻ������
	ULONG ulPinMappedDataCount;			//	ǣ��ӳ��������Ŀ��
	ULONG ulPinReadNoWait;				//	ǣ���첽����Ŀ��
	ULONG ulPinReadWait;				//	ǣ��ͬ������Ŀ��
	ULONG ulPinReadNoWaitMiss;			//	ǣ���첽����ͻ��Ŀ��
	ULONG ulPinReadWaitMiss;			//	ǣ��ͬ������ͻ��Ŀ��
	ULONG ulCopyReadNoWait;				//	�첽������������
	ULONG ulCopyReadWait;				//	ͬ��������������
	ULONG ulCopyReadNoWaitMiss;			//	�첽���������ϴ�����
	ULONG ulCopyReadWaitMiss;			//	ͬ�����������ϴ�����
	ULONG ulMdlReadNoWait;				//	�첽MDL��������
	ULONG ulMdlReadWait;				//	ͬ��MDL��������
	ULONG ulMdlReadNoWaitMiss;			//	�첽MDL�����ϴ�����

	ULONG ulMdlReadWaitMiss;			//	ͬ��MDL�����ϴ�����
	ULONG ulReadAheadIos;				//	��ǰ��������Ŀ��
	ULONG ulLazyWriteIos;				//	LAZYд������Ŀ��
	ULONG ulLazyWritePages;				//	LAZYдҳ�ļ���Ŀ��
	ULONG ulDataFlushes;				//	����ˢ�´�����
	ULONG ulDataPages;				//	����ˢ��ҳ����
	ULONG ulContextSwitches;			//	# �����л���Ŀ��
	ULONG ulFirstLevelTbFills;			//	��һ�㻺������������
	ULONG ulSecondLevelTbFills;			//	�ڶ��㻺������������
	ULONG ulSystemCall;				//	ϵͳ���ô�����

}DE_SYSTEM_PERFORMANCE_INFORMATION, *PDE_SYSTEM_PERFORMANCE_INFORMATION;

typedef struct tagDeSysInfo
{
	tagDeSysInfo()
	{
		memset( this, 0, sizeof(tagDeSysInfo) );
	}

	//	memory info
	DWORDLONG ullMemPhyTotal;		//	�ܹ������ڴ�(��λ:MB)
	DWORDLONG ullMemPhyAvail;		//	���������ڴ�(��λ:MB)
	DWORDLONG ullMemVirTotal;		//	�ܹ�ҳ���ļ�(��λ:MB)
	DWORDLONG ullMemVirAvail;		//	����ҳ���ļ�(��λ:MB)
	
	//	kernel memory info
	DWORDLONG ullMemKernelPaged;		//	paged
	DWORDLONG ullMemKernelNP;		//	nonpaged
	DWORDLONG ullMemKernelTotal;		//	total

	//	I/O
	LARGE_INTEGER liReadTransferCount;
	LARGE_INTEGER liWriteTransferCount;
	LARGE_INTEGER liOtherTransferCount;

}STDESYSINFO, *PSTDESYSINFO;

/**
 *	class of CDeSysInfo
 */
class CDeSysInfo
{
public:
	CDeSysInfo();
	virtual ~CDeSysInfo();

	BOOL Query( STDESYSINFO * pstSysInfo );

private:
	BOOL IsWow64();

private:
	PROCNTQSI m_pfnNtQuerySystemInformation;
};

#endif // __DESYSINFO_HEADER__
