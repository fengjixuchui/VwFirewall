// VwLicencePanDmNew.cpp: implementation of the CVwLicencePanDmNew class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "VwLicencePanDmNew.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CVwLicencePanDmNew::CVwLicencePanDmNew()
{
}

CVwLicencePanDmNew::~CVwLicencePanDmNew()
{
}

/**
 *	@ Public
 *	���ݲ�Ʒ�����ȡ��Ʒ����
 *	������
 */
BOOL CVwLicencePanDmNew::GetMLangPrTypeNameByCode( LPCTSTR lpcszPrType, LPTSTR lpszPrTypeName, DWORD dwSize )
{
	if ( NULL == lpcszPrType || NULL == lpszPrTypeName || 0 == dwSize )
	{
		return FALSE;
	}

	if ( 0 == _tcsicmp( CVWLICENCE_PRODUCTTYPE_COMADV, lpcszPrType ) )
	{
		//	�߼���˾��
		LoadString( AfxGetInstanceHandle(), IDS_STRING_PRTYPE_COMADV, lpszPrTypeName, dwSize );
	}
	else if ( 0 == _tcsicmp( CVWLICENCE_PRODUCTTYPE_COMNORMAL, lpcszPrType ) )
	{
		//	��ͨ��˾��
		LoadString( AfxGetInstanceHandle(), IDS_STRING_PRTYPE_COMNORMAL, lpszPrTypeName, dwSize );
	}
	else if ( 0 == _tcsicmp( CVWLICENCE_PRODUCTTYPE_PSLUNLMT, lpcszPrType ) )
	{
		//	������˰�
		LoadString( AfxGetInstanceHandle(), IDS_STRING_PRTYPE_PSLUNLMT, lpszPrTypeName, dwSize );
	}
	else if ( 0 == _tcsicmp( CVWLICENCE_PRODUCTTYPE_PSLADV, lpcszPrType ) )
	{
		//	�߼����˰�
		LoadString( AfxGetInstanceHandle(), IDS_STRING_PRTYPE_PSLADV, lpszPrTypeName, dwSize );
	}
	else if ( 0 == _tcsicmp( CVWLICENCE_PRODUCTTYPE_PSLNORMAL, lpcszPrType ) )
	{
		//	��ͨ���˰�
		LoadString( AfxGetInstanceHandle(), IDS_STRING_PRTYPE_PSLNORMAL, lpszPrTypeName, dwSize );
	}
	else if ( 0 == _tcsicmp( CVWLICENCE_PRODUCTTYPE_PSLRENEW, lpcszPrType ) )
	{
		//	����
		LoadString( AfxGetInstanceHandle(), IDS_STRING_PRTYPE_PSLRENEW, lpszPrTypeName, dwSize );
	}
	else if ( 0 == _tcsicmp( CVWLICENCE_PRODUCTTYPE_UPGRADE, lpcszPrType ) )
	{
		//	����
		LoadString( AfxGetInstanceHandle(), IDS_STRING_PRTYPE_UPGRADE, lpszPrTypeName, dwSize );
	}
	else if ( 0 == _tcsicmp( CVWLICENCE_PRODUCTTYPE_REMOTEHELP, lpcszPrType ) )
	{
		//	Զ��Э������
		LoadString( AfxGetInstanceHandle(), IDS_STRING_PRTYPE_REMOTEHELP, lpszPrTypeName, dwSize );
	}
	else
	{
		//	��������汾
		LoadString( AfxGetInstanceHandle(), IDS_STRING_PRTYPE_EVALUATION, lpszPrTypeName, dwSize );
	}
	
	return TRUE;
}