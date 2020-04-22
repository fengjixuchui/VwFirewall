// ProcCfg.h: interface for the CProcCfg class.
//
//////////////////////////////////////////////////////////////////////

#ifndef __PROCCFG_HEADER__
#define __PROCCFG_HEADER__


#include "VwConstAntileechs.h"
#include "procdb.h"
#include "DeAdsiOpIIS.h"


//////////////////////////////////////////////////////////////////////////
//	const
#define CPROCCFG_IFH_TYPE_WEBSITE	0	//	��ͨ����
#define CPROCCFG_IFH_TYPE_SPIDER	1	//	���������֩��
#define CPROCCFG_IFH_TYPE_URL		2	//	URL
#define CPROCCFG_IFH_TYPE_CLIENTIP	3	//	ClientIP

#define CPROCCFG_IBH_TYPE_URL		0	//	URL
#define CPROCCFG_IBIP_TYPE_IP		0	//	IP




//////////////////////////////////////////////////////////////////////////

//	[main_table]
typedef struct tagMainTable
{
	tagMainTable()
	{
		memset( this, 0, sizeof(tagMainTable) );
	}
	LONG	m_id;			//	�����ֶ�
	TCHAR	m_key[64];		//	ע����
	TCHAR	m_vw_ver[32];		//	����汾
	TCHAR	m_vw_system[32];
	TCHAR	m_reg_ipaddr[16];
	TCHAR	m_reg_host[64];
	INT	m_is_reg_user;			//	�Ƿ�����ʽ�汾
	TCHAR	m_reg_prtype[ 32 ];		//	�������
	TCHAR	m_reg_prtypecs[ 64 ];		//	�������У����

	TCHAR	m_reg_check_date[ 64 ];		//	�����ʱ��
	INT	m_reg_expire_type;		//	��������: 0 ���ޣ�1������
	TCHAR	m_reg_create_date[ 64 ];	//	ע������
	TCHAR	m_reg_expire_date[ 64 ];	//	����ʱ��
	INT	m_max_allow_dirs;		//	�������Ŀ¼����
	INT	m_max_friend_hosts;		//	�������վ������

	LONG	m_use_proxy;			//	������������ǽ��������Ҫ�� HTTP_X_FORWARDED_FOR �ڻ����û�ָ���Ľ��л�ȡԶ���û��� IP ��ַ
	TCHAR	m_proxy_variable[ 64 ];		//	�� HTTP ͷ�У��û�ָ���Ļ�ȡԶ�� IP ��ַ�Ľڵı�������

	LONG	m_login_algorithm;		//	login algorithm
	TCHAR	m_login_password[ 64 ];		//	login password

}STMAINTABLE, *PSTMAINTABLE;

//	[iis_table]
typedef struct tagIisTable
{
	tagIisTable()
	{
		memset( this, 0, sizeof(tagIisTable) );
	}

	LONG	iis_id;
	TCHAR	iis_siteid[32];
	LONG	iis_use;				//	�Ƿ���ӵ��������У�ֻ�б���ӵ�����������������ò���Ч��iis_use Ͻ���� iis_start
	LONG	iis_start;				//	�Ƿ���: 0Ϊ�رգ�1Ϊ����
	LONG	iis_start_time_switch;			//	��ʱ����
	LONG	iis_enable_protects;			//	�Ƿ�����:����������
	LONG	iis_enable_exceptions;			//	�Ƿ�����:���������
	LONG	iis_enable_limits;			//	�Ƿ�����:���Ƶ�����
	LONG	iis_enable_security;			//	�Ƿ�����:��ȫ������
	LONG	iis_enable_logs;			//	�Ƿ�����:��־������
	LONG	iis_enable_vh;				//	�Ƿ��� VirtualHtml �Ż�
	LONG	iis_type;				//	�������ͣ�0Ϊ��ͨ������1Ϊ��ǿ����
	TCHAR	iis_pubvsid_key[MAX_PATH];		//	VSID ��Ϣ
	LONG	iis_pubvsid_keytime;			//	VSID ����Чʱ��
	LONG	iis_pubvsid_usebrowserin;		//	Ҫ���û����밲װ�ͻ��˲��
	LONG	iis_pubvsid_usecookie;			//	�Ƿ�ʹ�� COOKIE ������ VSID ��Ϣ
	LONG	iis_pubvsid_type;			//	���㷽����0��ͨ��1����仯
	LONG	iis_pubvsid_auto_expired;		//	VSID ʹ��һ�κ��Զ�����: Ŀǰ�汾���� iis_pubvsid_type=1 ʱ��Ч
	TCHAR	iis_pubvsid_cookiepage[MAX_PATH];	//	��ֲ COOKIE ��ҳ��
	LONG	iis_pubvsid_cookiepage_len;		//	��ֲ COOKIE ��ҳ�泤�ȣ���ȡ��ʱ��̬���㡣
	TCHAR	iis_apppoolid[ 128 ];			//	AppPoolId

}STIISTABLE, *PSTIISTABLE;

//	[iis_blockext_table]
typedef struct tagIisBlockExtTable
{
	tagIisBlockExtTable()
	{
		memset( this, 0, sizeof(tagIisBlockExtTable) );
	}
	LONG	ibe_id;
	TCHAR	iis_siteid[32];			//	���� iis_table ���ֶ�
	LONG	ibe_use;			//	�Ƿ�ʹ�ã�0Ϊ��ʹ�ã�1Ϊʹ��
	TCHAR	ibe_name[ 32 ];			//	��չ��
	LONG	ibe_config;			//	�Ƿ�����1-Deny all HTTP requests, 2-anti-leechs, 4-limit playonly, 8-limit thread, 16-limit speed
	LONG	ibe_lmt_thread;			//	�����߳�
	LONG	ibe_lmt_speed;			//	�����ٶ�
	LONG	ibe_action;			//	��������1-Send warning messages��ʾ���;�����Ϣ, 2-��ʾʹ���ļ��滻
	TCHAR	ibe_lmt_player[8];		//	����չ������ʹ���ĸ����������š�MP ��ʾ Window Media Player��RP ��ʾ Real Player��
	TCHAR	ibe_replacefile[ MAX_PATH ];	//	����չ���ľ����滻�ļ�·��

} STIISBLOCKEXTTABLE, *PSTIISBLOCKEXTTABLE;

//	[iis_blockdirs_table]
typedef struct tagIisBlockDirsTable
{
	tagIisBlockDirsTable()
	{
		memset( this, 0, sizeof(tagIisBlockDirsTable) );
	}
	LONG	ibd_id;
	TCHAR	iis_siteid[32];			//	���� iis_table ���ֶ�
	LONG	ibd_use;			//	�Ƿ�ʹ�ã�0Ϊ��ʹ�ã�1Ϊʹ��
	TCHAR	ibd_name[ 64 ];			//	Ŀ¼����
	LONG	ibd_config;			//	�Ƿ�����1-limit thread, 2-limit speed
	LONG	ibd_lmt_thread;			//	�����߳�
	LONG	ibd_lmt_speed;			//	�����ٶ�
	UINT	uBdNameLen;			//	Ŀ¼���Ƴ���
	BOOL	bHasWildcard;			//	�Ƿ���ͨ���

}STIISBLOCKDIRSTABLE, *PSTIISBLOCKDIRSTABLE;

//	[iis_allowdirs_table]
typedef struct tagIisAllowDirsTable
{
	tagIisAllowDirsTable()
	{
		memset( this, 0, sizeof(tagIisAllowDirsTable) );
	}
	LONG	iad_id;
	TCHAR	iis_siteid[32];			//	���� iis_table ���ֶ�
	LONG	iad_use;			//	�Ƿ�ʹ�ã�0Ϊ��ʹ�ã�1Ϊʹ��
	LONG	iad_except_config;		//	����� ACTION ����
	TCHAR	iad_name[ 64 ];			//	Ŀ¼����
	UINT	uAdNameLen;			//	Ŀ¼���Ƴ���
	BOOL	bHasWildcard;			//	�Ƿ���ͨ���

}STIISALLOWDIRSTABLE, *PSTIISALLOWDIRSTABLE;

//	[iis_friendhosts_table]
typedef struct tagIisFriendHostsTable
{
	tagIisFriendHostsTable()
	{
		memset( this, 0, sizeof(tagIisFriendHostsTable) );
	}
	LONG	ifh_id;
	TCHAR	iis_siteid[32];			//	���� iis_table ���ֶ�
	LONG	ifh_use;			//	�Ƿ�ʹ�ã�0Ϊ��ʹ�ã�1Ϊʹ��
	LONG	ifh_type;			//	���ͣ�0��ʾ��ͨվ�㣬1��ʾ���������֩��
	LONG	ifh_except_config;		//	����� ACTION ����
	TCHAR	ifh_name[ 260 ];		//	URL�������������������֩�������
	DWORD	ifh_name_dwval;			//	ClientIP ��Ҫ�� name ת���� DWORD ֵ�洢

}STIISFRIENDHOSTSTABLE, *PSTIISFRIENDHOSTSTABLE;

//	[iis_blockhosts_table]
typedef struct tagIisBlockHostsTable
{
	tagIisBlockHostsTable()
	{
		memset( this, 0, sizeof(tagIisBlockHostsTable) );
	}
	LONG	ibh_id;
	TCHAR	iis_siteid[32];			//	���� iis_table ���ֶ�
	LONG	ibh_use;			//	�Ƿ�ʹ�ã�0Ϊ��ʹ�ã�1Ϊʹ��
	LONG	ibh_type;			//	���ͣ�0��ʾ��ͨվ�㣬1��ʾ���������֩��
	TCHAR	ibh_name[ 260 ];		//	URL
	
}STIISBLOCKHOSTSTABLE, *PSTIISBLOCKHOSTSTABLE;

//	[iis_blockip_table]
typedef struct tagIisBlockIPTable
{
	tagIisBlockIPTable()
	{
		memset( this, 0, sizeof(tagIisBlockIPTable) );
	}
	LONG	ibip_id;
	TCHAR	iis_siteid[ 32 ];		//	���� iis_table ���ֶ�
	LONG	ibip_use;			//	�Ƿ�ʹ�ã�0Ϊ��ʹ�ã�1Ϊʹ��
	LONG	ibip_type;			//	���ͣ�0��ʾ����IP��ַ��1��ʾͨ���IP��ַ
	TCHAR	ibip_name[ 18 ];		//	IP ��ַ
	DWORD	ibip_ip_value;			//	IP ��ַ�� 32 λ���� VALUE

}STIISBLOCKIPTABLE, *PSTIISBLOCKIPTABLE;


//	[iis_security_table]
typedef struct tagIisSecurityTable
{
	tagIisSecurityTable()
	{
		memset( this, 0, sizeof(tagIisSecurityTable) );
	}
	LONG	isc_id;
	TCHAR	iis_siteid[32];				//	���� iis_table ���ֶ�
	LONG	isc_enable_iis_disguise;		//	�Ƿ��� IIS αװ����
	LONG	isc_iis_disguise_cfg;			//	IIS αװ������
	LONG	isc_enable_antatt;			//	�Ƿ����� CC ��������
	LONG	isc_antatt_maxvisit;			//	�� CC ������һ���Ӷ��ٸ�����;ܾ�����
	LONG	isc_antatt_sleep;			//	�� CC ��������������ͣЪ��ã���λ����
	LONG	isc_antatt_disabled_proxy;		//	�� CC ������proxy ��ֹ����
							//	�� CC ������������
							//	�� CC ������������
	LONG	isc_enable_url_filter;			//	�Ƿ��� URL ����
	LONG	isc_enable_content_filter;		//	�Ƿ�ʼ���ݹ���
	LONG	isc_enable_lmt_urllen;			//	�Ƿ����� URL �ĳ���
	LONG	isc_lmt_urllen_cfg;			//	���� URL ���ȵ���ֵ����λ���ַ����֣�
	LONG	isc_enable_lmt_urlchars;		//	�Ƿ����� URL �в������������ַ�����
	TCHAR	isc_lmt_urlchars_cfg[ MAX_PATH ];	//	��������ֵ������ַ�����
	LONG	isc_enable_lmt_contentlen;		//	�Ƿ����������ݳ���
	LONG	isc_lmt_contentlen_sendbyserver;	//	���Ʒ��������ͳ��Ĵ�С
	LONG	isc_lmt_contentlen_postbyclient;	//	���ƴӿͻ��� POST ���������Ĵ�С
	LONG	isc_enable_lmt_dayip;			//	�Ƿ��������ش�������
	LONG	isc_lmt_dayip;				//	�� IP ���ش�������
#ifdef RUN_ENV_SERVICE
	TCHAR	szUrlCharsCfg[32][MAX_SINGLEURLCHARSLEN];	//	isc_lmt_urlchars_cfg ������չ��
	DWORD	dwUrlCharsCfgCount;				//	UrlCharsCfg ��Ч����
#endif

}STIISSECURITYTABLE, *PSTIISSECURITYTABLE;

//	[iis_log_table]
typedef struct tagIisLogable
{
	tagIisLogable()
	{
		memset( this, 0, sizeof(tagIisLogable) );
	}
	LONG	ilg_id;
	TCHAR	iis_siteid[32];				//	���� iis_table ���ֶ�
	LONG	ilg_logtimeperiod;			//	��־��ʽ��0 ÿСʱ��1ÿ�죬2ÿ�ܣ�3ÿ��
	LONG	ilg_logsize;				//	��־�ļ���С����λ MB

}STIISLOGABLE, *PSTIISLOGABLE;

//	[iis_advanced_table]
typedef struct tagIisAdvancedTable
{
	tagIisAdvancedTable()
	{
		memset( this, 0, sizeof(tagIisAdvancedTable) );
	}
	LONG	iav_id;
	TCHAR	iis_siteid[32];				//	���� iis_table ���ֶ�
	LONG	iav_wm_show_vwtitle;			//	������Ϣ���Ƿ���ʾVW����
	LONG	iav_wm_model;				//	������Ϣ��ģʽ��1-normal, 2-expert, 3-htmlfile
	LONG	iav_wm_http_status;			//	������Ϣ��http status
	LONG	iav_wm_delay_time;			//	������Ϣ��ͣ��ʱ��
	TCHAR	iav_wm_sitename[ MAX_PATH ];		//	������Ϣ����վ����
	TCHAR	iav_wm_redirect_url[ MAX_PATH ];	//	������Ϣ��ת�� URL
	TCHAR	iav_wm_sitelogo_url[ MAX_PATH ];	//	������Ϣ��վ��LOGO
	TCHAR	iav_wm_topinfo[ 1024 ];			//	������Ϣ��topinfo
	TCHAR	iav_wm_info[ 1024 ];			//	������Ϣ���û��Զ���ľ�����Ϣ
	TCHAR	iav_wm_htmlfile[ MAX_PATH ];		//	������Ϣ���û��Զ���� HTML �ļ�

}STIISADVANCEDTABLE, *PSTIISADVANCEDTABLE;








class CProcCfg : public CProcDb
{
public:
	CProcCfg( BOOL bShowMessageBox = TRUE );
	virtual ~CProcCfg();

	//	�������ݿ�
	BOOL	UpdateDatabase();


	BOOL	GetMainTableInfoById( UINT m_id, PSTMAINTABLE pstMainTable );
	BOOL	SaveMainTableInfoById( UINT m_id, LPSTR lpszField, LPSTR lpcszValue );
	BOOL	SaveMainTableInfoById( UINT m_id, LPSTR lpszField, LONG lnValue );

	BOOL	GetAllHostInfo( vector<STHOSTINFO> & vcHostInfoAll );
	BOOL	SaveAllHostInfo( vector<STHOSTINFO> & vcHostInfoAll );

	BOOL	TryToCreateNewSiteCfg( LPCSTR lpcszSiteId );

	//	..
	BOOL	GetAllIisTableInfo( vector<STIISTABLE> & vcIis );
	BOOL	GetIisTableInfoBySiteId( LPCSTR lpcszSiteId, PSTIISTABLE pstSiteInfo );
	BOOL	SaveIisTableInfoBySiteId( LPCSTR lpcszSiteId, PSTIISTABLE pstSiteInfo );
	BOOL	UpdateIisTableUseById( LONG iis_id, LONG iis_use );
	BOOL	UpdateIisTableUseBySiteId( LPCSTR lpcszSiteId, LONG iis_use );
	BOOL	UpdateIisTableRootPathBySiteId( LPCSTR lpcszSiteId, LPCSTR lpcszRootPath );
	BOOL	UpdateIisTableAppPoolIdBySiteId( LPCSTR lpcszSiteId, LPCSTR lpcszAppPoolId );

	BOOL	CopyGeneralSettingTo( LPSTR lpszSiteId, LPSTR lpszSiteIdCopyTo );


	//	..
	BOOL	QueryValueForIisBlockExtTable( _RecordsetPtr rs, STIISBLOCKEXTTABLE * pstIisBlockExtTable );
	UINT	GetIisBlockExtTableCountBySiteId( LPSTR lpszSiteId );
	BOOL	DeleteAllIisBlockExtTableInfoBySiteId( LPSTR lpszSiteId );
	BOOL	GetIisBlockExtTableInfoById( LONG ibe_id, STIISBLOCKEXTTABLE * pstIisBlockExtTable );

	BOOL	GetAllIisBlockExtTableInfoBySiteIdEx( LPSTR lpszSiteId, vector<STIISBLOCKEXTTABLE> & vcBlockExts );
	BOOL	DeleteIisBlockExtTableInfoById( LONG ibe_id );
	BOOL	SaveIisBlockExtTableInfoById( LONG ibe_id, STIISBLOCKEXTTABLE * pstIisBlockExtTable );

	BOOL	GetAllIisBlockDirTableInfoBySiteId( LPSTR lpszSiteId, vector<STIISBLOCKDIRSTABLE> & vcBlockDirs );
	BOOL	SaveIisBlockDirTableInfoById( LONG ibd_id, STIISBLOCKDIRSTABLE * pstIisBlockDirTable );
	BOOL	DeleteIisBlockDirTableInfoById( LONG ibd_id );
	BOOL	DeleteAllIisBlockDirTableInfoBySiteId( LPSTR lpszSiteId );

	BOOL	CopyProtectedSettingTo( LPSTR lpszSiteId, LPSTR lpszSiteIdCopyTo );


	//
	//	for exceptions
	//
	BOOL	GetAllIisAllowDirTableInfoBySiteId( LPSTR lpszSiteId, vector<STIISALLOWDIRSTABLE> & vcAllowDirs );
	BOOL	SaveIisAllowDirTableInfoById( LONG iad_id, STIISALLOWDIRSTABLE * pstIisAllowDirTable );
	BOOL	DeleteIisAllowDirTableInfoById( LONG iad_id );
	BOOL	DeleteAllIisAllowDirTableInfoBySiteId( LPSTR lpszSiteId );

	//	
	BOOL	GetAllIisFriendHostsTableInfoBySiteId( LPSTR lpszSiteId, vector<STIISFRIENDHOSTSTABLE> & vcFriendHosts );
	BOOL	SaveIisFriendHostsTableInfoById( LONG ifh_id, STIISFRIENDHOSTSTABLE * pstIisFriendHostsTable );
	BOOL	DeleteIisFriendHostsTableInfoById( LONG ifh_id );
	BOOL	DeleteAllIisFriendHostsTableInfoBySiteId( LPSTR lpszSiteId );

	BOOL	CopyExceptionsSettingTo( LPSTR lpszSiteId, LPSTR lpszSiteIdCopyTo );


	//
	//	for limit
	//
	//	[iis_blockhosts_table]
	BOOL	GetAllIisBlockHostsTableInfoBySiteId( LPSTR lpszSiteId, vector<STIISBLOCKHOSTSTABLE> & vcBlockHosts );
	BOOL	SaveIisBlockHostsTableInfoById( LONG ibh_id, STIISBLOCKHOSTSTABLE * pstIisBlockHostsTable );
	BOOL	DeleteIisBlockHostsTableInfoById( LONG ibh_id );
	BOOL	DeleteAllIisBlockHostsTableInfoBySiteId( LPSTR lpszSiteId );

	//	[iis_blockip_table]
	BOOL	GetAllIisBlockIPTableInfoBySiteId( LPSTR lpszSiteId, vector<STIISBLOCKIPTABLE> & vcBlockIP );
	BOOL	SaveIisBlockIPTableInfoById( LONG ibip_id, STIISBLOCKIPTABLE * pstIisBlockIPTable );
	BOOL	DeleteIisBlockIPTableInfoById( LONG ibip_id );
	BOOL	DeleteAllIisBlockIPTableInfoBySiteId( LPSTR lpszSiteId );

	BOOL	CopyLimitSettingTo( LPSTR lpszSiteId, LPSTR lpszSiteIdCopyTo );



	//
	//	for security
	//
	BOOL	GetIisSecurityTableInfoBySiteId( LPSTR lpszSiteId, STIISSECURITYTABLE * pstIisSecurity );
	BOOL	SaveIisSecurityTableInfoById( LONG isc_id, STIISSECURITYTABLE * pstIisSecurity );
	BOOL	DeleteIisSecurityTableInfoById( LONG isc_id );
	BOOL	DeleteAllIisSecurityTableInfoBySiteId( LPSTR lpszSiteId );

	BOOL	CopySecuritySettingTo( LPSTR lpszSiteId, LPSTR lpszSiteIdCopyTo );


	//
	//	for log
	//
	BOOL	GetIisLogTableInfoBySiteId( LPSTR lpszSiteId, STIISLOGABLE * pstIisLog );
	BOOL	SaveIisLogTableInfoById( LONG ilg_id, STIISLOGABLE * pstIisLog );
	BOOL	DeleteIisLogTableInfoById( LONG ilg_id );
	BOOL	DeleteAllIisLogTableInfoBySiteId( LPSTR lpszSiteId );

	BOOL	CopyLogSettingTo( LPSTR lpszSiteId, LPSTR lpszSiteIdCopyTo );


	//
	//	for advanced
	//
	BOOL	GetIisAdvancedTableInfoBySiteId( LPSTR lpszSiteId, STIISADVANCEDTABLE * pstIisAdvanced );
	BOOL	SaveIisAdvancedTableInfoById( LONG iav_id, STIISADVANCEDTABLE * pstIisAdvanced );
	BOOL	DeleteIisAdvancedTableInfoById( LONG iav_id );
	BOOL	DeleteAllIisAdvancedTableInfoBySiteId( LPSTR lpszSiteId );

	BOOL	CopyAdvancedSettingTo( LPSTR lpszSiteId, LPSTR lpszSiteIdCopyTo );

};




#endif // __PROCCFG_HEADER__
