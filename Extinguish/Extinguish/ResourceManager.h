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

	// menu functions; text

	Microsoft::WRL::ComPtr<IDWriteFactory> pDWriteFactory;
	Microsoft::WRL::ComPtr<IDWriteTextFormat> pTextFormat;
	Microsoft::WRL::ComPtr<ID2D1Factory> pD2DFactory;
	Microsoft::WRL::ComPtr<ID2D1HwndRenderTarget> pRT;
	Microsoft::WRL::ComPtr<ID2D1SolidColorBrush> pBrush;
	D2D1_RECT_F layoutRect;

	//private helper functions
	Skeleton LoadInSkeleton(std::string path);
	Animation LoadInAnimation(std::string path);
	void LoadAndCreateShaders();
	void LoadInMesh(std::string path);
	void LoadInBasicMesh(std::string path);
	void LoadInAnimationSetsAndMeshes();
	void DoFBXExporting();
	void CreateInputLayouts();
	void LoadInTextures();
	void LoadButtonResources(HWND hwnd_);
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

	IDWriteFactory * GetWriteFactory() { return pDWriteFactory.Get(); }
	IDWriteTextFormat * GetTextFormat() { return pTextFormat.Get(); }
	ID2D1Factory * GetID2D1Factory() { return pD2DFactory.Get(); }
	ID2D1HwndRenderTarget * GetRenderTarget() { return pRT.Get(); }
	ID2D1SolidColorBrush * GetBrush() { return pBrush.Get(); }
	D2D1_RECT_F * GetRect() { return &layoutRect; }


	//unsigned int GetPixelShaderIndex(std::string name);
	//unsigned int GetVertexShaderIndex(std::string name);
	//unsigned int GetComputeShaderIndex(std::string name);
	//unsigned int GetVertexBufferIndex(std::string name);
	//unsigned int GetIndexBufferIndex(std::string name);
	//unsigned int GetInputLayoutIndex(std::string name);
};