#ifndef __DEFILEENCRYPT_HEADER__
#define __DEFILEENCRYPT_HEADER__


#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <tchar.h>
#include <stdio.h>
#include <stdlib.h>



/**
 *	const value
 */
#define BASE_XOR_VALUE		(0x04020626)
#define BASE_PLUS_VALUE		(0x00970702)
#define DEF_ENCRYPT_KEY		(0x03070201)

#define DEFILEENCRYPT_HEADER_LENGTH		64
#define DEFILEENCRYPT_HEADER_FLAG		"de_encrypted_file"

#define CONST_VWFILEENCRYPT_MAX_FILEMAP_LENGTH	( 100 * 1024 * 1024 )

/**
 *	�����ļ�ͷ�ṹ����֤������ 64 ���ֽ�
 */
typedef struct tagDeEncryptedFileHeader
{
	tagDeEncryptedFileHeader()
	{
		memset( this, 0, sizeof(tagDeEncryptedFileHeader) );

		dwVersion	= 1;
		dwBuild		= 1001;
		dwKey		= DEF_ENCRYPT_KEY;
		strcpy( szHeaderFlag, DEFILEENCRYPT_HEADER_FLAG );
	}

	DWORD  dwVersion;
	DWORD  dwBuild;
	DWORD  dwKey;
	TCHAR  szHeaderFlag[ 32 ];
	TCHAR  szOther[ 20 ];

}STDEENCRYPTEDFILEHEADER, *PSTDEENCRYPTEDFILEHEADER;




/**
 *	class of CDeFileEncrypt
 */
class CDeFileEncrypt
{
public:
	CDeFileEncrypt();
	virtual ~CDeFileEncrypt();

	//	�Ƿ��Ǽ��ܺ���ļ�
	BOOL IsEncryptedFile( LPCTSTR lpcszFilePath );

	//	��ָ���ļ��еõ����ܺ������
	BOOL GetDecodedBufferFromFile( IN LPCTSTR lpcszFilePath, OUT LPTSTR lpszDecodedBuffer, IN DWORD dwSize );

	//	������ܺ�� buffer ���ļ�
	BOOL SaveEncryptedBufferToFile( IN LPCTSTR lpcszBuffer, IN LPCTSTR lpcszFilePath );

	//	����/���� �ļ�
	BOOL EncryptFile( LPCTSTR lpcszSrcFilePath, LPCTSTR lpcszDstFilePath );

	//	����/���� buffer
	VOID EncryptBuffer( BYTE * pBuf, int nLen, DWORD dwKey );

private:

	BOOL IsEncryptedFileByByte( BYTE * pbHeaderBuffer );
	BOOL GetHeaderInfo( BYTE * pbHeaderBuffer, STDEENCRYPTEDFILEHEADER & stHeader );

	INT  myrand( unsigned int * pSeed );
	ULONG GetCRC( PUCHAR lpData, ULONG uLength );

private:
	DWORD	m_dwKey;
};




#endif	// __DEFILEENCRYPT_HEADER__