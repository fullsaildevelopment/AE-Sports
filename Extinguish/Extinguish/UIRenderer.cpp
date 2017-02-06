#include "UIRenderer.h"



UIRenderer::UIRenderer()
{
}


UIRenderer::~UIRenderer()
{
	if (pBrush)
		pBrush.Reset();

	if (stateBlock)
		stateBlock.Reset();

	if (pTextFormat)
		pTextFormat.Reset();
	if (pTextLayout)
		pTextLayout.Reset();

	if (pWBitmap)
	pWBitmap.Reset();
	if (pBitmap)
	pBitmap.Reset();
	if (IWICfactory)
	IWICfactory.Reset();
	if (IWICdecoder)
	IWICdecoder.Reset();
	if (pBRT)
	pBRT.Reset();
}


void UIRenderer::Init(bool _isButton, float fontSize, ResourceManager* resources, DeviceResources* deviceResources, ID3D11DepthStencilState * state)
{
	pDWriteFactory = deviceResources->GetWriteFactory();
	pD2DFactory = deviceResources->GetID2D1Factory();
	layoutRect = deviceResources->GetRect();
	pRT = deviceResources->GetRenderTarget();
	//pTextLayout = resources->GetTextLayout();
	devResources = deviceResources;
	isButton = _isButton;
	depthStencilState = state;
	d2DevContext = deviceResources->Get2DContext();


	HRESULT result = pDWriteFactory->CreateTextFormat(L"Consolas",
		NULL,
		DWRITE_FONT_WEIGHT_REGULAR,
		DWRITE_FONT_STYLE_NORMAL,
		DWRITE_FONT_STRETCH_NORMAL,
		fontSize,
		L"en-us",
		pTextFormat.GetAddressOf()
	);

	if (SUCCEEDED(result))
	{
		result = pTextFormat->SetTextAlignment(DWRITE_TEXT_ALIGNMENT_CENTER);
		result = pTextFormat->SetParagraphAlignment(DWRITE_PARAGRAPH_ALIGNMENT_NEAR);
	}

	if (SUCCEEDED(result))
	{
		result = d2DevContext->CreateSolidColorBrush(
			D2D1::ColorF(D2D1::ColorF::Black),
			pBrush.GetAddressOf()
		);
	}

}


void UIRenderer::Update(float dt)
{
	ID3D11DeviceContext* devContext = devResources->GetDeviceContext();

	GameObject * temp = GetGameObject();
	Button * theButton = temp->GetComponent<Button>();

	pDWriteFactory->CreateTextLayout(
		theButton->getText().c_str(),
		theButton->getLength(),
		pTextFormat.Get(),
		theButton->getWidth(),
		theButton->getHeight(),
		&pTextLayout
	);

	ZeroMemory(&textMetrics, sizeof(DWRITE_TEXT_METRICS));
	pTextLayout->GetMetrics(&textMetrics);
}

void UIRenderer::Render()
{
	ID3D11DeviceContext* devContext = devResources->GetDeviceContext();

	GameObject * temp = GetGameObject();
	Button * theButton = temp->GetComponent<Button>();
	HRESULT hr;


	pD2DFactory->CreateDrawingStateBlock(stateBlock.GetAddressOf());
	//devContext->OMSetDepthStencilState(depthStencilState, 1);
	d2DevContext->SaveDrawingState(stateBlock.Get());
	d2DevContext->BeginDraw();
	d2DevContext->SetTransform(D2D1::IdentityMatrix());
	if (pBitmap)
	{
		D2D1_SIZE_F rtSize = d2DevContext->GetSize();
		
		d2DevContext->DrawBitmap(pBitmap.Get(), theButton->getRect(rtSize));
	}

	DWRITE_TEXT_RANGE textRange = { 0, theButton->getLength() };
	hr = pTextLayout->SetTypography(theButton->getTypography(), textRange);

	d2DevContext->DrawTextLayout(
		D2D1::Point2F(theButton->getOriginX(), theButton->getOriginY()),
		pTextLayout.Get(),
		pBrush.Get()
	);

	hr = d2DevContext->EndDraw();
	/*if (hr != D2DERR_RECREATE_TARGET && hr != S_OK)
	{
		float t = 0;
	}*/
	d2DevContext->RestoreDrawingState(stateBlock.Get());

	if (GetGameObject()->GetName() == "debugUI") {
		RenderDebugUI(theButton);
	}



	stateBlock.Reset();
	
}

void UIRenderer::RenderDebugUI(Button * theButton)
{
	ImGui::BeginMainMenuBar();

	ImGui::Text("DEBUG BAR >>");

	if (theButton->iShowFPS())
	{
		ImGui::RenderText(ImVec2(650.0f, 2.0f), theButton->getFPS().c_str());
	}

	ImGui::RenderText(ImVec2(350.0f, 2.0f), theButton->getTime().c_str());

	ImGui::EndMainMenuBar();

	//ImGui::SetNextWindowSize(ImVec2(180.0f, 80.0f));
	//ImGui::Begin("Quick Networking");
	//
	//if (ImGui::Button("         Host         "))
	//{
	//	// start server
	//	float temp = 0.0f;
	//}

	//if (ImGui::Button("         Join         "))
	//{
	//	if (ImGui::IsMouseDown(0))
	//	{
	//		// join server
	//	}
	//}

	//ImGui::End();

	// if you need anything for debugging purposes, just add it here
}

void UIRenderer::DecodeBitmap(PCWSTR address)
{
	HRESULT hr;

	hr = CoCreateInstance(CLSID_WICImagingFactory, nullptr,
		CLSCTX_INPROC_SERVER,
		IID_PPV_ARGS(&IWICfactory));

	hr = IWICfactory->CreateDecoderFromFilename(
		address,
		NULL,
		GENERIC_READ,
		WICDecodeMetadataCacheOnLoad,
		&IWICdecoder
	);

	IWICBitmapFrameDecode * pFrame = NULL;

	if (SUCCEEDED(hr))
	{
		hr = IWICdecoder->GetFrame(0, &pFrame);
	}

	IWICFormatConverter * pConvertedSource = NULL;
	if (SUCCEEDED(hr))
	{
		hr = IWICfactory->CreateFormatConverter(&pConvertedSource);
	}

	if (SUCCEEDED(hr))
	{
		hr = pConvertedSource->Initialize(pFrame,
			GUID_WICPixelFormat32bppPBGRA,
			WICBitmapDitherTypeNone,
			NULL,
			0.0f,
			WICBitmapPaletteTypeCustom
		);
	}

	if (pConvertedSource && !pBitmap)
	{
		D2D1_RENDER_TARGET_PROPERTIES rtp = D2D1::RenderTargetProperties();
		rtp.dpiX = 96.0f;
		rtp.dpiY = 96.0f;

		//pD2DFactory->CreateWicBitmapRenderTarget(pWBitmap.Get(), rtp, &pBRT);

		//IWICBitmapSource * source;
		d2DevContext->CreateBitmapFromWicBitmap(pConvertedSource, NULL, pBitmap.GetAddressOf());

	}

	//delete pFrame;
	//delete pConvertedSource;
}