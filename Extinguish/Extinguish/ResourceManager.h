#pragma once
#include "Animation.h"
#include "AnimationSet.h"
#include "Skeleton.h"
#include "HashString.h"
#include "DeviceResources.h"
#include <map>
#include <string>

//class Animation;
//class BindPose;
//class AnimationSet;

class ResourceManager
{
private:
	std::string resourcesPath = "../Resources/";
	std::string debugPath = "../Bin/x64/Debug/";
	std::map<unsigned int, AnimationSet> animationSets;
	unsigned int animationSetIndex;

	//Hashstrings 
	HashString animationSetsTable;
	HashString vertexShadersTable;
	HashString pixelShadersTable;
	HashString computeShadersTable;


	ID3D11Device* device;
	ID3D11DeviceContext* devContext;
	vector<void*> vertices;
	vector<Microsoft::WRL::ComPtr<ID3D11InputLayout>> inputLayouts;
	vector<Microsoft::WRL::ComPtr<ID3D11PixelShader>> pixelShaders;
	vector<Microsoft::WRL::ComPtr<ID3D11VertexShader>> vertexShaders;
	vector<Microsoft::WRL::ComPtr<ID3D11ComputeShader>> computeShaders;

	//private helper functions
	Skeleton LoadInSkeleton(std::string path);
	Animation LoadInAnimation(std::string path);
	void LoadAndCreateShaders();
	void LoadInMesh(std::string path);

public:
	ResourceManager();
	~ResourceManager();

	void Init(DeviceResources const * devResources);
	void LoadInAnimationSetsAndMeshes();

	//getters
	AnimationSet* GetAnimationSet(std::string animation);
	ID3D11PixelShader* GetPixelShader(unsigned int index);
	ID3D11VertexShader* GetVertexShader(unsigned int index);
	ID3D11ComputeShader* GetComputeShader(unsigned int index);
	unsigned int GetPixelShaderIndex(std::string name);
	unsigned int GetVertexShaderIndex(std::string name);
	unsigned int GetComputeShaderIndex(std::string name);
};