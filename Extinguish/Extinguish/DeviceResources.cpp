#include "DeviceResources.h"

void DeviceResources::Init(HWND hwnd)
{
	windowHandle = hwnd;
	//create swapchainDesc
	DXGI_SWAP_CHAIN_DESC swapChainDesc;

	ZeroMemory(&swapChainDesc, sizeof(DXGI_SWAP_CHAIN_DESC));

	//swapChainDesc.BufferDesc = braynsBufferDesc;
	swapChainDesc.Windowed = true;
	swapChainDesc.BufferCount = 1;
	swapChainDesc.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	swapChainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
	swapChainDesc.OutputWindow = hwnd;
	swapChainDesc.SampleDesc.Count = 1;
	swapChainDesc.SampleDesc.Quality = 0;
	swapChainDesc.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;

	//create swapchain and device
	UINT flags = D3D11_CREATE_DEVICE_BGRA_SUPPORT;

#if (_DEBUG)
	{
		flags |= D3D11_CREATE_DEVICE_DEBUG;
	}
#endif
	const D3D_FEATURE_LEVEL featureLevels[] =
	{
		D3D_FEATURE_LEVEL_11_1,
		D3D_FEATURE_LEVEL_11_0,
		D3D_FEATURE_LEVEL_10_1,
		D3D_FEATURE_LEVEL_10_0,
		D3D_FEATURE_LEVEL_9_3,
		D3D_FEATURE_LEVEL_9_2,
		D3D_FEATURE_LEVEL_9_1,
	};


	D3D_FEATURE_LEVEL m_featureLevel;


	HRESULT swapResult = D3D11CreateDeviceAndSwapChain(NULL, D3D_DRIVER_TYPE_HARDWARE, NULL, flags, featureLevels, ARRAYSIZE(featureLevels), D3D11_SDK_VERSION, &swapChainDesc, swapChain.GetAddressOf(), device.GetAddressOf(), &m_featureLevel, deviceContext.GetAddressOf());

	/*IDXGIFactory1 *pFactory = NULL;

	swapChain->GetParent(__uuidof (IDXGIFactory1), (void **)&pFactory);
	if (pFactory)
	{
		pFactory->MakeWindowAssociation(hwnd, DXGI_MWA_NO_ALT_ENTER);
		pFactory->Release();
	}*/

	HRESULT scBufferResult = swapChain.Get()->GetBuffer(0, __uuidof(ID3D11Texture2D), (LPVOID*)swapChainBuffer.GetAddressOf()); //this returns address of back buffer in swapChain

	Microsoft::WRL::ComPtr<IDXGIDevice> dxgiDevice;
	HRESULT res;

	if (!DEBUG_GRAPHICS) 
	{
		res = device.As(&dxgiDevice);

		// res = device.Get()->QueryInterface(__uuidof(IDXGIDevice), (void**)dxgiDevice.GetAddressOf());

		res = D2D1CreateDevice(dxgiDevice.Get(), NULL, &p2DDevice);
		res = p2DDevice->CreateDeviceContext(D2D1_DEVICE_CONTEXT_OPTIONS_NONE, &p2DDeviceContext);

		// create Direct2D target bitmap
		Microsoft::WRL::ComPtr<ID2D1Bitmap1> d2dTargetBitmap;

		// specify the desired bitmap properties
		// gives E_INVALIDARG, don't use
		D2D1_BITMAP_PROPERTIES1 bp = D2D1::BitmapProperties1(D2D1_BITMAP_OPTIONS_TARGET | D2D1_BITMAP_OPTIONS_CANNOT_DRAW, D2D1::PixelFormat(DXGI_FORMAT_UNKNOWN, D2D1_ALPHA_MODE_PREMULTIPLIED), 96.0f, 96.0f);

		// Direct2D needs the dxgi version of the back buffer
		Microsoft::WRL::ComPtr<IDXGISurface> dxgiBuffer;

		res = swapChain.Get()->GetBuffer(0, __uuidof(IDXGISurface), &dxgiBuffer);

		res = p2DDeviceContext->CreateBitmapFromDxgiSurface(dxgiBuffer.Get(), bp, &d2dTargetBitmap);

		p2DDeviceContext->SetTarget(d2dTargetBitmap.Get());
	}
	//create render target view by linking with back buffer
	HRESULT rtvResult = device->CreateRenderTargetView(swapChainBuffer.Get(), nullptr, renderTargetView.GetAddressOf());

	//create depth stencil buffer 
	CD3D11_TEXTURE2D_DESC dsDesc(DXGI_FORMAT_D24_UNORM_S8_UINT, lround((float)CLIENT_WIDTH), lround((float)CLIENT_HEIGHT), 1, 1, D3D11_BIND_DEPTH_STENCIL);
	HRESULT dsvBufferResult = device->CreateTexture2D(&dsDesc, nullptr, depthStencilBuffer.GetAddressOf());


	// Clear the second depth stencil state before setting the parameters.

	CD3D11_DEPTH_STENCIL_DESC depthDisabledStencilDesc;
	ZeroMemory(&depthDisabledStencilDesc, sizeof(depthDisabledStencilDesc));

	// Now create a second depth stencil state which turns off the Z buffer for 2D rendering.  The only difference is 
	// that DepthEnable is set to false, all other parameters are the same as the other depth stencil state.
	depthDisabledStencilDesc.DepthEnable = false;
	depthDisabledStencilDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
	depthDisabledStencilDesc.DepthFunc = D3D11_COMPARISON_LESS;
	depthDisabledStencilDesc.StencilEnable = true;
	depthDisabledStencilDesc.StencilReadMask = 0xFF;
	depthDisabledStencilDesc.StencilWriteMask = 0xFF;
	depthDisabledStencilDesc.FrontFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
	depthDisabledStencilDesc.FrontFace.StencilDepthFailOp = D3D11_STENCIL_OP_INCR;
	depthDisabledStencilDesc.FrontFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
	depthDisabledStencilDesc.FrontFace.StencilFunc = D3D11_COMPARISON_ALWAYS;
	depthDisabledStencilDesc.BackFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
	depthDisabledStencilDesc.BackFace.StencilDepthFailOp = D3D11_STENCIL_OP_DECR;
	depthDisabledStencilDesc.BackFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
	depthDisabledStencilDesc.BackFace.StencilFunc = D3D11_COMPARISON_ALWAYS;

	device->CreateDepthStencilState(&depthDisabledStencilDesc, depthDisabledStencilState.GetAddressOf());


	//create depth stencil view now that depth stencil buffer is done created
	CD3D11_DEPTH_STENCIL_VIEW_DESC depthStencilViewDesc(D3D11_DSV_DIMENSION_TEXTURE2D);
	HRESULT dsvResult = device->CreateDepthStencilView(depthStencilBuffer.Get(), &depthStencilViewDesc, depthStencilView.GetAddressOf());

	//set render target view and link depth stencil view with rtv
	deviceContext->OMSetRenderTargets(1, renderTargetView.GetAddressOf(), depthStencilView.Get());

	//set up viewport
	ZeroMemory(&viewPort, sizeof(D3D11_VIEWPORT));

	viewPort.TopLeftX = 0;
	viewPort.TopLeftY = 0;
	viewPort.Width = CLIENT_WIDTH;
	viewPort.Height = CLIENT_HEIGHT;
	viewPort.MinDepth = 0.0f;
	viewPort.MaxDepth = 1.0f;

	deviceContext->RSSetViewports(1, &viewPort);

	//set up mvp constant buffer
	CD3D11_BUFFER_DESC mvpBufferDesc(sizeof(ModelViewProjectionConstantBuffer), D3D11_BIND_CONSTANT_BUFFER);
	HRESULT mvpResult = device->CreateBuffer(&mvpBufferDesc, NULL, mvpConstantBuffer.GetAddressOf());

	//set up bone offset buffer
	CD3D11_BUFFER_DESC boneBufferDesc(sizeof(BoneOffsetConstantBuffer), D3D11_BIND_CONSTANT_BUFFER);
	HRESULT boneOffsetResult = device->CreateBuffer(&boneBufferDesc, NULL, boneOffsetConstantBuffer.GetAddressOf());
	int temp = sizeof(BoneOffsetConstantBuffer);

	if (!DEBUG_GRAPHICS) {
		LoadButtonResources(hwnd);
		ImGui_ImplDX11_Init(hwnd, device.Get(), deviceContext.Get());
	}

	device.As(&pDebug);

	HRESULT dRes = pDebug->ReportLiveDeviceObjects(D3D11_RLDO_DETAIL);
}

void DeviceResources::ResizeWindow(uint16_t w, uint16_t h, bool fullScreen)
{
	if (!DEBUG_GRAPHICS) {
		ImGui_ImplDX11_InvalidateDeviceObjects();
	}
	ID3D11RenderTargetView* nullViews[] = { nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr };
	deviceContext->OMSetRenderTargets(ARRAYSIZE(nullViews), nullViews, nullptr);

	renderTargetView.Reset();
	depthStencilView.Reset();
	if (depthStencilBuffer.GetAddressOf())
		depthStencilBuffer.Get()->Release();
	if (swapChain.GetAddressOf())
		swapChainBuffer.Get()->Release();

	if (!DEBUG_GRAPHICS)
	{
		pRT.Get()->Release();
		p2DDeviceContext->SetTarget(nullptr);
		//p2DDeviceContext.Get()->Release();
	}

	deviceContext->Flush();

	swapChain->SetFullscreenState(fullScreen, NULL);

	HRESULT res;
	res = swapChain->ResizeBuffers(0, w, h, DXGI_FORMAT_UNKNOWN, 0);
	if (res == DXGI_ERROR_DEVICE_REMOVED || res == DXGI_ERROR_DEVICE_RESET || res == DXGI_ERROR_UNSUPPORTED)
	{
		throw runtime_error("SwapChain resize not work, go home and dont come back");
	}

	HRESULT scBufferResult = swapChain.Get()->GetBuffer(0, __uuidof(ID3D11Texture2D), (LPVOID*)swapChainBuffer.GetAddressOf());

	device->CreateRenderTargetView(swapChainBuffer.Get(), nullptr, renderTargetView.GetAddressOf());

	CD3D11_TEXTURE2D_DESC depthStencilDesc(DXGI_FORMAT_D24_UNORM_S8_UINT, w, h, 1, 1, D3D11_BIND_DEPTH_STENCIL);
	res = device->CreateTexture2D(&depthStencilDesc, nullptr, depthStencilBuffer.GetAddressOf());

	CD3D11_DEPTH_STENCIL_VIEW_DESC depthStencilViewDesc(D3D11_DSV_DIMENSION_TEXTURE2D);
	res = device->CreateDepthStencilView(depthStencilBuffer.Get(), &depthStencilViewDesc, depthStencilView.GetAddressOf());

	D3D11_VIEWPORT vp;
	vp.Width = w;
	vp.Height = h;
	vp.MinDepth = 0.0f;
	vp.MaxDepth = 1.0f;
	vp.TopLeftX = 0;
	vp.TopLeftY = 0;
	deviceContext->RSSetViewports(1, &vp);

	deviceContext->OMSetRenderTargets(1, renderTargetView.GetAddressOf(), depthStencilView.Get());



	if (!DEBUG_GRAPHICS)
	{
		res = p2DDevice->CreateDeviceContext(D2D1_DEVICE_CONTEXT_OPTIONS_NONE, p2DDeviceContext.GetAddressOf());

		// create Direct2D target bitmap
		Microsoft::WRL::ComPtr<ID2D1Bitmap1> d2dTargetBitmap;

		// specify the desired bitmap properties
		// gives E_INVALIDARG, don't use
		D2D1_BITMAP_PROPERTIES1 bp = D2D1::BitmapProperties1(D2D1_BITMAP_OPTIONS_TARGET | D2D1_BITMAP_OPTIONS_CANNOT_DRAW, D2D1::PixelFormat(DXGI_FORMAT_UNKNOWN, D2D1_ALPHA_MODE_PREMULTIPLIED), 96.0f, 96.0f);

		// Direct2D needs the dxgi version of the back buffer
		Microsoft::WRL::ComPtr<IDXGISurface> dxgiBuffer;

		res = swapChain.Get()->GetBuffer(0, __uuidof(IDXGISurface), &dxgiBuffer);

		res = p2DDeviceContext->CreateBitmapFromDxgiSurface(dxgiBuffer.Get(), bp, &d2dTargetBitmap);

		p2DDeviceContext->SetTarget(d2dTargetBitmap.Get());
		LoadButtonResources(windowHandle);
	}

	
}

void DeviceResources::Clear()
{
	//clear views
	deviceContext->ClearRenderTargetView(renderTargetView.Get(), DirectX::Colors::DimGray);
	deviceContext->ClearDepthStencilView(depthStencilView.Get(), D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);
}

void DeviceResources::Shutdown()
{
	ImGui_ImplDX11_Shutdown();
	swapChain->SetFullscreenState(FALSE, NULL);

	p2DDevice.Reset();
	p2DDeviceContext.Reset();
	if (pD2DFactory)
	{
		//	delete pD2DFactory.Get();
		pD2DFactory.Reset();
	}
	pDWriteFactory.Reset();
	if (pRT)
	{
		//	delete pRT.Get();
		pRT.Reset();
	}

	depthDisabledStencilState.Reset();
	depthStencilView.Reset();
	mvpConstantBuffer.Reset();
	boneOffsetConstantBuffer.Reset();

	device.Reset();
	deviceContext.Reset();
	swapChain.Reset();
	swapChainBuffer.Reset();
	renderTargetView.Reset();
	depthStencilBuffer.Reset();


	if (pDebug)
	{
		HRESULT res = pDebug->ReportLiveDeviceObjects(D3D11_RLDO_DETAIL);
	}


}

void DeviceResources::Present()
{
	//swap back buffer with buffer
	if (!DEBUG_GRAPHICS)
		ImGui::Render();

	HRESULT swapResult = swapChain->Present(1, 0);
}


void DeviceResources::LoadButtonResources(HWND hwnd_)
{
	D2D1_FACTORY_OPTIONS options;
	ZeroMemory(&options, sizeof(D2D1_FACTORY_OPTIONS));
	options.debugLevel = D2D1_DEBUG_LEVEL_NONE;
#if defined(_DEBUG)
	options.debugLevel = D2D1_DEBUG_LEVEL_INFORMATION;
#endif


	HRESULT result;
	if (!pD2DFactory)
		result = D2D1CreateFactory(D2D1_FACTORY_TYPE_SINGLE_THREADED, __uuidof(ID2D1Factory2), &options, (void**)pD2DFactory.GetAddressOf());
	else
		result = S_OK;

	if (SUCCEEDED(result))
	{
		if (!pDWriteFactory)
		{
			result = DWriteCreateFactory(DWRITE_FACTORY_TYPE_SHARED,
				__uuidof(IDWriteFactory),
				(IUnknown**)pDWriteFactory.GetAddressOf());
		}
	}

	// device dependent

	RECT rc;
	GetClientRect(hwnd_, &rc);

	D2D1_SIZE_U size = D2D1::SizeU(rc.right - rc.left, rc.bottom - rc.top);

	result = pD2DFactory->CreateHwndRenderTarget(
		D2D1::RenderTargetProperties(),
		D2D1::HwndRenderTargetProperties(hwnd_, size),
		pRT.GetAddressOf()
	);

	layoutRect = D2D1::RectF(
		static_cast<FLOAT>(rc.left) / 2.0f,
		static_cast<FLOAT>(rc.top) / 2.0f,
		static_cast<FLOAT>(rc.right - rc.left) / 2.0f,
		static_cast<FLOAT>(rc.bottom - rc.top) / 2.0f
	);
}