// ProcPacket.cpp: implementation of the ProcPacket class.
//
//////////////////////////////////////////////////////////////////////

#include "DrvFunc.h"


/**
 *	���: �ļ�ϵͳ����
 *	��ָ�� IRP �´����²��ļ�����
 */
NTSTATUS drvfunc_pass_through( IN PDEVICE_OBJECT pDevObj, IN PIRP Irp )
{
	if ( NULL == Irp )
	{
		return STATUS_SUCCESS;
	}
	if ( NULL == pDevObj )
	{
		Irp->IoStatus.Status = STATUS_SUCCESS;
		IoCompleteRequest( Irp, IO_NO_INCREMENT );
		return STATUS_SUCCESS;
	}
	
	NTSTATUS status = STATUS_SUCCESS;
	PDEVICE_EXTENSION pDevExt;
	
	__try
	{
		if ( pDevObj == g_pControlDevObj )
		{
			//
			//	���������ѣ�ֱ�����
			//
			status = STATUS_SUCCESS;
			Irp->IoStatus.Status = status;
			IoCompleteRequest( Irp, IO_NO_INCREMENT );
		}
		else
		{
			//
			//	���������Լ���Ѱ����һ���ҽ�
			//
			pDevExt = (PDEVICE_EXTENSION)pDevObj->DeviceExtension;
			if ( pDevExt )
			{
				//
				//	�� IRP �����²��ļ�ϵͳ����һ��FILTER�����
				//
				IoCopyCurrentIrpStackLocationToNext( Irp );
				IoSetCompletionRoutine( Irp, NULL, NULL, FALSE, FALSE, FALSE );
				status = IoCallDriver( pDevExt->pFileSystemDeviceObject, Irp );
			}
			else
			{
				//
				//	û�з�����һ������ô��ֱ����ɰ�
				//
				status = STATUS_SUCCESS;
				Irp->IoStatus.Status = status;
				IoCompleteRequest( Irp, IO_NO_INCREMENT );
			}
		}
	}
	__except( EXCEPTION_EXECUTE_HANDLER )
	{
	}
	
	return status;
}

/**
 *	�Ƿ���ϵͳ�Ķ�������Ҫ�Ź�
 */
BOOL drvfunc_ispass_systemaction( INT nOPType )
{
	BOOL bRet;
	if ( KeGetCurrentIrql() > DISPATCH_LEVEL )
	{
		return TRUE;
	}

	//if( !g_bStartFilter )
	//	return TRUE;
	
	if ( g_bStopDriver )
	{
		return TRUE;
	}

	bRet = FALSE;
	switch( nOPType )
	{
	case SYSACTION_FSD:
		{
			if ( g_bStopFsdFilter )
			{
				bRet = TRUE;
			}
		}
		break;
	case SYSACTION_REG:
		{
			if ( g_bStopRegFilter )
			{
				bRet = TRUE;
			}
		}
		break;
	case SYSACTION_PROC:
		{
			if ( g_bStopProcFilter )
			{
				bRet = TRUE;
			}
		}
		break;
	}

	//	����ǲ��ǺϷ�����
	/*if( !bRet )
		bRet = IsAuthPid( DrvGetCurPid(), DrvGetCurTid() );*/
	
	return bRet;
}

/**
 *	ȡ�õ�ǰϵͳ�� Windows Ŀ¼
 */
BOOL drvfunc_get_windowsdir( IN PDRIVER_OBJECT DriverObject, OUT PWCHAR pwszWindowDir, IN USHORT uSize )
{
	BOOL   bRet	= FALSE;
	HANDLE hFileHandle;
	PWCHAR pwszBuffer;
	WCHAR  wcTemp;
	USHORT uLen;
	NTSTATUS ntStatus;
	OBJECT_ATTRIBUTES ObjectAttributes;
	UNICODE_STRING usFileName;
	IO_STATUS_BLOCK IoStatus;
	PFILE_NAME_INFORMATION pstNameInfo;
	PFILE_OBJECT pstFileObject;
	POBJECT_NAME_INFORMATION pstObjectNameInfo;

	//	�򿪵��ļ����� "\SystemRoot"
	RtlInitUnicodeString( &usFileName, L"\\SystemRoot" );
	InitializeObjectAttributes( &ObjectAttributes, &usFileName, OBJ_CASE_INSENSITIVE|OBJ_KERNEL_HANDLE, NULL, NULL );
	ntStatus = ZwCreateFile
		(
			&hFileHandle, 
			(SYNCHRONIZE | FILE_READ_ATTRIBUTES), 
			&ObjectAttributes, &IoStatus, 
			NULL, FILE_ATTRIBUTE_NORMAL, FILE_SHARE_READ, FILE_OPEN,
			FILE_DIRECTORY_FILE|FILE_SYNCHRONOUS_IO_NONALERT, NULL, 0
		);
	if ( NT_SUCCESS( ntStatus ) && NT_SUCCESS( IoStatus.Status ) )
	{
		ntStatus = ObReferenceObjectByHandle( hFileHandle, FILE_READ_DATA, *IoFileObjectType, KernelMode, (void **)&pstFileObject, NULL );
		if ( NT_SUCCESS( ntStatus ) )
		{
			pstObjectNameInfo	= NULL;
			ntStatus = IoQueryFileDosDeviceName( pstFileObject, &pstObjectNameInfo );
			if ( NT_SUCCESS( ntStatus ) )
			{
				wcTemp = pstObjectNameInfo->Name.Buffer[ pstObjectNameInfo->Name.Length - sizeof(WCHAR) ];
				if ( L'\\' != wcTemp && L'/' != wcTemp )
				{
					_snwprintf( pwszWindowDir, uSize-sizeof(WCHAR), L"\\??\\%s\\", pstObjectNameInfo->Name.Buffer );
				}
				else
				{
					_snwprintf( pwszWindowDir, uSize-sizeof(WCHAR), L"\\??\\%s", pstObjectNameInfo->Name.Buffer );
				}

				//	..
				bRet = TRUE;
			}
		}

		ZwClose( hFileHandle );
	}

	return bRet;
}


/**
 *	��ȡ��ǰϵͳ����ʱ��
 */
NTSTATUS drvfunc_get_localtime( PCHAR pszCurrentTime, ULONG uSize )
{
	LARGE_INTEGER lnSystemTime;
	LARGE_INTEGER lnLocalTime;
	TIME_FIELDS tfTimeFields;
	NTSTATUS ntStatus = STATUS_UNSUCCESSFUL;

	if ( pszCurrentTime )
	{
		KeQuerySystemTime( &lnSystemTime );
		ExSystemTimeToLocalTime( &lnSystemTime, &lnLocalTime );
		RtlTimeToTimeFields( &lnLocalTime, &tfTimeFields );
	
		_snprintf( pszCurrentTime, uSize-sizeof(char), "%d-%02d-%02d %02d:%02d:%02d",
			tfTimeFields.Year, tfTimeFields.Month, tfTimeFields.Day,
			tfTimeFields.Hour, tfTimeFields.Minute, tfTimeFields.Second );
		// ..
		ntStatus = STATUS_SUCCESS;
		KdPrint(("MyGetLocalTime: Current time is %s", pszCurrentTime ));
	}
	else
	{
		KdPrint(("MyGetLocalTime: pszCurrentTime is a NULL point"));
	}
	return ntStatus;
}

/**
 *	write log
 */
NTSTATUS drvfunc_write_log( const PCHAR pszData )
{
	HANDLE hFileHandle;
	OBJECT_ATTRIBUTES ObjectAttributes;
	UNICODE_STRING usFileName;
	IO_STATUS_BLOCK IoStatus;
	NTSTATUS ntStatus = STATUS_UNSUCCESSFUL;
	FILE_POSITION_INFORMATION filePos;
	FILE_STANDARD_INFORMATION StandardInfo;
	ULONG nWriteBytes;

	RtlInitUnicodeString( &usFileName, LOG_FILEPATH );
	InitializeObjectAttributes
		(
			&ObjectAttributes,			// ptr to structure
			&usFileName,				// ptr to file spec
			OBJ_CASE_INSENSITIVE|OBJ_KERNEL_HANDLE,	// attributes
			NULL,					// root directory handle
			NULL					// ptr to security descriptor
		);

	ntStatus = ZwCreateFile
		(
			&hFileHandle,				// returned file handle
			SYNCHRONIZE | FILE_READ_DATA| FILE_WRITE_DATA,    // desired access
			&ObjectAttributes,			// ptr to object attributes
			&IoStatus,				// ptr to I/O status block
			NULL,					// alloc size = none
			FILE_ATTRIBUTE_NORMAL,
			FILE_SHARE_READ|FILE_SHARE_WRITE,
			FILE_OPEN_IF,
			FILE_SYNCHRONOUS_IO_NONALERT,
			NULL,					// eabuffer
			0					// ealength
		);
	if ( NT_SUCCESS(ntStatus) && NT_SUCCESS(IoStatus.Status) )
	{
		ntStatus = ZwQueryInformationFile
			(
				hFileHandle, &IoStatus, &StandardInfo,
				sizeof(FILE_STANDARD_INFORMATION), FileStandardInformation
			);
		if ( NT_SUCCESS( ntStatus ) )
		{
			filePos.CurrentByteOffset = StandardInfo.EndOfFile;
			ntStatus = ZwSetInformationFile
				(
					hFileHandle, &IoStatus, &filePos, 
					sizeof(FILE_POSITION_INFORMATION),FilePositionInformation
				);
			if ( NT_SUCCESS(ntStatus) )
			{
				nWriteBytes = strlen(pszData);
				ntStatus = ZwWriteFile
					(
						hFileHandle, NULL, NULL, NULL,
						&IoStatus, (PVOID)pszData, nWriteBytes, NULL, NULL
					);
				if ( NT_SUCCESS(ntStatus) && IoStatus.Information == nWriteBytes )
				{
					ntStatus = STATUS_SUCCESS;
					KdPrint(("KWriteOpToLogFile: Write Data %s\n", pszData ));
				}
				else
				{
					KdPrint(("KWriteOpToLogFile: Write Data is Error,status=0x%4x, Pre Write=%d, WriteBytes=%d\n",
						ntStatus, nWriteBytes, nWriteBytes ));
				}
			}
			else
			{
				KdPrint(("KWriteOpToLogFile: cann't Set End of File,status=0x%4x\n", ntStatus));
			}
		}
		else
		{
			KdPrint(("KWriteOpToLogFile: cann't get file size,status=0x%4x\n", ntStatus));
		}
		// ..
		ZwClose( hFileHandle );
	}
	else
	{
		KdPrint( ("KWriteOpToLogFile: Open %ws is fail, status=0x%4x\n",LOG_FILEPATH, ntStatus) );
	}

	return ntStatus;
}




/**
 *	get os version
 */
NTSTATUS drvfunc_get_osversion( PULONG pOsMajorVersion, PULONG pOsMinorVersion )
{
	typedef NTSTATUS (*PFNRTLGETVERSION)( IN OUT PRTL_OSVERSIONINFOW VersionInformation );
	UNICODE_STRING functionName;

	PFNRTLGETVERSION pfnRtlGetVersion;
	RTL_OSVERSIONINFOW versionInfo;
	NTSTATUS status = STATUS_UNSUCCESSFUL;

	if ( NULL == pOsMajorVersion || NULL == pOsMinorVersion )
	{
		KdPrint( ("drvfunc_get_osversion:: NULL == pOsMajorVersion || NULL == pOsMinorVersion\n") );
		return STATUS_UNSUCCESSFUL;
	}

#if 0
//#if WINVER >= 0x0501

//	������ע�͵�
/*
	// This case is supported on Windows XP/2003 and later operating system versions
	RtlInitUnicodeString( &functionName, L"RtlGetVersion" );
	pfnRtlGetVersion = (PFNRTLGETVERSION)MmGetSystemRoutineAddress( &functionName );
	if ( pfnRtlGetVersion )
	{
		versionInfo.dwOSVersionInfoSize = sizeof( RTL_OSVERSIONINFOW );
		status = pfnGetVersion( &versionInfo );

		if ( NT_SUCCESS(status) )
		{
			*pOsMajorVersion = versionInfo.dwMajorVersion;
			*pOsMinorVersion = versionInfo.dwMinorVersion;
			KdPrint( ("drvfunc_get_osversion::RtlGetVersion successfully.\n") );
		}
		else
		{
			KdPrint( ("drvfunc_get_osversion::RtlGetVersion failed.\n") );
		}
	}
*/
#else

	// This case is supported on Windows 2000/98/me versions of the operating system
	PsGetVersion( pOsMajorVersion, pOsMinorVersion, NULL, NULL );
	// ..
	status = STATUS_SUCCESS;
	KdPrint( ("drvfunc_get_osversion::PsGetVersion successfully.\n") );

#endif

	return status;
}


/**
 *	ȡ�ý���ƫ����
 */
ULONG drvfunc_get_processname_offset()
{
	ULONG		uRet = 0;
	ULONG		i = 0;
	PEPROCESS	curproc;

	curproc = PsGetCurrentProcess();
	__try
	{
		//
		// Scan for 12KB, hoping the KPEB never grows that big!
		// PAGE_SIZE=4k/8k
		for ( i = 0; i < 3*PAGE_SIZE; i++ )
		{
			if ( !strncmp( SYSNAME, (PCHAR)curproc + i, strlen(SYSNAME) ) )
			{
				uRet = i;
				break;
			}
		}
	}
	__except( EXCEPTION_EXECUTE_HANDLER )
	{
	}

	return uRet;
}

/**
 *	��ȡ������
 *	ȡ�õ�ǰ������,����·������:notepad.exe��
 */
VOID drvfunc_get_processname( IN ULONG uProcessNameOffset, OUT PCHAR pProcessName, IN ULONG uSize )
{
	//
	//	uProcessNameOffset	- [in]
	//	pProcessName		- [out] Pointer to get the process name, etc: "notepad.exe".
	//	uSize			- [in]
	//

	PEPROCESS       curproc;
	char            *nameptr;
	ULONG           i;

	//
	// We only do this if we determined the process name offset
	//
	if ( 0 == uProcessNameOffset )
		return ;
	//
	// Get a pointer to the current process block
	//
	curproc = PsGetCurrentProcess();

	//
	// Dig into it to extract the name. Make sure to leave enough room
	// in the buffer for the appended process ID.
	//
	nameptr   = (PCHAR)curproc + uProcessNameOffset;
	//..
	_snprintf( pProcessName, uSize-sizeof(char), "%s", nameptr );
}



/////////////////////////////////////////////////////////////////
//  ���:
//         �ļ�ϵͳ����
//  ����:
//         ȡ�õ�ǰ����������·��������:C:\WINNT\notepad.exe��
//  ������
//         
//  ����ֵ��
//         STATUS_SUCCESS	�ɹ�
//  ԭ��ȡ�� EPROCESS->PEB->RTL_USER_PROCESS_PARAMETER->ImagePathName
//	  ����ԭ��Ҳ����ȡ��Command Line
void drvfunc_get_processfullname( OUT PCHAR pszFullProcessName, IN ULONG uSize )
{
	ULONG uOsMajorVersion = 0;
	ULONG uOsMinorVersion = 0;
	DWORD dwAddress = 0;
	WCHAR wszProName[ MAX_PATH ] = {0};
	UNICODE_STRING usSource;
	ANSI_STRING asDest;

	if ( STATUS_SUCCESS != drvfunc_get_osversion( &uOsMajorVersion, &uOsMinorVersion ) )
		return;

	if ( PASSIVE_LEVEL != KeGetCurrentIrql() )
		return;

	dwAddress = (DWORD)PsGetCurrentProcess();
	if ( 0 == dwAddress || 0xFFFFFFFF == dwAddress )
		return;

	// Ŀǰֻ֧��Win 2000/xp/2003
	if ( uOsMajorVersion < 5 || uOsMinorVersion > 2 )
		return;

	// ȡ�� PEB����ͬƽ̨��λ���ǲ�ͬ�ġ�
	if ( uOsMajorVersion == 5 && uOsMinorVersion < 2 )
		dwAddress += BASE_PROCESS_PEB_OFFSET;
	else
		dwAddress += W2003_BASE_PROCESS_PEB_OFFSET;

	if ( (dwAddress = *(DWORD*)dwAddress) == 0 )
		return;

	// ͨ��pebȡ��RTL_USER_PROCESS_PARAMETERS
	dwAddress += BASE_PEB_PROCESS_PARAMETER_OFFSET;
	if ( (dwAddress = *(DWORD*)dwAddress) == 0 )
		return;

	// �� RTL_USER_PROCESS_PARAMETERS->ImagePathName ������·����ƫ��Ϊ38,
	dwAddress += BASE_PROCESS_PARAMETER_FULL_IMAGE_NAME;
	if ( (dwAddress = *(DWORD*)dwAddress) == 0 )
		return;

	// ..
	//_snwprintf( wszProName, sizeof(wszProName)-sizeof(WCHAR), "%s", (PCWSTR)dwAddress );	
	usSource.Buffer = (PWSTR)dwAddress;
	usSource.Length = wcslen((PWSTR)dwAddress)*sizeof(WCHAR);
	RtlUnicodeStringToAnsiString( &asDest, &usSource, TRUE );

	// ..
	_snprintf( pszFullProcessName, uSize-sizeof(char), "%s", asDest.Buffer );
}










/////////////////////////////////////////////////////////////////
//  ���:
//       	��ƽ̨��װ����
//  ����:
//         ����ָ���ֽڵ��ڴ棬��ȫ����0
//  ������
//         
//  ����ֵ��
//         != NULL	�ɹ�
//			== NULL û�з��䵽�ڴ�
/////////////////////////////////////////////////////////////////
//  �޸���ʷ:
//         ��ɭ�� 2004.10.21
/////////////////////////////////////////////////////////////////
PVOID drvfunc_malloc( ULONG MaxBuffer )
{
	PCHAR pNewPath	= NULL;

	pNewPath = (PCHAR)ExAllocatePool( NonPagedPool, MaxBuffer );
	if ( pNewPath )
	{
		RtlZeroMemory( pNewPath, MaxBuffer );
	}

	return pNewPath;
}


/////////////////////////////////////////////////////////////////
//  ���:
//       	��ƽ̨��װ����
//  ����:
//         �ͷ�ָ���ֽڵ��ڴ�
//  ������
//         
//  ����ֵ��
//
/////////////////////////////////////////////////////////////////
//  �޸���ʷ:
//         ��ɭ�� 2004.10.21
/////////////////////////////////////////////////////////////////
VOID drvfunc_free( PVOID pBuffer )
{
	if ( pBuffer )
	{
		ExFreePool( pBuffer );
		pBuffer = NULL;
	}
}

/////////////////////////////////////////////////////////////////
//  ���:
//       	��ƽ̨��װ����
//  ����:
//         ָ���ֽڵ��ڴ���Ϊ0
//  ������
//         
//  ����ֵ��
//
/////////////////////////////////////////////////////////////////
//  �޸���ʷ:
//         ��ɭ�� 2004.10.21
/////////////////////////////////////////////////////////////////
VOID drvfunc_zero_memory( PVOID pBuffer, int nLen )
{
	RtlZeroMemory( pBuffer, nLen );
}


/////////////////////////////////////////////////////////////////
//  ���:
//       	��ƽ̨��װ����
//  ����:
//         ����ָ���ֽڵ��ڴ�
//  ������
//         
//  ����ֵ��
//
/////////////////////////////////////////////////////////////////
//  �޸���ʷ:
//         ��ɭ�� 2004.10.21
/////////////////////////////////////////////////////////////////
VOID drvfunc_copy_memory( PVOID pDest, PVOID pSrc, int nLen )
{
	RtlCopyMemory( pDest, pSrc, nLen );
}



/////////////////////////////////////////////////////////////////
//  ���:
//         �ļ�ϵͳ����
//  ����:
//         ��Unicode �ַ���ת��ΪChar�ַ���
//	   ��ʱ���ǲ���
//  ������
//         
//  ����ֵ��
//         STATUS_SUCCESS	�ɹ�
/////////////////////////////////////////////////////////////////
NTSTATUS drvfunc_w2a( IN PWCHAR pSourceString, OUT PCHAR pDestString, IN USHORT nBufferLen )
{
	NTSTATUS status;
	UNICODE_STRING usSource;
	ANSI_STRING asDest;

	usSource.Buffer = pSourceString;
	usSource.Length = wcslen(pSourceString)*sizeof(WCHAR);
	
	status = RtlUnicodeStringToAnsiString( &asDest, &usSource, TRUE );
	if ( NT_SUCCESS(status) )
	{
		if ( asDest.Length < nBufferLen )
		{
			RtlCopyMemory( pDestString, asDest.Buffer, asDest.Length );
			pDestString[ asDest.Length ] = '\0';
		}
		else
		{
			pDestString[0] = '\0';
		}

		RtlFreeAnsiString( &asDest );
	}

	return status;
}

/////////////////////////////////////////////////////////////////
//  ���:
//         �ļ�ϵͳ����
//  ����:
//         �� WCHAR �ַ���ת��Ϊ CHAR �ַ���
//  ������
//         
//  ����ֵ��
//         STATUS_SUCCESS	�ɹ�
/////////////////////////////////////////////////////////////////
NTSTATUS drvfunc_w2c( IN PWCHAR pSourceString, OUT PCHAR pDestString, IN USHORT nBufferLen )
{
	NTSTATUS status;
	UNICODE_STRING usSource;
	ANSI_STRING asDest;
	
	usSource.Buffer		= pSourceString;
	usSource.Length		= wcslen(pSourceString)*sizeof(WCHAR);
	
	asDest.Length		= 0;
	asDest.Buffer		= pDestString;
	asDest.MaximumLength	= nBufferLen ;
	pDestString[0]		= '\0';
	
	status = RtlUnicodeStringToAnsiString( &asDest, &usSource, FALSE );
	if ( NT_SUCCESS(status) )
	{
		pDestString[ asDest.Length ] = '\0';
	}
	
	return status;
}
NTSTATUS drvfunc_w2c_ex( IN PWCHAR pwszSourceString, IN USHORT nSourceStringLength, OUT PCHAR pszDestString, IN USHORT nDestBufferLength )
{
	//
	//	pwszSourceString	- [in]  Դ�ַ���
	//	nSourceStringLength	- [in]  Դ�ַ������ȣ��� byte Ϊ��λ
	//	pszDestString		- [out] ����ַ���
	//	nDestBufferLength	- [in]  ����ַ������ȣ��� byte Ϊ��λ
	//	RETURN			- NTSTATUS
	//

	NTSTATUS status;
	UNICODE_STRING usSource;
	ANSI_STRING asDest;
	
	usSource.Buffer		= pwszSourceString;
	usSource.Length		= nSourceStringLength;
	
	asDest.Length		= 0;
	asDest.Buffer		= pszDestString;
	asDest.MaximumLength	= nDestBufferLength ;
	pszDestString[0]	= '\0';

	status = RtlUnicodeStringToAnsiString( &asDest, &usSource, FALSE );
	if ( NT_SUCCESS(status) )
	{
		pszDestString[ asDest.Length ] = '\0';
	}

	return status;
}

/////////////////////////////////////////////////////////////////
//  ���:
//         �ļ�ϵͳ����
//  ����:
//         �� UNICODE_STRING �ַ���ת��Ϊ CHAR �ַ���
//  ������
//         
//  ����ֵ��
//         STATUS_SUCCESS	�ɹ�
/////////////////////////////////////////////////////////////////
NTSTATUS drvfunc_u2c( IN PUNICODE_STRING pusSourceString, OUT PCHAR pDestString, IN USHORT nBufferLen )
{
	if ( NULL == pusSourceString || NULL == pDestString || 0 == nBufferLen )
	{
		return STATUS_UNSUCCESSFUL;
	}

	NTSTATUS status;
	ANSI_STRING asDest;
	
	//	Ŀ���ַ���
	asDest.Length		= 0;
	asDest.Buffer		= pDestString;
	asDest.MaximumLength	= nBufferLen ;
	pDestString[0]		= '\0';

	status = RtlUnicodeStringToAnsiString( &asDest, pusSourceString, FALSE );
	if ( NT_SUCCESS(status) )
	{
		pDestString[ asDest.Length ] = '\0';
	}

	return status;
}

/////////////////////////////////////////////////////////////////
//  ���:
//         �ļ�ϵͳ����
//  ����:
//         ��Char �ַ���ת��ΪUnicode�ַ���
//  ������
//         
//  ����ֵ��
//         STATUS_SUCCESS	�ɹ�
/////////////////////////////////////////////////////////////////
NTSTATUS drvfunc_c2w( IN PCHAR pSourceString, OUT PWCHAR pDestString, IN USHORT nBufferLen )
{
	NTSTATUS status;
	ANSI_STRING asSource;
	UNICODE_STRING usDest;

	asSource.Buffer		= pSourceString;
	asSource.Length		= strlen(pSourceString);

	usDest.Length		= 0;
	usDest.Buffer		= pDestString;
	usDest.MaximumLength	= nBufferLen ;
	pDestString[0]		= L'\0';

	status = RtlAnsiStringToUnicodeString( &usDest, &asSource, FALSE );
	if ( NT_SUCCESS(status) )
	{
		pDestString[ usDest.Length/2 ] = L'\0';
	}

	return status;
}

NTSTATUS drvfunc_a2u( IN PCHAR pSourceString, OUT PUNICODE_STRING pusDest )
{
	NTSTATUS status;
	ANSI_STRING asSource;

	asSource.Buffer = pSourceString;
	asSource.Length = strlen(pSourceString);

	return RtlAnsiStringToUnicodeString(pusDest, &asSource, TRUE);
}



/////////////////////////////////////////////////////////////////
// ����	�����ܺ���
// ����	������Key full pathname
ULONG drvfunc_get_fullkeyname( IN HANDLE hKey, IN PUNICODE_STRING pusSubKeyVal, IN PCHAR pFullKeyName )
{
	PVOID			pKey = NULL;
	ANSI_STRING		keyname;
	PUNICODE_STRING		pusKeyName;
	ULONG			actualLen;
	NTSTATUS		status;
	ULONG			nKeyLen = 0;
	ULONG			nRetBytes = 0;
 
	if( !pFullKeyName )
		return nRetBytes;

	//
	// If the fullname buffer is NULL, bail now
	//

	//
	// Is it a valid handle?
	//
	if ( pKey = drvfunc_get_pointer( hKey ))
	{
		//
		// See if we find the key in the hash table
		//
		drvfunc_release_pointer( pKey );

		pusKeyName = (PUNICODE_STRING) ExAllocateFromNPagedLookasideList(&g_FileNamePool);
		if ( !pusKeyName )
			return nRetBytes;
	
		//
		// We will only get here if key was created before we loaded - ask the Configuration
		// Manager what the name of the key is.
		//
		if ( pKey )
		{
			status = ObQueryNameString( pKey, (POBJECT_NAME_INFORMATION)pusKeyName, MAX_PATH, &actualLen );
			if ( NT_SUCCESS(status) )
			{
				if ( pusKeyName->Length > 0 )
				{
					status = RtlUnicodeStringToAnsiString( &keyname, pusKeyName, TRUE );
					if ( NT_SUCCESS(status) )
					{
						RtlCopyMemory(pFullKeyName, keyname.Buffer, keyname.Length);
						nKeyLen = keyname.Length;
						nRetBytes = nKeyLen;
					}
					RtlFreeAnsiString( &keyname );
				}
			}
		}
		ExFreeToNPagedLookasideList(&g_FileNamePool, pusKeyName );
	}

	//
	// Append subkey and value, if they are there
	//
	if ( pusSubKeyVal )
	{
		if ( (pusSubKeyVal->Buffer) && (pusSubKeyVal->Length > 0) )
		{
			keyname.Buffer = NULL;
			status = RtlUnicodeStringToAnsiString( &keyname, pusSubKeyVal, TRUE );
			if ( NT_SUCCESS(status) || (keyname.Length > 0) )
			{
				strcpy(pFullKeyName+nKeyLen, "\\");
				++nKeyLen;
				RtlCopyMemory(pFullKeyName+nKeyLen, keyname.Buffer, keyname.Length);
				nRetBytes += keyname.Length;
			}
			RtlFreeAnsiString( &keyname );
		}
	}
	return nRetBytes;
}


PCHAR drvfunc_get_win32_regpath( IN PCHAR pKeyName, IN OUT PCHAR* ppRoot )
{
	//////////////////////////////////
	// ע��Ŀǰֻ֧��������������
	// HKEY_CURRENT_USER
	// HKEY_CLASSES_ROOT
	// HKEY_LOCAL_MACHINE
	//////////////////////////////////
	PCHAR pSubKey = NULL;
	_strlwr( pKeyName );

	pSubKey = pKeyName;
	*ppRoot = "";
	//�ж�HKEY_CURRENT_USER
	if(strncmp(pKeyName, SYS_HKCU, SYS_HKCU_LEN) == 0)
	{
		pSubKey += SYS_HKCU_LEN;
		pSubKey += 1;
		while( (*pSubKey != '\\') && (*pSubKey != '\0'))
		{
			++pSubKey;
		}
		
		if(*pSubKey == '\0')
			return pKeyName;

		*ppRoot = "HKEY_CURRENT_USER";
	}
	else
	{
		//�ж�HKEY_CLASSES_ROOT
		if(strncmp(pKeyName, SYS_HKCR, SYS_HKCR_LEN) == 0)
		{
			pSubKey += SYS_HKCR_LEN;
			*ppRoot = "HKEY_CLASSES_ROOT";
		}
		else
		{
			//�ж�HKEY_LOCAL_MACHINE
			 if(strncmp(pKeyName, SYS_HKLM, SYS_HKLM_LEN) == 0)
			{
				pSubKey += SYS_HKLM_LEN;
				*ppRoot = "HKEY_LOCAL_MACHINE";
			}
			else
			{
				//�ж�"hkey_users\\.default"
				 if(strncmp(pKeyName, SYS_HKU, SYS_HKU_LEN) == 0)
				{
					pSubKey += SYS_HKU_LEN;
					*ppRoot = "HKEY_USERS\\.default";
				}
			}
		}
	}
	return pSubKey;
}


/////////////////////////////////////////////////////////////////
// ����		�����ܺ���
// ����		���������õľ��
// �������	: GetFullKeyName
PVOID drvfunc_get_pointer( IN HANDLE handle )
{
	PVOID pKey;

	// Ignore null handles
	if ( NULL == handle )
		return NULL;

	// Get the pointer the handle refers to
	if ( ObReferenceObjectByHandle( handle, 0, NULL, KernelMode, &pKey, NULL ) != STATUS_SUCCESS )
	{
		pKey = NULL;
	} 
	return pKey;
}

/////////////////////////////////////////////////////////////////
// ��������	�����ܺ���
// ����		���ݼ����õľ��
// �������	: GetFullKeyName
VOID drvfunc_release_pointer( IN PVOID object )
{
	if ( object )
	{
		ObDereferenceObject( object );
	}
}



//////////////////////////////////////////////////////////////////////////
//	for file


/**
 *	��һ��·�����ַ����м����ͷ��ʼ�����һ�� "/" or "\" ��λ��
 *	�Ӷ��õ�Ŀ¼�ĳ��ȣ��䳤�Ȱ������һ�� "\" �ĳ���
 */
INT drvfunc_get_dir_length( LPCSTR lpcszFilePath, UINT uFilePathLength )
{
	if ( NULL == lpcszFilePath || 0 == uFilePathLength )
	{
		return 0;
	}

	INT nDirLen;

	//
	//	�ҵ��ļ�ȫ·���У�Ŀ¼�Ľ�����
	//
	for ( nDirLen = ( uFilePathLength - sizeof(CHAR) ); nDirLen >= 0; nDirLen -- )
	{
		if ( '\\' == lpcszFilePath[ nDirLen ] || '/' == lpcszFilePath[ nDirLen ] )
		{
			//
			//	�ӼӺ���� Dir �ĳ�����
			//
			nDirLen ++;

			break;
		}
	}

	return nDirLen;
}

/**
 *	�ж��ļ��Ƿ����
 */
BOOL drvfunc_is_dir_exist( IN LPCWSTR lpcwszFilename )
{
	if ( NULL == lpcwszFilename )
	{
		return FALSE;
	}
	if ( wcslen(lpcwszFilename) <= 4 )
	{
		return FALSE;
	}
	
	BOOL bRet;
	HANDLE hFileHandle;
	OBJECT_ATTRIBUTES ObjectAttributes;
	UNICODE_STRING usFileName;
	IO_STATUS_BLOCK IoStatus;
	NTSTATUS ntStatus;
	FILE_STANDARD_INFORMATION StandardInfo;
	//WCHAR wszFilenameNew[ MAX_PATH ];
	PWCHAR pwszNewFileName	= NULL;
	
	//
	//	��ʼ������ֵ
	//
	bRet = FALSE;

	//
	//	L"\\??\\C:\\DeFirewall.log"
	//
	if ( 0 != wcsncmp( L"\\??\\", lpcwszFilename, 4 ) )
	{
		pwszNewFileName = (PWCHAR)ExAllocatePool( NonPagedPool, MAX_WPATH );
		if ( pwszNewFileName )
		{
			RtlZeroMemory( pwszNewFileName, MAX_WPATH );
			_snwprintf( pwszNewFileName, MAX_WPATH-sizeof(WCHAR), L"\\??\\%s", lpcwszFilename );
			RtlInitUnicodeString( &usFileName, pwszNewFileName );
		}
		else
		{
			return FALSE;
		}
	}
	else
	{
		RtlInitUnicodeString( &usFileName, lpcwszFilename );
	}
	InitializeObjectAttributes( &ObjectAttributes, &usFileName, OBJ_CASE_INSENSITIVE|OBJ_KERNEL_HANDLE, NULL, NULL );

	ntStatus = ZwCreateFile
		(
			&hFileHandle,			// returned file handle
			FILE_LIST_DIRECTORY,		// desired access
			&ObjectAttributes,		// ptr to object attributes
			&IoStatus,			// ptr to I/O status block
			NULL,				// alloc size = none
			FILE_ATTRIBUTE_NORMAL,		// �е����ʣ����ʧ�ܿ��Գ������� NULL
			FILE_SHARE_READ,
			FILE_OPEN,
			FILE_DIRECTORY_FILE,		// ԭ���� FILE_SYNCHRONOUS_IO_NONALERT �����޸ĳ� 
			NULL,				// eabuffer
			0				// ealength
		);
	if ( NT_SUCCESS(ntStatus) && NT_SUCCESS(IoStatus.Status) )
	{
		bRet = TRUE;

		ZwClose( hFileHandle );
	}

	//
	//	free memory
	//
	if ( pwszNewFileName )
	{
		ExFreePool( pwszNewFileName );
		pwszNewFileName = NULL;
	}
	
	return bRet;
}


/**
 *	��ȡ�ļ����ݵĳ���
 */
DWORD drvfunc_get_filesize( LPCWSTR lpcwszFilename )
{
	if ( NULL == lpcwszFilename )
	{
		return 0;
	}
	if ( 0 == wcslen(lpcwszFilename) )
	{
		return 0;
	}

	DWORD dwRet;
	HANDLE hFileHandle;
	OBJECT_ATTRIBUTES ObjectAttributes;
	UNICODE_STRING usFileName;
	IO_STATUS_BLOCK IoStatus;
	NTSTATUS ntStatus;
	FILE_STANDARD_INFORMATION StandardInfo;

	//
	//	��ʼ������ֵ
	//
	dwRet = 0;

	RtlInitUnicodeString( &usFileName, lpcwszFilename );
	InitializeObjectAttributes( &ObjectAttributes, &usFileName, OBJ_CASE_INSENSITIVE|OBJ_KERNEL_HANDLE, NULL, NULL );

	ntStatus = ZwCreateFile
		(
			&hFileHandle,			// returned file handle
			SYNCHRONIZE | FILE_READ_DATA,	// desired access
			&ObjectAttributes,		// ptr to object attributes
			&IoStatus,			// ptr to I/O status block
			NULL,				// alloc size = none
			FILE_ATTRIBUTE_NORMAL,
			FILE_SHARE_READ,
			FILE_OPEN,
			FILE_SYNCHRONOUS_IO_NONALERT,
			NULL,				// eabuffer
			0				// ealength
		);
	if ( NT_SUCCESS(ntStatus) && NT_SUCCESS(IoStatus.Status) )
	{
		ntStatus = ZwQueryInformationFile( hFileHandle, &IoStatus, &StandardInfo, sizeof(FILE_STANDARD_INFORMATION), FileStandardInformation );
		if ( NT_SUCCESS( ntStatus ) )
		{
			dwRet = StandardInfo.EndOfFile.u.LowPart;
		}
		
		ZwClose( hFileHandle );
	}

	return dwRet;
}


/**
 *	��ȡ�ļ�����
 */
BOOL drvfunc_get_filecontent( IN LPCWSTR lpcwszFilename, OUT LPSTR lpszContent, IN DWORD dwSize )
{
	if ( NULL == lpcwszFilename )
	{
		return FALSE;
	}
	if ( 0 == wcslen(lpcwszFilename) )
	{
		return FALSE;
	}
	if ( NULL == lpszContent || 0 == dwSize )
	{
		return FALSE;
	}
	
	BOOL bRet;
	HANDLE hFileHandle;
	OBJECT_ATTRIBUTES ObjectAttributes;
	UNICODE_STRING usFileName;
	IO_STATUS_BLOCK IoStatus;
	NTSTATUS ntStatus;
	FILE_POSITION_INFORMATION filePos;
	FILE_STANDARD_INFORMATION StandardInfo;
	

	//
	//	��ʼ������ֵ
	//
	bRet	= FALSE;


	RtlInitUnicodeString( &usFileName, lpcwszFilename );
	InitializeObjectAttributes( &ObjectAttributes, &usFileName, OBJ_CASE_INSENSITIVE|OBJ_KERNEL_HANDLE, NULL, NULL );
	
	ntStatus = ZwCreateFile
		(
			&hFileHandle,			// returned file handle
			SYNCHRONIZE | FILE_READ_DATA,	// desired access
			&ObjectAttributes,		// ptr to object attributes
			&IoStatus,			// ptr to I/O status block
			NULL,				// alloc size = none
			FILE_ATTRIBUTE_NORMAL,
			FILE_SHARE_READ,
			FILE_OPEN,
			FILE_SYNCHRONOUS_IO_NONALERT,
			NULL,				// eabuffer
			0				// ealength
		);
	if ( NT_SUCCESS(ntStatus) && NT_SUCCESS(IoStatus.Status) )
	{
		ntStatus = ZwQueryInformationFile( hFileHandle, &IoStatus, &StandardInfo, sizeof(FILE_STANDARD_INFORMATION), FileStandardInformation );
		if ( NT_SUCCESS( ntStatus ) && StandardInfo.EndOfFile.QuadPart > 0 )
		{
			ntStatus = ZwReadFile
				(
					hFileHandle, NULL, NULL, NULL,
					&IoStatus,
					lpszContent,
					( dwSize - sizeof(CHAR) ),
					NULL, NULL
				);
			if ( NT_SUCCESS(ntStatus) && IoStatus.Information == ( dwSize - sizeof(CHAR) ) )
			{
				bRet = TRUE;
			}
		}
		else
		{
			KdPrint(("procconfig_load_protected_dir: cann't get file size,status=0x%4x\n", ntStatus));
		}
		
		ZwClose( hFileHandle );
	}
	else
	{
		KdPrint( ("procconfig_load_protected_dir: Open %ws is fail, status=0x%4x\n",LOG_FILEPATH, ntStatus) );
	}
	
	return bRet;
}

/**
 *	�����ļ������ȡ�ļ� DOS ȫ·��
 *	UNICODE_STRING
 */
BOOL drvfunc_get_filename_by_handle( IN HANDLE hFileHandle, OUT UNICODE_STRING * pusFilename )
{
	if ( NULL == hFileHandle || NULL == pusFilename )
	{
		return FALSE;
	}

	BOOL bRet = FALSE;
	NTSTATUS ntStatus;

	PWCHAR pBuffer;
	PFILE_OBJECT pstFileObject;
	POBJECT_NAME_INFORMATION pstObjectNameInfo;

	ntStatus = ObReferenceObjectByHandle( hFileHandle, FILE_READ_DATA, *IoFileObjectType, KernelMode, (void **)&pstFileObject, NULL );
	if ( NT_SUCCESS( ntStatus ) )
	{
		ntStatus = IoQueryFileDosDeviceName( pstFileObject, &pstObjectNameInfo );
		if ( NT_SUCCESS( ntStatus ) )
		{
			//pusFilename->Length = 0;
			RtlCopyUnicodeString( pusFilename, & pstObjectNameInfo->Name );

			bRet = TRUE;
		}
	}

	return bRet;
}

/**
 *	�����ļ������ȡ�ļ� DOS ȫ·��
 *	Ansi
 */
UINT drvfunc_get_ansifilename_by_handle( IN HANDLE hFileHandle, OUT PCHAR pszFilename, IN USHORT uSize )
{
	//
	//	hFileHandle	- [in]  �ļ����
	//	pszFilename	- [out] ���ػ�����
	//	uSize		- [in]  ���ػ������Ĵ�С
	//	RETURN		- ����ɹ����򷵻ؿ������ļ�������
	//

	if ( NULL == hFileHandle || NULL == pszFilename || 0 == uSize )
	{
		return 0;
	}

	UINT uRet	= 0;
	NTSTATUS ntStatus;
	
	PWCHAR pBuffer;
	PFILE_OBJECT pstFileObject;
	POBJECT_NAME_INFORMATION pstObjectNameInfo;

	ntStatus = ObReferenceObjectByHandle( hFileHandle, FILE_READ_DATA, *IoFileObjectType, KernelMode, (void **)&pstFileObject, NULL );
	if ( NT_SUCCESS( ntStatus ) )
	{
		ntStatus = IoQueryFileDosDeviceName( pstFileObject, &pstObjectNameInfo );
		if ( NT_SUCCESS( ntStatus ) )
		{
			ntStatus = drvfunc_w2a( pstObjectNameInfo->Name.Buffer, pszFilename, uSize );
			if ( NT_SUCCESS( ntStatus ) )
			{
				//
				//	���ת���ɹ�����ô����һ���ַ����ĳ���
				//
				uRet = strlen(pszFilename);
			}
		}
	}
	
	return uRet;
}


/**
 *	ɾ���ļ�
 */
BOOL drvfunc_delete_file( IN UNICODE_STRING * pusFilename )
{
	//
	//	pusFilename	- �ļ�����L"\\??\\h:\\aa"
	//	RETURN		- TRUE / FALSE
	//

	if ( ! pusFilename->Buffer || 0 == pusFilename->Length )
	{
		return FALSE;
	}

	OBJECT_ATTRIBUTES ObjectAttributes;

	InitializeObjectAttributes( &ObjectAttributes, pusFilename, OBJ_CASE_INSENSITIVE|OBJ_KERNEL_HANDLE, NULL, NULL );

	return NT_SUCCESS( ZwDeleteFile( &ObjectAttributes ) );
}




//////////////////////////////////////////////////////////////////////////
//	for string


/**
 *	��ȡָ�����Ƶ� Cookie Value( �����ִ�Сд )
 */
UINT drvfunc_get_casecookie_value( LPCSTR lpszCookieString, LPCSTR lpszCookieName, LPCSTR lpcszEndStr, LPSTR lpszValue, DWORD dwSize )
{
	//
	//	lpszCookieString	- [in]  ���� COOKIE ���ַ���
	//	lpszCookieName		- [in]  COOKIE ���ƣ����Բ����ִ�Сд�Ĳ���
	//	lpcszEndStr		- [in]  ������β
	//	lpszValue		- [out] ����ֵ
	//	dwSize			- [in]  ����ֵ��������С
	//	RETURN			- TRUE / FALSE
	//
	//	for example:
	//		drvfunc_get_casecookie_value( "name=xing;", "xing=", ";", szValue, sizeof(szValue) );
	//
	
	if ( NULL == lpszCookieString || NULL == lpszCookieName || NULL == lpcszEndStr || NULL == lpszValue )
	{
		return 0;
	}
	if ( 0 == strlen(lpszCookieString) || 0 == strlen(lpszCookieName) || 0 == strlen(lpcszEndStr) )
	{
		return 0;
	}
	
	UINT    uRetLen;
	LPCTSTR	lpszHead;
	LPTSTR	lpszMove;
	UINT	uStrLen;
	UINT    uCKNameLen;
	UINT    i;

	uRetLen		= 0;
	uStrLen		= strlen( lpszCookieString );
	uCKNameLen	= strlen( lpszCookieName );
	
	for ( i = 0; i < uStrLen; i ++ )
	{
		lpszHead = lpszCookieString + i;
		if ( 0 == _strnicmp( lpszHead, lpszCookieName, uCKNameLen ) )
		{
			lpszHead += uCKNameLen;
			if ( lpszHead )
			{
				//
				//	�������ݵ�����ֵ������
				//
				_snprintf( lpszValue, dwSize-sizeof(TCHAR), "%s", lpszHead );

				//
				//	����β��
				//
				lpszMove = strstr( lpszValue, lpcszEndStr );
				if ( lpszMove )
				{
					*lpszMove = 0;
				}

				uRetLen = strlen(lpszValue);

				break;
			}
		}
	}
	
	return uRetLen;
}