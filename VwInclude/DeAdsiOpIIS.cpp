// AdsiOpIIS.cpp: implementation of the CDeAdsiOpIIS class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "DeAdsiOpIIS.h"

#include "DeLib.h"
#pragma comment( lib, "DeLib.lib" )



//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CDeAdsiOpIIS::CDeAdsiOpIIS()
{
	CoInitialize(NULL);
}

CDeAdsiOpIIS::~CDeAdsiOpIIS()
{
	CoUninitialize();
}

/**
 *	@ public
 *	��ȡվ��ĸ���
 */
DWORD CDeAdsiOpIIS::GetSiteCount()
{
	DWORD dwCount = 0;
	CComPtr<IADsContainer> iContainer;
	IEnumVARIANT *pEnum = NULL;
	IADs *pADs;
	VARIANT vtVar;
	ULONG ulFetched	= 0L;
	CComBSTR bstrValue;
	
	
	if ( S_OK == ADsGetObject( L"IIS://localhost/w3svc", IID_IADsContainer, (void**)&iContainer ) )
	{
		// Create an enumerator object in the container.
		if ( S_OK == ADsBuildEnumerator( iContainer, &pEnum ) )
		{
			// Get the next contained object.
			while( S_OK == ADsEnumerateNext( pEnum, 1, &vtVar, &ulFetched ) && ulFetched > 0 )
			{
				if ( S_OK == V_DISPATCH(&vtVar)->QueryInterface( IID_IADs, (void**)&pADs ) )
				{
					if ( S_OK == pADs->get_Name(&bstrValue) && wcslen(bstrValue) > 0 )
					{
						if ( 0 != _wcsicmp( bstrValue, L"Filters" ) &&
							0 != _wcsicmp( bstrValue, L"Info" ) &&
							0 != _wcsicmp( bstrValue, L"AppPools" )
							)
						{
							dwCount ++;
						}
					}
					pADs->Release();
				}
				
				// Release the VARIANT.
				VariantClear( &vtVar );
			}
			
			// Free Enumer
			ADsFreeEnumerator( pEnum );
			pEnum = NULL;
		}

		iContainer.Release();
	}
	
	return dwCount;
}

/*
BOOL CDeAdsiOpIIS::GetAllSiteInfo( PVOID pDataInfoData, DWORD dwSiteCount )
{
	if ( NULL == pDataInfoData )
		return FALSE;

	BOOL bRet			= FALSE;
	TCHAR szTemp[ MAX_PATH ]	= {0};
	DWORD dwIndex			= 0;
	CComPtr<IADsContainer> iContainer;
	IEnumVARIANT *pEnum		= NULL;
	IADs *pADs;
	VARIANT vtVar;
	ULONG ulFetched			= 0L;
	CComBSTR bstrValue;
	CComPtr<IADs> iAdsSpec;
	VARIANT vtVarSub;


	PSTSITEINFO pstSiteInfo = (PSTSITEINFO)pDataInfoData;
	if ( pstSiteInfo )
	{
		// ..
		if ( S_OK == ADsGetObject( L"IIS://localhost/w3svc", IID_IADsContainer, (void**)&iContainer ) )
		{
			// Create an enumerator object in the container.
			if ( S_OK == ADsBuildEnumerator( iContainer, &pEnum ) )
			{
				// Get the next contained object.
				while( S_OK == ADsEnumerateNext( pEnum, 1, &vtVar, &ulFetched ) && ulFetched > 0 )
				{
					if ( S_OK == V_DISPATCH(&vtVar)->QueryInterface( IID_IADs, (void**)&pADs ) )
					{
						if ( S_OK == pADs->get_Name(&bstrValue) && wcslen(bstrValue) > 0 &&
							0 != _wcsicmp( bstrValue, L"Filters" ) &&
							0 != _wcsicmp( bstrValue, L"Info" ) &&
							0 != _wcsicmp( bstrValue, L"AppPools" )
						)
						{
							if ( S_OK == iContainer->GetObject( _bstr_t("IIsWebServer"), bstrValue, (IDispatch**)&iAdsSpec ) )
							{
								iAdsSpec->Get( _bstr_t("ServerComment"), &vtVarSub );

								// Get Site Id
								WideCharToMultiByte( CP_ACP, 0, bstrValue, wcslen(bstrValue), pstSiteInfo[dwIndex].szSiteId, sizeof(pstSiteInfo[dwIndex].szSiteId), NULL, NULL );
								// Get Metabase path
								_sntprintf( pstSiteInfo[dwIndex].szMetaPath, sizeof(pstSiteInfo[dwIndex].szMetaPath)/sizeof(TCHAR)-1, "IIS://localhost/w3svc/%s", pstSiteInfo[dwIndex].szSiteId );
								// Get ServerComment
								WideCharToMultiByte( CP_ACP, 0, vtVarSub.bstrVal, wcslen(vtVarSub.bstrVal), pstSiteInfo[dwIndex].szServerComment, sizeof(pstSiteInfo[dwIndex].szServerComment), NULL, NULL );

								bRet = TRUE;
								dwIndex ++;
								iAdsSpec.Release();
								VariantClear( &vtVarSub );
							}
						}

						pADs->Release();
					}

					VariantClear( &vtVar );
				}

				ADsFreeEnumerator( pEnum );
				pEnum = NULL;
			}

			iContainer.Release();
		}
	}

	return bRet;
}
*/


/**
 *	@ Public
 *	��ȡŶ����վ����Ϣ
 */
BOOL CDeAdsiOpIIS::GetAllSiteInfo( vector<STSITEINFO> & vcSiteInfoAll )
{
	BOOL bRet			= FALSE;
	TCHAR szTemp[ MAX_PATH ]	= {0};
	CComPtr<IADsContainer> iContainer;
	IEnumVARIANT *pEnum		= NULL;
	IADs *pADs;
	VARIANT vtVar;
	ULONG ulFetched			= 0L;
	CComBSTR bstrValue;
	VARIANT vtAppPoolId;
	VARIANT vtServerComment;
	VARIANT vtRootPath;
	STSITEINFO stSiteInfo;
	WCHAR wszMetaPath[ MAX_PATH ]	= {0};

	try
	{
		// ..
		if ( S_OK == ADsGetObject( CDEADSIOPIIS_WCHR_METAPATH_W3SVC, IID_IADsContainer, (void**)&iContainer ) )
		{
			// Create an enumerator object in the container.
			if ( S_OK == ADsBuildEnumerator( iContainer, &pEnum ) )
			{
				// Get the next contained object.
				while( S_OK == ADsEnumerateNext( pEnum, 1, &vtVar, &ulFetched ) && ulFetched > 0 )
				{
					memset( & stSiteInfo, 0, sizeof(stSiteInfo) );
					
					if ( S_OK == V_DISPATCH(&vtVar)->QueryInterface( IID_IADs, (void**)&pADs ) )
					{
						if ( S_OK == pADs->get_Name(&bstrValue) && wcslen(bstrValue) > 0 &&
							0 != _wcsicmp( bstrValue, L"Filters" ) &&
							0 != _wcsicmp( bstrValue, L"Info" ) &&
							0 != _wcsicmp( bstrValue, L"AppPools" )
						)
						{
							//	Get Site Id
							WideCharToMultiByte( CP_ACP, 0, bstrValue, (int)wcslen(bstrValue),
								stSiteInfo.szSiteId, sizeof(stSiteInfo.szSiteId), NULL, NULL );

							//	Get Metabase path
							_sntprintf( stSiteInfo.szMetaPath, sizeof(stSiteInfo.szMetaPath)/sizeof(TCHAR)-1,
								"IIS://localhost/w3svc/%s", stSiteInfo.szSiteId );
							_snwprintf( stSiteInfo.wszMetaPath, sizeof(stSiteInfo.wszMetaPath)/sizeof(WCHAR)-1,
								L"IIS://localhost/w3svc/%s", bstrValue );

							//	for root path
							_snwprintf( wszMetaPath, sizeof(wszMetaPath)-sizeof(WCHAR), L"%s/%s", CDEADSIOPIIS_WCHR_METAPATH_W3SVC, bstrValue );

							VariantInit( &vtAppPoolId );
							VariantClear( &vtAppPoolId );
							if ( MetaGetData( wszMetaPath, _T("IIsWebVirtualDir"), _T("Root"), _T("AppPoolId"), vtAppPoolId ) )
							{
								//	Get AppPoolId
								WideCharToMultiByte( CP_ACP, 0, vtAppPoolId.bstrVal, (int)wcslen(vtAppPoolId.bstrVal),
									stSiteInfo.szAppPoolId, sizeof(stSiteInfo.szAppPoolId), NULL, NULL );
							}

							VariantInit( &vtServerComment );
							VariantInit( &vtRootPath );
							if ( MetaGetData( CDEADSIOPIIS_WCHR_METAPATH_W3SVC, _T("IIsWebServer"), stSiteInfo.szSiteId, _T("ServerComment"), vtServerComment ) &&
								MetaGetData( wszMetaPath, _T("IIsWebVirtualDir"), _T("Root"), _T("Path"), vtRootPath ) )
							{
								//	Get ServerComment
								WideCharToMultiByte( CP_ACP, 0, vtServerComment.bstrVal, (int)wcslen(vtServerComment.bstrVal),
									stSiteInfo.szServerComment, sizeof(stSiteInfo.szServerComment), NULL, NULL );

								//	Get RootPath
								WideCharToMultiByte( CP_ACP, 0, vtRootPath.bstrVal, (int)wcslen(vtRootPath.bstrVal),
									stSiteInfo.szRootPath, sizeof(stSiteInfo.szRootPath), NULL, NULL );

								stSiteInfo.uIndex = (ULONG)( vcSiteInfoAll.size() );
								vcSiteInfoAll.push_back( stSiteInfo );

								bRet = TRUE;
							}
							VariantClear( &vtServerComment );
							VariantClear( &vtRootPath );
							
						}
						
						pADs->Release();
					}
					
					VariantClear( &vtVar );
				}
				
				ADsFreeEnumerator( pEnum );
				pEnum = NULL;
			}
			
			iContainer.Release();
		}
	}
	catch ( ... )
	{
	}	

	return bRet;
}


/**
 *	@ Public
 *	��ȡ���� HOST
 */
BOOL CDeAdsiOpIIS::GetAllHostInfo( vector<STHOSTINFO> & vcHostInfoAll, LPCTSTR lpcszSpecSites /*=NULL*/, BOOL bMustServerAutoStart /*=TRUE*/ )
{
	//
	//	vcHostInfoAll		- [out]    �������е� HOST �б�
	//	lpcszSpecSites		- [in/opt] ����һ����������վ��(siteid)�б�ֻ���������б��е�վ��
	//					   ��ʽ���£�"|22|33|44|"
	//	bMustServerAutoStart	- [in/opt] վ���״̬������������
	//	RETURN			- TRUE / FALSE
	//

	BOOL bRet				= FALSE;
	CComPtr<IADsContainer> iContainer;
	IEnumVARIANT *pEnum			= NULL;
	IADs *pADs;
	VARIANT vtVar;
	ULONG ulFetched				= 0L;
	CComBSTR bstrValue;
	CComPtr<IADs> iAdsSpec;
	VARIANT vtVarSub;
	BOOL  bSpecSite;
	TCHAR szSiteId[ 64 ]			= {0};
	TCHAR szTmpSiteId[ 128 ]		= {0};
	STHOSTINFO stHostInfo;
	BOOL  bSaveItem;

	// ..
	if ( S_OK == ADsGetObject( L"IIS://localhost/w3svc", IID_IADsContainer, (void**)&iContainer ) )
	{
		// Create an enumerator object in the container.
		if ( S_OK == ADsBuildEnumerator( iContainer, &pEnum ) )
		{
			// Get the next contained object.
			while( S_OK == ADsEnumerateNext( pEnum, 1, &vtVar, &ulFetched ) && ulFetched > 0 )
			{
				memset( & stHostInfo, 0, sizeof(stHostInfo) );

				if ( S_OK == V_DISPATCH(&vtVar)->QueryInterface( IID_IADs, (void**)&pADs ) )
				{
					if ( S_OK == pADs->get_Name(&bstrValue) && wcslen(bstrValue) > 0 &&
						0 != _wcsicmp( bstrValue,L"Filters" ) &&
						0 != _wcsicmp( bstrValue,L"Info" ) &&
						0 != _wcsicmp( bstrValue, L"AppPools" )
					)
					{
						if ( S_OK == iContainer->GetObject( _bstr_t("IIsWebServer"), bstrValue, (IDispatch**)&iAdsSpec ) )
						{
							bRet = TRUE;

							bSaveItem	= TRUE;

							if ( bMustServerAutoStart )
							{
								//	Ҫ��վ��״̬Ϊ�����������е�
								VariantInit( &vtVarSub );
								if ( S_OK == iAdsSpec->Get( _bstr_t("ServerAutoStart"), &vtVarSub ) )
								{
									if ( 0 == vtVarSub.intVal || 0 == vtVarSub.iVal )
									{
										//	վ��״̬�����Ѿ��ر�
										bSaveItem = FALSE;
									}
								}
								VariantClear( &vtVarSub );
							}

							if ( bSaveItem )
							{
								//	Get Site Id
								memset( szSiteId, 0, sizeof(szSiteId) );
								WideCharToMultiByte( CP_ACP, 0, bstrValue, (int)wcslen(bstrValue),
									szSiteId, sizeof(szSiteId), NULL, NULL );

								bSpecSite = TRUE;
								if ( lpcszSpecSites && strlen( lpcszSpecSites ) > 2 )
								{
									_sntprintf( szTmpSiteId, sizeof(szTmpSiteId)/sizeof(TCHAR)-1, _T("|%s|"), szSiteId );
									if ( ! strstr( lpcszSpecSites, szTmpSiteId ) )
									{
										bSpecSite = FALSE;
									}
								}

								//	Get ServerBindings
								VariantInit( &vtVarSub );

								//
								//	��ȡ ServerBindings ���ҽ�������� push �� vcHostInfoAll
								//
								if ( S_OK == iAdsSpec->Get( _bstr_t("ServerBindings"), &vtVarSub ) )
								{
									//	Parse bindings and put them to vcHostInfoAll
									ParseBindingsAndPuts( szSiteId, vtVarSub, vcHostInfoAll, bSpecSite );
								}
								VariantClear( &vtVarSub );
							}

							//	release
							iAdsSpec.Release();
						}
					}
					
					pADs->Release();
				}
				
				VariantClear( &vtVar );
			}
			
			ADsFreeEnumerator( pEnum );
			pEnum = NULL;
		}
		
		iContainer.Release();
	}
	
	return bRet;
}


/**
 *	@ Public
 *	��װ������
 */
BOOL CDeAdsiOpIIS::InstallFilter( LPCTSTR lpcszMetaPath, LPCTSTR lpcszFilterDesc, LPCTSTR lpcszFilterName, LPCTSTR lpcszFilterPath, BOOL bAutoRemoveAllFilter /*= TRUE*/ )
{
	//
	//	lpcszMetaPath		- [in] Metabase Path, for example:
	//					"IIS://localhost/w3svc/1"	: Filter will install at the site which siteid=1
	//					"IIS://localhost/w3svc"		: Filter will install at the global
	//	lpcszFilterDesc		- [in] Filter ������
	//	lpcszFilterName		- [in] Filter ������
	//	lpcszFilterPath		- [in] Filter ��Ӳ��·��
	//


//	if ( NULL == lpcszMetaPath || 0 == _tcslen(lpcszMetaPath) )
//		return FALSE;
	if ( NULL == lpcszFilterDesc || 0 == _tcslen(lpcszFilterDesc) )
	{
		return FALSE;
	}
	if ( NULL == lpcszFilterName || 0 == _tcslen(lpcszFilterName) )
	{
		return FALSE;
	}
	if ( NULL == lpcszFilterPath || 0 == _tcslen(lpcszFilterPath) )
	{
		return FALSE;
	}
	if ( FALSE == PathFileExists(lpcszFilterPath) )
	{
		return FALSE;
	}


	BOOL bRet = FALSE;

	DWORD dwUnInsCount			= 0;
	CComPtr<IADsContainer> iContainer;
	CComPtr<IADsContainer> iContainer2;
	CComPtr<IADs> iAds;
	CComPtr<IADs> iAds2;
	VARIANT vLoadOrder;
	WCHAR wszFilterName[ MAX_PATH ]		= {0};
	TCHAR szFilterName[ MAX_PATH ]		= {0};
	WCHAR wszMetaPath[ MAX_PATH ]		= {0};
	TCHAR szMetaPath[ MAX_PATH ]		= {0};
	TCHAR szFilterFilePath[ MAX_PATH ]	= {0};
	TCHAR szBuffer[ MAX_PATH ]		= {0};
	TCHAR szFilterLoadOrder[ MAX_PATH ]	= {0};


	//
	//	����� X64 λ����ϵͳ������Ҫ�޸� Metabase ���ݿ⣬�� IIS ֧�� 32 λ������ X64 ϵͳ�ϼ�������
	//	
	Enable32BitAppOnWin64();


	////////////////////////////////////////////////////////////
	//	ת��խ�ֽڵ� MetaPath �����ֽ�
	if ( lpcszMetaPath && _tcslen( lpcszMetaPath ) )
	{
		//	�û�δָ������װ��ȫ��
		if ( 0 == MultiByteToWideChar( CP_ACP, 0, lpcszMetaPath, (int)_tcslen(lpcszMetaPath)+sizeof(TCHAR), wszMetaPath, sizeof(wszMetaPath)/sizeof(wszMetaPath[0]) ) )
		{
			return FALSE;
		}
	}
	else
	{
		//	ʹ��Ĭ��ֵ����װ��ȫ��
		_snwprintf( wszMetaPath, sizeof(wszMetaPath)-sizeof(WCHAR), L"%s", CDEADSIOPIIS_WCHR_METAPATH_W3SVC );
	}


	////////////////////////////////////////////////////////////
	// ��װ֮ǰ����������Ѿ���װ��վ���ϵĸ� Filter
	if ( bAutoRemoveAllFilter )
	{
		while ( FindFilterMetaInfo( lpcszFilterPath,
				szFilterName, sizeof(szFilterName),
				szMetaPath, sizeof(szMetaPath),
				szFilterFilePath, sizeof(szFilterFilePath) ) )
		{
			UnInstallSpecFilter( szFilterName, szMetaPath );

			// ..
			if ( dwUnInsCount++ > 500 )
				break;
		}
	}


	//////////////////////////////////////////////////////////////////////////
	//
	if ( S_OK == ADsGetObject( wszMetaPath, IID_IADsContainer, (void**)&iContainer ) )
	{
		if ( S_OK == iContainer->GetObject(_bstr_t("IIsFilters"),_bstr_t("Filters"),(IDispatch**)&iAds) )
		{
			//	��һ������ FilterLoadOrder �б����һ��
			iAds->Get( _bstr_t("FilterLoadOrder"), &vLoadOrder );
			WideCharToMultiByte( CP_ACP, 0, vLoadOrder.bstrVal, (int)wcslen(vLoadOrder.bstrVal), szBuffer, sizeof(szBuffer), NULL, NULL );
			if ( _tcslen(szBuffer) > 0 )
			{
				if ( _tcsstr( szBuffer, lpcszFilterName ) )
				{
					_sntprintf( szFilterLoadOrder, sizeof(szFilterLoadOrder)/sizeof(TCHAR)-1, _T("%s"), szBuffer );
				}
				else
				{
					_sntprintf( szFilterLoadOrder, sizeof(szFilterLoadOrder)/sizeof(TCHAR)-1, _T("%s,%s"), szBuffer, lpcszFilterName );
				}
			}
			else
			{
				_sntprintf( szFilterLoadOrder, sizeof(szFilterLoadOrder)/sizeof(TCHAR)-1, _T("%s"), lpcszFilterName );
			}
			iAds->Put( _bstr_t("FilterLoadOrder"), _variant_t(szFilterLoadOrder) );
			iAds->SetInfo();

			//	�ڶ�����д����Ϣ
			if ( S_OK == iAds->QueryInterface(IID_IADsContainer,(void**)&iContainer2) )
			{
				bRet = TRUE;
				
				if ( S_OK == iContainer2->Create(_bstr_t("IIsFilter"),_bstr_t(lpcszFilterName),(IDispatch**)&iAds2) )
				{
					iAds2->Put( _bstr_t("FilterPath"), _variant_t(lpcszFilterPath) );
					iAds2->SetInfo();
				}

				iAds2.Release();
			}

			iAds.Release();
		}

		iContainer.Release();
	}
	
	return bRet;
}

/**
 *	@ Public
 *	���ƶ��Ĺ��������򵽵�һλ��ʹ�ü��ظ�������
 */
BOOL CDeAdsiOpIIS::OrderFilterInFirst( LPCTSTR lpcszMetaPath, LPCTSTR lpcszFilterName )
{
	//
	//	lpcszFilterName		- [in] Filter ������
	//

	if ( NULL == lpcszFilterName || 0 == _tcslen(lpcszFilterName) )
	{
		return FALSE;
	}

	BOOL bRet = FALSE;

	DWORD dwUnInsCount			= 0;
	CComPtr<IADsContainer> iContainer;
	CComPtr<IADs> iAds;
	VARIANT vLoadOrder;
	WCHAR wszFilterName[ MAX_PATH ]		= {0};
	TCHAR szFilterName[ MAX_PATH ]		= {0};
	WCHAR wszMetaPath[ MAX_PATH ]		= {0};
	TCHAR szMetaPath[ MAX_PATH ]		= {0};
	TCHAR szBuffer[ MAX_PATH ]		= {0};
	TCHAR szFilterLoadOrder[ MAX_PATH ]	= {0};
	TCHAR szNewFilterLoadOrder[ MAX_PATH ]	= {0};


	////////////////////////////////////////////////////////////
	//	ת��խ�ֽڵ� MetaPath �����ֽ�
	if ( lpcszMetaPath && _tcslen( lpcszMetaPath ) )
	{
		//	�û�δָ������װ��ȫ��
		if ( 0 == MultiByteToWideChar( CP_ACP, 0, lpcszMetaPath, (int)_tcslen(lpcszMetaPath)+sizeof(TCHAR), wszMetaPath, sizeof(wszMetaPath)/sizeof(wszMetaPath[0]) ) )
		{
			return FALSE;
		}
	}
	else
	{
		//	ʹ��Ĭ��ֵ����װ��ȫ��
		_snwprintf( wszMetaPath, sizeof(wszMetaPath)-sizeof(WCHAR), L"%s", CDEADSIOPIIS_WCHR_METAPATH_W3SVC );
	}

	//////////////////////////////////////////////////////////////////////////
	//
	if ( S_OK == ADsGetObject( wszMetaPath, IID_IADsContainer, (void**)&iContainer ) )
	{
		if ( S_OK == iContainer->GetObject(_bstr_t("IIsFilters"),_bstr_t("Filters"),(IDispatch**)&iAds) )
		{
			//
			//	1����ȡ FilterLoadOrder �б�
			//	2��ɾ�������������������
			//	3��������װ��������������������Ʒ��ڵ�һλ
			//	4������֮
			//
			iAds->Get( _bstr_t("FilterLoadOrder"), &vLoadOrder );
			WideCharToMultiByte( CP_ACP, 0, vLoadOrder.bstrVal, (int)wcslen(vLoadOrder.bstrVal), szBuffer, sizeof(szBuffer), NULL, NULL );
			if ( _tcslen(szBuffer) > 0 )
			{
				if ( _tcsstr( szBuffer, _T(",") ) && _tcsstr( szBuffer, lpcszFilterName ) )
				{
					if ( DeleteSpecStrFromList( szBuffer, lpcszFilterName, ",", szFilterLoadOrder, sizeof(szFilterLoadOrder) ) )
					{
						//	..
						_sntprintf( szNewFilterLoadOrder, sizeof(szNewFilterLoadOrder)/sizeof(TCHAR)-1, _T("%s,%s"), lpcszFilterName, szFilterLoadOrder );

						//	Save new FilterLoadOrder
						iAds->Put( _bstr_t("FilterLoadOrder"), _variant_t(szNewFilterLoadOrder) );
						iAds->SetInfo();
					}
				}
			}

			iAds.Release();
		}

		iContainer.Release();
	}

	return bRet;
}

/**
 *	@ Public
 *	ж������ָ�� DESC �� Filter
 */
BOOL CDeAdsiOpIIS::UnInstallFilter( LPCTSTR lpcszFilterDescIn, LPCTSTR lpcszFilterNameIn, LPCTSTR lpcszFilterPathIn )
{
	//
	//	lpcszFilterDescIn	- [in] ����������
	//	lpcszFilterNameIn	- [in] ���������ƣ����� "VirtualWall"
	//	lpcszFilterPathIn	- [in] �������ļ�ȫ·�������� "c:\\VirtualWall\\VirtualWall.dll"
	//	RETURN			- TRUE / FLASE
	//
	
	BOOL bRet = FALSE;

	DWORD dwUnInsCount			= 0;
	TCHAR szFilterName[ MAX_PATH ]		= {0};
	TCHAR szMetaPath[ MAX_PATH ]		= {0};
	TCHAR szFilterFilePath[ MAX_PATH ]	= {0};

	////////////////////////////////////////
	// ж������
	while ( FindFilterMetaInfo( lpcszFilterPathIn,
			szFilterName, sizeof(szFilterName),
			szMetaPath, sizeof(szMetaPath),
			szFilterFilePath, sizeof(szFilterFilePath) )
	)
	{
		bRet = TRUE;
		UnInstallSpecFilter( szFilterName, szMetaPath );

		// ..
		if ( dwUnInsCount++ > 100 )
			break;
	}

	return bRet;
}


/**
 *	@ Public
 *	UnInstall Special Filter
 */
BOOL CDeAdsiOpIIS::UnInstallSpecFilter( LPCTSTR lpcszFilterName, LPCTSTR lpcszMetaPath )
{
	if ( NULL == lpcszFilterName || 0 == _tcslen(lpcszFilterName) )
		return FALSE;
	if ( NULL == lpcszMetaPath || 0 == _tcslen(lpcszMetaPath) )
		return FALSE;


	BOOL bRet = FALSE;

	CComPtr<IADsContainer> iContainer;
	CComPtr<IADsContainer> iContainer2;
	CComPtr<IADs> iAds;
	CComPtr<IADs> iAds2;
	VARIANT vLoadOrder;
	WCHAR wszMetaPath[ MAX_PATH ]		= {0};
	TCHAR szBuffer[ MAX_PATH ]		= {0};
	TCHAR szFilterLoadOrder[ MAX_PATH ]	= {0};

	if ( 0 == MultiByteToWideChar( CP_ACP, 0, lpcszMetaPath, (int)( _tcslen(lpcszMetaPath)+sizeof(TCHAR) ), wszMetaPath, sizeof(wszMetaPath)/sizeof(wszMetaPath[0]) ) )
	{
		return FALSE;
	}

	//////////////////////////////////////////////////////////////////////////
	//
	if ( S_OK == ADsGetObject( wszMetaPath, IID_IADsContainer, (void**)&iContainer ) )
	{
		if ( S_OK == iContainer->GetObject( _bstr_t("IIsFilters"), _bstr_t("Filters"),(IDispatch**)&iAds ) )
		{
			//	��һ������ FilterLoadOrder �б����һ��
			iAds->Get( _bstr_t("FilterLoadOrder"), &vLoadOrder );
			WideCharToMultiByte( CP_ACP, 0, vLoadOrder.bstrVal, (int)wcslen(vLoadOrder.bstrVal), szBuffer, sizeof(szBuffer), NULL, NULL );
			if ( _tcslen(szBuffer) > 0 )
			{
				if ( DeleteSpecStrFromList( szBuffer, lpcszFilterName, ",", szFilterLoadOrder, sizeof(szFilterLoadOrder) ) )
				{
					//	..
					iAds->Put( _bstr_t("FilterLoadOrder"), _variant_t(szFilterLoadOrder) );
					iAds->SetInfo();

					//	�ڶ�����ɾ�� Filter ��Ϣ
					if ( S_OK == iAds->QueryInterface( IID_IADsContainer, (void**)&iContainer2 ) )
					{
						if ( S_OK  == iContainer2->Delete( _bstr_t("IIsFilter"), _bstr_t(lpcszFilterName) ) )
						{
							bRet = TRUE;
						}
						//	..
						iContainer2.Release();
					}
				}
			}

			iAds.Release();
		}

		iContainer.Release();
	}

	return bRet;
}


/**
 *	@ Public
 *	Get Filter State
 */
DWORD CDeAdsiOpIIS::GetFilterState( LPCTSTR lpcszFilterFilePathIn, LPTSTR lpszFilterFilePath, DWORD dwFfpSize )
{
	if ( NULL == lpcszFilterFilePathIn || 0 == _tcslen(lpcszFilterFilePathIn) )
	{
		return 0;
	}

	DWORD dwRet	= 0;

	CComPtr<IADsContainer> iContainer;
	CComPtr<IADs> iAds;
	VARIANT vState;
	WCHAR wszMetaPath[ MAX_PATH ]		= {0};
	TCHAR szMetaPath[ MAX_PATH ]		= {0};
	TCHAR szFilterName[ MAX_PATH ]		= {0};
	TCHAR szFilterFilePath[ MAX_PATH ]	= {0};


	//	Firstly, Find metabase path by filter name
	if ( FindFilterMetaInfo( lpcszFilterFilePathIn,
			szFilterName, sizeof(szFilterName),
			szMetaPath, sizeof(szMetaPath),
			szFilterFilePath, sizeof(szFilterFilePath) ) )
	{
		if ( lpszFilterFilePath && dwFfpSize )
		{
			_sntprintf( lpszFilterFilePath, dwFfpSize/sizeof(TCHAR)-1, _T("%s"), szFilterFilePath );
		}

		//	Get info ...
		_tcscat( szMetaPath, "/Filters" );
		if ( MultiByteToWideChar( CP_ACP, 0, szMetaPath, (int)( _tcslen(szMetaPath)+sizeof(TCHAR) ), wszMetaPath, sizeof(wszMetaPath)/sizeof(wszMetaPath[0]) ) > 0 )
		{
			if ( S_OK == ADsGetObject( wszMetaPath, IID_IADsContainer, (void**)&iContainer ) )
			{
				VariantInit( &vState );
				if ( S_OK == iContainer->GetObject( _bstr_t("IIsFilter"),_bstr_t(szFilterName),(IDispatch**)&iAds ) )
				{
					iAds->Get( _bstr_t("FilterState"), &vState );
					dwRet = vState.lVal;

					iAds.Release();
				}
				VariantClear( &vState );

				iContainer.Release();
			}
		}
	}

	return dwRet;
}

/**
 *	@ Public
 *	Get Filter Enabled
 */
DWORD CDeAdsiOpIIS::GetFilterEnabled( LPCTSTR lpcszFilterFilePathIn, LPTSTR lpszFilterFilePath, DWORD dwFfpSize )
{
	if ( NULL == lpcszFilterFilePathIn || 0 == _tcslen(lpcszFilterFilePathIn) )
	{
		return 0;
	}
	
	DWORD dwRet	= 0;
	
	CComPtr<IADsContainer> iContainer;
	CComPtr<IADs> iAds;
	VARIANT vState;
	WCHAR wszMetaPath[ MAX_PATH ]		= {0};
	TCHAR szMetaPath[ MAX_PATH ]		= {0};
	TCHAR szFilterName[ MAX_PATH ]		= {0};
	TCHAR szFilterFilePath[ MAX_PATH ]	= {0};
	
	
	//	Firstly, Find metabase path by filter name
	if ( FindFilterMetaInfo( lpcszFilterFilePathIn,
		szFilterName, sizeof(szFilterName),
		szMetaPath, sizeof(szMetaPath),
		szFilterFilePath, sizeof(szFilterFilePath) ) )
	{
		if ( lpszFilterFilePath && dwFfpSize )
		{	
			_sntprintf( lpszFilterFilePath, dwFfpSize/sizeof(TCHAR)-1, _T("%s"), szFilterFilePath );
		}	
		
		//	Get info ...
		_tcscat( szMetaPath, "/Filters" );
		if ( MultiByteToWideChar( CP_ACP, 0, szMetaPath, (int)( _tcslen(szMetaPath)+sizeof(TCHAR) ), wszMetaPath, sizeof(wszMetaPath)/sizeof(wszMetaPath[0]) ) > 0 )
		{
			if ( S_OK == ADsGetObject( wszMetaPath, IID_IADsContainer, (void**)&iContainer ) )
			{
				//VariantInit( &vState );
				if ( S_OK == iContainer->GetObject( _bstr_t("IIsFilter"),_bstr_t(szFilterName),(IDispatch**)&iAds ) )
				{
					iAds->Get( _bstr_t("FilterEnabled"), &vState );
					dwRet = vState.bVal ? 1 : 0;

					iAds.Release();
				}
				//VariantClear( &vState );

				iContainer.Release();
			}
		}
	}

	return dwRet;
}

/**
 *	@ Public
 *	�������Ƿ�װ��
 */
BOOL CDeAdsiOpIIS::IsFilterLoaded( LPCTSTR lpcszFilterFilePathIn, LPTSTR lpszFilterFilePath, DWORD dwFfpSize )
{
	//
	//	lpcszFilterFilePathIn	- [in]  ָ���������ļ�·��
	//	lpszFilterFilePath	- [out] ���ص�·������ʵû��ɶ�ã�Ϲ��
	//	dwFfpSize		- [in]  ���ػ�������С
	//

	if ( NULL == lpcszFilterFilePathIn || 0 == _tcslen(lpcszFilterFilePathIn) )
	{
		return FALSE;
	}

	BOOL  bRet	= FALSE;
	DWORD dwState	= 0;
	ENUMDEWINDOWSSYSTYPE osver;


	osver	= delib_get_shellsystype();
	if ( _DEOS_WINDOWS_2008 == osver || _DEOS_WINDOWS_VISTA == osver || _DEOS_WINDOWS_2008_R2 == osver )
	{
		if ( GetFilterEnabled( lpcszFilterFilePathIn, lpszFilterFilePath, dwFfpSize ) )
		{
			bRet = TRUE;
		}
		else
		{
			bRet = FALSE;
		}
	}
	else
	{
		dwState = GetFilterState( lpcszFilterFilePathIn, lpszFilterFilePath, dwFfpSize );
		if ( MD_FILTER_STATE_LOADED == dwState )
		{
			bRet = TRUE;
		}
		else if ( MD_FILTER_STATE_UNLOADED == dwState )
		{
			bRet = FALSE;
		}
	}
	// ..
	return bRet;
}


//
//	for extension
//
BOOL CDeAdsiOpIIS::InstallExtension( LPCTSTR lpcszExtensionName, LPCTSTR lpcszExtensionFile )
{
	//
	//	lpcszExtensionName	- [in] ��չ�����ƣ����������Ӣ��
	//	lpcszExtensionFile	- [in] ��չ�����ļ�ȫ·��
	//	RETURN			- TRUE / FALSE
	//

	if ( NULL == lpcszExtensionName || 0 == _tcslen(lpcszExtensionName) )
	{
		return FALSE;
	}
	if ( NULL == lpcszExtensionFile || 0 == _tcslen(lpcszExtensionFile) )
	{
		return FALSE;
	}
	if ( ! PathFileExists( lpcszExtensionFile ) )
	{
		return FALSE;
	}

	BOOL bRet = FALSE;
	BOOL bSaveScriptMaps;
	BOOL bSaveWebSvcExt;

//	VARIANT vArrScriptMaps;
//	VARIANT vArrScriptMapsNew;
//	vector<STMETASCRIPTMAP> vcScriptMaps;
//	STMETASCRIPTMAP stMapNew;
	vector<STSITEINFO> vcSiteInfoAll;
	vector<STSITEINFO>::iterator its;
	//STSITEINFO * pstSite;

	VARIANT vArrWebSvcExt;
	VARIANT vArrWebSvcExtNew;
	vector<STMETAWEBSVCEXTRESTRICTIONLIST> vcWebSvcExt;
	STMETAWEBSVCEXTRESTRICTIONLIST stWebSvcExt;


	//
	//	����� X64 λ����ϵͳ������Ҫ�޸� Metabase ���ݿ⣬�� IIS ֧�� 32 λ������ X64 ϵͳ�ϼ�������
	//	
	Enable32BitAppOnWin64();


	bSaveScriptMaps	= FALSE;
	bSaveWebSvcExt	= FALSE;

	//
	//	��װ����ͨ���Ӧ�ó���ӳ�䡱
	//
	if ( AddOrUpdateScriptMaps( CDEADSIOPIIS_WCHR_METAPATH_LOCALHOST, _T("IIsWebService"), _T("W3SVC"), 0, _T("*"), lpcszExtensionFile, "" ) )
	{
		//	Ӧ�õ�������վ��
		vcSiteInfoAll.clear();
		if ( GetAllSiteInfo( vcSiteInfoAll ) && vcSiteInfoAll.size() )
		{
			for ( its = vcSiteInfoAll.begin(); its != vcSiteInfoAll.end(); its ++ )
			{
				//pstSite = (STSITEINFO*)its;
				AddOrUpdateScriptMaps( (*its).wszMetaPath, _T("IIsWebVirtualDir"), _T("root"), 0, _T("*"), lpcszExtensionFile, "" );
			}
		}

		bSaveScriptMaps = TRUE;
	}

	//
	//	��װ����Web������չ��
	//
	VariantInit( &vArrWebSvcExt );
	if ( MetaGetData( CDEADSIOPIIS_WCHR_METAPATH_LOCALHOST, _T("IIsWebService"), _T("W3SVC"), _T("WebSvcExtRestrictionList"), vArrWebSvcExt ) )
	{
		vcWebSvcExt.clear();
		if ( ParseWebSvcExtRestrictionList( vArrWebSvcExt, vcWebSvcExt ) )
		{
			//
			//	����ļ��Ѿ����ڣ���ɵ���(��Ϊ�п���·������ͬ)
			//
			if ( IsFilenameExistInWebSvcExtRestrictionList( vcWebSvcExt, lpcszExtensionFile ) )
			{
				DeleteFilenameFromWebSvcExtRestrictionList( vcWebSvcExt, lpcszExtensionFile );
			}

			memset( & stWebSvcExt, 0, sizeof(stWebSvcExt) );
			
			stWebSvcExt.dwAllowDenyFlag	= 1;		//	Ĭ������Ϊ����
			_sntprintf( stWebSvcExt.szExtensionPath, sizeof(stWebSvcExt.szExtensionPath)/sizeof(TCHAR)-1, _T("%s"), lpcszExtensionFile );
			stWebSvcExt.dwUIDeletableFlag	= 1;		//	�����û��Լ�ɾ��
			_tcscpy( stWebSvcExt.szGroupID, _T("") );	//	û����
			_sntprintf( stWebSvcExt.szApplicationName, sizeof(stWebSvcExt.szApplicationName)/sizeof(TCHAR)-1, _T("%s"), lpcszExtensionName );
			
			vcWebSvcExt.push_back( stWebSvcExt );

			if ( BuildWebSvcExtRestrictionList( vcWebSvcExt, vArrWebSvcExtNew ) )
			{
				//
				//	���浽 Metabase
				//
				bSaveWebSvcExt = MetaSetData
				(
					CDEADSIOPIIS_WCHR_METAPATH_LOCALHOST,
					_T("IIsWebService"),
					_T("W3SVC"),
					_T("WebSvcExtRestrictionList"),
					vArrWebSvcExtNew
				);
			}
		}
	}

	return ( bSaveScriptMaps && bSaveWebSvcExt );
}

/**
 *	@ Public
 *	����վ�ϵ� ScriptMaps ӳ�䵽��վ��
 */
BOOL CDeAdsiOpIIS::ApplyExtensionScriptMapsToChildrenSite( LPCTSTR lpcszExtensionName, LPCTSTR lpcszExtensionFile )
{
	//
	//	lpcszExtensionName	- [in] ��չ�����ƣ����������Ӣ��
	//	lpcszExtensionFile	- [in] ��չ�����ļ�ȫ·��
	//	RETURN			- TRUE / FALSE
	//
	if ( NULL == lpcszExtensionName || 0 == _tcslen(lpcszExtensionName) )
	{
		return FALSE;
	}
	if ( NULL == lpcszExtensionFile || 0 == _tcslen(lpcszExtensionFile) )
	{
		return FALSE;
	}
	if ( ! PathFileExists( lpcszExtensionFile ) )
	{
		return FALSE;
	}

	BOOL bRet = FALSE;

	vector<STSITEINFO> vcSiteInfoAll;
	vector<STSITEINFO>::iterator its;
	//STSITEINFO * pstSite;

	if ( IsExtensionInstalled( lpcszExtensionName, lpcszExtensionFile ) )
	{
		if ( AddOrUpdateScriptMaps( CDEADSIOPIIS_WCHR_METAPATH_LOCALHOST, _T("IIsWebService"), _T("W3SVC"), 0, _T("*"), lpcszExtensionFile, "" ) )
		{
			//	Ӧ�õ�������վ��
			vcSiteInfoAll.clear();
			if ( GetAllSiteInfo( vcSiteInfoAll ) && vcSiteInfoAll.size() )
			{
				for ( its = vcSiteInfoAll.begin(); its != vcSiteInfoAll.end(); its ++ )
				{
					//pstSite = (STSITEINFO*)its;
					AddOrUpdateScriptMaps( (*its).wszMetaPath, _T("IIsWebVirtualDir"), _T("root"), 0, _T("*"), lpcszExtensionFile, "" );
				}
			}

			bRet = TRUE;
		}
	}

	return bRet;
}


/**
 *	@ Public
 *	ж����չ
 */
BOOL CDeAdsiOpIIS::UninstallExtension( LPCTSTR lpcszExtensionName, LPCTSTR lpcszExtensionFile )
{
	//
	//	lpcszExtensionName	- [in] ��չ�����ƣ����������Ӣ��
	//	lpcszExtensionFile	- [in] ��չ�����ļ�ȫ·��
	//	RETURN			- TRUE / FALSE
	//

	if ( NULL == lpcszExtensionName || 0 == _tcslen(lpcszExtensionName) )
	{
		return FALSE;
	}
	if ( NULL == lpcszExtensionFile || 0 == _tcslen(lpcszExtensionFile) )
	{
		return FALSE;
	}
	if ( ! PathFileExists( lpcszExtensionFile ) )
	{
		return FALSE;
	}

	BOOL bRet = FALSE;
	BOOL bSaveScriptMaps;
	BOOL bSaveWebSvcExt;
//	CComPtr<IADsContainer> iContainer;
//	CComPtr<IADs> iAds;

//	VARIANT vArrScriptMaps;
//	VARIANT vArrScriptMapsNew;
//	vector<STMETASCRIPTMAP> vcScriptMaps;
//	STMETASCRIPTMAP stMapNew;
	vector<STSITEINFO> vcSiteInfoAll;
	vector<STSITEINFO>::iterator its;
	//STSITEINFO * pstSite;

	VARIANT vArrWebSvcExt;
	VARIANT vArrWebSvcExtNew;
	vector<STMETAWEBSVCEXTRESTRICTIONLIST> vcWebSvcExt;
	STMETAWEBSVCEXTRESTRICTIONLIST stWebSvcExt;


	bSaveScriptMaps	= FALSE;
	bSaveWebSvcExt	= FALSE;

	//
	//	ж������ ScriptMaps
	//
	if ( DeleteScriptMaps( CDEADSIOPIIS_WCHR_METAPATH_LOCALHOST, _T("IIsWebService"), _T("W3SVC"), lpcszExtensionFile ) )
	{
		//	Ӧ�õ�������վ��
		vcSiteInfoAll.clear();
		if ( GetAllSiteInfo( vcSiteInfoAll ) && vcSiteInfoAll.size() )
		{
			for ( its = vcSiteInfoAll.begin(); its != vcSiteInfoAll.end(); its ++ )
			{
				//pstSite = (STSITEINFO*)its;
				DeleteScriptMaps( (*its).wszMetaPath, _T("IIsWebVirtualDir"), _T("root"), lpcszExtensionFile );
			}
		}
		
		bSaveScriptMaps = TRUE;
	}

	//
	//	ж�ء�Web������չ��
	//
	VariantInit( &vArrWebSvcExt );
	if ( MetaGetData( CDEADSIOPIIS_WCHR_METAPATH_LOCALHOST, _T("IIsWebService"), _T("W3SVC"), _T("WebSvcExtRestrictionList"), vArrWebSvcExt ) )
	{
		vcWebSvcExt.clear();
		if ( ParseWebSvcExtRestrictionList( vArrWebSvcExt, vcWebSvcExt ) )
		{
			//
			//	����б��д��ڣ���ô�ʹ��б���ɾ�����ļ�
			//
			if ( IsFilenameExistInWebSvcExtRestrictionList( vcWebSvcExt, lpcszExtensionFile ) )
			{
				if ( DeleteFilenameFromWebSvcExtRestrictionList( vcWebSvcExt, lpcszExtensionFile ) )
				{
					//
					//	�����µ��б�
					//
					if ( BuildWebSvcExtRestrictionList( vcWebSvcExt, vArrWebSvcExtNew ) )
					{
						//
						//	���浽 Metabase
						//
						bSaveWebSvcExt = MetaSetData
						(
							CDEADSIOPIIS_WCHR_METAPATH_LOCALHOST,
							_T("IIsWebService"),
							_T("W3SVC"),
							_T("WebSvcExtRestrictionList"),
							vArrWebSvcExtNew
						);
					}
				}
			}
			else
			{
				bSaveWebSvcExt = TRUE;
			}
		}
	}

	return ( bSaveScriptMaps && bSaveWebSvcExt );
}

BOOL CDeAdsiOpIIS::IsExtensionInstalled( LPCTSTR lpcszExtensionName, LPCTSTR lpcszExtensionFile )
{
	//
	//	lpcszExtensionName	- [in] ��չ�����ƣ����������Ӣ��
	//	lpcszExtensionFile	- [in] ��չ�����ļ�ȫ·��
	//	RETURN			- TRUE / FALSE
	//

	if ( NULL == lpcszExtensionName || 0 == _tcslen(lpcszExtensionName) )
	{
		return FALSE;
	}
	if ( NULL == lpcszExtensionFile || 0 == _tcslen(lpcszExtensionFile) )
	{
		return FALSE;
	}
	if ( ! PathFileExists( lpcszExtensionFile ) )
	{
		return FALSE;
	}

	BOOL bScriptMapsExist;
	BOOL bWebSvcExtExist;

	VARIANT vArrScriptMaps;
	vector<STMETASCRIPTMAP> vcScriptMaps;

	VARIANT vArrWebSvcExt;
	vector<STMETAWEBSVCEXTRESTRICTIONLIST> vcWebSvcExt;


	//
	//	��ʼ��
	//
	bScriptMapsExist	= FALSE;
	bWebSvcExtExist		= FALSE;

	//
	//	��ʼ���
	//
	VariantInit( &vArrScriptMaps );
	if ( MetaGetData( CDEADSIOPIIS_WCHR_METAPATH_LOCALHOST, _T("IIsWebService"), _T("W3SVC"), _T("ScriptMaps"), vArrScriptMaps ) )
	{
		vcScriptMaps.clear();
		if ( ParseScriptMaps( vArrScriptMaps, vcScriptMaps ) )
		{
			//
			//	������� map �ļ��Ƿ����
			//
			bScriptMapsExist = IsFilenameExistInScriptMaps( vcScriptMaps, lpcszExtensionFile );
		}
		vcScriptMaps.clear();
	}
	VariantClear( &vArrScriptMaps );

	VariantInit( &vArrWebSvcExt );
	if ( MetaGetData( CDEADSIOPIIS_WCHR_METAPATH_LOCALHOST, _T("IIsWebService"), _T("W3SVC"), _T("WebSvcExtRestrictionList"), vArrWebSvcExt ) )
	{
		vcWebSvcExt.clear();
		if ( ParseWebSvcExtRestrictionList( vArrWebSvcExt, vcWebSvcExt ) )
		{
			//
			//	������� map �ļ��Ƿ����
			//
			bWebSvcExtExist = IsFilenameExistInWebSvcExtRestrictionList( vcWebSvcExt, lpcszExtensionFile );
		}
		vcWebSvcExt.clear();
	}
	VariantClear( &vArrWebSvcExt );

	return ( bScriptMapsExist && bWebSvcExtExist );
}





//////////////////////////////////////////////////////////////////////////
//	Private








//////////////////////////////////////////////////////////////////////////
// Function:
//	Find Metainfo by lpcszFilterDesc
//
BOOL CDeAdsiOpIIS::FindFilterMetaInfo( LPCTSTR lpcszFilterPathIn,
				    LPTSTR lpszFilterName, DWORD dwFnSize,
				    LPTSTR lpszFilterParentPath, DWORD dwFpSize,
				    LPTSTR lpszFilterFilePath, DWORD dwFfpSize )
{
	//
	//	lpcszFilterPathIn	- [in]  �������ļ�ȫ·��
	//	lpszFilterName		- [out] Pointer to a character null-terminated string containing the Filter name
	//	dwFnSize		- [in]  Specifies the length of the lpszFilterName buffer
	//	lpszFilterParentPath	- [out] Pointer to a character null-terminated string containing the Filter parent path
	//	dwFpSize		- [in]  Specifies the length of the lpszFilterParentPath buffer
	//	lpszFilterFilePath	- [out] Pointer to a character null-terminated string containing the Filter file path
	//	dwFfpSize		- [in]  Specifies the length of the lpszFilterFilePath buffer
	//	RETURN			- TRUE / FALSE
	//


	if ( NULL == lpcszFilterPathIn )
	{
		return FALSE;
	}
	if ( 0 == _tcslen(lpcszFilterPathIn) )
	{
		return FALSE;
	}

	BOOL bFind	= FALSE;
	CComPtr<IADsContainer> iContainer;
	IEnumVARIANT * pEnum = NULL;
	IADs * pADs;
	VARIANT vtVar;
	ULONG ulFetched	= 0L;
	CComBSTR bstrValue;

	CComPtr<IADs> iAds;
	VARIANT vLoadOrder;
	TCHAR szLoadOrder[ 1024 ]		= {0};

	CComPtr<IADs> iAdsSpec;
	VARIANT vtFilterDesc;
	VARIANT vtFilterFilePath;
	TCHAR szFilterFilePath[ MAX_PATH ]	= {0};
	TCHAR szFilterName[ MAX_PATH ]		= {0};
	TCHAR szBuffer[ MAX_PATH ]		= {0};

	TCHAR szSiteId[ MAX_PATH ]	= {0};
	TCHAR szFindFilterPath[ MAX_PATH ]	= {0};
	WCHAR wszFindFilterPath[ MAX_PATH ]	= {0};
	CComPtr<IADsContainer> iContainerSub;
	IEnumVARIANT * pEnumSub = NULL;
	IADs * pADsSub;
	VARIANT vtVarSub;


	//	�ȶ�ȡȫ��վ��� Filter �� LoadOrder �б�
	memset( szLoadOrder, 0, sizeof(szLoadOrder) );
	VariantClear( &vLoadOrder );
	if ( MetaGetData( L"IIS://localhost/w3svc", _T("IIsFilters"), _T("Filters"), _T("FilterLoadOrder"), vLoadOrder ) )
	{
		WideCharToMultiByte( CP_ACP, 0, vLoadOrder.bstrVal, (int)wcslen(vLoadOrder.bstrVal), szLoadOrder, sizeof(szLoadOrder), NULL, NULL );
	}
	VariantClear( &vLoadOrder );


	// [1] �ȼ��ȫ�ֵ� IIsFilters ���Ƿ���
	if ( S_OK == ADsGetObject( L"IIS://localhost/w3svc/Filters", IID_IADsContainer, (void**)&iContainer ) )
	{
		//	Create an enumerator object in the container.
		if ( S_OK == ADsBuildEnumerator( iContainer, &pEnum ) )
		{
			// Get the next contained object.
			while( S_OK == ADsEnumerateNext( pEnum, 1, &vtVar, &ulFetched ) && ulFetched > 0 )
			{
				if ( S_OK == V_DISPATCH(&vtVar)->QueryInterface( IID_IADs, (void**)&pADs ) )
				{
					if ( S_OK == pADs->get_Name(&bstrValue) && wcslen(bstrValue) > 0 )
					{
						memset( szFilterName, 0, sizeof(szFilterName) );
						WideCharToMultiByte( CP_ACP, 0, bstrValue, (int)wcslen(bstrValue), szFilterName, sizeof(szFilterName), NULL, NULL );
 						if ( S_OK == iContainer->GetObject( _bstr_t("IIsFilter"), bstrValue, (IDispatch**)&iAdsSpec ) )
						{
							iAdsSpec->Get( _bstr_t("FilterPath"), &vtFilterFilePath );
							memset( szFilterFilePath, 0, sizeof(szFilterFilePath) );
							WideCharToMultiByte( CP_ACP, 0, vtFilterFilePath.bstrVal, (int)wcslen(vtFilterFilePath.bstrVal), szFilterFilePath, sizeof(szFilterFilePath), NULL, NULL );
							//	..
							iAdsSpec->Get( _bstr_t("FilterDescription"), &vtFilterDesc );
							memset( szBuffer, 0, sizeof(szBuffer) );
							WideCharToMultiByte( CP_ACP, 0, vtFilterDesc.bstrVal, (int)wcslen(vtFilterDesc.bstrVal), szBuffer, sizeof(szBuffer), NULL, NULL );
							
							// ############################################################
							// �Ƚ� �ļ��� �㷨��ֻҪ�ҵ��ļ�����ͬ�ļ���
							if ( _tcslen(szFilterFilePath) &&
								0 == _tcsicmp( PathFindFileName(szFilterFilePath), PathFindFileName(lpcszFilterPathIn) ) &&
								strstr( szLoadOrder, szFilterName ) )
							{
								if ( lpszFilterParentPath && dwFpSize )
								{
									_sntprintf( lpszFilterParentPath, dwFpSize/sizeof(TCHAR)-1, _T("IIS://localhost/w3svc") );
								}
								if ( lpszFilterName && dwFnSize )
								{
									_sntprintf( lpszFilterName, dwFnSize/sizeof(TCHAR)-1, _T("%s"), szFilterName );
								}
								if ( lpszFilterFilePath && dwFfpSize )
								{
									_sntprintf( lpszFilterFilePath, dwFfpSize/sizeof(TCHAR)-1, _T("%s"), szFilterFilePath );
								}

								bFind = TRUE;
							}

							VariantClear( &vtFilterFilePath );
							VariantClear( &vtFilterDesc );
							
							iAdsSpec.Release();

							//SysFreeString( bstrValue );
							//bstrValue.Detach();
						}

						//SysFreeString( bstrValue );
						//bstrValue.Detach();
					}

					pADs->Release();
				}

				// Release the VARIANT.
				VariantClear( &vtVar );

				if ( bFind )
				{
					break;
				}
			}

			// Free Enumer
			ADsFreeEnumerator( pEnum );
			pEnum = NULL;
		}

		iContainer.Release();
	}


	// [2] ���ÿ��վ��� Filter
	if ( ! bFind )
	{
		if ( S_OK == ADsGetObject( L"IIS://localhost/w3svc", IID_IADsContainer, (void**)&iContainer ) )
		{
			// Create an enumerator object in the container.
			if ( S_OK == ADsBuildEnumerator( iContainer, &pEnum ) )
			{
				// Get the next contained object.
				while( S_OK == ADsEnumerateNext( pEnum, 1, &vtVar, &ulFetched ) && ulFetched > 0 )
				{
					if ( S_OK == V_DISPATCH(&vtVar)->QueryInterface( IID_IADs, (void**)&pADs ) )
					{
						if ( S_OK == pADs->get_Name(&bstrValue) && wcslen(bstrValue) > 0 &&
							0 != _wcsicmp(bstrValue,L"Filters") && 0 != _wcsicmp(bstrValue,L"Info") )
						{
							memset( szSiteId, 0, sizeof(szSiteId) );
							WideCharToMultiByte( CP_ACP, 0, bstrValue, (int)wcslen(bstrValue), szSiteId, sizeof(szSiteId), NULL, NULL );

							_sntprintf( szFindFilterPath, sizeof(szFindFilterPath)/sizeof(TCHAR)-1, _T("IIS://localhost/w3svc/%s"), szSiteId );
							MultiByteToWideChar( CP_ACP, 0, szFindFilterPath, (int)_tcslen(szFindFilterPath)+sizeof(TCHAR), wszFindFilterPath, sizeof(wszFindFilterPath)/sizeof(wszFindFilterPath[0]) );

							//	�ȶ�ȡ��վ��� LoadOrder �б�
							memset( szLoadOrder, 0, sizeof(szLoadOrder) );
							if ( S_OK == ADsGetObject( wszFindFilterPath, IID_IADsContainer, (void**)&iContainerSub ) )
							{
								if ( S_OK == iContainerSub->GetObject( _bstr_t("IIsFilters"), _bstr_t("Filters"),(IDispatch**)&iAds ) )
								{
									iAds->Get( _bstr_t("FilterLoadOrder"), &vLoadOrder );
									WideCharToMultiByte( CP_ACP, 0, vLoadOrder.bstrVal, (int)wcslen(vLoadOrder.bstrVal), szLoadOrder, sizeof(szLoadOrder), NULL, NULL );
									//	..
									VariantClear( &vLoadOrder );
									iAds.Release();
								}
								iContainerSub.Release();
							}

							_sntprintf( szFindFilterPath, sizeof(szFindFilterPath)/sizeof(TCHAR)-1, _T("IIS://localhost/w3svc/%s/Filters"), szSiteId );
							MultiByteToWideChar( CP_ACP, 0, szFindFilterPath, (int)_tcslen(szFindFilterPath)+sizeof(TCHAR), wszFindFilterPath, sizeof(wszFindFilterPath)/sizeof(wszFindFilterPath[0]) );							

							//////////////////////////////////////////////////////////////////////////
							//	��ȡ���վ���µ����� Filter
							if ( S_OK == ADsGetObject( wszFindFilterPath, IID_IADsContainer, (void**)&iContainerSub ) )
							{
								//	Create an enumerator object in the container.
								if ( S_OK == ADsBuildEnumerator( iContainerSub, &pEnumSub ) )
								{
									//	Get the next contained object.
									while( S_OK == ADsEnumerateNext( pEnumSub, 1, &vtVarSub, &ulFetched ) && ulFetched > 0 )
									{
										if ( S_OK == V_DISPATCH(&vtVarSub)->QueryInterface( IID_IADs, (void**)&pADsSub ) )
										{
											if ( S_OK == pADsSub->get_Name(&bstrValue) && wcslen(bstrValue) > 0 )
											{
												// ..
												memset( szFilterName, 0, sizeof(szFilterName) );
												WideCharToMultiByte( CP_ACP, 0, bstrValue, (int)wcslen(bstrValue), szFilterName, sizeof(szFilterName), NULL, NULL );
												if ( S_OK == iContainerSub->GetObject( _bstr_t("IIsFilter"), bstrValue, (IDispatch**)&iAdsSpec ) )
												{
													iAdsSpec->Get( _bstr_t("FilterPath"), &vtFilterFilePath );
													memset( szFilterFilePath, 0, sizeof(szFilterFilePath) );
													WideCharToMultiByte( CP_ACP, 0, vtFilterFilePath.bstrVal, (int)wcslen(vtFilterFilePath.bstrVal), szFilterFilePath, sizeof(szFilterFilePath), NULL, NULL );
													//	..													
													iAdsSpec->Get( _bstr_t("FilterDescription"), &vtFilterDesc );
													memset( szBuffer, 0, sizeof(szBuffer) );
													WideCharToMultiByte( CP_ACP, 0, vtFilterDesc.bstrVal, (int)wcslen(vtFilterDesc.bstrVal), szBuffer, sizeof(szBuffer), NULL, NULL );
													
													// ############################################################
													//	�Ƚ� �ļ��� �㷨��ֻҪ�ҵ��ļ�����ͬ�ļ���
													if ( _tcslen(szFilterFilePath) &&
														0 == _tcsicmp( PathFindFileName(szFilterFilePath), PathFindFileName(lpcszFilterPathIn) ) &&
														strstr( szLoadOrder, szFilterName ) )
													{
														if ( lpszFilterParentPath && dwFpSize )
														{	
															_sntprintf( lpszFilterParentPath, dwFpSize/sizeof(TCHAR)-1, "IIS://localhost/w3svc/%s", szSiteId );
														}	
														if ( lpszFilterName && dwFnSize )
														{	
															_sntprintf( lpszFilterName, dwFnSize/sizeof(TCHAR)-1, "%s", szFilterName );
														}	
														if ( lpszFilterFilePath && dwFfpSize )
														{	
															_sntprintf( lpszFilterFilePath, dwFfpSize/sizeof(TCHAR)-1, "%s", szFilterFilePath );
														}	

														bFind = TRUE;
													}
													
													VariantClear( &vtFilterFilePath );
													VariantClear( &vtFilterDesc );
													iAdsSpec.Release();

													//SysFreeString( bstrValue );
													//bstrValue.Detach();
												}

												//SysFreeString( bstrValue );
												//bstrValue.Detach();
											}
											pADsSub->Release();
										}
										VariantClear( &vtVarSub );

										if ( bFind )
										{	
											break;
										}
									}

									// Free Enumer
									ADsFreeEnumerator( pEnumSub );
									pEnumSub = NULL;
								}

								iContainerSub.Release();
							}
							//////////////////////////////////////////////////////////////////////////

							//SysFreeString( bstrValue );
							//bstrValue.Detach();
						}

						pADs->Release();
					}

					// Release the VARIANT.
					VariantClear( &vtVar );
				}

				// Free Enumer
				ADsFreeEnumerator( pEnum );
				pEnum = NULL;
			}

			iContainer.Release();
		}
	}

	return bFind;
}




/**
 *	@ private
 *	���� binding ���ұ��浽 vector ��
 */
BOOL CDeAdsiOpIIS::ParseBindingsAndPuts( LPCTSTR lpcszSiteId, VARIANT & vtVarSub, vector<STHOSTINFO> & vcHostInfoAll, BOOL bSpecSite /* = FALSE */ )
{
	if ( NULL == lpcszSiteId )
	{
		return FALSE;
	}
	if ( 0 == strlen(lpcszSiteId) )
	{
		return FALSE;
	}

	STHOSTINFO stHostInfo;
	SAFEARRAY * psa			= NULL;
	VARIANT * varArray		= NULL;
	UINT uDim			= 0;
	LONG lLbound			= 0;
	LONG lRbound			= 0;
	LONG i				= 0;
	TCHAR szBinding[ MAX_PATH ]	= {0};
	TCHAR szTemp[ MAX_PATH ]	= {0};
	LPTSTR lpszMove			= NULL;
	LPTSTR lpszHead			= NULL;


	if ( vtVarSub.vt == ( VT_ARRAY|VT_VARIANT ) )
	{
		psa = vtVarSub.parray;
		if ( S_OK == SafeArrayAccessData( psa, (VOID**)&varArray ) )
		{
			uDim = SafeArrayGetDim( psa );
			if ( 1 == uDim )
			{
				SafeArrayGetLBound( psa, 1, &lLbound );
				SafeArrayGetUBound( psa, 1, &lRbound );

				for ( i = lLbound; i <= lRbound; i++ )
				{
					if ( varArray[i].vt == VT_BSTR )
					{
						memset( & stHostInfo, 0, sizeof(stHostInfo) );
						_sntprintf( stHostInfo.szSiteId, sizeof(stHostInfo.szSiteId)/sizeof(TCHAR)-1, _T("%s"), lpcszSiteId );

						_bstr_t bstmp( varArray[i].bstrVal, true );
						_sntprintf( szBinding, sizeof(szBinding)/sizeof(TCHAR)-1, _T("%s"), (LPCTSTR)bstmp );
						if ( strlen(szBinding) )
						{
							lpszHead = szBinding;
							lpszMove = strstr( lpszHead, ":" );
							if ( lpszMove )
							{
								if ( 0 != strcmp( lpszHead, lpszMove ) )
								{
									//	127.0.0.1:80:xing.com
									memcpy( stHostInfo.szIpAddr, lpszHead, lpszMove-lpszHead );
								}
								else
								{
									//	:80:xing.com
									strcpy( stHostInfo.szIpAddr, "" );
								}

								lpszHead = lpszMove + sizeof(TCHAR);
								lpszMove = strstr( lpszHead, ":" );
								if ( lpszMove )
								{
									memset( szTemp, 0, sizeof(szTemp) );
									memcpy( szTemp, lpszHead, lpszMove-lpszHead );
									stHostInfo.uPort = atol(szTemp);

									lpszHead = lpszMove + sizeof(TCHAR);
									_sntprintf( stHostInfo.szHostName, sizeof(stHostInfo.szHostName)/sizeof(TCHAR)-1, _T("%s"), lpszHead );
									if ( 0 == strlen( stHostInfo.szHostName ) )
									{
										strcpy( stHostInfo.szHostName, "localhost" );
									}

									//	�Ƿ�ָ����
									stHostInfo.lnUse = ( bSpecSite ? 1 : 0 );

									//	..
									vcHostInfoAll.push_back( stHostInfo );
								}
							}
						}
					}
				}
			}

			if ( varArray )
			{
				VariantClear( varArray );
				varArray = NULL;
			}
			SafeArrayUnaccessData( psa );
			psa = NULL;
		}
	}

	return TRUE;
}


/**
 *	@ private
 *	����� ScriptMaps
 */
BOOL CDeAdsiOpIIS::AddOrUpdateScriptMaps(
	LPCWSTR	lpcwszPathName,
	LPCTSTR lpcszKeyType,
	LPCTSTR	lpcszKeyName,
	DWORD	dwFlag,
	LPCTSTR lpcszExtensionName,
	LPCTSTR lpcszExtensionFile,
	LPCTSTR lpcszVerbs )
{
	//
	//
	//	ScriptMaps
	//	<Extension>, <ScriptProcessor>, <Flags>, <IncludedVerbs>
	//	<Flags> is an integer represnting a bitmask with one of the following values:
	//	--------------------------------------------------------------------------------
	//	MD_SCRIPTMAPFLAG_SCRIPT			= 1	The script is allowed to run in directories given Script permission. If this value is not set, then the script can only be executed in directories that are flagged for Execute permission.
	//	MD_SCRIPTMAPFLAG_CHECK_PATH_INFO	= 4	The server attempts to access the PATH_INFO portion of the URL, as a file, before starting the scripting engine. If the file can't be opened, or doesn't exist, an error is returned to the client.
	//	MD_SCRIPTMAPFLAG_SCRIPT |
	//	MD_SCRIPTMAPFLAG_CHECK_PATH_INFO	= 5	Both of the above conditions are TRUE.
	//

	if ( NULL == lpcwszPathName || 0 == wcslen(lpcwszPathName) )
	{
		return FALSE;
	}
	if ( NULL == lpcszKeyType || 0 == _tcslen(lpcszKeyType) )
	{
		return FALSE;
	}
	if ( NULL == lpcszKeyName || 0 == _tcslen(lpcszKeyName) )
	{
		return FALSE;
	}
	if ( NULL == lpcszExtensionName || 0 == _tcslen(lpcszExtensionName) )
	{
		return FALSE;
	}
	if ( NULL == lpcszExtensionFile || 0 == _tcslen(lpcszExtensionFile) )
	{
		return FALSE;
	}

	BOOL bRet		= FALSE;
	BOOL bReadScriptMaps	= FALSE;
	VARIANT vArrScriptMaps;
	VARIANT vArrScriptMapsNew;
	vector<STMETASCRIPTMAP> vcScriptMaps;
	STMETASCRIPTMAP stMapNew;

	//
	//	��װ����ͨ���Ӧ�ó���ӳ�䡱
	//
	VariantInit( &vArrScriptMaps );
	bReadScriptMaps = MetaGetData( lpcwszPathName, lpcszKeyType, lpcszKeyName, _T("ScriptMaps"), vArrScriptMaps );
	if ( ( VT_ARRAY|VT_VARIANT ) == vArrScriptMaps.vt )
	{
		bReadScriptMaps = TRUE;
	}
	else
	{
		if ( 0 != _wcsnicmp( lpcwszPathName, CDEADSIOPIIS_WCHR_METAPATH_LOCALHOST, wcslen(CDEADSIOPIIS_WCHR_METAPATH_LOCALHOST) ) )
		{
			//	������Ƕ�ȡ��Ŀ¼��ȫ��վ
			//	�򣬴�ȫ��վ��ȡ�������Ƹ���վ��
			VariantInit( &vArrScriptMaps );
			bReadScriptMaps = MetaGetData( CDEADSIOPIIS_WCHR_METAPATH_LOCALHOST, lpcszKeyType, _T("W3SVC"), _T("ScriptMaps"), vArrScriptMaps );
		}
	}

	if ( bReadScriptMaps )
	{
		vcScriptMaps.clear();
		if ( ParseScriptMaps( vArrScriptMaps, vcScriptMaps ) )
		{
			//
			//	��� map �Ѿ����ڣ���ɵ���(��Ϊ�п���·������ͬ)
			//
			if ( IsFilenameExistInScriptMaps( vcScriptMaps, lpcszExtensionFile ) )
			{
				DeleteFilenameFromScriptMaps( vcScriptMaps, lpcszExtensionFile );
			}
			
			memset( & stMapNew, 0, sizeof(stMapNew) );

			stMapNew.dwFlag	= dwFlag;
			_sntprintf( stMapNew.szExtension, sizeof(stMapNew.szExtension)/sizeof(TCHAR)-1, _T("%s"), lpcszExtensionName );
			_sntprintf( stMapNew.szFilename, sizeof(stMapNew.szFilename)/sizeof(TCHAR)-1, _T("%s"), lpcszExtensionFile );
			_sntprintf( stMapNew.szVerbs, sizeof(stMapNew.szVerbs)/sizeof(TCHAR)-1, _T("%s"), lpcszVerbs );
			
			vcScriptMaps.push_back( stMapNew );

			if ( BuildScriptMaps( vcScriptMaps, vArrScriptMapsNew ) )
			{
				//
				//	���浽 MetaBase
				//
				bRet = MetaSetData( lpcwszPathName, lpcszKeyType, lpcszKeyName, _T("ScriptMaps"), vArrScriptMapsNew );
			}
		}
	}

	return bRet;
}

/**
 *	@ private
 *	ɾ�� ScriptMaps
 */
BOOL CDeAdsiOpIIS::DeleteScriptMaps( LPCWSTR lpcwszPathName, LPCTSTR lpcszKeyType, LPCTSTR lpcszKeyName, LPCTSTR lpcszExtensionFile )
{
	if ( NULL == lpcwszPathName || 0 == wcslen(lpcwszPathName) )
	{
		return FALSE;
	}
	if ( NULL == lpcszKeyType || 0 == _tcslen(lpcszKeyType) )
	{
		return FALSE;
	}
	if ( NULL == lpcszKeyName || 0 == _tcslen(lpcszKeyName) )
	{
		return FALSE;
	}
	if ( NULL == lpcszExtensionFile || 0 == _tcslen(lpcszExtensionFile) )
	{
		return FALSE;
	}

	BOOL bRet	= FALSE;
	VARIANT vArrScriptMaps;
	VARIANT vArrScriptMapsNew;
	vector<STMETASCRIPTMAP> vcScriptMaps;
	STMETASCRIPTMAP stMapNew;

	//
	//	ж������ ScriptMaps
	//
	VariantInit( &vArrScriptMaps );
	if ( MetaGetData( lpcwszPathName, lpcszKeyType, lpcszKeyName, _T("ScriptMaps"), vArrScriptMaps ) )
	{
		vcScriptMaps.clear();
		if ( ParseScriptMaps( vArrScriptMaps, vcScriptMaps ) )
		{
			//
			//	����б��д��ڣ���ô�ʹ��б���ɾ�����ļ�
			//
			if ( IsFilenameExistInScriptMaps( vcScriptMaps, lpcszExtensionFile ) )
			{
				if ( DeleteFilenameFromScriptMaps( vcScriptMaps, lpcszExtensionFile ) )
				{
					//
					//	�����µ��б�
					//
					if ( BuildScriptMaps( vcScriptMaps, vArrScriptMapsNew ) )
					{
						//
						//	���浽 MetaBase
						//
						bRet = MetaSetData( lpcwszPathName, lpcszKeyType, lpcszKeyName, _T("ScriptMaps"), vArrScriptMapsNew );
					}
				}
			}
			else
			{
				bRet = TRUE;
			}
		}
	}

	return bRet;
}

/**
 *	@ private
 *	���� ScriptMaps
 */
BOOL CDeAdsiOpIIS::ParseScriptMaps( VARIANT & vtVarData, vector<STMETASCRIPTMAP> & vcScriptMaps )
{
	STMETASCRIPTMAP stMap;
	SAFEARRAY * psa			= NULL;
	VARIANT * varArray		= NULL;
	UINT uDim			= 0;
	LONG lLbound			= 0;
	LONG lRbound			= 0;
	LONG i				= 0;
	TCHAR szTemp[ MAX_PATH ]	= {0};
	
	BOOL bParseSucc;
	STTEXTLINE * pstParseStrList	= NULL;
	INT nParseStrCount;
	INT nPsi;
	INT nVerbsLen;
	

	if ( vtVarData.vt == ( VT_ARRAY|VT_VARIANT ) )
	{
		psa = vtVarData.parray;
		if ( S_OK == SafeArrayAccessData( psa,(VOID**)&varArray ) )
		{
			uDim = SafeArrayGetDim( psa );
			if ( 1 == uDim )
			{
				SafeArrayGetLBound( psa, 1, &lLbound );
				SafeArrayGetUBound( psa, 1, &lRbound );

				for ( i = lLbound; i <= lRbound; i++ )
				{
					if ( VT_BSTR != varArray[i].vt )
					{
						continue;
					}

					memset( &stMap, 0, sizeof(stMap) );
					bParseSucc	= FALSE;

					_bstr_t bstmp( varArray[i].bstrVal, true );
					_sntprintf( stMap.szLine, sizeof(stMap.szLine)/sizeof(TCHAR)-1, _T("%s"), (LPCTSTR)bstmp );
					if ( _tcslen(stMap.szLine) )
					{
						//
						//	�����ַ���
						//
						nParseStrCount = delib_split_string( stMap.szLine, _T(","), NULL, FALSE );
						if ( nParseStrCount >= 3 )
						{
							pstParseStrList = new STTEXTLINE[ nParseStrCount ];
							if ( pstParseStrList )
							{
								//
								//	������Ϊ����һ�� map ���ݳɹ�
								//
								bParseSucc = TRUE;


								memset( pstParseStrList, 0, sizeof(STTEXTLINE)*nParseStrCount );
								delib_split_string( stMap.szLine, _T(","), pstParseStrList, FALSE );

								//	��ȡ extension ����
								_sntprintf( stMap.szExtension, sizeof(stMap.szExtension)/sizeof(TCHAR)-1, _T("%s"), pstParseStrList[ 0 ].szLine );

								//	��ȡ�ļ�·��
								_sntprintf( stMap.szFilename, sizeof(stMap.szFilename)/sizeof(TCHAR)-1, _T("%s"), pstParseStrList[ 1 ].szLine );

								//	��ȡ ��� ����
								stMap.dwFlag	= atoi( pstParseStrList[ 2 ].szLine );

								//	��ȡ szVerbs
								if ( nParseStrCount > 3 )
								{
									nVerbsLen = 0;
									for ( nPsi = 3; nPsi < nParseStrCount; nPsi ++ )
									{
										nVerbsLen += (INT)( _tcslen(pstParseStrList[ nPsi ].szLine) + sizeof(TCHAR) );
									}
									if ( nVerbsLen < sizeof(stMap.szVerbs) )
									{
										for ( nPsi = 3; nPsi < nParseStrCount; nPsi ++ )
										{
											_tcscat( stMap.szVerbs, pstParseStrList[ nPsi ].szLine );
											if ( nPsi != ( nParseStrCount - 1 ) )
											{
												_tcscat( stMap.szVerbs, _T(",") );
											}
										}
									}
								}

								delete [] pstParseStrList;
								pstParseStrList = NULL;
							}
						}

					}

					//
					//	�������ݵ� vector
					//
					if ( bParseSucc )
					{
						vcScriptMaps.push_back( stMap );
					}
				}
			}

			if ( varArray )
			{
				VariantClear( varArray );
				varArray = NULL;
			}
			SafeArrayUnaccessData( psa );
			psa = NULL;
		}
	}
	
	return TRUE;
}


/**
 *	@ private
 *	�������鴴�� ScriptMaps
 */
BOOL CDeAdsiOpIIS::BuildScriptMaps( vector<STMETASCRIPTMAP> & vcScriptMaps, VARIANT & vtVarData )
{
	if ( 0 == vcScriptMaps.size() )
	{
		return FALSE;
	}

	BOOL bRet	= FALSE;
	//STMETASCRIPTMAP * pstMap;
//	HRESULT hr;
	SAFEARRAY * psa;
	SAFEARRAYBOUND aDim[1];			//	a one dimensional array
	vector<STMETASCRIPTMAP>::iterator it;
	LONG ArrLong[ 1 ];
	LONG lnIndex;
	LONG lnEnd;

	//
	//	Create SafeArray of VARIANT BSTRs
	//
	aDim[0].lLbound		= 0;  // Visual Basic arrays start with index 0
	aDim[0].cElements	= (ULONG)( vcScriptMaps.size() );

	psa = SafeArrayCreate( VT_VARIANT, 1, aDim );	//	create a 1D SafeArray of VARIANTS
	if ( psa )
	{
		lnIndex	= aDim[0].lLbound;
		lnEnd	= aDim[0].lLbound + aDim[0].cElements;

		for ( it = vcScriptMaps.begin(); it != vcScriptMaps.end(); it ++ )
		{
			if ( lnIndex >= lnEnd )
			{
				break;
			}

			//pstMap = (STMETASCRIPTMAP*)it;
			//if ( (*it) )
			{
				_sntprintf
				(
					(*it).szLine,
					sizeof((*it).szLine)/sizeof(TCHAR)-1,
					_T("%s,%s,%d%s%s"),
					(*it).szExtension,
					(*it).szFilename,
					(*it).dwFlag,
					( _tcslen((*it).szVerbs) ? "," : "" ),
					(*it).szVerbs
				);

				VARIANT vOut;
				BSTR  bstrTmp	= A2BSTR( _T("") );

				VariantInit(&vOut);

				bstrTmp	= A2BSTR( _T((*it).szLine) );
				vOut.vt = VT_BSTR;
				vOut.bstrVal = bstrTmp;

				//	��������ֵ
				ArrLong[0]	= lnIndex;

				if ( S_OK == SafeArrayPutElement( psa, ArrLong, &vOut ) )
				{
					bRet = TRUE;
				}

				VariantClear( &vOut );
				// does a deep destroy of source VARIANT
			}

			//
			//	��������
			//
			lnIndex ++;
		}

		vtVarData.vt		= ( VT_ARRAY|VT_VARIANT );
		vtVarData.parray	= psa;
	}

	//
	//	clean up here only if you do not return SafeArray as an [out, retval]
	//
	//SafeArrayDestroy( psa );	// again does a deep destroy
	
	return bRet;
}

/**
 *	@ private
 *	����ļ����Ƿ������ ScriptMap ��
 */
BOOL CDeAdsiOpIIS::IsFilenameExistInScriptMaps( vector<STMETASCRIPTMAP> & vcScriptMaps, LPCTSTR lpcszExtensionFile )
{
	if ( NULL == lpcszExtensionFile )
	{
		return FALSE;
	}

	BOOL bRet	= FALSE;
	vector<STMETASCRIPTMAP>::iterator it;
	//STMETASCRIPTMAP * pstMap;

	bRet	= FALSE;
	for ( it = vcScriptMaps.begin(); it != vcScriptMaps.end(); it ++ )
	{
		//pstMap = it;
		//if ( (*it) )
		{
			if ( 0 == _tcsicmp( PathFindFileName((*it).szFilename), PathFindFileName(lpcszExtensionFile) ) )
			{
				bRet = TRUE;
				break;
			}
		}
	}

	return bRet;
}

/**
 *	@ private
 *	�� ScriptMap ��ɾ��ָ���ļ���
 */
BOOL CDeAdsiOpIIS::DeleteFilenameFromScriptMaps( vector<STMETASCRIPTMAP> & vcScriptMaps, LPCTSTR lpcszExtensionFile )
{
	if ( NULL == lpcszExtensionFile )
	{
		return FALSE;
	}
	
	BOOL bRet	= FALSE;
	vector<STMETASCRIPTMAP>::iterator it;
	//STMETASCRIPTMAP * pstMap;

	bRet	= FALSE;
	for ( it = vcScriptMaps.begin(); it != vcScriptMaps.end(); it ++ )
	{
		//pstMap = it;
		//if ( (*it) )
		{
			if ( 0 == _tcsicmp( PathFindFileName((*it).szFilename), PathFindFileName(lpcszExtensionFile) ) )
			{
				//	�� vector ��ɾ��
				vcScriptMaps.erase( it );

				bRet = TRUE;
				break;
			}
		}
	}

	return bRet;
}


/**
 *	@ private
 *	���� WebSvcExtRestrictionList
 */
BOOL CDeAdsiOpIIS::ParseWebSvcExtRestrictionList( VARIANT & vtVarData, vector<STMETAWEBSVCEXTRESTRICTIONLIST> & vcWebSvcExt )
{
	//
	//	vtVarData	- [in]
	//	vcWebSvcExt	- [out] vector ��������
	//	RETURN
	//
	//	WebSvcExtRestrictionList ��ʽ:
	//		AllowDenyFlag,ExtensionPath,UIDeletableFlag,GroupID,ApplicationName
	//	����:
	//		0,*.exe
	//		0,*.dll
	//		1,C:\WINDOWS\system32\inetsrv\ssinc.dll,0,SSINC,�ڷ������˵İ����ļ�
	//		0,C:\WINDOWS\system32\inetsrv\httpext.dll,0,WEBDAV,WebDAV
	//		1,C:\Inetpub\wwwroot\DeInfoMFilter\DeInfoMFilter.dll,1,,DeInfoMFilter
	//

	STMETAWEBSVCEXTRESTRICTIONLIST stWebSvcExt;
	SAFEARRAY * psa			= NULL;
	VARIANT * varArray		= NULL;
	UINT uDim			= 0;
	LONG lLbound			= 0;
	LONG lRbound			= 0;
	LONG i				= 0;
	TCHAR szTemp[ MAX_PATH ]	= {0};

	BOOL bParseSucc;
	STTEXTLINE * pstParseStrList	= NULL;
	INT nParseStrCount;


	if ( vtVarData.vt == ( VT_ARRAY|VT_VARIANT ) )
	{
		psa = vtVarData.parray;
		if ( S_OK == SafeArrayAccessData( psa,(VOID**)&varArray ) )
		{
			uDim = SafeArrayGetDim( psa );
			if ( 1 == uDim )
			{
				SafeArrayGetLBound( psa, 1, &lLbound );
				SafeArrayGetUBound( psa, 1, &lRbound );

				for ( i = lLbound; i <= lRbound; i++ )
				{
					if ( VT_BSTR != varArray[i].vt )
					{
						continue;
					}

					memset( &stWebSvcExt, 0, sizeof(stWebSvcExt) );
					bParseSucc	= FALSE;

					_bstr_t bstmp( varArray[i].bstrVal, true );
					_sntprintf( stWebSvcExt.szLine, sizeof(stWebSvcExt.szLine)/sizeof(TCHAR)-1, _T("%s"), (LPCTSTR)bstmp );
					if ( _tcslen(stWebSvcExt.szLine) )
					{
						//
						//	�����ַ���
						//
						nParseStrCount = delib_split_string( stWebSvcExt.szLine, _T(","), NULL, TRUE );
						if ( nParseStrCount >= 2 )
						{
							pstParseStrList = new STTEXTLINE[ nParseStrCount ];
							if ( pstParseStrList )
							{
								//
								//	������Ϊ����һ�� map ���ݳɹ�
								//
								bParseSucc = TRUE;


								memset( pstParseStrList, 0, sizeof(STTEXTLINE)*nParseStrCount );
								delib_split_string( stWebSvcExt.szLine, _T(","), pstParseStrList, TRUE );

								stWebSvcExt.dwAllowDenyFlag = atoi( pstParseStrList[ 0 ].szLine );
								_sntprintf( stWebSvcExt.szExtensionPath, sizeof(stWebSvcExt.szExtensionPath)/sizeof(TCHAR)-1, _T("%s"), pstParseStrList[ 1 ].szLine );
								if ( nParseStrCount > 2 )
								{
									stWebSvcExt.dwUIDeletableFlag = atoi( pstParseStrList[ 2 ].szLine );
								}
								if ( nParseStrCount > 3 )
								{
									_sntprintf( stWebSvcExt.szGroupID, sizeof(stWebSvcExt.szGroupID)/sizeof(TCHAR)-1, _T("%s"), pstParseStrList[ 3 ].szLine );
								}
								if ( nParseStrCount > 4 )
								{
									_sntprintf( stWebSvcExt.szApplicationName, sizeof(stWebSvcExt.szApplicationName)/sizeof(TCHAR)-1, _T("%s"), pstParseStrList[ 4 ].szLine );
								}

								delete [] pstParseStrList;
								pstParseStrList = NULL;
							}
						}

					}

					//
					//	�������ݵ� vector
					//
					if ( bParseSucc )
					{
						vcWebSvcExt.push_back( stWebSvcExt );
					}
				}
			}

			if ( varArray )
			{
				VariantClear( varArray );
				varArray = NULL;
			}
			SafeArrayUnaccessData( psa );
			psa = NULL;
		}
	}
	
	return TRUE;
}

/**
 *	@ private
 *	�����µ� WebSvcExtRestrictionList
 */
BOOL CDeAdsiOpIIS::BuildWebSvcExtRestrictionList( vector<STMETAWEBSVCEXTRESTRICTIONLIST> & vcWebSvcExt, VARIANT & vtVarData )
{
	//
	//
	//
	//	WebSvcExtRestrictionList ��ʽ:
	//		AllowDenyFlag,ExtensionPath,UIDeletableFlag,GroupID,ApplicationName
	//	����:
	//		0,*.exe
	//		0,*.dll
	//		1,C:\WINDOWS\system32\inetsrv\ssinc.dll,0,SSINC,�ڷ������˵İ����ļ�
	//		0,C:\WINDOWS\system32\inetsrv\httpext.dll,0,WEBDAV,WebDAV
	//		1,C:\Inetpub\wwwroot\DeInfoMFilter\DeInfoMFilter.dll,1,,DeInfoMFilter
	//

	if ( 0 == vcWebSvcExt.size() )
	{
		return FALSE;
	}
	
	BOOL bRet	= FALSE;
	//STMETAWEBSVCEXTRESTRICTIONLIST * pstWebSvcExt;
	SAFEARRAY * psa;
	SAFEARRAYBOUND aDim[1];			//	a one dimensional array
	vector<STMETAWEBSVCEXTRESTRICTIONLIST>::iterator it;
	LONG ArrLong[ 1 ];
	LONG lnIndex;
	LONG lnEnd;

	//
	//	Create SafeArray of VARIANT BSTRs
	//
	aDim[0].lLbound		= 0;			//	Visual Basic arrays start with index 0
	aDim[0].cElements	= (ULONG)( vcWebSvcExt.size() );

	psa = SafeArrayCreate( VT_VARIANT, 1, aDim );	//	create a 1D SafeArray of VARIANTS
	if ( psa )
	{
		lnIndex	= aDim[0].lLbound;
		lnEnd	= aDim[0].lLbound + aDim[0].cElements;

		for ( it = vcWebSvcExt.begin(); it != vcWebSvcExt.end(); it ++ )
		{
			if ( lnIndex >= lnEnd )
			{
				break;
			}

			//pstWebSvcExt = it;
			//if ( pstWebSvcExt )
			{
				if ( _tcslen((*it).szApplicationName) )
				{
					//
					//	������
					//
					_sntprintf
					(
						(*it).szLine,
						sizeof((*it).szLine)/sizeof(TCHAR)-1,
						_T("%d,%s,%d,%s,%s"),
						(*it).dwAllowDenyFlag,
						(*it).szExtensionPath,
						(*it).dwUIDeletableFlag,
						(*it).szGroupID,
						(*it).szApplicationName
					);
				}
				else
				{
					//
					//	���ƣ�0,*.exe
					//
					_sntprintf
					(
						(*it).szLine,
						sizeof((*it).szLine)/sizeof(TCHAR)-1,
						_T("%d,%s"),
						(*it).dwAllowDenyFlag,
						(*it).szExtensionPath
					);
				}

				VARIANT vOut;
				BSTR  bstrTmp	= A2BSTR( _T("") );
				
				VariantInit(&vOut);
				
				bstrTmp	= A2BSTR( _T((*it).szLine) );
				vOut.vt = VT_BSTR;
				vOut.bstrVal = bstrTmp;

				//	��������ֵ
				ArrLong[0]	= lnIndex;
				
				if ( S_OK == SafeArrayPutElement( psa, ArrLong, &vOut ) )
				{
					bRet = TRUE;
				}
				
				VariantClear( &vOut );
				// does a deep destroy of source VARIANT
			}
			
			//
			//	��������
			//
			lnIndex ++;
		}
		
		vtVarData.vt		= ( VT_ARRAY|VT_VARIANT );
		vtVarData.parray	= psa;
	}
	
	//
	//	clean up here only if you do not return SafeArray as an [out, retval]
	//
	//SafeArrayDestroy( psa );	// again does a deep destroy
	
	return bRet;
}

/**
 *	@ private
 *	�ж��ļ����� WebSvcExtRestrictionList ���Ƿ����
 */
BOOL CDeAdsiOpIIS::IsFilenameExistInWebSvcExtRestrictionList( vector<STMETAWEBSVCEXTRESTRICTIONLIST> & vcWebSvcExt, LPCTSTR lpcszExtensionFile )
{
	if ( NULL == lpcszExtensionFile )
	{
		return FALSE;
	}
	
	BOOL bRet	= FALSE;
	vector<STMETAWEBSVCEXTRESTRICTIONLIST>::iterator it;
	//STMETAWEBSVCEXTRESTRICTIONLIST * pstWebSvcExt;
	
	bRet	= FALSE;
	for ( it = vcWebSvcExt.begin(); it != vcWebSvcExt.end(); it ++ )
	{
		//pstWebSvcExt = it;
		//if ( pstWebSvcExt )
		{
			if ( 0 == _tcsicmp( PathFindFileName((*it).szExtensionPath), PathFindFileName(lpcszExtensionFile) ) )
			{
				bRet = TRUE;
				break;
			}
		}
	}
	
	return bRet;
}

/**
 *	@ private
 *	�� WebSvcExtRestrictionList ��ɾ��ָ���ļ���
 */
BOOL CDeAdsiOpIIS::DeleteFilenameFromWebSvcExtRestrictionList( vector<STMETAWEBSVCEXTRESTRICTIONLIST> & vcWebSvcExt, LPCTSTR lpcszExtensionFile )
{
	if ( NULL == lpcszExtensionFile )
	{
		return FALSE;
	}
	
	BOOL bRet	= FALSE;
	vector<STMETAWEBSVCEXTRESTRICTIONLIST>::iterator it;
	//STMETAWEBSVCEXTRESTRICTIONLIST * pstWebSvcExt;
	
	bRet	= FALSE;
	for ( it = vcWebSvcExt.begin(); it != vcWebSvcExt.end(); it ++ )
	{
		//pstWebSvcExt = it;
		//if ( pstWebSvcExt )
		{
			if ( 0 == _tcsicmp( PathFindFileName((*it).szExtensionPath), PathFindFileName(lpcszExtensionFile) ) )
			{
				//	�� vector ��ɾ��
				vcWebSvcExt.erase( it );

				bRet = TRUE;
				break;
			}
		}
	}
	
	return bRet;
}





//////////////////////////////////////////////////////////////////////////
// ���ܣ�
//	��ָ���ַ�����һ�����ַ�����ɾ��
// ����
//	lpcszStringInput	���ַ���
//	lpcszSpecStr		���ַ���
//	lpszStringOut		���ص��� LoadOrder ��������ַ
//	dwSize			���ػ�������С
BOOL CDeAdsiOpIIS::DeleteSpecStrFromList( LPCTSTR lpcszStringInput, LPCTSTR lpcszSpecStr, LPCTSTR lpcszSplitStr, LPTSTR lpszStringOut, DWORD dwSize )
{
	/*
		lpcszStringInput	- [in]  Specifies the lpcszStringInput
		lpcszSpecStr		- [in]  Specifies the lpcszSpecStr
		lpcszSplitStr		- [in]  Specifies lpcszSplitStr
		lpszStringOut		- [out] Pointer to a character null-terminated string containing the StringOut 
		dwSize			- [in]  Specifies the length of the lpszStringOut buffer
	*/

	if ( NULL == lpcszStringInput || 0 == _tcslen(lpcszStringInput) )
		return FALSE;
	if ( NULL == lpcszSpecStr || 0 == _tcslen(lpcszSpecStr) )
		return FALSE;
	if ( NULL == lpcszSplitStr || 0 == _tcslen(lpcszSplitStr) )
		return FALSE;
	if ( NULL == lpszStringOut || 0 == dwSize )
		return FALSE;

	// .mdb|.exe|.rar|.zip
	BOOL bRet	= FALSE;
	UINT uIndex	= 0;
	LPCTSTR lpszHead = NULL;
	LPTSTR lpszMove = NULL;
	TCHAR  szTmpExt[ MAX_PATH ] = {0};

	//	Only one Filter, set emtpy and return true
	if ( 0 == stricmp( lpcszStringInput, lpcszSpecStr ) )
	{
		strcpy( lpszStringOut, "" );
		return TRUE;
	}


	//	Init Exts List
	lpszHead = lpcszStringInput;
	lpszMove = NULL;
	while( lpszHead && _tcslen(lpszHead) > 0 )
	{
		_sntprintf( szTmpExt, sizeof(szTmpExt)/sizeof(TCHAR)-1, _T("%s"), lpszHead );
		lpszMove = _tcsstr( szTmpExt, lpcszSplitStr );
		if ( lpszMove )
		{
			*lpszMove = '\0';
			lpszHead += _tcslen(szTmpExt) + _tcslen(lpcszSplitStr);
		}
		else
		{
			lpszHead += _tcslen(szTmpExt);
		}
		
		if ( _tcslen(szTmpExt) > 0 && 0 != _tcsicmp( szTmpExt, lpcszSpecStr ) )
		{
			if ( _tcslen(lpszStringOut)+_tcslen(szTmpExt) < dwSize-sizeof(TCHAR)-1 )
			{
				bRet = TRUE;
				if ( 0 != uIndex )
					_tcscat( lpszStringOut, lpcszSplitStr );
				_tcscat( lpszStringOut, szTmpExt );
				
				uIndex ++;
			}
		}
	}
	return bRet;
}

/**
 *	��ȡָ������ֵ
 */
BOOL CDeAdsiOpIIS::MetaGetData( LPCWSTR lpcwszPathName, LPCTSTR lpcszKeyType, LPCTSTR lpcszKeyName, LPCTSTR lpcszValueName, VARIANT & vtVarSub )
{
	//
	//	lpcwszPathName	- [in] Metabase ��·�������磺L"IIS://localhost"
	//	lpcszKeyType	- [in] Ҫ�򿪵� lpszPathName �����ͣ����磺IIsWebService
	//	lpcszKeyName	- [in] Ҫ�򿪵��� key ���ƣ����磺W3SVC
	//	lpcszValueName	- [in] Ҫ��ȡֵ�ļ������֣����磺ScriptMaps
	//	vtVarSub	- [out] ���
	//	RETURN		- TRUE / FALSE
	//

	if ( NULL == lpcwszPathName || 0 == wcslen(lpcwszPathName) )
	{
		return FALSE;
	}
	if ( NULL == lpcszKeyType || 0 == _tcslen(lpcszKeyType) )
	{
		return FALSE;
	}
	if ( NULL == lpcszKeyName || 0 == _tcslen(lpcszKeyName) )
	{
		return FALSE;
	}
	if ( NULL == lpcszValueName || 0 == _tcslen(lpcszValueName) )
	{
		return FALSE;
	}

	BOOL bRet = FALSE;
	CComPtr<IADsContainer> iContainer;
	CComPtr<IADs> iAds;

	//
	//	��ʼ��װ
	//
	try
	{
		if ( S_OK == ADsGetObject( (LPWSTR)lpcwszPathName, IID_IADsContainer, (void**)&iContainer ) )
		{
			if ( S_OK == iContainer->GetObject(_bstr_t(lpcszKeyType),_bstr_t(lpcszKeyName),(IDispatch**)&iAds) )
			{
				bRet	= TRUE;
				
				//
				//	ScriptMaps
				//
				VariantInit( &vtVarSub );
				if ( S_OK == iAds->Get( _bstr_t(lpcszValueName), &vtVarSub ) )
				{
					bRet = TRUE;
				}
				//VariantClear( &vtVarSub );

				iAds.Release();
			}

			iContainer.Release();
		}
	}
	catch ( ... )
	{
	}

	return bRet;
}


/**
 *	����ָ������ֵ
 */
BOOL CDeAdsiOpIIS::MetaSetData( LPCWSTR lpcwszPathName, LPCTSTR lpcszKeyType, LPCTSTR lpcszKeyName, LPCTSTR lpcszValueName, VARIANT & vtVarSub )
{
	//
	//	lpcwszPathName	- [in] Metabase ��·�������磺L"IIS://localhost"
	//	lpcszKeyType	- [in] Ҫ�򿪵� lpszPathName �����ͣ����磺IIsWebService
	//	lpcszKeyName	- [in] Ҫ�򿪵��� key ���ƣ����磺W3SVC
	//	lpcszValueName	- [in] Ҫ��ȡֵ�ļ������֣����磺ScriptMaps
	//	vtVarSub	- [in] Ҫ�����ֵ
	//	RETURN		- TRUE / FALSE
	//

	if ( NULL == lpcwszPathName || 0 == wcslen(lpcwszPathName) )
	{
		return FALSE;
	}
	if ( NULL == lpcszKeyType || 0 == _tcslen(lpcszKeyType) )
	{
		return FALSE;
	}
	if ( NULL == lpcszKeyName || 0 == _tcslen(lpcszKeyName) )
	{
		return FALSE;
	}
	if ( NULL == lpcszValueName || 0 == _tcslen(lpcszValueName) )
	{
		return FALSE;
	}

	BOOL bRet = FALSE;
	CComPtr<IADsContainer> iContainer;
	CComPtr<IADs> iAds;
	
	//
	//	��ʼ��װ
	//
	if ( S_OK == ADsGetObject( (LPWSTR)lpcwszPathName, IID_IADsContainer, (void**)&iContainer ) )
	{
		if ( S_OK == iContainer->GetObject(_bstr_t(lpcszKeyType),_bstr_t(lpcszKeyName),(IDispatch**)&iAds) )
		{
			//
			//	���浽 MetaBase
			//
			if ( S_OK == iAds->Put( _bstr_t(lpcszValueName), vtVarSub ) )
			{
				if ( S_OK == iAds->SetInfo() )
				{
					bRet = TRUE;
				}
			}

			iAds.Release();
		}

		iContainer.Release();
	}

	return bRet;
}

/**
 *	��ȡָ������ֵ
 */
LONG CDeAdsiOpIIS::MetaGetValueByKeyInt( LPCTSTR lpcszSiteId, LPCTSTR lpcszKey )
{
	//
	//	lpcszSiteId	- [in] վ�� ID ���磺 1, 2
	//	lpcszKey	- [in] Ҫ��ȡ��ֵ�����ƣ����磺��MaxBandwidth, MaxConnections��
	//
	//	ReturnValue	- the value you want. return -1 if failed, otherwise is the value that >= 0
	//

	if ( NULL == lpcszSiteId || 0 == _tcslen(lpcszSiteId) )
		return -1;
	if ( NULL == lpcszKey || 0 == _tcslen(lpcszKey) )
		return -1;
	
	LONG lnRet	= -1;
	CComPtr<IADsContainer> iContainer;
	CComPtr<IADs> iAds;
	VARIANT vValue;

	if ( S_OK == ADsGetObject( L"IIS://localhost/w3svc", IID_IADsContainer, (void**)&iContainer ) )
	{
		if ( S_OK == iContainer->GetObject( _bstr_t("IIsWebServer"),_bstr_t(lpcszSiteId),(IDispatch**)&iAds ) )
		{
			//	MaxBandwidth, MaxConnections
			iAds->Get( _bstr_t( lpcszKey ), &vValue );
			lnRet = vValue.lVal;

			iAds.Release();
		}

		iContainer.Release();
	}

	return lnRet;
}

/**
 *	����ָ������ֵ
 */
BOOL CDeAdsiOpIIS::MetaSetValueByKeyInt( LPCTSTR lpcszSiteId, LPCTSTR lpcszKey, LONG lnValue )
{
	//
	//	lpcszSiteId	- [in] վ�� ID ���磺 1, 2
	//	lpcszKey	- [in] Ҫ��ȡ��ֵ�����ƣ����磺��MaxBandwidth, MaxConnections��
	//	lnValue		- [in] Ҫ���õ�ֵ
	//
	//	ReturnValue	- true / false
	//
	
	if ( NULL == lpcszSiteId || 0 == _tcslen(lpcszSiteId) )
		return -1;
	if ( NULL == lpcszKey || 0 == _tcslen(lpcszKey) )
		return -1;
	
	BOOL bRet	= FALSE;
	CComPtr<IADsContainer> iContainer;
	CComPtr<IADs> iAds;
	//VARIANT vValue;
	TCHAR szTemp[ MAX_PATH ]	= {0};


	if ( S_OK == ADsGetObject( L"IIS://localhost/w3svc", IID_IADsContainer, (void**)&iContainer ) )
	{
		if ( S_OK == iContainer->GetObject( _bstr_t("IIsWebServer"),_bstr_t(lpcszSiteId),(IDispatch**)&iAds ) )
		{
			_sntprintf( szTemp, sizeof(szTemp)/sizeof(TCHAR)-1, _T("%d"), lnValue );
			iAds->Put( _bstr_t(lpcszKey), _variant_t(szTemp) );
			if ( S_OK == iAds->SetInfo() )
			{
				bRet = TRUE;
			}

			iAds.Release();
		}

		iContainer.Release();
	}

	return bRet;
}


/**
 *	@ Private
 *	����� X64 λ����ϵͳ������Ҫ�޸� Metabase ���ݿ⣬�� IIS ֧�� 32 λ������ X64 ϵͳ�ϼ�������
 */
BOOL CDeAdsiOpIIS::Enable32BitAppOnWin64()
{
	//
	//	������ִ���ǣ�
	//	cscript.exe adsutil.vbs set W3SVC/AppPools/Enable32BitAppOnWin64 "true"
	//	RETURN		- TRUE/FALSE
	//

	BOOL bRet	= FALSE;

	if ( delib_is_wow64() )
	{
		bRet = MetaSetData
		(
			L"IIS://localhost/W3SVC",
			_T("IIsApplicationPools"),
			_T("AppPools"),
			_T("Enable32BitAppOnWin64"),
			_variant_t("1")
		);
	}

	return bRet;
}
