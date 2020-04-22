// DeChineseCode.cpp: implementation of the CDeChineseCode class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "DeChineseCode.h"


//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CDeChineseCode::CDeChineseCode()
{
}
CDeChineseCode::~CDeChineseCode()
{
}

BOOL CDeChineseCode::UTF8ToUnicode( wchar_t * pOut, CHAR * pszText )
{
	CHAR * uChar = (CHAR*)pOut;

	if ( NULL == pOut || NULL == pszText )
	{
		return FALSE;
	}

	uChar[ 1 ] = ( ( pszText[ 0 ] & 0x0F ) << 4 ) + ( ( pszText[ 1 ] >> 2 ) & 0x0F );
	uChar[ 0 ] = ( ( pszText[ 1 ] & 0x03 ) << 6 ) + ( pszText[ 2 ] & 0x3F );

	return TRUE;
}

BOOL CDeChineseCode::UnicodeToUTF8( CHAR * pOut, wchar_t * pszText )
{
	if ( NULL == pszText || NULL == pOut )
	{
		return FALSE;
	}

	//	ע�� WCHAR �ߵ��ֵ�˳�򣬵��ֽ���ǰ�����ֽ��ں�
	CHAR * pChar = (CHAR*)pszText;

	pOut[ 0 ] = ( 0xE0 | ( ( pChar[ 1 ] & 0xF0 ) >> 4 ) );
	pOut[ 1 ] = ( 0x80 | ( ( pChar[ 1 ] & 0x0F ) << 2 ) ) + ( ( pChar[0] & 0xC0 ) >> 6 );
	pOut[ 2 ] = ( 0x80 | ( pChar[ 0 ] & 0x3F ) );

	return TRUE;
}

BOOL CDeChineseCode::UnicodeToGB2312( CHAR * pOut, wchar_t uData )
{
	if ( NULL == pOut )
	{
		return FALSE;
	}

	if ( WideCharToMultiByte( CP_ACP,NULL, &uData, 1, pOut, sizeof(wchar_t), NULL, NULL ) > 0 )
	{
		return TRUE;
	}
	else
	{
		return FALSE;
	}
}

BOOL CDeChineseCode::Gb2312ToUnicode( wchar_t * pOut, CHAR * pszGbBuffer )
{
	if ( NULL == pOut || NULL == pszGbBuffer )
	{
		return FALSE;
	}

	if ( ::MultiByteToWideChar( CP_ACP, MB_PRECOMPOSED, pszGbBuffer, 2, pOut, 1 ) > 0 )
	{
		return TRUE;
	}
	else
	{
		return FALSE;
	}
}

BOOL CDeChineseCode::GB2312ToUTF8( string & pOut, CHAR * pszText, INT nInputLen )
{
	BOOL bRet;
	CHAR szBuf[ 4 ];
	CHAR * pstTemp;
	INT  nTempLen;
	wchar_t wcBuffer;
	USHORT uTemp;
	INT i	= 0;
	INT j	= 0;

	if ( NULL == pszText || 0 == nInputLen )
	{
		return FALSE;
	}

	//	...
	bRet = FALSE;

	__try
	{
		nTempLen	= nInputLen * 3;
		pstTemp		= new CHAR[ nTempLen ];
		if ( pstTemp )
		{
			memset( szBuf, 0, 4 );
			memset( pstTemp, 0, nTempLen );

			while ( i < nInputLen )
			{
				//	�����Ӣ��ֱ�Ӹ��ƾͿ���
				if ( *( pszText + i ) >= 0 )
				{
					pstTemp[ j++ ] = pszText[ i++ ];
				}
				else
				{
					wcBuffer = NULL;
					Gb2312ToUnicode( &wcBuffer, pszText+i );	
					UnicodeToUTF8( szBuf, &wcBuffer );

					uTemp	= 0;
					uTemp	= pstTemp[ j ]		= szBuf[ 0 ];
					uTemp	= pstTemp[ j + 1 ]	= szBuf[ 1 ];
					uTemp	= pstTemp[ j + 2 ]	= szBuf[ 2 ]; 

					j += 3;
					i += 2;
				}
			}

			pstTemp[ j ] = '\0';

			//	���ؽ��
			pOut	= pstTemp; 
			delete [] pstTemp;
			pstTemp	= NULL;

			bRet = TRUE;
		}
	}
	__except( EXCEPTION_EXECUTE_HANDLER )
	{
	}	

	return bRet;
}

BOOL CDeChineseCode::UTF8ToGB2312( CHAR * pszOutBuf, CHAR * pszText, INT nInputLen )
{
	BOOL bRet;
	CHAR sztemp[4];
	WCHAR wcTemp;
	INT i;
	INT j;

	if ( NULL == pszOutBuf || NULL == pszText || 0 == nInputLen )
	{
		return FALSE;
	}

	//	...
	bRet = FALSE;

	__try
	{
		memset( sztemp, 0, sizeof(sztemp) );
		i	= 0;
		j	= 0;

		while( i < nInputLen )
		{
			if ( pszText[ i ] > 0 )
			{
				pszOutBuf[ j++ ] = pszText[ i++ ]; 
			}
			else 
			{
				UTF8ToUnicode( &wcTemp, pszText + i );
				UnicodeToGB2312( sztemp, wcTemp );

				pszOutBuf[ j ]	= sztemp[ 0 ];
				pszOutBuf[ j + 1 ]	= sztemp[ 1 ];

				i += 3; 
				j += 2; 
			}
		}

		pszOutBuf[ j ]	= '\0';
		bRet		= TRUE;
	}
	__except( EXCEPTION_EXECUTE_HANDLER )
	{
	}	

	return bRet;
}

/**
 *	�ж��ַ����Ƿ��� UTF-8 �ַ���
 *	������ķ���
 */
BOOL CDeChineseCode::IsUtf8_byLiuJunMing( LPCTSTR lpcszString, UINT uLength )
{
	UINT i;
	UINT m;
	UINT uCount;
	UCHAR ucChar;

	if ( NULL == lpcszString || 0 == uLength )
	{
		return FALSE;
	}

	__try
	{
		for ( i = 0; i < uLength; )
		{
			ucChar = lpcszString[ i ];
			if ( ucChar >= 0 && ucChar < 0x7F )
			{
				i ++;
				continue;
			}
			if ( ucChar < 0xC0 || ucChar > 0xFD )
			{
				return FALSE;
			}
			uCount = ucChar > 0xFC ? 5 : ucChar > 0xF8 ? 4 : ucChar > 0xF0 ? 3 : ucChar > 0xE0 ? 2 : 1;
			if ( i + uCount > uLength )
			{
				return FALSE;
			}

			i ++;

			for ( m = 0; m < uCount; m ++ )
			{
				if ( lpcszString[ i ] < 0x80 || lpcszString[ i ] > 0xBF )
				{
					return FALSE;
				}
				i ++;
			}
		}
	}
	__except( EXCEPTION_EXECUTE_HANDLER )
	{
	}	

	return TRUE;
}

/**
 *	�ж��ַ����Ƿ��� UTF-8 �ַ���
 */
BOOL CDeChineseCode::IsUTF8Text( LPCTSTR lpcszBuffer, LONG lnLength )
{
	BOOL bRet;
	UCHAR * ucStart;
	UCHAR * ucEnd;

	if ( NULL == lpcszBuffer || 0 == lnLength )
	{
		return FALSE;
	}

	bRet	= TRUE;
	ucStart	= (UCHAR*)lpcszBuffer;
	ucEnd	= (UCHAR*)lpcszBuffer + lnLength;

	while ( ucStart < ucEnd )
	{
		if ( *ucStart < 0x80 )
		{
			//	(10000000): ֵС��0x80��ΪASCII�ַ�
			ucStart++;
		}
		else if ( *ucStart < (0xC0) )
		{
			//	(11000000): ֵ����0x80��0xC0֮���Ϊ��ЧUTF-8�ַ�
			bRet = FALSE;
			break;
		}
		else if ( *ucStart < (0xE0) ) 
		{
			//	(11100000): �˷�Χ��Ϊ2�ֽ�UTF-8�ַ�
			if ( ucStart >= ucEnd - 1 )
			{
				break;
			}
			if ( ( ucStart[ 1 ] & (0xC0)) != 0x80 )
			{
				bRet = FALSE;
				break;
			}
			ucStart += 2;
		}
		else if ( *ucStart < (0xF0) )
		{
			//	(11110000): �˷�Χ��Ϊ3�ֽ�UTF-8�ַ�
			if (ucStart >= ucEnd - 2)
			{
				break;
			}
			if ( ( ucStart[ 1 ] & (0xC0) ) != 0x80 || ( ucStart[ 2 ] & (0xC0) ) != 0x80 )
			{
				bRet = FALSE;
				break;
			}
			ucStart += 3;
		}
		else
		{
			bRet = FALSE;
			break;
		}
	}

	return bRet;
}










BOOL CDeChineseCode::IsUTF8( const char * pzInfo )
{
	MultiByteToWideChar( CP_UTF8, MB_ERR_INVALID_CHARS, pzInfo, -1, NULL, 0 );
	if ( ERROR_NO_UNICODE_TRANSLATION == GetLastError() )
	{
		return FALSE;
	}
	//	�ж��Ƿ���gb2312,ֻҪ��CP_UTF8��936���漴��.
	return TRUE;
}

/***************************************************
 * Function:       IsGB2312
 * Description:    �ж��Ƿ�gb2312�����ַ�
 * Input:          pzInfo:���ж��ַ�
 * Output:         
 * Return:         
 * Others:         
 ***************************************************/
BOOL CDeChineseCode::IsGB2312( const char * pzInfo )
{
	MultiByteToWideChar( 936, MB_ERR_INVALID_CHARS, pzInfo, -1, NULL, 0 );
	if ( ERROR_NO_UNICODE_TRANSLATION == GetLastError() )
	{
		return FALSE;
	}
	//	�ж��Ƿ��� CP_UTF8, ֻҪ�� 936 �� CP_UTF8 ���漴��.
	return TRUE;
}

/***************************************************
* Function:       IsGB
* Description:    �Ƿ�gb2312
* Input:          pText�����ж��ַ�
* Output:         
* Return:         
* Others:         
***************************************************/
int CDeChineseCode::IsGB( char *pText )
{
	unsigned char *sqChar = (unsigned char *)pText;
	if (sqChar[0] >= 0xa1)
	{
		if (sqChar[0] == 0xa3)
		{
			return 1;//ȫ���ַ�
		}
		else
		{
			return 2;//����
		}
	}
	else
	{
		return 0;//Ӣ�ġ����֡�Ӣ�ı��
	}
}

/***************************************************
* Function:       IsChinese
* Description:    �Ƿ�������
* Input:          pzInfo�����ж��ַ�
* Output:         
* Return:         
* Others:         
***************************************************/
bool CDeChineseCode::IsChinese( const char *pzInfo )
{
	int i;
	char *pText = (char*)pzInfo;
	while (*pText != '\0')
	{
		i = IsGB(pText);
		switch(i)
		{
		case 0:
			pText++;
			break;
		case 1:
			pText++;
			pText++;
			break;
		case 2:
			pText++;
			pText++;
			return TRUE;
			break;
		}
	}
	return false;
}






//////////////////////////////////////////////////////////////////////////
//	private



/*
inline BYTE CDeChineseCode::toHex(const BYTE &x)
{
	return x > 9 ? x + 55: x + 48;
}
inline BYTE CDeChineseCode::toByte(const BYTE &x)
{
	return x > 57? x - 55: x - 48;
}
CString CDeChineseCode::URLDecode(CString sIn)
{
	CString sOut;
	const int nLen = sIn.GetLength() + 1;
	register LPBYTE pOutTmp = NULL;
	LPBYTE pOutBuf = NULL;
	register LPBYTE pInTmp = NULL;
	LPBYTE pInBuf =(LPBYTE)sIn.GetBuffer(nLen);
	//alloc out buffer
	pOutBuf = (LPBYTE)sOut.GetBuffer(nLen);
	
	if(pOutBuf)
	{
		pInTmp = pInBuf;
		pOutTmp = pOutBuf;
		// do encoding
		while (*pInTmp)
		{
			if('%'==*pInTmp)
			{
				pInTmp++;
				*pOutTmp++ = (toByte(*pInTmp)%16<<4) + toByte(*(pInTmp+1))%16;//��4λ+��4λ
				pInTmp++;
			}
			else if('+'==*pInTmp)
				*pOutTmp++ = ' ';
			else
				*pOutTmp++ = *pInTmp;
			pInTmp++;
		}
		*pOutTmp = '\0';
		sOut.ReleaseBuffer();
	}
	sIn.ReleaseBuffer();
	
	return sOut;
}

CString CDeChineseCode::URLEncode(CString sIn)
{
	CString sOut;
	const int nLen = sIn.GetLength() + 1;
	register LPBYTE pOutTmp = NULL;
	LPBYTE pOutBuf = NULL;
	register LPBYTE pInTmp = NULL;
	LPBYTE pInBuf =(LPBYTE)sIn.GetBuffer(nLen);
	//alloc out buffer
	pOutBuf = (LPBYTE)sOut.GetBuffer(nLen*3);
	
	if(pOutBuf)
	{
		pInTmp = pInBuf;
		pOutTmp = pOutBuf;
		// do encoding
		while (*pInTmp)
		{
			if(isalnum(*pInTmp) || '-'==*pInTmp || '_'==*pInTmp || '.'==*pInTmp)
				*pOutTmp++ = *pInTmp;
			else if(isspace(*pInTmp))
				*pOutTmp++ = '+';
			else
			{
				*pOutTmp++ = '%';
				*pOutTmp++ = toHex(*pInTmp>>4);//��4λ
				*pOutTmp++ = toHex(*pInTmp%16);//��4λ
			}
			pInTmp++;
		}
		*pOutTmp = '\0';
		sOut.ReleaseBuffer();
	}
	sIn.ReleaseBuffer();
	
	return sOut;
}
*/