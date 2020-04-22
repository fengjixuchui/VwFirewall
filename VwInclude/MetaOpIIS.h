// MetaOpIIS.h: interface for the CAdsiOpIIS class.
//
//////////////////////////////////////////////////////////////////////
#ifndef __METAOPIIS_HEADER__
#define __METAOPIIS_HEADER__

//	�ǵ�������������ǰ�棬�����������
#include <objbase.h>
#include <initguid.h>

//	�����ATL��֧��
#include <atlbase.h>

#include <winsock2.h>
#pragma comment(lib, "ws2_32")

#include "Crc32.h"
#include "MetaOpIISHeader.h"








/**
 *	���ڲ���IIS�����ӡ�ɾ�����޸�վ������
 */
class CMetaOpIIS
{
public:
	CMetaOpIIS();
	CMetaOpIIS( STMETASITEINFO * pSiteinfo );
	CMetaOpIIS( STMETAVIRTUALDIR * pVDir );

	CMetaOpIIS & operator = ( STMETASITEINFO & pSiteinfo );
	CMetaOpIIS & operator = ( STMETAVIRTUALDIR & pVDir );

	virtual ~CMetaOpIIS(void);

private:	
//	STMETASITEINFO	m_stSiteInfo;
	CCrc32		m_cCrc32;

	//PWCHAR m_wszBuf;			//
	DWORD  m_dwLastSysErrorCode;
	DWORD  m_dwLastOperErrorCode;
	TCHAR  m_szLastErrorInfo[ MAX_PATH ];
	PWCHAR m_wszBuf;

	STMETAVIRTUALDIR	m_VDir;		//	����Ŀ¼
	CComPtr <IMSAdminBase>	m_pIMeta;	//	ָ��MSAdminBase�ӿڵ�����ָ��
	CComPtr <IWamAdmin2>	m_pIAppAdmin;	//	ָ��IWamAdmin�ӿڵ�����ָ��

public:
	BOOL initCom();				//	��ʼ��COM����METABASE
	VOID uninitCom();			//	����COM���ر� METABASE
	BOOL isInitComReady();

	//
	//	վ�����
	//
	BOOL createSite( STMETASITEINFO * pstSiteInfo );	//	����һ��վ�㣬����KEYNAME
	BOOL deleteSite( STMETASITEINFO * pstSiteInfo );	//	ɾ��һ��վ��
	BOOL modifySite( STMETASITEINFO * pstSiteInfo );	//	�޸�һ��վ������

	BOOL getAllDataFromMetabase( STMETAALLDATA * pstAllData );					//	��ȡ����վ����Ϣ���������̳ص���Ϣ
	BOOL getSiteDataFromKey( LPCWSTR lpcwszKeyName, STMETASITEINFO * pstSiteInfo );			//	��ȡ����վ�����Ϣ
	BOOL getAppPoolDataFromKey( LPCWSTR lpcwszKeyName, STMETAAPPPOOLINFO * pstAppPoolInfo );	//	��ȡĳ�� AppPool ����Ϣ

	//
	//	����Ŀ¼�Ĳ���
	//
	BOOL AddVDir();
	BOOL DeleteVDir();
	BOOL ModifyVDir();

	BOOL HttpRedirect();				//	����Ŀ¼HTTP�ض�λ
//	INT  GetNewKeyName( char * szNewKeyName );	//	��ȡ�½�����վ������
	BOOL ReadAllData();				//	��ȡ����վ������

	//	������������Ϣ
	VOID setLastErrorInfo( DWORD dwOperErrorCode, LPCTSTR lpszFmt, ... );

private:

	DWORD getNewSiteKey();

	//	��ȡһ��վ���µ�ĳ����¼��ֵ
	HRESULT getRecordData( METADATA_HANDLE hKeyName, METADATA_RECORD * pstMRec, 
			DWORD dwMDIdentifier, DWORD dwMDAttributes, DWORD dwMDUserType, DWORD dwMDDataType, LPCWSTR lpcwszBuf, DWORD dwBufLen,
			DWORD * pdwMDRequiredDataLen );

	//	�޸�һ��վ���µ�ĳ����¼��ֵ
	BOOL modifyRecordData( STMETAOPERRECORD * pstRec, DWORD dwMDIdentifier, DWORD dwMDAttributes, DWORD dwMDUserType, DWORD dwMDDataType, LPCWSTR lpcwszBuf, DWORD dwBufLen );

	BOOL mb_createApp( STMETASITEINFO * pstSiteInfo, LPWSTR lpszMDPath, DWORD dwAppMode );		//	Ϊһ��վ�㽨��APP
	BOOL mb_createKey( STMETAOPERRECORD * pstRec );				//	����һ���Ӽ�
	BOOL mb_renameKey( STMETAOPERRECORD * pstRec );				//	�޸�һ������
	BOOL mb_deleteKey( STMETAOPERRECORD * pstRec );				//	ɾ��һ���Ӽ�
	BOOL mb_getKeyData( STMETAOPERRECORD * pstRec );			//	��ȡ����ֵ

	BOOL ModifyData( STMETAOPERRECORD * pstRec );				//	�޸����ݣ���һ����������
	BOOL isValidSBindings( LPCTSTR lpcszBindings );				//	������Ϣ�Ƿ���Ч
	BOOL isValidSingleSBindings( string strSBindings );			//	��鵥������Ϣ����Ч��
	INT  formatSBindingsToMetabase( LPWSTR pwszSBindings );			//	���ַ�����ʽ(�� | �ָ�) ����� Metabase ��ʽ(�� NULL �ָ�)
	VOID formatSBindingsToString( LPSTR lpszBuffer, DWORD dwSize );		//	�� Metabase �� Binding(�� NULL �ָ�)��������ݿ��ʽ(�� | �ָ�)

	BOOL DelHttpRedirect( STMETAOPERRECORD &operRecord );		//	ĳվ���Ƿ��ض���


	//bool GetBindingData( STMETAOPERRECORD &operRecord, WCHAR *pwszBuf, DWORD &dwBufLen );	//	��ȡ������
};




#endif	// __METAOPIIS_HEADER__