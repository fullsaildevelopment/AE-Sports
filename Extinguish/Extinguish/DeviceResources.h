#pragma once
#include "Includes.h"

class DeviceResources
{
private:

	Microsoft::WRL::ComPtr<ID3D11Debug> pDebug;
	Microsoft::WRL::ComPtr<ID3D11Device>			device;
	Microsoft::WRL::ComPtr<ID3D11DeviceContext>		deviceContext;
	Microsoft::WRL::ComPtr<IDXGISwapChain>			swapChain;
	Microsoft::WRL::ComPtr<ID3D11Texture2D>			swapChainBuffer;
	Microsoft::WRL::ComPtr<ID3D11RenderTargetView>	renderTargetView;
	Microsoft::WRL::ComPtr<ID3D11Texture2D>			depthStencilBuffer;
	Microsoft::WRL::ComPtr<ID3D11DepthStencilState>				depthDisabledStencilState;
	Microsoft::WRL::ComPtr<ID3D11DepthStencilView>				depthStencilView;

	Microsoft::WRL::ComPtr<ID3D11Buffer>						mvpConstantBuffer;
	Microsoft::WRL::ComPtr<ID3D11Buffer>						boneOffsetConstantBuffer;
	D3D11_VIEWPORT viewPort;
	//Microsoft::WRL::ComPtr<ID3D11Texture2D>			shadowMapBuffer;
	//Microsoft::WRL::ComPtr<ID3D11RenderTargetView>	shadowMapRTV;
	//Microsoft::WRL::ComPtr<ID3D11DepthStencilState> depthStencilState;
	//Microsoft::WRL::ComPtr<ID3D11Texture2D>						shadowMapDepthStencilBuffer;
	//Microsoft::WRL::ComPtr<ID3D11DepthStencilView>				shadowMapDSV;
	//Microsoft::WRL::ComPtr<ID3D11ShaderResourceView>			shadowMapSRV;

	Microsoft::WRL::ComPtr<ID2D1Device>			  p2DDevice;
	Microsoft::WRL::ComPtr<ID2D1DeviceContext>    p2DDeviceContext;
	Microsoft::WRL::ComPtr<ID2D1Factory2>		  pD2DFactory;
	Microsoft::WRL::ComPtr<IDWriteFactory>		  pDWriteFactory;
	Microsoft::WRL::ComPtr<ID2D1HwndRenderTarget> pRT;

	//Microsoft::WRL::ComPtr<HWND> drawWindow;

	D2D1_RECT_F layoutRect;
	
public:
	void Init(HWND hwnd);
	void Present();
	void Clear();
	void Shutdown();

	void ResizeWindow(uint16_t w, uint16_t h);

	void LoadButtonResources(HWND hwnd_);
	//Getters
	//ID3D11DepthStencilState * GetStencilState() { return depthStencilState.Get(); }
	ID3D11DepthStencilState * GetDisableStencilState() { return depthDisabledStencilState.Get(); }

	ID2D1DeviceContext * Get2DContext() { return p2DDeviceContext.Get(); }

	ID3D11Device* GetDevice() const { return device.Get(); }
	ID3D11DeviceContext* GetDeviceContext() const { return deviceContext.Get(); }
	IDXGISwapChain* GetSwapChain() const { return swapChain.Get(); }
	ID3D11DepthStencilView* GetDepthStencilView() const { return depthStencilView.Get(); }
	//ID3D11DepthStencilView* GetShadowMapDSV() const { return shadowMapDSV.Get(); }
	ID3D11RenderTargetView* GetRenderTargetView() const { return renderTargetView.Get(); }
	Microsoft::WRL::ComPtr<ID3D11RenderTargetView> GetRenderTargetViewComPtr() { return renderTargetView; }
	//ID3D11RenderTargetView* GetShadowMapRTV() const { return shadowMapRTV.Get(); }
	//ID3D11RenderTargetView* const * GetShadowMapRTVAddress() const { return shadowMapRTV.GetAddressOf(); }
	ID3D11RenderTargetView* const * GetRenderTargetViewAddress() const { return renderTargetView.GetAddressOf(); }
	//ID3D11ShaderResourceView* GetShadowMapSRV() const { return shadowMapSRV.Get(); }
	//ID3D11ShaderResourceView* const * GetShadowMapSRVAddress() const { return shadowMapSRV.GetAddressOf(); }
	ID3D11Buffer* GetMVPConstantBuffer() { return mvpConstantBuffer.Get(); }
	ID3D11Buffer* GetBoneOffsetConstantBuffer() { return boneOffsetConstantBuffer.Get(); }
	D3D11_VIEWPORT GetViewport() const { return viewPort; }

	D2D1_RECT_F * GetRect() { return &layoutRect; }
	IDWriteFactory * GetWriteFactory() { return pDWriteFactory.Get(); }
	ID2D1Factory2 * GetID2D1Factory() { return pD2DFactory.Get(); }
	ID2D1HwndRenderTarget * GetRenderTarget() { return pRT.Get(); }

};