
#include "Includes.h"
//#include <windows.h>
#include <windowsx.h>
//#include <vld.h>
#include "Application.h"
#include "XTime.h"
#include <iostream>
#include <stdio.h>
#include <io.h>
#include <fcntl.h>
#include <Windows.h>
#include <stdio.h>
#include <DbgHelp.h>

#pragma comment(lib, "dbghelp.lib")

//forward declarations
LONG WINAPI errorFunc(_EXCEPTION_POINTERS *pExceptionInfo);
void Potato();

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
{
	Application app;
	bool run = true;

	app.Init(hInstance, hPrevInstance, lpCmdLine, nCmdShow);

//#if _DEBUG
	AllocConsole();

	HANDLE handle_out = GetStdHandle(STD_OUTPUT_HANDLE);
	int hCrt = _open_osfhandle((intptr_t)handle_out, _O_TEXT);
	FILE* hf_out = _fdopen(hCrt, "w");
	setvbuf(hf_out, NULL, _IONBF, 1);
	*stdout = *hf_out;

	freopen_s(&hf_out, "CONOUT$", "w", stdout);
//#endif

	float fixedTime = 0;
	XTime timer;
	timer.Restart();
	timer.Signal();

	printf("Testing\n");
	//set up function that will create dump file
	SetUnhandledExceptionFilter(errorFunc);

	while (run)
	{
		run = app.Update(timer.Delta());

		fixedTime += timer.Delta();

		//if (fixedTime >= 0.02f)
		{
			app.FixedUpdate(fixedTime);
			fixedTime = 0;
		}

		timer.Signal();
	}

	app.Shutdown();

	return 0;
}

LONG WINAPI errorFunc(_EXCEPTION_POINTERS *pExceptionInfo)
{
	HANDLE hFile = ::CreateFile("dumpfile.mdmp", GENERIC_WRITE, FILE_SHARE_WRITE, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);

	if (hFile != INVALID_HANDLE_VALUE)
	{
		_MINIDUMP_EXCEPTION_INFORMATION ExInfo;

		ExInfo.ThreadId = ::GetCurrentThreadId();
		ExInfo.ExceptionPointers = pExceptionInfo;
		ExInfo.ClientPointers = NULL;
		MiniDumpWriteDump(GetCurrentProcess(), GetCurrentProcessId(), hFile, MiniDumpNormal, &ExInfo, NULL, NULL);
		//MessageBox("Dump File Saved look x directory please email to developer at the following email adress crashdmp@gmail.com with the subject Gamename - Version ");
		::CloseHandle(hFile);
	}

	return 0;
}