#pragma once



#include "Includes.h"
#include "DeviceResources.h"
#include "ResourceManager.h"
#include "Component.h"
#include "Button.h"
#include "MeterBar.h"

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
	DWRITE_TEXT_ALIGNMENT alignmentText = DWRITE_TEXT_ALIGNMENT_CENTER;
	DWRITE_PARAGRAPH_ALIGNMENT alignmentParagrah = DWRITE_PARAGRAPH_ALIGNMENT_CENTER;
	
	Microsoft::WRL::ComPtr<IWICBitmap>		   pWBitmap;
	Microsoft::WRL::ComPtr<ID2D1Bitmap>		   pBitmap;
	Microsoft::WRL::ComPtr<ID2D1Bitmap>		   pBitmapHovered;
	Microsoft::WRL::ComPtr<IWICImagingFactory> IWICfactory;
	Microsoft::WRL::ComPtr<IWICBitmapDecoder>  IWICdecoder;
	Microsoft::WRL::ComPtr<ID2D1RenderTarget>  pBRT;
	D2D1_RECT_F * layoutRect;
	bool isButton = false;
	float left;
	wstring font;
	float curSize;
	float opacity = 0.0f;
	bool fadeOut = false;

	/* Decides which renderer function to call in render */
	Button * theButton;
	MeterBar * theBar;

	void RenderDebugUI(Button * theButton);

public:
	UIRenderer();
	~UIRenderer();
	void Init(bool isButton, float fontSize, DeviceResources* deviceResources, Button * button, wstring font, D2D1::ColorF fontColor);
	void Init(bool isButton, DeviceResources* deviceResources, MeterBar * meter);
	void ReInit();
	void Update(float _dt) override;
	void Render();
	void DecodeBitmap(PCWSTR address);
	void MakeRTSize();
	void InitMetrics();
	void setAlignment(DWRITE_TEXT_ALIGNMENT text, DWRITE_PARAGRAPH_ALIGNMENT paragraph) { alignmentText = text; alignmentParagrah = paragraph; }
	void SetFontSize(float fontSize);
	void SetColor(D2D1::ColorF fontColor);
	wstring GetFont() { return font; }
	bool getIsButton() { return isButton; }
	float getFontSize() { return curSize; }
	void setFadeOut(bool tf) { fadeOut = tf; }

	void RemoveBitmap();
};
