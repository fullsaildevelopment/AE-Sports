#pragma once
//libraries
//#pragma comment(lib,"ws2_32")
#pragma comment (lib, "d3d11.lib")
#pragma comment (lib, "d3dcompiler.lib")
#pragma comment(lib, "d2d1.lib")
#pragma comment(lib, "dwrite.lib")
#pragma comment(lib, "windowscodecs.lib")

//includes
#include <WinSock2.h>
#include <Windows.h>
#include <Wincodec.h>
#include <d3d11.h>
//#include <d2d1.h>
//#include <d2d1_1.h>
#include <d2d1_2.h>
#include <dwrite.h>
//#include <dwrite_1.h>
#include <dwrite_2.h>
#include <d2d1effects.h>
#include <iostream>
#include <wrl\client.h>
#include <DirectXMath.h>
#include <DirectXColors.h>
#include <vector>
#include <string>
#include <d3dcompiler.h> //needed to compile shaders
#include <time.h>
#include <DXGIDebug.h>
#include <atlbase.h>
//#include <vld.h> 
#include <stdlib.h>
#include "DDSTextureLoader.h"	 
#include "../ShaderStructures/ShaderStructures.h"

#include "..\Bin\imgui\imgui.h"
#include "..\Bin\imgui\imgui_internal.h"
#include "..\Bin\imgui\imgui_impl_dx11.h"

//defines
#define CLIENT_WIDTH 1000
#define CLIENT_HEIGHT 800
#define DEBUG_GRAPHICS 1

//enums
enum VertexShaderTypes
{
	vsBASIC = 0,
	vsBIND,
	vsDEPTHPREPASS,
	vsDEPTHSHADOW,
	vsSHADOW
};
enum PixelShaderTypes
{
	psBASIC = 0,
	psNORMALMAPPED,
	psDEPTHSHADOW,
	psSHADOW,
};

using namespace std;