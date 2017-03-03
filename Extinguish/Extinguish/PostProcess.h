#pragma once
#include "CommonStates.h"
#include "SpriteBatch.h"
#include <DirectXMath.h>
#include "ReadData.h"
#include "DeviceResources.h"

class PostProcess
{
	DeviceResources* devRes;
	ID3D11Device* device;
	ID3D11DeviceContext* deviceContext;
	///////////Bloom///////////
	RECT m_fullscreenRect;
	std::unique_ptr<DirectX::CommonStates> m_states;
	std::unique_ptr<DirectX::SpriteBatch> m_spriteBatch;
	Microsoft::WRL::ComPtr<ID3D11PixelShader> m_bloomExtractPS;
	Microsoft::WRL::ComPtr<ID3D11PixelShader> m_bloomCombinePS;
	Microsoft::WRL::ComPtr<ID3D11PixelShader> m_gaussianBlurPS;
	
	Microsoft::WRL::ComPtr<ID3D11Buffer> m_bloomParams;
	Microsoft::WRL::ComPtr<ID3D11Buffer> m_blurParamsWidth;
	Microsoft::WRL::ComPtr<ID3D11Buffer> m_blurParamsHeight;

	Microsoft::WRL::ComPtr<ID3D11Texture2D> m_backBuffer;

	Microsoft::WRL::ComPtr<ID3D11Texture2D> m_sceneTex;
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> m_sceneSRV;
	Microsoft::WRL::ComPtr<ID3D11RenderTargetView> m_sceneRT;

	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> m_rt1SRV;
	Microsoft::WRL::ComPtr<ID3D11RenderTargetView> m_rt1RT;

	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> m_rt2SRV;
	Microsoft::WRL::ComPtr<ID3D11RenderTargetView> m_rt2RT;

	RECT m_bloomRect;
	/////////////////////////////
public:
	void CreateDevice(DeviceResources* devR);

	void CreateResources();

	void DoPostProcess();

	void Clear();

};