#ifndef __VWCONSTBASE_HEADER__
#define __VWCONSTBASE_HEADER__



//////////////////////////////////////////////////////////////////////////
#define alerti( msg ) MessageBox( msg, "Information", MB_ICONINFORMATION )
#define alertw( msg ) MessageBox( msg, "Warning", MB_ICONWARNING )
#define alerte( msg ) MessageBox( msg, "Error", MB_ICONERROR )


//////////////////////////////////////////////////////////////////////////
//	for user message
enum
{
	UM_DATACHANGE	= ( WM_USER + 0x1000 ),
	UM_DISABLEAPPLY,
	UM_OPENHTMLHELP,
	UM_OPENCFGFILE,			//	for VirtualHtml Only
	UM_CTRL_BUTTON,			//	for ctrl buttons
	UM_DO_NEXT			//	do next, a common message
};

enum
{
	WPARAM_HEMLHELP_OVERVIEW = 0x1000,			//	������ר�Ұ����ĵ���ҳ
	WPARAM_HEMLHELP_ABOUTVSID,				//	���� VSID ��ϸ��
	WPARAM_HEMLHELP_AUTOEXPIRED,				//	���ڡ����ڡ�VSID�Զ����ϡ���ʹ��˵����
	WPARAM_HEMLHELP_ABOUTPROTECTION,			//	���ڱ������õ�ϸ��
	WPARAM_HEMLHELP_ABOUTSUPERMODE,				//	���ڳ�ǿ����ģʽ
	WPARAM_HEMLHELP_ABOUTVWBROWSERIN,			//	���ڿͻ��˲��
	WPARAM_HEMLHELP_ABOUTFRIENDLYHOST_NOTICE1,		//	��������վ�������ע��
	WPARAM_HEMLHELP_ABOUTFRIENDLYHOST_NETSHOP_ANTILEECHS,	//	��������վ������ã����������ͼƬ������
	WPARAM_HEMLHELP_ABOUTVHTML_SOLUTIONS			//	ʹ��SDK�ļ����÷���
};

enum
{
	WPARAM_HEMLHELP_VWPANDM_OVERVIEW = 0x1100,		//	�򿪰����ĵ���ҳ
	WPARAM_HEMLHELP_VWPANDM_GET_SUBDOMAIN			//	��λ�ȡ������
};

enum
{
	WPARAM_HEMLHELP_VWCMANTILEECH_OVERVIEW = 0x1200,	//	�򿪰����ĵ���ҳ
	WPARAM_HEMLHELP_VWCMANTILEECH_RTSP_ABOUT_ALERTFILE,	//	���ھ����滻�ļ���ʹ�÷���
	WPARAM_HEMLHELP_VWCMANTILEECH_RTSP_ABOUT_ANTILEECH,	//	���� Rtsp ��ý��Э�������
	WPARAM_HEMLHELP_VWCMANTILEECH_RTSP_HOW_TO_LINK,		//	�����ȷ�ص��ñ�������Դ��
	WPARAM_HEMLHELP_VWCMANTILEECH_RTSP_ABOUTVSID,		//	���� VSID ��ϸ��
};

//	for VwFirewall
enum
{
	WPARAM_HEMLHELP_VWFIREWALL_OVERVIEW = 0x1300,
	WPARAM_HEMLHELP_VWFIREWALL_CTRLSTATUS_FIREWALL,		//	�������ر� ����ǽ
	WPARAM_HEMLHELP_VWFIREWALL_CTRLSTATUS_FILE,		//	�������ر� �ļ���
	WPARAM_HEMLHELP_VWFIREWALL_CTRLSTATUS_FILE_SET,		//	���� �ļ���
	WPARAM_HEMLHELP_VWFIREWALL_CTRLSTATUS_DOMAIN,		//	�������ر� ����������
	WPARAM_HEMLHELP_VWFIREWALL_CTRLSTATUS_DOMAIN_SET,	//	���� ����������
	WPARAM_HEMLHELP_VWFIREWALL_CTRLSTATUS_RDP,		//	�������ر� Զ�����氲ȫ
	WPARAM_HEMLHELP_VWFIREWALL_CTRLSTATUS_RDP_SET,		//	���� Զ�����氲ȫ

	WPARAM_HEMLHELP_VWFIREWALL_DONEXT_RECOVER_REG,		//	�ָ� ע�����һ��
	WPARAM_HEMLHELP_VWFIREWALL_DATA_DATACHANGE,		//	�޸���Ϣ
};


#define CONST_DLLFILE_DELIB		"DeLib.dll"
#define CONST_DLLFILE_DELIBNPP		"DeLibNPp.dll"
#define CONST_DLLFILE_DELIBSKS		"DeLibSks.dll"
#define CONST_DLLFILE_DELIBPS		"DeLibPs.dll"



#endif	//	__VWCONSTBASE_HEADER__