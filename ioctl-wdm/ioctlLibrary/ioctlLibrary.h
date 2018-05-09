#pragma once

#ifdef IOCTLLIBRARY_EXPORTS
#define IOCTLDLL_API __declspec(dllexport) 
#else
#define IOCTLDLL_API __declspec(dllimport) 
#endif

extern "C"
{
	IOCTLDLL_API void Desc(void);
	IOCTLDLL_API HANDLE OpenDevice(char* device);
	IOCTLDLL_API void CloseDevice(HANDLE hd);
	IOCTLDLL_API int ReadRegister(HANDLE hDevice, BYTE* inBuf, int inSize, BYTE* outBuf, int outSize);
	IOCTLDLL_API int WriteRegister(HANDLE hDevice, BYTE* inBuf, int inSize);
	IOCTLDLL_API int Interrupt(HANDLE hDevice, int iAction);
	IOCTLDLL_API int Buffered(HANDLE hDevice, char* inBuf, int inSize, char* outBuf, int outSize);
}