#include "Renderer.h"
#include "GameObject.h"

//structors
Renderer::Renderer() : Component(nullptr)
{
	blender = nullptr;
}

Renderer::~Renderer()
{
	delete blender;
	blender = nullptr;
}


//basic
void Renderer::Init(std::string mesh, std::string psName, std::string vsName, std::string csName, std::string curAnimName, XMFLOAT4X4 projection, DeviceResources* deviceResources)
{
	indexBuffer = ResourceManager::GetSingleton()->GetIndexBuffer(mesh);
	vertexBuffer = ResourceManager::GetSingleton()->GetVertexBuffer(mesh);
	inputLayout = ResourceManager::GetSingleton()->GetInputLayout(vsName);
	pixelShader = ResourceManager::GetSingleton()->GetPixelShader(psName);
	vertexShader = ResourceManager::GetSingleton()->GetVertexShader(vsName);
	computeShader = ResourceManager::GetSingleton()->GetComputeShader(csName);
	diffuseSRV = ResourceManager::GetSingleton()->GetShaderResourceView(mesh);

	if (mesh == "Mage")
	{
		teamcolorSRV = ResourceManager::GetSingleton()->GetShaderResourceView("TC_Mage");
	}
	if (mesh == "Crosse")
	{
		teamcolorSRV = ResourceManager::GetSingleton()->GetShaderResourceView("TC_Crosse");
	}

	vertexStride = ResourceManager::GetSingleton()->GetVertexStride(mesh);
	numVerts = ResourceManager::GetSingleton()->GetNumVertices(mesh);
	numIndices = ResourceManager::GetSingleton()->GetNumIndices(mesh);
	devResources = deviceResources;
	m_instanced = nullptr;
	numIns = 0;
	SetProjection(projection);

	if (!curAnimName.empty())
	{
		blender = new Blender();
		blender->SetAnimationSet(ResourceManager::GetSingleton()->GetAnimationSet(mesh));
		blender->Init(curAnimName, "");
	}
}

void Renderer::Init(int numInstences, float3* instanced, unsigned int* color, std::string mesh, std::string psName, std::string vsName, std::string csName, std::string curAnimName, XMFLOAT4X4 projection, DeviceResources* deviceResources)
{
	indexBuffer = ResourceManager::GetSingleton()->GetIndexBuffer(mesh);
	vertexBuffer = ResourceManager::GetSingleton()->GetVertexBuffer(mesh);
	inputLayout = ResourceManager::GetSingleton()->GetInputLayout(vsName);
	pixelShader = ResourceManager::GetSingleton()->GetPixelShader(psName);
	vertexShader = ResourceManager::GetSingleton()->GetVertexShader(vsName);
	computeShader = ResourceManager::GetSingleton()->GetComputeShader(csName);
	diffuseSRV = ResourceManager::GetSingleton()->GetShaderResourceView(mesh);
	vertexStride = ResourceManager::GetSingleton()->GetVertexStride(mesh);
	numVerts = ResourceManager::GetSingleton()->GetNumVertices(mesh);
	numIndices = ResourceManager::GetSingleton()->GetNumIndices(mesh);
	devResources = deviceResources;
	m_instanced = instanced;
	m_instancedcolor = color;
	numIns = numInstences;
	instancedBuffer = ResourceManager::GetSingleton()->CreateInstancedBuffer(numInstences, instanced);
	instancedBuffer2 = ResourceManager::GetSingleton()->CreateInstancedBuffer(numInstences, color);
	


	SetProjection(projection);

	if (!curAnimName.empty())
	{
		blender = new Blender();
		blender->SetAnimationSet(ResourceManager::GetSingleton()->GetAnimationSet(mesh));
		blender->Init(curAnimName, "");
	}
}

void Renderer::Update(float dt)
{
	ID3D11DeviceContext* devContext = devResources->GetDeviceContext();

	//update blender
	//if (blender)
	//{
	//	blender->Update(dt * 0.5f, 0);
	//}

	////set shaders
	devContext->VSSetShader(vertexShader, NULL, NULL);
	devContext->PSSetShader(pixelShader, NULL, NULL);

	//set input layout
	devContext->IASetInputLayout(inputLayout);

	//update and set constant buffers
	ID3D11Buffer* mvpConstantBuffer = devResources->GetMVPConstantBuffer();
	devContext->UpdateSubresource(mvpConstantBuffer, NULL, NULL, &mvpData, NULL, NULL);
	devContext->VSSetConstantBuffers(0, 1, &mvpConstantBuffer);

	if (boneOffsets.size())
	{
		//std::vector<DirectX::XMFLOAT4X4> boneOffsets = blender->GetBoneOffsets();

		BoneOffsetConstantBuffer boneOffsetsData;
		for (int i = 0; i < boneOffsets.size(); ++i)
		{
			boneOffsetsData.boneOffsets[i] = boneOffsets[i];
		}

		if (!boneOffsets.empty())
		{
			ID3D11Buffer* boneOffsetConstantBuffer = devResources->GetBoneOffsetConstantBuffer();
			devContext->UpdateSubresource(boneOffsetConstantBuffer, NULL, NULL, &boneOffsetsData, NULL, NULL);
			devContext->VSSetConstantBuffers(1, 1, &boneOffsetConstantBuffer);
		}
	}

	if (teamcolorBuffer)
	{
		devContext->UpdateSubresource(teamcolorBuffer.Get(), NULL, NULL, &TeamColor, NULL, NULL);

		devContext->PSSetConstantBuffers(3, 1, &teamcolorBuffer);
	}

	//set vertex buffer
	UINT offset = 0;

	devContext->IASetVertexBuffers(0, 1, &vertexBuffer, &vertexStride, &offset);

	//set shader resource view
	devContext->PSSetShaderResources(0, 1, &diffuseSRV);
	if(teamcolorSRV)
		devContext->PSSetShaderResources(3, 1, &teamcolorSRV);
	//devContext->PSSetShaderResources(2, 1, specSRV.GetAddressOf());
	//devContext->PSSetShaderResources(3, 1, devResources->GetShadowMapSRVAddress());

}

void Renderer::Render()
{
	ID3D11DeviceContext* devContext = devResources->GetDeviceContext();
	//Draw!
	if (numIns > 0)
	{
		devContext->IASetIndexBuffer(indexBuffer.Get(), DXGI_FORMAT_R32_UINT, 0);

		D3D11_MAPPED_SUBRESOURCE mapstuff;
		D3D11_MAPPED_SUBRESOURCE mapstuff2;

		devContext->Map(instancedBuffer.Get(), 0, D3D11_MAP_WRITE_DISCARD, 0, &mapstuff);
		devContext->Map(instancedBuffer2.Get(), 0, D3D11_MAP_WRITE_DISCARD, 0, &mapstuff2);

		float3* mapyay = (float3*)mapstuff.pData;
		unsigned int* mapyay2 = (unsigned int*)mapstuff2.pData;

		for (int i = 0; i < numIns; ++i)
		{
			mapyay[i] = m_instanced[i];
			mapyay2[i] = m_instancedcolor[i];
		}

		devContext->Unmap(instancedBuffer.Get(), 0);
		devContext->Unmap(instancedBuffer2.Get(), 0);

		unsigned int strides[] = { vertexStride, sizeof(float3), sizeof(unsigned int) };
		unsigned int offsets[] = { 0, 0, 0 };
		ID3D11Buffer* buf_ptrs[] = { vertexBuffer, instancedBuffer.Get(), instancedBuffer2.Get() };

		devContext->IASetVertexBuffers(0, 3, buf_ptrs, strides, offsets);
		//and finally... draw models
		devContext->DrawIndexedInstanced(numIndices, numIns, 0, 0, 0);
	}
	else if (indexBuffer)
	{
		devContext->IASetIndexBuffer(indexBuffer.Get(), DXGI_FORMAT_R32_UINT, 0);

		//and finally... draw model
		devContext->DrawIndexed(numIndices, 0, 0);
	}
	else
	{
		devContext->Draw(numVerts, 0);
	}
}

//getters//

std::vector<DirectX::XMFLOAT4X4> Renderer::GetBoneOffsets()
{
	if (blender)
	{
		return blender->GetBoneOffsets();
	}
	vector<DirectX::XMFLOAT4X4> b;
	return b;
}

std::vector<DirectX::XMFLOAT4X4> Renderer::GetBonesWorlds()
{
	if (blender)
	{
		return blender->GetBonesWorlds();
	}
	vector<DirectX::XMFLOAT4X4> b;
	return b;
}

Blender* Renderer::GetBlender()
{
	return blender;
}

//setters//
void Renderer::SetTeamColor(float4 c) 
{ 
	TeamColor = c;
	teamcolorBuffer = ResourceManager::GetSingleton()->CreateConstantBuffer(&TeamColor);
}

void Renderer::SetCatch(float c)
{
	TeamColor.w = c;
	ResourceManager::GetSingleton()->UpdateConstantBuffer(teamcolorBuffer.Get(),&TeamColor);
}

void Renderer::SetCatch(float4 c)
{
	TeamColor = c;
	ResourceManager::GetSingleton()->UpdateConstantBuffer(teamcolorBuffer.Get(), &TeamColor);
}


void Renderer::SetModel(XMMATRIX& model)
{
	XMFLOAT4X4 tempModel;
	XMStoreFloat4x4(&tempModel, model);
	mvpData.model = tempModel;
}

void Renderer::SetModel(XMFLOAT4X4& model)
{
	mvpData.model = model;
}

void Renderer::SetView(XMFLOAT4X4 view)
{
	mvpData.view = view;
}
void Renderer::SetProjection(XMFLOAT4X4 projection)
{
	mvpData.projection = projection;
}

void Renderer::SetCurAnimation(int animIndex)
{
	blender->SetCurAnimation(animIndex);
}

void Renderer::SetNextAnimation(std::string animName)
{
	blender->SetNextAnimation(animName);
}

void Renderer::SetNextAnimation(int animIndex)
{
	blender->SetNextAnimation(animIndex);
}

void Renderer::SetBlendInfo(BlendInfo info)
{
	blender->SetBlendInfo(info);
}

void Renderer::SetBoneOffsets(std::vector<DirectX::XMFLOAT4X4>& boneOffsets)
{
	this->boneOffsets = boneOffsets;
}