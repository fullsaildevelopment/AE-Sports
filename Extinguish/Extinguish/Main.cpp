
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

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
{
	Application app;
	bool run = true;

	app.Init(hInstance, hPrevInstance, lpCmdLine, nCmdShow);

#if _DEBUG
	AllocConsole();

	HANDLE handle_out = GetStdHandle(STD_OUTPUT_HANDLE);
	int hCrt = _open_osfhandle((intptr_t)handle_out, _O_TEXT);
	FILE* hf_out = _fdopen(hCrt, "w");
	setvbuf(hf_out, NULL, _IONBF, 1);
	*stdout = *hf_out;

	freopen_s(&hf_out, "CONOUT$", "w", stdout);
#endif
	float fixedTime = 0;
	XTime timer;
	timer.Restart();
	timer.Signal();
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