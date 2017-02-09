#pragma once

#ifdef GRAPHICS_DEBUG
#define GRAPHICS 0
#else
#define GRAPHICS 1
#endif

#include "Includes.h"
#include "DeviceResources.h"
#include "ResourceManager.h"
#include "Component.h"
#include "Button.h"

class GameObject;
class Button;
class UIRenderer : public Component
{
private:
	DeviceResources* devResources;
	ID2D1Factory2 * pD2DFactory;
	IDWriteFactory * pDWriteFactory;
	ID2D1HwndRenderTarget * pRT;
	Microsoft::WRL::ComPtr<ID2D1DrawingStateBlock> stateBlock;
	DWRITE_TEXT_METRICS textMetrics;
	ID2D1DeviceContext * d2DevContext;
	ID3D11DepthStencilState * depthStencilState;
	Microsoft::WRL::ComPtr<IDWriteTextFormat> pTextFormat;
	Microsoft::WRL::ComPtr<IDWriteTextLayout> pTextLayout;
	Microsoft::WRL::ComPtr<ID2D1SolidColorBrush> pBrush;
	
	Microsoft::WRL::ComPtr<IWICBitmap>		   pWBitmap;
	Microsoft::WRL::ComPtr<ID2D1Bitmap>		   pBitmap;
	Microsoft::WRL::ComPtr<ID2D1Bitmap>		   pBitmapHovered;
	Microsoft::WRL::ComPtr<IWICImagingFactory> IWICfactory;
	Microsoft::WRL::ComPtr<IWICBitmapDecoder>  IWICdecoder;
	Microsoft::WRL::ComPtr<ID2D1RenderTarget>  pBRT;
	Button * theButton;
	D2D1_RECT_F * layoutRect;
	bool isButton = false;
	float left;

	void RenderDebugUI(Button * theButton);

public:
	UIRenderer();
	~UIRenderer();
	void Init(bool isButton, float fontSize, DeviceResources* deviceResources, Button * button, D2D1::ColorF fontColor);
	void Update(float dt) override;
	void Render();
	void DecodeBitmap(PCWSTR address);
	void MakeRTSize();
	void InitMetrics();
};

