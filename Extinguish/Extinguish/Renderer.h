#pragma once
#include <DirectXMath.h>
#include <vector>
#include <string>
#include "Component.h"
#include "Blender.h"
#include "ResourceManager.h"
#include "DeviceResources.h"
#include "../ShaderStructures/ShaderStructures.h"
#include "Button.h"
#include "vec4.h"

class GameObject;

class Renderer : public Component
{
private:
	Blender* blender;

	float4 TeamColor = { 1,1,1,1 };

	DeviceResources* devResources;
	ID3D11Buffer* vertexBuffer;
	ID3D11Buffer* instancedBuffer;
	ID3D11Buffer* instancedBuffer2;
	ID3D11Buffer* indexBuffer;
	ID3D11Buffer* teamcolorBuffer;
	ID3D11InputLayout* inputLayout;
	ID3D11VertexShader* vertexShader;
	ID3D11PixelShader* pixelShader;
	ID3D11ComputeShader* computeShader;
	ID3D11ShaderResourceView* diffuseSRV;
	ID3D11ShaderResourceView* teamcolorSRV;
	ModelViewProjectionConstantBuffer mvpData;
	
	unsigned int vertexStride;
	unsigned int numVerts;
	unsigned int numIndices;
	float3* m_instanced;
	unsigned int* m_instancedcolor;
	int numIns;
	std::vector<DirectX::XMFLOAT4X4> boneOffsets;

public:
	Renderer();
	~Renderer();

	void Init(std::string mesh, std::string psName, std::string vsName, std::string csName, std::string curAnimName, XMFLOAT4X4 projection, DeviceResources* deviceResources);
	void Init(int numInstences, float3* instanced, unsigned int* color, std::string mesh, std::string psName, std::string vsName, std::string csName, std::string curAnimName, XMFLOAT4X4 projection, DeviceResources* deviceResources);
	void Update(float dt) override;
	void Render();

	//getters
	std::vector<DirectX::XMFLOAT4X4> GetBoneOffsets();
	std::vector<DirectX::XMFLOAT4X4> GetBonesWorlds();
	Blender* GetBlender();

	//setters
	void SetCurAnimation(int animIndex);
	void SetNextAnimation(std::string animName);
	void SetNextAnimation(int animIndex);
	void SetModel(XMMATRIX& model);
	void SetModel(XMFLOAT4X4& model);
	void SetView(XMFLOAT4X4 view);
	void SetProjection(XMFLOAT4X4 projection);
	void SetBlendInfo(BlendInfo info);
	void SetBoneOffsets(std::vector<DirectX::XMFLOAT4X4>& boneOffsets);
	void SetTeamColor(float4 c);
	//void SetInverseBindPoses(std::vector<DirectX::XMFLOAT4X4> poses) { boneOffsets = poses; }
	//void SetBonesWorlds(std::vector<DirectX::XMFLOAT4X4> worlds) { bonesWorlds = worlds; }
};