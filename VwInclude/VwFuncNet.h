// VwFuncNet.h: interface for the CVwFunc class.
//
//////////////////////////////////////////////////////////////////////

#ifndef __VWFUNCNET_HEADER__
#define __VWFUNCNET_HEADER__


#include <stdio.h>
#include <stdlib.h>
#include <tchar.h>

#include <nb30.h>
#pragma comment( lib, "netapi32.lib" )

#ifndef FD_SET
#include "Winsock2.h"
#endif
#pragma comment( lib, "Ws2_32.lib" )

#include "VwFuncBase.h"


static const DWORD VERSION_MAJOR	= 1;
static const DWORD VERSION_MINOR	= 1;


/**
 *	�ṹ�嶨��
 */
typedef struct tagAstat
{
	ADAPTER_STATUS adapt;
	NAME_BUFFER NameBuff[ 30 ];
}STASTAT, * PSTASTAT;

typedef struct tagVwfIpAddr
{
	TCHAR szIpAddr[ 32 ];

}STVWFIPADDR, *PSTVWFIPADDR;



//	���ַ����� IP ��ַת��������
DWORD _vwfunc_get_ipaddr_value_from_string( LPCSTR lpcszIpAddr );

//	��ȡ���б��ص� IP ��ַ
BOOL _vwfunc_get_all_ipaddr( vector<STVWFIPADDR> & vcIpAddr );

//	��ȡ MAC ��ַ
BOOL _vwfunc_get_mac_address( LPCTSTR lpcszNetBiosName, LPTSTR lpszMacAddress, DWORD dwSize );

//	��ȡӲ�����к�
BOOL _vwfunc_get_hdisk_serialnumber( LPTSTR lpszSerialNumber, DWORD dwSize );




#endif // __VWFUNCNET_HEADER__
