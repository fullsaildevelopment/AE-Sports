#include "DeviceResources.h"

void DeviceResources::Init(HWND hwnd)
{
	//drawWindow = hwnd;
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
	UINT flags = DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH;
	flags |= D3D11_CREATE_DEVICE_BGRA_SUPPORT;

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

	Microsoft::WRL::ComPtr<IDXGIDevice> dxgiDevice;
	HRESULT res;
	res = device.As(&dxgiDevice);
	
	// res = device.Get()->QueryInterface(__uuidof(IDXGIDevice), (void**)dxgiDevice.GetAddressOf());

	res = D2D1CreateDevice(dxgiDevice.Get(), NULL, &p2DDevice);
	res = p2DDevice->CreateDeviceContext(D2D1_DEVICE_CONTEXT_OPTIONS_NONE, &p2DDeviceContext);

	//Set up back buffer
	HRESULT scBufferResult = swapChain.Get()->GetBuffer(0, __uuidof(ID3D11Texture2D), (LPVOID*)swapChainBuffer.GetAddressOf()); //this returns address of back buffer in swapChain

	// create Direct2D target bitmap
	Microsoft::WRL::ComPtr<ID2D1Bitmap1> d2dTargetBitmap;

	// specify the desired bitmap properties
	/*D2D1_BITMAP_PROPERTIES1 bp;
	bp.pixelFormat.format = DXGI_FORMAT_B8G8R8A8_UNORM;
	bp.pixelFormat.alphaMode = D2D1_ALPHA_MODE_IGNORE;
	bp.dpiX = DEFAULT_DPI;
	bp.dpiY = DEFAULT_DPI;
	bp.bitmapOptions = D2D1_BITMAP_OPTIONS_TARGET | D2D1_BITMAP_OPTIONS_CANNOT_DRAW;
	bp.colorContext = nullptr;*/

	// Direct2D needs the dxgi version of the back buffer
	Microsoft::WRL::ComPtr<IDXGISurface> dxgiBuffer;

	res = swapChain.Get()->GetBuffer(0, __uuidof(IDXGISurface), &dxgiBuffer);

	res = p2DDeviceContext->CreateBitmapFromDxgiSurface(dxgiBuffer.Get(), NULL, d2dTargetBitmap.GetAddressOf());

	p2DDeviceContext->SetTarget(d2dTargetBitmap.Get());

	//create shadow map buffer
	D3D11_TEXTURE2D_DESC smTextDesc;
	ZeroMemory(&smTextDesc, sizeof(D3D11_TEXTURE2D_DESC));

	smTextDesc.Width = CLIENT_WIDTH;
	smTextDesc.Height = CLIENT_HEIGHT;
	smTextDesc.MipLevels = 1;
	smTextDesc.ArraySize = 1;
	smTextDesc.Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
	smTextDesc.SampleDesc.Count = 1;
	smTextDesc.Usage = D3D11_USAGE_DEFAULT;
	smTextDesc.BindFlags = D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE;
	smTextDesc.CPUAccessFlags = 0;
	smTextDesc.MiscFlags = 0;

	HRESULT smBufferResult = device->CreateTexture2D(&smTextDesc, NULL, shadowMapBuffer.GetAddressOf());

	//create render target view by linking with back buffer
	HRESULT rtvResult = device->CreateRenderTargetView(swapChainBuffer.Get(), nullptr, renderTargetView.GetAddressOf());

	//create shadowMapRTV by linking with shadowMapBuffer
	HRESULT smRtvResult = device->CreateRenderTargetView(shadowMapBuffer.Get(), nullptr, shadowMapRTV.GetAddressOf());

	//create depth stencil buffer 
	CD3D11_TEXTURE2D_DESC dsDesc(DXGI_FORMAT_D24_UNORM_S8_UINT, lround((float)CLIENT_WIDTH), lround((float)CLIENT_HEIGHT), 1, 1, D3D11_BIND_DEPTH_STENCIL);
	HRESULT dsvBufferResult = device->CreateTexture2D(&dsDesc, nullptr, depthStencilBuffer.GetAddressOf());

	//create shadow map stv buffer
	CD3D11_TEXTURE2D_DESC smDsDesc(DXGI_FORMAT_D24_UNORM_S8_UINT, lround((float)CLIENT_WIDTH), lround((float)CLIENT_HEIGHT), 1, 1, D3D11_BIND_DEPTH_STENCIL);
	HRESULT smDsvBufferResult = device->CreateTexture2D(&smDsDesc, nullptr, shadowMapDepthStencilBuffer.GetAddressOf());


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

	//create shadow map DSV
	CD3D11_DEPTH_STENCIL_VIEW_DESC smDepthStencilViewDesc(D3D11_DSV_DIMENSION_TEXTURE2D);
	HRESULT smDvResult = device->CreateDepthStencilView(shadowMapDepthStencilBuffer.Get(), &smDepthStencilViewDesc, shadowMapDSV.GetAddressOf());

	//create shadow SRV
	CD3D11_SHADER_RESOURCE_VIEW_DESC smSRVDesc(D3D11_SRV_DIMENSION_TEXTURE2D);
	HRESULT smSRVResult = device->CreateShaderResourceView(shadowMapBuffer.Get(), &smSRVDesc, shadowMapSRV.GetAddressOf());

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

	LoadButtonResources(hwnd);
	ImGui_ImplDX11_Init(hwnd, device.Get(), deviceContext.Get());

	device.As(&pDebug);
}

void DeviceResources::ResizeWindow(uint16_t w, uint16_t h)
{
	ID3D11RenderTargetView* nullViews[] = { nullptr };
	deviceContext->OMSetRenderTargets(ARRAYSIZE(nullViews), nullViews, nullptr);
	renderTargetView = nullptr;
	shadowMapRTV = nullptr;
	p2DDeviceContext->SetTarget(nullptr);
	depthStencilView = nullptr;
	swapChainBuffer = nullptr;
	shadowMapBuffer = nullptr;

	deviceContext->Flush();

	//create swapchain
	swapChain->ResizeBuffers(2,lround(w), lround(h), DXGI_FORMAT_B8G8R8A8_UNORM,0);

	//Set up back buffer
	HRESULT scBufferResult = swapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), (LPVOID*)swapChainBuffer.GetAddressOf()); //this returns address of back buffer in swapChain

	device->CreateRenderTargetView(swapChainBuffer.Get(), nullptr, &renderTargetView);
	
	CD3D11_TEXTURE2D_DESC depthStencilDesc(
		DXGI_FORMAT_D24_UNORM_S8_UINT,
		lround(w),
		lround(h),
		1, // This depth stencil view has only one texture.
		1, // Use a single mipmap level.
		D3D11_BIND_DEPTH_STENCIL
	);

	device->CreateTexture2D(
		&depthStencilDesc,
		nullptr,
		&depthStencilBuffer
	);

	CD3D11_DEPTH_STENCIL_VIEW_DESC depthStencilViewDesc(D3D11_DSV_DIMENSION_TEXTURE2D);
	HRESULT dsvResult = device->CreateDepthStencilView(depthStencilBuffer.Get(), &depthStencilViewDesc, depthStencilView.GetAddressOf());

	viewPort = CD3D11_VIEWPORT(
		0.0f,
		0.0f,
		w,
		h
	);

	deviceContext->RSSetViewports(1, &viewPort);
	
	// create Direct2D target bitmap
	Microsoft::WRL::ComPtr<ID2D1Bitmap1> d2dTargetBitmap;

	// specify the desired bitmap properties
	/*D2D1_BITMAP_PROPERTIES1 bp;
	bp.pixelFormat.format = DXGI_FORMAT_B8G8R8A8_UNORM;
	bp.pixelFormat.alphaMode = D2D1_ALPHA_MODE_IGNORE;
	bp.dpiX = DEFAULT_DPI;
	bp.dpiY = DEFAULT_DPI;
	bp.bitmapOptions = D2D1_BITMAP_OPTIONS_TARGET | D2D1_BITMAP_OPTIONS_CANNOT_DRAW;
	bp.colorContext = nullptr;*/

	// Direct2D needs the dxgi version of the back buffer
	Microsoft::WRL::ComPtr<IDXGISurface> dxgiBuffer;

	swapChain->GetBuffer(0, __uuidof(IDXGISurface), &dxgiBuffer);

	p2DDeviceContext->CreateBitmapFromDxgiSurface(dxgiBuffer.Get(), NULL, d2dTargetBitmap.GetAddressOf());

	p2DDeviceContext->SetTarget(d2dTargetBitmap.Get());

	//create shadow map buffer
	D3D11_TEXTURE2D_DESC smTextDesc;
	ZeroMemory(&smTextDesc, sizeof(D3D11_TEXTURE2D_DESC));

	smTextDesc.Width = w;
	smTextDesc.Height = h;
	smTextDesc.MipLevels = 1;
	smTextDesc.ArraySize = 1;
	smTextDesc.Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
	smTextDesc.SampleDesc.Count = 1;
	smTextDesc.Usage = D3D11_USAGE_DEFAULT;
	smTextDesc.BindFlags = D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE;
	smTextDesc.CPUAccessFlags = 0;
	smTextDesc.MiscFlags = 0;

	HRESULT smBufferResult = device->CreateTexture2D(&smTextDesc, NULL, shadowMapBuffer.GetAddressOf());

	//create render target view by linking with back buffer
	HRESULT rtvResult = device->CreateRenderTargetView(swapChainBuffer.Get(), nullptr, renderTargetView.GetAddressOf());

	//create shadowMapRTV by linking with shadowMapBuffer
	HRESULT smRtvResult = device->CreateRenderTargetView(shadowMapBuffer.Get(), nullptr, shadowMapRTV.GetAddressOf());

	//create depth stencil buffer 
	//CD3D11_TEXTURE2D_DESC dsDesc(DXGI_FORMAT_D24_UNORM_S8_UINT, lround((float)w), lround((float)h), 1, 1, D3D11_BIND_DEPTH_STENCIL);
	//HRESULT dsvBufferResult = device->CreateTexture2D(&dsDesc, nullptr, depthStencilBuffer.GetAddressOf());

	//create shadow map stv buffer
	CD3D11_TEXTURE2D_DESC smDsDesc(DXGI_FORMAT_D24_UNORM_S8_UINT, lround((float)w), lround((float)h), 1, 1, D3D11_BIND_DEPTH_STENCIL);
	HRESULT smDsvBufferResult = device->CreateTexture2D(&smDsDesc, nullptr, shadowMapDepthStencilBuffer.GetAddressOf());


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

	//create shadow map DSV
	CD3D11_DEPTH_STENCIL_VIEW_DESC smDepthStencilViewDesc(D3D11_DSV_DIMENSION_TEXTURE2D);
	HRESULT smDvResult = device->CreateDepthStencilView(shadowMapDepthStencilBuffer.Get(), &smDepthStencilViewDesc, shadowMapDSV.GetAddressOf());

	//create shadow SRV
	CD3D11_SHADER_RESOURCE_VIEW_DESC smSRVDesc(D3D11_SRV_DIMENSION_TEXTURE2D);
	HRESULT smSRVResult = device->CreateShaderResourceView(shadowMapBuffer.Get(), &smSRVDesc, shadowMapSRV.GetAddressOf());
	
	//set render target view and link depth stencil view with rtv
	//deviceContext->OMSetRenderTargets(1, renderTargetView.GetAddressOf(), depthStencilView.Get());

	//set up mvp constant buffer
	//CD3D11_BUFFER_DESC mvpBufferDesc(sizeof(ModelViewProjectionConstantBuffer), D3D11_BIND_CONSTANT_BUFFER);
	//HRESULT mvpResult = device->CreateBuffer(&mvpBufferDesc, NULL, mvpConstantBuffer.GetAddressOf());
	//
	////set up bone offset buffer
	//CD3D11_BUFFER_DESC boneBufferDesc(sizeof(BoneOffsetConstantBuffer), D3D11_BIND_CONSTANT_BUFFER);
	//HRESULT boneOffsetResult = device->CreateBuffer(&boneBufferDesc, NULL, boneOffsetConstantBuffer.GetAddressOf());
	//int temp = sizeof(BoneOffsetConstantBuffer);

	//LoadButtonResources(*drawWindow.Get());
	//ImGui_ImplDX11_Init(*drawWindow.Get(), device.Get(), deviceContext.Get());

	//device.As(&pDebug);
}

void DeviceResources::Clear()
{
	//clear views
	deviceContext->ClearRenderTargetView(renderTargetView.Get(), DirectX::Colors::SkyBlue);
	deviceContext->ClearDepthStencilView(depthStencilView.Get(), D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);
}

void DeviceResources::Shutdown()
{
	ImGui_ImplDX11_Shutdown();
	swapChain->SetFullscreenState(FALSE, NULL);

	p2DDevice.Reset();
	p2DDeviceContext.Reset();
	pD2DFactory.Reset();
	pDWriteFactory.Reset();
	pRT.Reset();

	depthDisabledStencilState.Reset();
	shadowMapDepthStencilBuffer.Reset();
	depthStencilView.Reset();
	shadowMapDSV.Reset();
	shadowMapSRV.Reset();
	mvpConstantBuffer.Reset();
	boneOffsetConstantBuffer.Reset();

	device.Reset();
	deviceContext.Reset();
	swapChain.Reset();
	swapChainBuffer.Reset();
	shadowMapBuffer.Reset();
	renderTargetView.Reset();
	shadowMapRTV.Reset();
	depthStencilBuffer.Reset();


	if (pDebug)
		pDebug->ReportLiveDeviceObjects(D3D11_RLDO_DETAIL);
}

void DeviceResources::Present()
{
	//swap back buffer with buffer
	ImGui::Render();
	HRESULT swapResult = swapChain->Present(0, 0);
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
	result = D2D1CreateFactory(D2D1_FACTORY_TYPE_SINGLE_THREADED,
		__uuidof(ID2D1Factory2),
		&options,
		&pD2DFactory);


	if (SUCCEEDED(result))
	{
		result = DWriteCreateFactory(DWRITE_FACTORY_TYPE_SHARED,
			__uuidof(IDWriteFactory),
			&pDWriteFactory);
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