#pragma once

#include <DirectXMath.h>
#include <vector>
#include <string>
#include "Component.h"
#include "ResourceManager.h"
#include "DeviceResources.h"
#include "../ShaderStructures/ShaderStructures.h"
#include "vec4.h"



class ShieldRender : public Component
{
private:
	float scrollRate[2];
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
	float noiseOffsets[4];

	float blendFactor[4] = { 0.0f, 0.0f, 0.0f, 0.0f };
	UINT sampleMask = 0xffffffff;
public:
	ShieldRender(GameObject* o);
	void Init(std::string mesh, XMFLOAT4X4 projection, DeviceResources* deviceResources, float rate1, float rate2, std::string noiseMap1 = "ShieldBubble", std::string noiseMap2 = "ShieldBubble");
	void SetProjection(XMFLOAT4X4 projection);
	void Render(ID3D11RasterizerState* front, ID3D11RasterizerState* back);
	void SetModel(XMFLOAT4X4& model);
	void SetView(XMFLOAT4X4 view);
	void SetShieldColor(float4 c);
	void UpdateOffsets(float dt);
};