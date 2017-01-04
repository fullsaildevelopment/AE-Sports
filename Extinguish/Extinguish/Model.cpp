#include "Model.h"
#include <fstream>
using namespace DirectX;

//for bind vertices
void Model::Init(VertexShaderTypes shaderType, ID3D11VertexShader* vShader, ID3D11PixelShader* pShader, ID3D11InputLayout* iLayout, string tPath, string nPath, XMMATRIX& model, XMFLOAT4X4 view, XMFLOAT4X4 projection, XMFLOAT4X4* boneOffData, std::wstring name)
{
	vertexType = shaderType;
	vertexShader = vShader;
	pixelShader = pShader;
	inputLayout = iLayout;
	texturePath = tPath;
	normalPath = nPath;
	SetModel(model);
	mvpData.view = view;
	mvpData.projection = projection;
	//SetBoneOffsetData(boneOffData);
	//LoadMesh(name);
}

//for basic vertices
void Model::Init(VertexShaderTypes shaderType, ID3D11VertexShader* vShader, ID3D11VertexShader* preDepthPassVShader, ID3D11PixelShader* pShader, ID3D11InputLayout* iLayout, string tPath, XMMATRIX& model, XMFLOAT4X4 view, XMFLOAT4X4 projection, std::wstring name)
{
	vertexType = shaderType;
	vertexShader = vShader;
	preDepthPassVertexShader = preDepthPassVShader;
	pixelShader = pShader;
	inputLayout = iLayout;
	texturePath = tPath;
	SetModel(model);
	mvpData.view = view;
	mvpData.projection = projection;

	if (name.size())
	{
		//LoadBasicMesh(name);
	}
}

//void Model::LoadMesh(std::wstring name)
//{
//	std::wstring fullPath;
//
//	fullPath = resourcesPath;
//	fullPath += name + L"/" + name + L".mesh";
//
//
//	std::ifstream bin;
//	std::vector<Vertex> verts;
//	std::vector<unsigned int> indices;
//	unsigned int numVerts;
//	unsigned int numIndices;
//
//	bin.open(fullPath, std::ios::binary);
//
//	if (bin.is_open())
//	{
//		//Read Header
//		bin.read((char*)&numVerts, sizeof(unsigned int));
//		bin.read((char*)&numIndices, sizeof(unsigned int));
//
//		//resize based off of header
//		verts.resize(numVerts);
//		indices.resize(numIndices);
//
//		//read in verts
//		bin.read((char*)verts.data(), sizeof(Vertex) * numVerts);
//
//		//read in names
//		bin.read((char*)indices.data(), sizeof(unsigned int) * numIndices);
//
//		bin.close();
//
//		mVertices = verts;
//		mIndices = indices;
//	}
//
//}

//void Model::LoadBasicMesh(std::wstring name)
//{
//	std::wstring fullPath;
//
//	fullPath = resourcesPath;
//	fullPath += name + L"/" + name + L".bmesh";
//
//
//	std::ifstream bin;
//	std::vector<VS_BasicInput> verts;
//	std::vector<unsigned int> indices;
//	unsigned int numVerts;
//	unsigned int numIndices;
//
//	bin.open(fullPath, std::ios::binary);
//
//	if (bin.is_open())
//	{
//		//Read Header
//		bin.read((char*)&numVerts, sizeof(unsigned int));
//		bin.read((char*)&numIndices, sizeof(unsigned int));
//
//		//resize based off of header
//		verts.resize(numVerts);
//		indices.resize(numIndices);
//
//		//read in verts
//		bin.read((char*)verts.data(), sizeof(VS_BasicInput) * numVerts);
//
//		//read in names
//		bin.read((char*)indices.data(), sizeof(unsigned int) * numIndices);
//
//		bin.close();
//
//		mBasicVertices = verts;
//		mIndices = indices;
//	}
//}

void Model::CreateDevResources(DeviceResources const * deviceResources)
{
	devResources = deviceResources;
	device = devResources->GetDevice();
	devContext = devResources->GetDeviceContext();

	//create vertex bufer
	D3D11_SUBRESOURCE_DATA vertexBufferData;
	vertexBufferData.SysMemPitch = 0;
	vertexBufferData.SysMemSlicePitch = 0;

	if (vertexType == VertexShaderTypes::vsBASIC) //vertices are a vector of Vertex though, not VS_BasicInput, but because it's a pointer to memory, we're ok as long as the stride is good
	{
		vertexBufferData.pSysMem = mBasicVertices.data();

		CD3D11_BUFFER_DESC vertexBufferDesc(sizeof(VS_BasicInput) * (unsigned int)mBasicVertices.size(), D3D11_BIND_VERTEX_BUFFER);
		device->CreateBuffer(&vertexBufferDesc, &vertexBufferData, vertexBuffer.GetAddressOf());
	}
	else if (vertexType == VertexShaderTypes::vsBIND)
	{
		vertexBufferData.pSysMem = mVertices.data();

		CD3D11_BUFFER_DESC vertexBufferDesc(sizeof(Vertex) * (unsigned int)mVertices.size(), D3D11_BIND_VERTEX_BUFFER);
		device->CreateBuffer(&vertexBufferDesc, &vertexBufferData, vertexBuffer.GetAddressOf());
	}

	//create index buffer
	if (mIndices.data())
	{
		D3D11_SUBRESOURCE_DATA indexBufferData;
		indexBufferData.pSysMem = mIndices.data();
		indexBufferData.SysMemPitch = 0;
		indexBufferData.SysMemSlicePitch = 0;

		CD3D11_BUFFER_DESC indexBufferDesc(sizeof(unsigned int) * (unsigned int)mIndices.size(), D3D11_BIND_INDEX_BUFFER);

		device->CreateBuffer(&indexBufferDesc, &indexBufferData, indexBuffer.GetAddressOf());
	}

	//create constant buffers
	CD3D11_BUFFER_DESC mvpBufferDesc(sizeof(ModelViewProjectionConstantBuffer), D3D11_BIND_CONSTANT_BUFFER);
	HRESULT hrtemp = device->CreateBuffer(&mvpBufferDesc, NULL, mvpConstantBuffer.GetAddressOf());

	CD3D11_BUFFER_DESC lightMvpBufferDesc(sizeof(ModelViewProjectionConstantBuffer), D3D11_BIND_CONSTANT_BUFFER);
	HRESULT lightHrtemp = device->CreateBuffer(&lightMvpBufferDesc, NULL, lightMvpConstantBuffer.GetAddressOf());

	//bone offsets
	if (vertexType == VertexShaderTypes::vsBIND)
	{
		CD3D11_BUFFER_DESC boneOffsetDesc(sizeof(BoneOffsetConstantBuffer), D3D11_BIND_CONSTANT_BUFFER);
		device->CreateBuffer(&boneOffsetDesc, NULL, boneOffsetConstantBuffer.GetAddressOf());
	}

	//create shader resource view with texture path
	wstring wideTexturePath = wstring(texturePath.begin(), texturePath.end());
	HRESULT baseTextResult = CreateDDSTextureFromFile(device, wideTexturePath.c_str(), nullptr, textureSRV.GetAddressOf());

	if (normalPath.size())
	{
		wstring wideNormalPath = wstring(normalPath.begin(), normalPath.end());
		HRESULT baseNormalResult = CreateDDSTextureFromFile(device, wideNormalPath.c_str(), nullptr, normalSRV.GetAddressOf());
	}

	if (specPath.size())
	{
		wstring wideSpecPath = wstring(specPath.begin(), specPath.end());
		HRESULT baseNormalResult = CreateDDSTextureFromFile(device, wideSpecPath.c_str(), nullptr, specSRV.GetAddressOf());
	}

}

void Model::Render()
{
	//do shadow depth press pass
	if (preShadowVertexShader)
	{
		DepthPrePassShadow();
	}

	//do light depth pre pass

	//use computer shader to find what lights

	//reset stuff I turned off in depth pre pass
	devContext->OMSetRenderTargets(1, devResources->GetRenderTargetViewAddress(), devResources->GetDepthStencilView());

	////set shaders
	devContext->VSSetShader(vertexShader.Get(), NULL, NULL);
	devContext->PSSetShader(pixelShader.Get(), NULL, NULL);

	//set input layout
	devContext->IASetInputLayout(inputLayout.Get());

	//update constant buffers
	devContext->UpdateSubresource(mvpConstantBuffer.Get(), NULL, NULL, &mvpData, NULL, NULL);

	if (vertexType == VertexShaderTypes::vsBIND)
	{
		devContext->UpdateSubresource(boneOffsetConstantBuffer.Get(), NULL, NULL, &boneOffsetData, NULL, NULL);
	}

	devContext->UpdateSubresource(lightMvpConstantBuffer.Get(), NULL, NULL, &lightMVPData, NULL, NULL);

	//set constant buffers
	devContext->VSSetConstantBuffers(0, 1, mvpConstantBuffer.GetAddressOf());

	if (vertexType == VertexShaderTypes::vsBIND)
	{
		devContext->VSSetConstantBuffers(1, 1, boneOffsetConstantBuffer.GetAddressOf());
	}

	devContext->VSSetConstantBuffers(2, 1, lightMvpConstantBuffer.GetAddressOf());

	//set vertex buffer
	if (vertexType == VertexShaderTypes::vsBIND)
	{
		UINT stride = sizeof(Vertex);
		UINT offset = 0;

		devContext->IASetVertexBuffers(0, 1, vertexBuffer.GetAddressOf(), &stride, &offset);
	}
	else
	{
		UINT stride = sizeof(VS_BasicInput);
		UINT offset = 0;

		devContext->IASetVertexBuffers(0, 1, vertexBuffer.GetAddressOf(), &stride, &offset);
	}

	//set shader resource view
	devContext->PSSetShaderResources(0, 1, textureSRV.GetAddressOf());
	devContext->PSSetShaderResources(1, 1, normalSRV.GetAddressOf());
	devContext->PSSetShaderResources(2, 1, specSRV.GetAddressOf());
	devContext->PSSetShaderResources(3, 1, devResources->GetShadowMapSRVAddress());

	//set index buffer
	if (mIndices.data())
	{
		devContext->IASetIndexBuffer(indexBuffer.Get(), DXGI_FORMAT_R32_UINT, 0);

		//and finally... draw model
		devContext->DrawIndexed((unsigned int)mIndices.size(), 0, 0);
	}
	else
	{
		if (vertexType == VertexShaderTypes::vsBIND)
		{
			devContext->Draw((unsigned int)mVertices.size(), 0);
		}
		else
		{
			devContext->Draw((unsigned int)mBasicVertices.size(), 0);
		}
	}
}

//setters
void Model::SetModel(XMMATRIX& model)
{
	XMFLOAT4X4 tempModel;

	XMStoreFloat4x4(&tempModel, model);
	mvpData.model = tempModel;
}

void Model::SetBoneOffsetData(vector<XMFLOAT4X4> data)
{
	for (int i = 0; i < data.size(); ++i)
	{
		boneOffsetData.boneOffsets[i] = data[i];
	}
}

void Model::SetShadowData(ID3D11VertexShader* vShaderPreShadow, ID3D11PixelShader* pShaderPreShadow, XMMATRIX& model, XMFLOAT4X4 view, XMFLOAT4X4 projection)
{
	preShadowVertexShader = vShaderPreShadow;
	preShadowPixelShader = pShaderPreShadow;

	XMFLOAT4X4 tempModel;

	XMStoreFloat4x4(&tempModel, model);
	lightMVPData.model = tempModel;

	lightMVPData.view = view;
	lightMVPData.projection = projection;
}

//helper functions

void Model::DepthPrePass()
{
	UINT stride = sizeof(VS_BasicInput);
	UINT offset = 0;

	//set RTV and STV
	ID3D11RenderTargetView* nullRTV = NULL;

	devContext->OMSetRenderTargets(1, &nullRTV, devResources->GetDepthStencilView());

	//set shaders
	devContext->VSSetShader(preDepthPassVertexShader.Get(), NULL, NULL);
	devContext->PSSetShader(NULL, NULL, 0);

	//set input layout
	devContext->IASetInputLayout(inputLayout.Get());

	//update and set MVP cbuffer
	devContext->UpdateSubresource(mvpConstantBuffer.Get(), NULL, NULL, &mvpData, NULL, NULL);
	devContext->VSSetConstantBuffers(0, 1, mvpConstantBuffer.GetAddressOf());

	//set vertex buffer
	devContext->IASetVertexBuffers(0, 1, vertexBuffer.GetAddressOf(), &stride, &offset);

	//set textureSRV
	devContext->PSSetShaderResources(0, 1, textureSRV.GetAddressOf());

	//set index buffer then draw
	if (mIndices.data())
	{
		devContext->IASetIndexBuffer(indexBuffer.Get(), DXGI_FORMAT_R32_UINT, 0);

		//and finally... draw model
		devContext->DrawIndexed((unsigned int)mIndices.size(), 0, 0);
	}
	else
	{
		if (vertexType == VertexShaderTypes::vsBIND)
		{
			devContext->Draw((unsigned int)mVertices.size(), 0);
		}
		else
		{
			devContext->Draw((unsigned int)mBasicVertices.size(), 0);
		}
	}
}

void Model::LightCulling()
{
	ID3D11RenderTargetView* nullRTV = NULL;
	ID3D11ShaderResourceView* nullSRV = NULL;
	ID3D11DepthStencilView* nullDSV = NULL;
	const float res = 16.0f;
	const unsigned int NUM_TILES_X = (unsigned int)((float)CLIENT_WIDTH / res);
	const unsigned int NUM_TILES_Y = (unsigned int)((float)CLIENT_HEIGHT / res);

	//set render target and dsv to null
	devContext->OMSetRenderTargets(1, &nullRTV, nullDSV);

	//set shaders
	devContext->VSSetShader(NULL, NULL, NULL);
	devContext->PSSetShader(NULL, NULL, 0);
	//devContext->CSSetShader(
	//dispatch threads
	devContext->Dispatch(NUM_TILES_X, NUM_TILES_Y, 1);
}

void Model::DepthPrePassShadow()
{
	UINT stride = sizeof(Vertex);
	UINT offset = 0;

	//set RTV and STV
	//ID3D11RenderTargetView* nullRTV = NULL;

	devContext->OMSetRenderTargets(1, devResources->GetShadowMapRTVAddress(), devResources->GetShadowMapDSV());

	//set shaders
	devContext->VSSetShader(preShadowVertexShader.Get(), NULL, NULL);
	devContext->PSSetShader(preShadowPixelShader.Get(), NULL, NULL);

	//set input layout
	devContext->IASetInputLayout(inputLayout.Get());

	//update and set MVP cbuffer
	devContext->UpdateSubresource(lightMvpConstantBuffer.Get(), NULL, NULL, &lightMVPData, NULL, NULL);
	devContext->VSSetConstantBuffers(0, 1, lightMvpConstantBuffer.GetAddressOf());

	//update set bone offset cbuffer
	devContext->UpdateSubresource(boneOffsetConstantBuffer.Get(), NULL, NULL, &boneOffsetData, NULL, NULL);
	devContext->VSSetConstantBuffers(1, 1, boneOffsetConstantBuffer.GetAddressOf());

	//set vertex buffer
	devContext->IASetVertexBuffers(0, 1, vertexBuffer.GetAddressOf(), &stride, &offset);

	//set index buffer then draw
	if (mIndices.data())
	{
		devContext->IASetIndexBuffer(indexBuffer.Get(), DXGI_FORMAT_R32_UINT, 0);

		//and finally... draw model
		devContext->DrawIndexed((unsigned int)mIndices.size(), 0, 0);
	}
	else
	{
		if (vertexType == VertexShaderTypes::vsBIND)
		{
			devContext->Draw((unsigned int)mVertices.size(), 0);
		}
		else
		{
			devContext->Draw((unsigned int)mBasicVertices.size(), 0);
		}
	}
}