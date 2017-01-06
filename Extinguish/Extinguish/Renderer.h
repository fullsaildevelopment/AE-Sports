#pragma once
#include <DirectXMath.h>
#include <vector>
#include <string>
#include "Component.h"
#include "Blender.h"
#include "ResourceManager.h"

class Renderer : public Component
{
private:
	Blender blender;
	
	//per object info
	//int indexBuffer;
	//int vertexBuffer;
	////int vertexSlice; //how many bytes is a vertex
	//int numOfVertices;
	ID3D11Buffer* vertexBuffer;
	ID3D11Buffer* indexBuffer;
	ID3D11InputLayout* inputLayout;
	ID3D11VertexShader* vertexShader;
	ID3D11PixelShader* pixelShader;
	ID3D11ComputeShader* computeShader;

	//per type info
	//int inputLayout;
	//int pixelShader;
	//int vertexShader;
	//int computeShader;

	//int material;
	//std::vector<DirectX::XMFLOAT4X4> boneOffsets;
	//std::vector<DirectX::XMFLOAT4X4> bonesWorlds;

public:
	Renderer();

	void Init(std::string mesh, std::string psName, std::string vsName, std::string csName, ResourceManager* resources);
	void Update() override;

	//getters
	std::vector<DirectX::XMFLOAT4X4>& GetBoneOffsets();
	std::vector<DirectX::XMFLOAT4X4>& GetBonesWorlds();

	//setters
	//void SetInverseBindPoses(std::vector<DirectX::XMFLOAT4X4> poses) { boneOffsets = poses; }
	//void SetBonesWorlds(std::vector<DirectX::XMFLOAT4X4> worlds) { bonesWorlds = worlds; }
};