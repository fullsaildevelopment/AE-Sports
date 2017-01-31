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
#include "GameObject.h"
class Renderer : public Component
{
private:
	Blender* blender;
	
	ID3D11Buffer* vertexBuffer;
	ID3D11Buffer* indexBuffer;
	ID3D11InputLayout* inputLayout;
	ID3D11VertexShader* vertexShader;
	ID3D11PixelShader* pixelShader;
	ID3D11ComputeShader* computeShader;
	ID3D11ShaderResourceView* diffuseSRV;
	DeviceResources* devResources;
	ModelViewProjectionConstantBuffer mvpData;
	IDWriteFactory * pDWriteFactory;
	IDWriteTextFormat * pTextFormat;
	ID2D1Factory * pD2DFactory;
	ID2D1HwndRenderTarget * pRT;
	ID2D1SolidColorBrush * pBrush;
	unsigned int vertexStride;
	unsigned int numVerts;
	unsigned int numIndices;

	bool isButton = false;
	D2D1_RECT_F * layoutRect;

public:
	Renderer();
	~Renderer();

	void Init(std::string mesh, std::string psName, std::string vsName, std::string csName, std::string curAnimName, XMFLOAT4X4 projection, ResourceManager* resources, DeviceResources* deviceResources);
	void Update(float dt, InputManager* input) override;

	//getters
	std::vector<DirectX::XMFLOAT4X4> GetBoneOffsets();
	std::vector<DirectX::XMFLOAT4X4> GetBonesWorlds();
	Blender* GetBlender();

	//setters
	void SetNextAnimation(std::string animName);
	void SetModel(XMMATRIX& model);
	void SetModel(XMFLOAT4X4& model);
	void SetView(XMFLOAT4X4 view);
	void SetProjection(XMFLOAT4X4 projection);
	void SetBlendInfo(BlendInfo info);
	void SetIsButton(bool tf) { isButton = tf; }
	//void SetInverseBindPoses(std::vector<DirectX::XMFLOAT4X4> poses) { boneOffsets = poses; }
	//void SetBonesWorlds(std::vector<DirectX::XMFLOAT4X4> worlds) { bonesWorlds = worlds; }
};