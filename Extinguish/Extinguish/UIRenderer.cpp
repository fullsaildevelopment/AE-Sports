#include "UIRenderer.h"
#include "GameObject.h"
#include "Button.h"
#include "MeterBar.h"




UIRenderer::UIRenderer()
{
}

UIRenderer::~UIRenderer()
{
	if (pBrush)
	{
		delete pBrush.Get();
		pBrush.Reset();
	}

	if (stateBlock) {
		delete stateBlock.Get();
		stateBlock.Reset();
	}

	if (pTextFormat)
	{
		delete pTextFormat.Get();
		pTextFormat.Reset();
	}
	if (pTextLayout)
	{
		delete pTextLayout.Get();
		pTextLayout.Reset();
	}

	if (pWBitmap)
	{
		delete pWBitmap.Get();
		pWBitmap.Reset();
	}
	if (pBitmap)
	{
		delete pBitmap.Get();
		pBitmap.Reset();
	}
	if (IWICfactory)
	{
		delete IWICfactory.Get();
		IWICfactory.Reset();
	}
	if (IWICdecoder)
	{
		delete IWICdecoder.Get();
		IWICdecoder.Reset();
	}
	if (pBRT)
	{
		delete pBRT.Get();
		pBRT.Reset();
	}

	if (theButton)
		delete theButton;
}

void UIRenderer::ReInit(float ratio)
{
	pDWriteFactory = devResources->GetWriteFactory();
	pD2DFactory = devResources->GetID2D1Factory();
	layoutRect = devResources->GetRect();
	pRT = devResources->GetRenderTarget();
	d2DevContext = devResources->Get2DContext();
	if (ratio != 1.0f)
		SetFontSize(getFontSize() * ratio);

	if (theButton)
	{
		if (theButton->getText() != L"")
		{

			if (pTextFormat)
			{

				HRESULT res = pTextFormat->SetTextAlignment(alignmentText);
				pTextFormat->SetParagraphAlignment(alignmentParagrah);

				res = pDWriteFactory->CreateTextLayout(
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
		}
	}

}

void UIRenderer::Init(bool _isButton, float fontSize, DeviceResources* deviceResources, Button * button, wstring font, D2D1::ColorF fontColor)
{
	pDWriteFactory = deviceResources->GetWriteFactory();
	pD2DFactory = deviceResources->GetID2D1Factory();
	layoutRect = deviceResources->GetRect();
	pRT = deviceResources->GetRenderTarget();
	//pTextLayout = resources->GetTextLayout();
	devResources = deviceResources;
	isButton = _isButton;
	theButton = button;
	d2DevContext = deviceResources->Get2DContext();

	this->font = font;
	curSize = fontSize;

	if (font != L"") {
		HRESULT result;
		IDWriteFontCollection* pFontCollection = NULL;

		result = pDWriteFactory->GetSystemFontCollection(&pFontCollection);
		result = pDWriteFactory->CreateTextFormat(font.c_str(),
			pFontCollection,
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
			result = pTextFormat->SetParagraphAlignment(DWRITE_PARAGRAPH_ALIGNMENT_CENTER);
		}

		if (SUCCEEDED(result))
		{
			result = d2DevContext->CreateSolidColorBrush(
				D2D1::ColorF(fontColor),
				pBrush.GetAddressOf()
			);

			if (result != S_OK)
				float temp = 0.0f;
		}
	}
}


void UIRenderer::Init(bool _isButton, DeviceResources* deviceResources, MeterBar * meter)
{
	pDWriteFactory = deviceResources->GetWriteFactory();
	pD2DFactory = deviceResources->GetID2D1Factory();
	layoutRect = deviceResources->GetRect();
	pRT = deviceResources->GetRenderTarget();
	//pTextLayout = resources->GetTextLayout();
	devResources = deviceResources;
	isButton = _isButton;
	theBar = meter;
	d2DevContext = deviceResources->Get2DContext();
}

void UIRenderer::InitMetrics()
{
	HRESULT res = pDWriteFactory->CreateTextLayout(
		theButton->getText().c_str(),
		theButton->getLength(),
		pTextFormat.Get(),
		theButton->getWidth(),
		theButton->getHeight(),
		&pTextLayout
	);

	ZeroMemory(&textMetrics, sizeof(DWRITE_TEXT_METRICS));
	pTextLayout->GetMetrics(&textMetrics);
	left = textMetrics.left;

}

void UIRenderer::Update(float _dt)
{
	if (theButton) {
		if (theButton->getText() != L"") {
			//ID3D11DeviceContext* devContext = devResources->GetDeviceContext();

			if (pTextFormat)
			{

			HRESULT res = pTextFormat->SetTextAlignment(alignmentText);
			pTextFormat->SetParagraphAlignment(alignmentParagrah);

			res = pDWriteFactory->CreateTextLayout(
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

		}
	}
}

void UIRenderer::Render()
{
	if (isEnabled())
	{
		//ID3D11DeviceContext* devContext = devResources->GetDeviceContext();

		HRESULT hr;

		pD2DFactory->CreateDrawingStateBlock(stateBlock.GetAddressOf());
		//devContext->OMSetDepthStencilState(depthStencilState, 1);
		d2DevContext->SaveDrawingState(stateBlock.Get());

		//d2DevContext->BeginDraw();
		d2DevContext->SetTransform(D2D1::IdentityMatrix());

		if (theButton) {
			if (theButton->getActive()) {

				if (pBitmap)
				{
					//(!theButton->isHovered() && !theButton->stayHovered()) || 
					if ((!theButton->isSelected() && !theButton->getRemainSelected()) || !pBitmapHovered)
						d2DevContext->DrawBitmap(pBitmap.Get(), theButton->getRect(), opacity,
							D2D1_BITMAP_INTERPOLATION_MODE::D2D1_BITMAP_INTERPOLATION_MODE_NEAREST_NEIGHBOR);
					else
						d2DevContext->DrawBitmap(pBitmapHovered.Get(), theButton->getRect(), opacity, D2D1_BITMAP_INTERPOLATION_MODE::D2D1_BITMAP_INTERPOLATION_MODE_NEAREST_NEIGHBOR);

				}

				if (theButton->getText() != L"" && theButton->getText() != L"Titans with Sticks") {

					DWRITE_TEXT_RANGE textRange = { 0, theButton->getLength() };
					hr = pTextLayout->SetTypography(theButton->getTypography(), textRange);

					d2DevContext->DrawTextLayout(
						D2D1::Point2F(theButton->getOriginX(), theButton->getOriginY()),
						pTextLayout.Get(),
						pBrush.Get()
					);
				}
				/*if (hr != D2DERR_RECREATE_TARGET && hr != S_OK)
				{
					float t = 0;
				}*/

				if (GetGameObject()->GetName() == "debugUI") {
					RenderDebugUI(theButton);
				}
			}
		}

		if (theBar)
		{
			if (theBar->getActive())
			{
				d2DevContext->DrawBitmap(pBitmap.Get(), theBar->getRect2(), opacity,
					D2D1_BITMAP_INTERPOLATION_MODE::D2D1_BITMAP_INTERPOLATION_MODE_NEAREST_NEIGHBOR);
				d2DevContext->DrawBitmap(pBitmapHovered.Get(), theBar->getRect(), opacity,
					D2D1_BITMAP_INTERPOLATION_MODE::D2D1_BITMAP_INTERPOLATION_MODE_NEAREST_NEIGHBOR);
			}
		}

		//hr = d2DevContext->EndDraw();
		d2DevContext->RestoreDrawingState(stateBlock.Get());
		stateBlock.Reset();
	}
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

		d2DevContext->CreateBitmapFromWicBitmap(pConvertedSource, NULL, pBitmap.GetAddressOf());

	}
	else
	{

		D2D1_RENDER_TARGET_PROPERTIES rtp = D2D1::RenderTargetProperties();
		rtp.dpiX = 96.0f;
		rtp.dpiY = 96.0f;

		d2DevContext->CreateBitmapFromWicBitmap(pConvertedSource, NULL, pBitmapHovered.GetAddressOf());
	}

	//delete pFrame;
	//delete pConvertedSource;
}

// use with resize
void UIRenderer::MakeRTSize()
{
	D2D1_SIZE_F rtSize = d2DevContext->GetSize();
	if (theButton)
		theButton->setRT(rtSize);
	if (theBar)
		theBar->setRT(rtSize);
}

//setters
void UIRenderer::SetFontSize(float fontSize)
{
	IDWriteFontCollection* pFontCollection = NULL;
	pDWriteFactory->GetSystemFontCollection(&pFontCollection);
	curSize = fontSize;

	pDWriteFactory->CreateTextFormat(font.c_str(),
		pFontCollection,
		DWRITE_FONT_WEIGHT_REGULAR,
		DWRITE_FONT_STYLE_NORMAL,
		DWRITE_FONT_STRETCH_NORMAL,
		fontSize,
		L"en-us",
		pTextFormat.GetAddressOf());
}

void UIRenderer::SetColor(D2D1::ColorF fontColor)
{
	d2DevContext->CreateSolidColorBrush(
		D2D1::ColorF(fontColor),
		pBrush.GetAddressOf()
	);
}

void UIRenderer::RemoveBitmap()
{
	if (pBitmap)
	{
		//delete pBitmap.Get();
		pBitmap.Reset();
		pBitmap = nullptr;
	}
}