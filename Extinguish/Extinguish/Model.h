#pragma once
#include "../ShaderStructures/ShaderStructures.h"
#include "DeviceResources.h"

// THIS CLASS HAS DEPRECATED //
// THIS WAS THE RENDERER BACK IN RTA //
// I USE IT FOR REFERENCE WHEN I'M WRITING MY RENDERER //

class Model
{
private:
	std::wstring resourcesPath = L"../Resources/";

	const DeviceResources* devResources;
	ID3D11Device* device;
	ID3D11DeviceContext* devContext;

	//string filePath;
	string texturePath;
	string normalPath;
	string specPath;
	vector<Vertex> mVertices;
	vector<VS_BasicInput> mBasicVertices;
	vector<unsigned int> mIndices;
	VertexShaderTypes vertexType;

	//devices
	Microsoft::WRL::ComPtr<ID3D11Buffer> vertexBuffer;
	Microsoft::WRL::ComPtr<ID3D11Buffer> indexBuffer;
	Microsoft::WRL::ComPtr<ID3D11VertexShader> vertexShader;
	Microsoft::WRL::ComPtr<ID3D11VertexShader> preDepthPassVertexShader;
	Microsoft::WRL::ComPtr<ID3D11VertexShader> preShadowVertexShader;
	Microsoft::WRL::ComPtr<ID3D11PixelShader > pixelShader;
	Microsoft::WRL::ComPtr<ID3D11PixelShader > preShadowPixelShader;
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> textureSRV;
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> normalSRV;
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> specSRV;
	Microsoft::WRL::ComPtr<ID3D11InputLayout> inputLayout;

	//constant buffer data
	ModelViewProjectionConstantBuffer mvpData;
	ModelViewProjectionConstantBuffer lightMVPData;
	//Microsoft::WRL::ComPtr<ID3D11Buffer> mvpConstantBuffer;
	//Microsoft::WRL::ComPtr<ID3D11Buffer> lightMvpConstantBuffer;
	BoneOffsetConstantBuffer boneOffsetData;
	//Microsoft::WRL::ComPtr<ID3D11Buffer> boneOffsetConstantBuffer;

	//Private helper functions
	void DepthPrePass();
	void LightCulling();
	void DepthPrePassShadow();

public:
	//for model with bones
	void Init(VertexShaderTypes shaderType, ID3D11VertexShader* vShader, ID3D11PixelShader* pShader, ID3D11InputLayout* iLayout, string tPath, string nPath, XMMATRIX& model, XMFLOAT4X4 view, XMFLOAT4X4 projection, XMFLOAT4X4* boneOffData, std::wstring name);
	
	//for a basic model
	void Init(VertexShaderTypes shaderType, ID3D11VertexShader* vShader, ID3D11VertexShader* preDepthPassVShader, ID3D11PixelShader* pShader, ID3D11InputLayout* iLayout, string tPath, XMMATRIX& model, XMFLOAT4X4 view, XMFLOAT4X4 projection, std::wstring name);
	void SetBasicVerts(vector<VS_BasicInput> verts) { mBasicVertices = verts; }
	void SetNormalMap(vector<VS_BasicInput> verts) { mBasicVertices = verts; }
	void SetSpecMap(string sPath) { specPath = sPath; }
	void SetIndices(vector<unsigned int> ind) { mIndices = ind; }
	//void LoadMesh(std::wstring name);
	//void LoadBasicMesh(std::wstring name);
	void CreateDevResources(DeviceResources const * deviceResources);
	void Render();

	//getters

	//setters 
	//void SetFilePath(string path) { filePath = path; }
	//void SetTexturePath(string path) { texturePath = path; }
	//void SetVertexShader(ID3D11VertexShader* vs, Shadertypes type) { vertexShader = vs; vertexType = type; }
	//void SetPixelShader(ID3D11PixelShader* ps) { pixelShader = ps; }
	//void SetInputLayout(ID3D11InputLayout* ip) { inputLayout = ip; }
	//void SetVertices(vector<Vertex> verts) { vertices = verts; }
	//void SetVertices(vector<VS_BasicInput> verts) { basicVertices = verts; }
	//void SetIndices(vector<unsigned int> ind) { indices = ind; }
	void SetModel(XMMATRIX& model);
	void SetView(XMFLOAT4X4 view) { mvpData.view = view; }
	//void SetProjection(XMFLOAT4X4 projection) { mvpData.projection = projection; }
	void SetBoneOffsetData(vector<XMFLOAT4X4> data);
	void SetShadowData(ID3D11VertexShader* vShaderPreShadow, ID3D11PixelShader* pShaderPreShadow, XMMATRIX& model, XMFLOAT4X4 view, XMFLOAT4X4 projection);
};