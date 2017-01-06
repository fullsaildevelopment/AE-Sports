#pragma once
#include "Animation.h"
#include "AnimationSet.h"
#include "Skeleton.h"
#include "HashString.h"
#include "DeviceResources.h"
#include <map>
#include <string>
#include "../Bin/FBXLoader/FBXLoader.h"
#include "FriendlyIOTransformNode.h"

//class Animation;
//class BindPose;
//class AnimationSet;

class ResourceManager
{
private:
	std::string resourcesPath = "../Resources/";
	std::string debugPath = "../Bin/x64/Debug/";
	std::string ddsPath = "../Assets/Textures/DDS/";
	std::map<unsigned int, AnimationSet> animationSets;
	unsigned int animationSetIndex;

	//Hashstrings 
	HashString vertexBuffersTable;
	HashString indexBuffersTable;
	HashString animationSetsTable;
	HashString vertexShadersTable;
	HashString pixelShadersTable;
	HashString computeShadersTable;
	HashString inputLayoutsTable;
	HashString shaderResourceViewsTable;

	ID3D11Device* device;
	ID3D11DeviceContext* devContext;
	std::vector<Microsoft::WRL::ComPtr<ID3D11Buffer>> vertexBuffers;
	std::vector<Microsoft::WRL::ComPtr<ID3D11Buffer>> indexBuffers;
	std::vector<Microsoft::WRL::ComPtr<ID3D11InputLayout>> inputLayouts;
	std::vector<Microsoft::WRL::ComPtr<ID3D11PixelShader>> pixelShaders;
	std::vector<Microsoft::WRL::ComPtr<ID3D11VertexShader>> vertexShaders;
	std::vector<Microsoft::WRL::ComPtr<ID3D11ComputeShader>> computeShaders;
	std::vector<Microsoft::WRL::ComPtr<ID3D11ShaderResourceView>> shaderResourceViews;
	std::vector<unsigned int> vertexStrides;
	std::vector<unsigned int> numVertices;
	std::vector<unsigned int> numIndices;

	//private helper functions
	Skeleton LoadInSkeleton(std::string path);
	Animation LoadInAnimation(std::string path);
	void LoadAndCreateShaders();
	void LoadInMesh(std::string path);
	void LoadInAnimationSetsAndMeshes();
	void DoFBXExporting();
	void CreateInputLayouts();
	void LoadInTextures();
public:
	ResourceManager();
	~ResourceManager();

	void Init(DeviceResources const* devResources);

	//getters
	AnimationSet* GetAnimationSet(std::string animation);
	ID3D11Buffer* GetVertexBuffer(std::string name);
	ID3D11Buffer* GetIndexBuffer(std::string name);
	ID3D11InputLayout* GetInputLayout(std::string name);
	ID3D11PixelShader* GetPixelShader(std::string name);
	ID3D11VertexShader* GetVertexShader(std::string name);
	ID3D11ComputeShader* GetComputeShader(std::string name);
	ID3D11ShaderResourceView* GetShaderResourceView(std::string name);
	int GetVertexStride(std::string name);
	int GetNumIndices(std::string name);
	int GetNumVertices(std::string name);

	//unsigned int GetPixelShaderIndex(std::string name);
	//unsigned int GetVertexShaderIndex(std::string name);
	//unsigned int GetComputeShaderIndex(std::string name);
	//unsigned int GetVertexBufferIndex(std::string name);
	//unsigned int GetIndexBufferIndex(std::string name);
	//unsigned int GetInputLayoutIndex(std::string name);
};