#ifndef __DELIBLCS_HEADER__
#define __DELIBLCS_HEADER__



#ifndef __DELIBLCS_STRUCT__
#define __DELIBLCS_STRUCT__

/**
 *	�ṹ�嶨��
 */
typedef struct tagDeLibLcs
{
	tagDeLibLcs()
	{
		memset( this, 0, sizeof(tagDeLibLcs) );
	}
	HWND  hParentDlg;
	BOOL  bReged;			//	�Ƿ���ʽ�汾
	BOOL  bCreateGeneWithDiskSN;	//	�Ƿ�ʹ��Ӳ�����к���Ϊ���� gene �Ĳ���

	TCHAR szSoftName[ MAX_PATH ];	//	��Ʒ���ƣ����磺"deremote"
	TCHAR szPrKey[ MAX_PATH ];	//	���� gene ��Կ��
	TCHAR szHttpKey[ MAX_PATH ];	//	http ͨѶԿ��
	
	TCHAR szWorkDir[ MAX_PATH ];	//	����Ŀ¼

//	TCHAR szGene[ 128 ];		//	32 λ��������
//	TCHAR szRegCode[ 128 ];		//	ע����
//	TCHAR szActCode[ 128 ];		//	������
	
	TCHAR szBuyUrl[ MAX_PATH ];	//	ֱ�ӹ��������
	
}STDELIBLCS, *LPSTDELIBLCS;

#endif	//	__DELIBLCS_STRUCT__





/**
 *	������������
 */
HINSTANCE __stdcall delib_lcs_get_instance();
BOOL __stdcall delib_lcs_showdlg( STDELIBLCS * pstLcInfo );








#endif	//	__DELIBLCS_HEADER__