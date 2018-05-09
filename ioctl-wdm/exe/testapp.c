/*++

Copyright (c) 1990-98  Microsoft Corporation All Rights Reserved

Module Name:

    testapp.c

Abstract:

Environment:

    Win32 console multi-threaded application

--*/
#include <windows.h>
#include <winioctl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <strsafe.h>
#include <sys\sioctl.h>

typedef void (*DESC_FUNC)(void);
typedef HANDLE (*OPEN_DEV_FUNC)(char*);
typedef void (*CLOSE_DEV_FUNC)(HANDLE);
typedef int (*READ_REG_FUNC)(HANDLE, BYTE*, int, BYTE*, int);
typedef int (*WRITE_REG_FUNC)(HANDLE, BYTE*, int);
typedef int (*INT_FUNC)(HANDLE, int);
typedef int(*BUFFER_FUNC)(HANDLE, char*, int, char*, int);

DESC_FUNC call_func;
OPEN_DEV_FUNC open_func;
CLOSE_DEV_FUNC close_func;
READ_REG_FUNC read_reg_func;
WRITE_REG_FUNC write_reg_func;
INT_FUNC int_func;
BUFFER_FUNC buffer_func;

// x:regisers y:number z:status
#define SET_GPIO(x,y,z) ( z==1?x|(0x1<<(y-1)):(x&~(0x1<<y)))
#define OFF_GPIO(x) ( x & 0x80 )

BOOLEAN
ManageDriver(
    _In_ LPCTSTR  DriverName,
    _In_ LPCTSTR  ServiceName,
    _In_ USHORT   Function
    );

BOOLEAN
SetupDriverName(
    _Inout_updates_bytes_all_(BufferLength) PCHAR DriverLocation,
    _In_ ULONG BufferLength
    );

char OutputBuffer[100];
char InputBuffer[100];

BYTE outReg[8];
BYTE inReg[8];

VOID __cdecl
main(
    _In_ ULONG argc,
    _In_reads_(argc) PCHAR argv[]
    )
{
    HANDLE hDevice;
    TCHAR driverLocation[MAX_PATH];
	int iReturn;

    UNREFERENCED_PARAMETER(argc);
    UNREFERENCED_PARAMETER(argv);

	//
	// The driver is not started yet so let us the install the driver.
	// First setup full path to driver name.
	//
	
	if (!SetupDriverName(driverLocation, sizeof(driverLocation))) {

		return;
	}

	if (!ManageDriver(DRIVER_NAME,
		driverLocation,
		DRIVER_FUNC_INSTALL
		)) {

		printf("Unable to install driver. \n");

		//
		// Error - remove driver.
		//

		ManageDriver(DRIVER_NAME,
			driverLocation,
			DRIVER_FUNC_REMOVE
			);

		return;
	}
	
	// Load the DLL
	HINSTANCE hinstLib = LoadLibrary("ioctlLibrary.dll");
	if (hinstLib == NULL)
	{
		printf("Unable to load library\n");
		return;
	}

	// Get the function pointer

	call_func = (DESC_FUNC)GetProcAddress(hinstLib, "Desc");
	if (call_func == NULL)
	{
		FreeLibrary(hinstLib);
		printf("Unable to load function\n");
		return;
	}

	open_func = (OPEN_DEV_FUNC)GetProcAddress(hinstLib, "OpenDevice");
	if (open_func == NULL)
	{
		FreeLibrary(hinstLib);
		printf("Unable to load function\n");
		return;
	}

	close_func = (CLOSE_DEV_FUNC)GetProcAddress(hinstLib, "CloseDevice");
	if (close_func == NULL)
	{
		FreeLibrary(hinstLib);
		printf("Unable to load function\n");
		return;
	}

	read_reg_func = (READ_REG_FUNC)GetProcAddress(hinstLib, "ReadRegister");
	if (read_reg_func == NULL)
	{
		FreeLibrary(hinstLib);
		printf("Unable to load function\n");
		return;
	}

	write_reg_func = (WRITE_REG_FUNC)GetProcAddress(hinstLib, "WriteRegister");
	if (write_reg_func == NULL)
	{
		FreeLibrary(hinstLib);
		printf("Unable to load function\n");
		return;
	}

	int_func = (INT_FUNC)GetProcAddress(hinstLib, "Interrupt");
	if (int_func == NULL)
	{
		FreeLibrary(hinstLib);
		printf("Unable to load function\n");
		return;
	}

	buffer_func = (BUFFER_FUNC)GetProcAddress(hinstLib, "Buffered");
	if (buffer_func == NULL)
	{
		FreeLibrary(hinstLib);
		printf("Unable to load function\n");
		return;
	}

	call_func();

	hDevice = open_func("\\\\.\\IoctlTest");

	printf("\nTurn all GPIO off:\n");
	inReg[0] = OFF_GPIO(inReg[0]);
	iReturn = write_reg_func(hDevice, inReg, 1);

	printf("\nset GPIO5 on:\n");
	inReg[0] = SET_GPIO(inReg[0], 5, 1);
	iReturn = write_reg_func(hDevice, inReg, 1);

	printf("\nset GPIO3 on:\n");
	inReg[0] = SET_GPIO(inReg[0], 3, 1);
	iReturn = write_reg_func(hDevice, inReg, 1);

	iReturn = read_reg_func(hDevice, inReg, 1, outReg, 1);
	printf("\nREAD GPIO state: %02x\n", outReg[0]);

	printf("\nset INT on:\n");
	iReturn = int_func(hDevice, 1);

	printf("\nset INT off:\n");
	iReturn = int_func(hDevice, 0);

	//// buffer test
	StringCbCopy(InputBuffer, sizeof(InputBuffer),
		"This String is from User space;");

	printf("\nCalling DeviceIoControl METHOD_BUFFERED:\n");

	memset(OutputBuffer, 0, sizeof(OutputBuffer));

	iReturn = buffer_func(hDevice, InputBuffer, (int)strlen(InputBuffer) + 1, OutputBuffer, sizeof(OutputBuffer));
	printf("    OutBuffer (%d): %s\n", iReturn, OutputBuffer);
	//// end of buffer test

	close_func(hDevice);

	ManageDriver(DRIVER_NAME,
		driverLocation,
		DRIVER_FUNC_REMOVE
		);
		

}


