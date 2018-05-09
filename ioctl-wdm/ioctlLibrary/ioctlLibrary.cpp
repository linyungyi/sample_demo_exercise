// ioctlLibrary.cpp : 定義 DLL 應用程式的匯出函式。
//

#include "stdafx.h"
#include <iostream>
#include "ioctlLibrary.h"

#include <WINIOCTL.h>
#include "..\sys\sioctl.h"

extern "C"
{
	IOCTLDLL_API void Desc(void)
	{
		printf("This library is powered by Albert\n");
	}

	IOCTLDLL_API HANDLE OpenDevice(char* device)
	{
		HANDLE hDevice;

		printf("device : %s\n", device);

		hDevice = CreateFile( device,
			GENERIC_READ | GENERIC_WRITE,
			0,
			NULL,
			CREATE_ALWAYS,
			FILE_ATTRIBUTE_NORMAL,
			NULL);

		if (hDevice == INVALID_HANDLE_VALUE) {
			printf("Error: CreatFile Failed : %d\n", GetLastError());
			return NULL;
		}

		return hDevice;
	}

	IOCTLDLL_API void CloseDevice(HANDLE hd)
	{
		CloseHandle(hd);
	}

	IOCTLDLL_API int ReadRegister(HANDLE hDevice, BYTE* inBuf, int inSize, BYTE* outBuf, int outSize)
	{
		int ret;
		char OutputBuffer[32];
		char InputBuffer[32];
		BOOL bRc;
		ULONG bytesReturned;

		memcpy(InputBuffer, inBuf, inSize);

		bRc = DeviceIoControl(hDevice,
			(DWORD)IOCTL_SIOCTL_READ_REGISTER,
			InputBuffer,
			inSize,
			OutputBuffer,
			outSize,
			&bytesReturned,
			NULL
			);

		if (!bRc)
		{
			printf("Error in DeviceIoControl : %d %08x", GetLastError(), IOCTL_SIOCTL_READ_REGISTER);
			return 0;

		}

		memcpy(outBuf, OutputBuffer, bytesReturned);

		ret = bytesReturned;

		return ret;
	}

	IOCTLDLL_API int WriteRegister(HANDLE hDevice, BYTE* inBuf, int inSize)
	{
		int ret;
		char OutputBuffer[32];
		char InputBuffer[32];
		BOOL bRc;
		ULONG bytesReturned;

		memcpy(InputBuffer, inBuf, inSize);

		printf("WriteRegister set %02x\n", (BYTE)inBuf[0]);

		bRc = DeviceIoControl(hDevice,
			(DWORD)IOCTL_SIOCTL_WRITE_REGISTER,
			InputBuffer,
			inSize,
			OutputBuffer,
			sizeof(OutputBuffer),
			&bytesReturned,
			NULL
			);

		if (!bRc)
		{
			printf("Error in DeviceIoControl : %d %08x", GetLastError(), IOCTL_SIOCTL_WRITE_REGISTER);
			return 0;

		}

		printf("WriteRegister return %02x\n", (BYTE)OutputBuffer[0]);

		ret = bytesReturned;

		return ret;
	}

	IOCTLDLL_API int Interrupt(HANDLE hDevice, int iAction)
	{
		int ret;
		char OutputBuffer[32];
		char InputBuffer[32];
		BOOL bRc;
		ULONG bytesReturned;

		if (iAction)
			InputBuffer[0] = 0x1;
		else
			InputBuffer[0] = 0x0;

		bRc = DeviceIoControl(hDevice,
			(DWORD)IOCTL_SIOCTL_INTERRUPT,
			InputBuffer,
			1,
			OutputBuffer,
			sizeof(OutputBuffer),
			&bytesReturned,
			NULL
			);

		if (!bRc)
		{
			printf("Error in DeviceIoControl : %d %08x", GetLastError(), IOCTL_SIOCTL_INTERRUPT);
			return 0;

		}

		printf("Interrupt return OutputBuffer %02x\n", (BYTE)OutputBuffer[0]);

		ret = bytesReturned;

		return ret;
	}

	IOCTLDLL_API int Buffered(HANDLE hDevice, char* inBuf, int inSize, char* outBuf, int outSize)
	{
		int ret=0;
		BOOL bRc;
		ULONG bytesReturned;

		bRc = DeviceIoControl(hDevice,
			(DWORD)IOCTL_SIOCTL_BUFFERED,
			inBuf,
			inSize,
			outBuf,
			outSize,
			&bytesReturned,
			NULL
			);

		if (!bRc)
		{
			printf("Error in DeviceIoControl : %d %08x", GetLastError(), IOCTL_SIOCTL_BUFFERED);
			return 0;

		}

		ret = bytesReturned;

		return ret;
	}

}
