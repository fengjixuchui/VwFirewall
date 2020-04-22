//////////////////////////////////////////////////////////////////////
#ifndef __PROCDB_HEADER__
#define __PROCDB_HEADER__



#include <vector>
using namespace std;



//////////////////////////////////////////////////////////////////////////
#pragma warning(disable : 4146)
#import "C:\Program Files\Common Files\System\ado\msado15.dll" named_guids rename("EOF","adoEOF")
using namespace ADODB;


class CProcDb
{
public:
//	CProcDb();
	CProcDb( BOOL bShowMessageBox = TRUE );
	virtual ~CProcDb();

public:
	TCHAR m_szSql[ 4096 ];
	TCHAR m_szDbPath[ MAX_PATH ];
	BOOL  m_bShowMessageBox;

	_variant_t m_varValue;
	_variant_t m_RecordsAffected;
	_ConnectionPtr m_conn;
	_RecordsetPtr m_rs;
	VARIANT m_vtNull;

	//	�����Ƿ���ʾ MessageBox
	VOID SetShowMessageBox( BOOL bShowMessageBox );

	//	�������ݿ��ļ�����״̬
	BOOL MakeFileWritable( LPCTSTR lpszFilePath );
	BOOL IsFileWritable( LPCTSTR lpszFilePath );
	BOOL IsDbFileWritable();

	//	�������ݿ�
	BOOL IsConnDatabase();
	BOOL ConnDatabase( LPCTSTR lpcszDataFile );
	VOID CloseDatabase();


	//	������
	BOOL GetSafeString( LPCTSTR lpcszStr, LPTSTR lpszNewStr, DWORD dwSize );
	VOID KillBadChar( LPSTR lpszStr );
	VOID DecodeBadChar( LPSTR lpszStr );

	//	ִ��
	BOOL CompactDatabase();
	BOOL IsExistTable( LPCSTR lpcszTableName );
	BOOL IsExistColumn( LPCSTR lpcszTableName, LPCSTR lpcszColumnName );
	LONG QueryIntValue( _RecordsetPtr rs, LPCSTR lpcszKey );
	VOID QueryStringValue( _RecordsetPtr rs, LPCSTR lpcszKey, LPTSTR lpszValue, DWORD dwSize );
	BOOL ExectueSql( LPCSTR lpcszSql, BOOL bAlertError = TRUE );
	
};

#endif // __PROCDB_HEADER__
