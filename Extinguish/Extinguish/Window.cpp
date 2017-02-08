#include "Window.h"
#include <iostream>

//function prototype
LRESULT CALLBACK WindowProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam);
LRESULT CALLBACK InputProc(HWND _hWnd, UINT _msg, WPARAM _w, LPARAM _l);

void Window::Init(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
{
	RegisterWindowClass(hInstance, hPrevInstance, lpCmdLine, nCmdShow);
	CreateWind(hInstance);
	ShowWind(nCmdShow);
}

extern LRESULT ImGui_ImplDX11_WndProcHandler(HWND hWnd, UINT msg);// , WPARAM wParam, LPARAM lParam);
int Window::Update(InputManager* input)
{
	MSG msg;
	bool handledMsg = true;


	while (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
	{
		ImGui_ImplDX11_WndProcHandler(Window::GetHWND(), msg.message);// , wParam, lParam);
		switch (msg.message)
		{
			case (WM_KEYDOWN):
				input->SetKeyboardKey((unsigned int)msg.wParam, true);
				break;
			case (WM_KEYUP):
				input->SetKeyboardKey((unsigned int)msg.wParam, false);
				break;
			case (WM_SYSKEYDOWN):
				input->SetKeyboardKey((unsigned int)msg.wParam, true);
				break;
			case (WM_SYSKEYUP):
				input->SetKeyboardKey((unsigned int)msg.wParam, false);
				break;
			case (WM_LBUTTONDOWN):
				input->SetMouseButtons(0, true);
				break;
			case (WM_LBUTTONUP):
				input->SetMouseButtons(0, false);
				break;
			case (WM_MBUTTONDOWN):
				input->SetMouseButtons(1, true);
				break;
			case (WM_MBUTTONUP):
				input->SetMouseButtons(1, false);
				break;
			case (WM_RBUTTONDOWN):
				input->SetMouseButtons(2, true);
				break;
			case (WM_RBUTTONUP):
				input->SetMouseButtons(2, false);
				break;
			case (WM_MOUSEMOVE):
				input->SetMousePosition(msg.lParam);
				break;
			default:
				handledMsg = false;
				break;
		}

		//std::cout << handledMsg << std::endl;

		if (!handledMsg) //let windows handle message
		{
			//send message to windowproc
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
	}

	//std::cout << "Window";

	return msg.message;

	//MSG msg;
	//ZeroMemory(&msg, sizeof(MSG));

	//while (PeekMessage(&msg, NULL, 0U, 0U, PM_REMOVE))
	//{
	//	TranslateMessage(&msg);
	//	DispatchMessage(&msg);
	//}

	//return 1;
}

void Window::RegisterWindowClass(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
{
	//clear out window class
	ZeroMemory(&wc, sizeof(WNDCLASSEX));

	//fill in window class struct
	wc.cbSize = sizeof(WNDCLASSEX);
	wc.style = CS_HREDRAW | CS_VREDRAW; //so if we move the window vertically or horizontally, it will be redrawn
	wc.lpfnWndProc = WindowProc; //points to the window procedure, which will handle messages
	wc.hInstance = hInstance;
	wc.hCursor = LoadCursor(NULL, IDC_ARROW); //loads the cursor image
	//wc.hbrBackground = CreateSolidBrush(RGB(0, 0, 255)); //if we ever want a more custom background
	//wc.hbrBackground = (HBRUSH)COLOR_WINDOW; //I'm not sure if it's needed because directx render target view will color window
	wc.lpszClassName = "WindowClass1";

	//register the window class
	RegisterClassEx(&wc);
}

void Window::CreateWind(HINSTANCE hInstance)
{
	RECT wr = { 0, 0, CLIENT_WIDTH, CLIENT_HEIGHT };
	AdjustWindowRect(&wr, WS_OVERLAPPEDWINDOW, FALSE); // This function takes in client size (where we want to draw) and returns rect that is window size (includes bars)

	hwnd = CreateWindowEx(NULL, "WindowClass1", "AE Sports", WS_OVERLAPPEDWINDOW, 300, 25, wr.right - wr.left, wr.bottom - wr.top, NULL, NULL, hInstance, NULL);

	bool result;

	if (!hwnd)
	{
		result = false;
	}
}

void Window::ShowWind(int nCmdShow)
{
	ShowWindow(hwnd, nCmdShow);
}

LRESULT CALLBACK WindowProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	//InputProc(hwnd, message, wParam, lParam);

	//std::cout << "Proc";

	switch (message)
	{
	case WM_DESTROY:
		PostQuitMessage(0);
		return 0;
		break;
	case WM_SIZE: //lParam holds Width and Height as first 16 bits = width and last are height
		uint16_t w = lParam;
		uint16_t h = lParam >> 16;
		break;
		//default:
		//	DefWindowProc(hwnd, message, wParam, lParam);
		//	break;
	}
	//std::cout << "Down: Held Down? " << ((lParam & (1 << 30)) >> 30) << "\n";

	return DefWindowProc(hwnd, message, wParam, lParam);
}

LRESULT CALLBACK InputProc(HWND _hWnd, UINT msg, WPARAM _w, LPARAM _l)
{
	//MSG msg;
	bool handledMsg = true;

	InputManager* input = InputManager::GetSingleton();

	//if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
	{
		switch (msg)
		{
		case (WM_KEYDOWN):
			input->SetKeyboardKey(msg, true);
			break;
		case (WM_KEYUP):
			input->SetKeyboardKey(msg, false);
			break;
		case (WM_SYSKEYDOWN):
			input->SetKeyboardKey(msg, true);
			break;
		case (WM_SYSKEYUP):
			input->SetKeyboardKey(msg, false);
			break;
		case (WM_LBUTTONDOWN):
			input->SetMouseButtons(0, true);
			break;
		case (WM_LBUTTONUP):
			input->SetMouseButtons(0, false);
			break;
		case (WM_MBUTTONDOWN):
			input->SetMouseButtons(1, true);
			break;
		case (WM_MBUTTONUP):
			input->SetMouseButtons(1, false);
			break;
		case (WM_RBUTTONDOWN):
			input->SetMouseButtons(2, true);
			break;
		case (WM_RBUTTONUP):
			input->SetMouseButtons(2, false);
			break;
		case (WM_MOUSEMOVE):
			input->SetMousePosition(msg);
			break;
		default:
			return 1;
			//handledMsg = false;
			break;
		}

		//std::cout << handledMsg << std::endl;

		//if (!handledMsg) //let windows handle message
		//{
		//	//send message to windowproc
		//	TranslateMessage(&msg);
		//	DispatchMessage(&msg);
		//}
	}

	std::cout << msg << endl;

	return 0;
}

//getters
const HWND Window::GetHWND()
{
	return hwnd;
}