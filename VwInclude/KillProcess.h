/*
 *
 *	�������߶�һ�Ƽ����޹�˾
 *	��Ȩ���� (C) 3721.Com
 *	Copyright (C) 3721.Com, All right reserved.
*/

/*
 * ------------------------------------------------------------
 *
 *	��Ҫ���ܣ�
 *		�ɵ����̺ͷ���
 *
 *
 * ------------------------------------------------------------
 *
 *	������ʷ��
 *
 *		2004-06-16	�����Ǵ���
 *
 *
 * ------------------------------------------------------------
 *
 *	���߱�ע��
 *
 *
 *
 * ------------------------------------------------------------
 */





#ifndef __KILLPROCESS_HEADER__
#define __KILLPROCESS_HEADER__



//////////////////////////////////////////////////////////////////////////
//
#define TA_FAILED		0
#define TA_SUCCESS_CLEAN	1
#define TA_SUCCESS_KILL		2
#define TA_SUCCESS_16		3



struct TERMINFO{
	DWORD dwID;
	DWORD dwThread;
};


BOOL WINAPI KillProcess32ByPID( DWORD dwPID, DWORD dwTimeout, TCHAR * pszError );

DWORD WINAPI KillProcess32( DWORD dwPID, DWORD dwTimeout );
DWORD WINAPI KillProcess16( DWORD dwPID, DWORD dwThread, WORD w16Task, DWORD dwTimeout );

BOOL CALLBACK TerminateAppEnum( HWND hwnd, LPARAM lParam );
BOOL CALLBACK Terminate16AppEnum( HWND hwnd, LPARAM lParam );



#endif // __KILLPROCESS_HEADER__
