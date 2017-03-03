#include "PostProcess.h"


namespace BLOOM
{
	struct VS_BLOOM_PARAMETERS
	{
		float bloomThreshold;
		float blurAmount;
		float bloomIntensity;
		float baseIntensity;
		float bloomSaturation;
		float baseSaturation;
		uint8_t na[8];
	};

	static_assert(!(sizeof(VS_BLOOM_PARAMETERS) % 16),
		"VS_BLOOM_PARAMETERS needs to be 16 bytes aligned");

	struct VS_BLUR_PARAMETERS
	{
		static const size_t SAMPLE_COUNT = 15;

		XMFLOAT4 sampleOffsets[SAMPLE_COUNT];
		XMFLOAT4 sampleWeights[SAMPLE_COUNT];

		void SetBlurEffectParameters(float dx, float dy,
			const VS_BLOOM_PARAMETERS& params)
		{
			sampleWeights[0].x = ComputeGaussian(0, params.blurAmount);
			sampleOffsets[0].x = sampleOffsets[0].y = 0.f;

			float totalWeights = sampleWeights[0].x;

			// Add pairs of additional sample taps, positioned
			// along a line in both directions from the center.
			for (size_t i = 0; i < SAMPLE_COUNT / 2; i++)
			{
				// Store weights for the positive and negative taps.
				float weight = ComputeGaussian(float(i + 1.f), params.blurAmount);

				sampleWeights[i * 2 + 1].x = weight;
				sampleWeights[i * 2 + 2].x = weight;

				totalWeights += weight * 2;

				// To get the maximum amount of blurring from a limited number of
				// pixel shader samples, we take advantage of the bilinear filtering
				// hardware inside the texture fetch unit. If we position our texture
				// coordinates exactly halfway between two texels, the filtering unit
				// will average them for us, giving two samples for the price of one.
				// This allows us to step in units of two texels per sample, rather
				// than just one at a time. The 1.5 offset kicks things off by
				// positioning us nicely in between two texels.
				float sampleOffset = float(i) * 2.f + 1.5f;

				float delta[2] = { dx*sampleOffset, dy*sampleOffset };

				// Store texture coordinate offsets for the positive and negative taps.
				sampleOffsets[i * 2 + 1].x = delta[0];
				sampleOffsets[i * 2 + 1].y = delta[1];
				sampleOffsets[i * 2 + 2].x = -delta[0];
				sampleOffsets[i * 2 + 2].y = -delta[1];
			}

			for (size_t i = 0; i < SAMPLE_COUNT; i++)
			{
				sampleWeights[i].x /= totalWeights;
			}
		}

	private:
		float ComputeGaussian(float n, float theta)
		{
			return (float)((1.0 / sqrtf(2 * XM_PI * theta))
				* expf(-(n * n) / (2 * theta * theta)));
		}
	};

	static_assert(!(sizeof(VS_BLUR_PARAMETERS) % 16),
		"VS_BLUR_PARAMETERS needs to be 16 bytes aligned");

	enum BloomPresets
	{
		Default = 0,
		Soft,
		Desaturated,
		Saturated,
		Blurry,
		Subtle,
		None
	};

	BloomPresets g_Bloom = Default;

	static const VS_BLOOM_PARAMETERS g_BloomPresets[] =
	{
		//Thresh  Blur Bloom  Base  BloomSat BaseSat
		{ 0.25f,  4,   1.25f, 1,    1,       1 }, // Default
		{ 0,      3,   1,     1,    1,       1 }, // Soft
		{ 0.5f,   8,   2,     1,    0,       1 }, // Desaturated
		{ 0.25f,  4,   2,     1,    2,       0 }, // Saturated
		{ 0,      2,   1,     0.1f, 1,       1 }, // Blurry
		{ 0.5f,   2,   1,     1,    1,       1 }, // Subtle
		{ 0.25f,  4,   1.25f, 1,    1,       1 }, // None
	};
};

using namespace BLOOM;


void PostProcess::CreateDevice(DeviceResources* devR)
{
	devRes = devR;
	device = devRes->GetDevice();
	deviceContext = devRes->GetDeviceContext();

	auto blob = DX::ReadData(L"BloomExtract.cso");
	device->CreatePixelShader(&blob.front(), blob.size(), nullptr, m_bloomExtractPS.ReleaseAndGetAddressOf());

	blob = DX::ReadData(L"BloomCombine.cso");
	device->CreatePixelShader(&blob.front(), blob.size(), nullptr, m_bloomCombinePS.ReleaseAndGetAddressOf());

	blob = DX::ReadData(L"GaussianBlur.cso");
	device->CreatePixelShader(&blob.front(), blob.size(), nullptr, m_gaussianBlurPS.ReleaseAndGetAddressOf());

	{
		CD3D11_BUFFER_DESC cbDesc(sizeof(VS_BLOOM_PARAMETERS),
			D3D11_BIND_CONSTANT_BUFFER);
		D3D11_SUBRESOURCE_DATA initData;
		initData.pSysMem = &g_BloomPresets[g_Bloom];
		initData.SysMemPitch = sizeof(VS_BLOOM_PARAMETERS);
		device->CreateBuffer(&cbDesc, &initData, m_bloomParams.ReleaseAndGetAddressOf());
	}

	{
		CD3D11_BUFFER_DESC cbDesc(sizeof(VS_BLUR_PARAMETERS),
			D3D11_BIND_CONSTANT_BUFFER);
		device->CreateBuffer(&cbDesc, nullptr, m_blurParamsWidth.ReleaseAndGetAddressOf());
		device->CreateBuffer(&cbDesc, nullptr, m_blurParamsHeight.ReleaseAndGetAddressOf());
	}

	m_states.reset(new CommonStates(device));
	m_spriteBatch.reset(new SpriteBatch(deviceContext));
}

void PostProcess::CreateResources()
{
	VS_BLUR_PARAMETERS blurData;
	blurData.SetBlurEffectParameters(1.f / (1000 / 2), 0, g_BloomPresets[g_Bloom]);
	deviceContext->UpdateSubresource(m_blurParamsWidth.Get(), 0, nullptr, &blurData, sizeof(VS_BLUR_PARAMETERS), 0);

	blurData.SetBlurEffectParameters(0, 1.f / (800 / 2), g_BloomPresets[g_Bloom]);
	deviceContext->UpdateSubresource(m_blurParamsHeight.Get(), 0, nullptr, &blurData, sizeof(VS_BLUR_PARAMETERS), 0);

	// Obtain the backbuffer for this window which will be the final 3D rendertarget.
	devRes->GetSwapChain()->GetBuffer(0, __uuidof(ID3D11Texture2D), &m_backBuffer);

	// Create a view interface on the rendertarget to use on bind.
	device->CreateRenderTargetView(m_backBuffer.Get(), nullptr, devRes->GetRenderTargetViewComPtr().ReleaseAndGetAddressOf());

	// Full-size render target for scene
	CD3D11_TEXTURE2D_DESC sceneDesc(DXGI_FORMAT_R8G8B8A8_UNORM, 1000, 800,
		1, 1, D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE);
	device->CreateTexture2D(&sceneDesc, nullptr, m_sceneTex.GetAddressOf());
	device->CreateRenderTargetView(m_sceneTex.Get(), nullptr, m_sceneRT.ReleaseAndGetAddressOf());
	device->CreateShaderResourceView(m_sceneTex.Get(), nullptr, m_sceneSRV.ReleaseAndGetAddressOf());

	// Half-size blurring render targets
	CD3D11_TEXTURE2D_DESC rtDesc(DXGI_FORMAT_R8G8B8A8_UNORM, 1000 / 2, 800 / 2,
		1, 1, D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE);
	Microsoft::WRL::ComPtr<ID3D11Texture2D> rtTexture1;
	device->CreateTexture2D(&rtDesc, nullptr, rtTexture1.GetAddressOf());
	device->CreateRenderTargetView(rtTexture1.Get(), nullptr, m_rt1RT.ReleaseAndGetAddressOf());
	device->CreateShaderResourceView(rtTexture1.Get(), nullptr, m_rt1SRV.ReleaseAndGetAddressOf());

	Microsoft::WRL::ComPtr<ID3D11Texture2D> rtTexture2;
	device->CreateTexture2D(&rtDesc, nullptr, rtTexture2.GetAddressOf());
	device->CreateRenderTargetView(rtTexture2.Get(), nullptr, m_rt2RT.ReleaseAndGetAddressOf());
	device->CreateShaderResourceView(rtTexture2.Get(), nullptr, m_rt2SRV.ReleaseAndGetAddressOf());

	m_bloomRect.left = 0;
	m_bloomRect.top = 0;
	m_bloomRect.right = 1000 / 2;
	m_bloomRect.bottom = 800 / 2;

	m_fullscreenRect.left = 0;
	m_fullscreenRect.top = 0;
	m_fullscreenRect.right = 1000;
	m_fullscreenRect.bottom = 800;

}

void PostProcess::DoPostProcess()
{
	ID3D11ShaderResourceView* null[] = { nullptr, nullptr };

	if (g_Bloom == None)
	{
		// Pass-through test
		deviceContext->CopyResource(m_backBuffer.Get(), m_sceneTex.Get());
	}
	else
	{
		// scene -> RT1 (downsample)
		deviceContext->OMSetRenderTargets(1, m_rt1RT.GetAddressOf(), nullptr);
		m_spriteBatch->Begin(SpriteSortMode_Immediate,
			nullptr, nullptr, nullptr, nullptr,
			[=]() {
			deviceContext->PSSetConstantBuffers(0, 1, m_bloomParams.GetAddressOf());
			deviceContext->PSSetShader(m_bloomExtractPS.Get(), nullptr, 0);
		});
		m_spriteBatch->Draw(m_sceneSRV.Get(), m_bloomRect);
		m_spriteBatch->End();
		
		// RT1 -> RT2 (blur horizontal)
		deviceContext->OMSetRenderTargets(1, m_rt2RT.GetAddressOf(), nullptr);
		m_spriteBatch->Begin(SpriteSortMode_Immediate,
			nullptr, nullptr, nullptr, nullptr,
			[=]() {
			deviceContext->PSSetShader(m_gaussianBlurPS.Get(), nullptr, 0);
			deviceContext->PSSetConstantBuffers(0, 1, m_blurParamsWidth.GetAddressOf());
		});
		m_spriteBatch->Draw(m_rt1SRV.Get(), m_bloomRect);
		m_spriteBatch->End();

		deviceContext->PSSetShaderResources(0, 2, null);

		// RT2 -> RT1 (blur vertical)
		deviceContext->OMSetRenderTargets(1, m_rt1RT.GetAddressOf(), nullptr);
		m_spriteBatch->Begin(SpriteSortMode_Immediate,
			nullptr, nullptr, nullptr, nullptr,
			[=]() {
			deviceContext->PSSetShader(m_gaussianBlurPS.Get(), nullptr, 0);
			deviceContext->PSSetConstantBuffers(0, 1, m_blurParamsHeight.GetAddressOf());
		});
		m_spriteBatch->Draw(m_rt2SRV.Get(), m_bloomRect);
		m_spriteBatch->End();
		
		// RT1 + scene
		deviceContext->OMSetRenderTargets(1, devRes->GetRenderTargetViewComPtr().GetAddressOf(), nullptr);
		m_spriteBatch->Begin(SpriteSortMode_Immediate,
			nullptr, nullptr, nullptr, nullptr,
			[=]() {
			deviceContext->PSSetShader(m_bloomCombinePS.Get(), nullptr, 0);
			deviceContext->PSSetShaderResources(1, 1, m_sceneSRV.GetAddressOf());
			deviceContext->PSSetConstantBuffers(0, 1, m_bloomParams.GetAddressOf());
		});
		m_spriteBatch->Draw(m_rt1SRV.Get(), m_fullscreenRect);
		m_spriteBatch->End();
	}

	deviceContext->PSSetShaderResources(0, 2, null);
}

void PostProcess::Clear()
{
	deviceContext->ClearRenderTargetView(m_sceneRT.Get(), Colors::CornflowerBlue);
	deviceContext->ClearDepthStencilView(devRes->GetDepthStencilView(), D3D11_CLEAR_DEPTH, 1.0f, 0);
	deviceContext->OMSetRenderTargets(1, m_sceneRT.GetAddressOf(), devRes->GetDepthStencilView());
}