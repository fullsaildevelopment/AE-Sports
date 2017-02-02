#pragma once
#include "Includes.h"
#include "DeviceResources.h"
#include "ResourceManager.h"
#include "Component.h"
#include "Button.h"
#include "GameObject.h"
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

	D2D1_RECT_F * layoutRect;
	bool isButton = false;

	float originX, originY;

public:
	UIRenderer();
	~UIRenderer();
	void Init(bool isButton, float fontSize, ResourceManager* resources, DeviceResources* deviceResources, ID3D11DepthStencilState * state);
	void Update(float dt) override;
	void Render();

	void setOrigin(float x, float y) { originX = x; originY = y; }
};

