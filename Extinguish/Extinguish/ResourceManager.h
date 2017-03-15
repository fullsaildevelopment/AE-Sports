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
#include "vec3.h"
#include "vec4.h"


//class Animation;
//class BindPose;
//class AnimationSet;

struct ED2Mesh;

class ResourceManager
{
private:
	static ResourceManager* singleton;

	std::string resourcesPath = "../Resources/";
	std::string debugPath = "../Bin/x64/Debug/";
	std::string releasePath = "../Bin/x64/Release/";
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

	// client server wrappers

	bool isServer = true;
	bool isMultiplayer = false;
	bool gamePaused = false;

	//private helper functions
	Skeleton LoadInSkeleton(std::string path);
	Animation LoadInAnimation(std::string path);
	void LoadAndCreateShaders();
	void LoadInMesh(std::string path);
	void LoadInBasicMesh(std::string path, ED2Mesh* collmesh);
	void LoadInAnimationSetsAndMeshes();
	void DoFBXExporting();
	void CreateInputLayouts();
	void LoadInTextures();
public:
	std::vector<ED2Mesh*> collisionMeshes;
	ResourceManager();
	~ResourceManager();

	void Init(DeviceResources const* devResources);
	void Shutdown();

	// setters
	void SetMultiplayer(bool m) { isMultiplayer = m; }
	void SetPaused(bool m) { gamePaused = m; }
	void SetServer(bool m) { isServer = m; }

	//getters
	static ResourceManager* GetSingleton();
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
	bool IsPaused() { return gamePaused; }
	bool IsMultiplayer() { return isMultiplayer; }
	bool IsServer() { return isServer; }
	//IDWriteFactory * GetWriteFactory() { return pDWriteFactory.Get(); }
	//IDWriteTextFormat * GetTextFormat() { return pTextFormat.Get(); }
	//ID2D1Factory * GetID2D1Factory() { return pD2DFactory.Get(); }
	//ID2D1HwndRenderTarget * GetRenderTarget() { return pRT.Get(); }
	//ID2D1SolidColorBrush * GetBrush() { return pBrush.Get(); }
	/*D2D1_RECT_F * GetRect() { return &layoutRect; }*/
	//IDWriteTextLayout * GetTextLayout() { return pTextLayout.Get(); }

	template <typename T>
	ID3D11Buffer* CreateInstancedBuffer(int num, T* instanced)
	{
		ID3D11Buffer* m_instanceBuffer;
		D3D11_BUFFER_DESC instanceBufferDesc;
		D3D11_SUBRESOURCE_DATA instancedData;

		instanceBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
		instanceBufferDesc.ByteWidth = sizeof(T) * num;
		instanceBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
		instanceBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
		instanceBufferDesc.MiscFlags = 0;
		instanceBufferDesc.StructureByteStride = 0;

		instancedData.pSysMem = instanced;
		instancedData.SysMemPitch = 0;
		instancedData.SysMemSlicePitch = 0;

		// Create the instance buffer.
		device->CreateBuffer(&instanceBufferDesc, &instancedData, &m_instanceBuffer);
		return m_instanceBuffer;
	}

	template <typename T>
	ID3D11Buffer* CreateConstantBuffer(T* data)
	{
		ID3D11Buffer* m_constantBuffer;
		D3D11_BUFFER_DESC BufferDesc;
		D3D11_SUBRESOURCE_DATA Data;

		BufferDesc.Usage = D3D11_USAGE_DEFAULT;
		BufferDesc.ByteWidth = sizeof(T);
		BufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
		BufferDesc.CPUAccessFlags = 0;
		BufferDesc.MiscFlags = 0;
		BufferDesc.StructureByteStride = 0;

		Data.pSysMem = data;
		Data.SysMemPitch = 0;
		Data.SysMemSlicePitch = 0;

		// Create the instance buffer.
		device->CreateBuffer(&BufferDesc, &Data, &m_constantBuffer);
		return m_constantBuffer;
	}

	template <typename T>
	void UpdateConstantBuffer(ID3D11Buffer* cb, T* data)
	{
		if(cb)
			devContext->UpdateSubresource(cb, NULL, NULL, &data, NULL, NULL);
	}

	//unsigned int GetPixelShaderIndex(std::string name);
	//unsigned int GetVertexShaderIndex(std::string name);
	//unsigned int GetComputeShaderIndex(std::string name);
	//unsigned int GetVertexBufferIndex(std::string name);
	//unsigned int GetIndexBufferIndex(std::string name);
	//unsigned int GetInputLayoutIndex(std::string name);
};