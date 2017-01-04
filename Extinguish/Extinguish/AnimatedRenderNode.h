#pragma once
#include <DirectXMath.h>
#include <vector>
#include <string>
#include "Component.h"
#include "Blender.h"

class AnimatedRenderNode : Component
{
private:
	Blender blender;
	
	//per object info
	int indexBuffer;
	int vertexBuffer;
	int vertexSlice; //how many bytes is a vertex
	int numOfVertices;

	//per type info
	int inputLayout;
	int pixelShader;
	int vertexShader;
	int computeShader;

	//int material;
	//std::vector<DirectX::XMFLOAT4X4> boneOffsets;
	//std::vector<DirectX::XMFLOAT4X4> bonesWorlds;

public:
	AnimatedRenderNode();

	void Init(std::string mesh, std::string psName, std::string vsName, std::string csName);
	void Update() override;

	//getters
	std::vector<DirectX::XMFLOAT4X4>& GetBoneOffsets();
	std::vector<DirectX::XMFLOAT4X4>& GetBonesWorlds();

	//setters
	//void SetInverseBindPoses(std::vector<DirectX::XMFLOAT4X4> poses) { boneOffsets = poses; }
	//void SetBonesWorlds(std::vector<DirectX::XMFLOAT4X4> worlds) { bonesWorlds = worlds; }
};