#pragma once

#include <DirectXMath.h>
#include <vector>
#include <string>
#include "Component.h"
#include "ResourceManager.h"
#include "DeviceResources.h"
#include "../ShaderStructures/ShaderStructures.h"
#include "vec4.h"



class ShieldRender
{
private:
	float scrollRate;
	Microsoft::WRL::ComPtr<ID3D11BlendState> m_blendstate;
	Microsoft::WRL::ComPtr<ID3D11SamplerState> m_samplerstate;
	DeviceResources* devResources;
	ID3D11Buffer* vertexBuffer;
	ID3D11Buffer* indexBuffer;
	ID3D11InputLayout* inputLayout;
	ID3D11VertexShader* vertexShader;
	ID3D11PixelShader* pixelShader;
	ID3D11ShaderResourceView* diffuseSRV;
	ID3D11ShaderResourceView* Noisemap1SRV;
	ID3D11ShaderResourceView* Noisemap2SRV;
	ID3D11Buffer* shieldColorBuffer;
	ID3D11Buffer* noiseOffsetBuffer;
	ModelViewProjectionConstantBuffer mvpData;
	unsigned int vertexStride;
	unsigned int numVerts;
	unsigned int numIndices;
	float4 shieldColor;
	float noiseOffsets[2];

	float blendFactor[4] = { 0.0f, 0.0f, 0.0f, 0.0f };
	UINT sampleMask = 0xffffffff;
public:
	void Init(std::string mesh, XMFLOAT4X4 projection, DeviceResources* deviceResources, float rate = 1.0f);
	void SetProjection(XMFLOAT4X4 projection);
	void Render();
	void SetModel(XMFLOAT4X4& model);
	void SetView(XMFLOAT4X4 view);
	void SetShieldColor(float4 c);
	void UpdateOffsets(float dt);
};