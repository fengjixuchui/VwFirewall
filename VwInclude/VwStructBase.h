#ifndef __VWSTRUCTBASE_HEADER__
#define __VWSTRUCTBASE_HEADER__



/**
 *	Core Info ����ģ����Ϣ
 */
typedef struct tagVwCoreInfo
{
	//	R	- ��ʾ�Ǻ���ģ������Ľ��̶��������Ĳ���Ȩ��
	//	W	- ��ʶ��д
	tagVwCoreInfo()
	{
		memset( this, 0, sizeof(tagVwCoreInfo) );
	}

	BOOL  bEnterpriseVer;	//	[R]   �Ƿ�����ʽ�汾
	BOOL  bCfgChanged;	//	[R/W] �����ļ��Ƿ�ı�
	BOOL  bContinueWork;	//	[R]   ����ģ���Ƿ����ڹ���
	DWORD dwCoreTick;	//	[R]   ����ģ��� TickCount
	DWORD dwLoadCfgTick;	//	[R/W] װ��������Ϣ�� TickCount

}STVWCOREINFO, *PSTVWCOREINFO;





#endif	// __VWSTRUCTBASE_HEADER__