#include "Renderer.h"

//constructor
Renderer::Renderer()
{
	//initialize everything to -1, which means it wasn't set
	//indexBuffer = -1;
	//vertexBuffer = -1;
	////vertexSlice = -1; 
	//numOfVertices = -1;
	//inputLayout = -1;
	//pixelShader = -1;
	//vertexShader = -1;
	//computeShader = -1;
}

//basic
void Renderer::Init(std::string mesh, std::string psName, std::string vsName, std::string csName, unsigned int curAnimationIndex, ResourceManager* resources, DeviceResources const * deviceResources)
{
	indexBuffer = resources->GetIndexBuffer(mesh);
	vertexBuffer = resources->GetVertexBuffer(mesh);
	inputLayout = resources->GetInputLayout(vsName);
	pixelShader = resources->GetPixelShader(psName);
	vertexShader = resources->GetVertexShader(vsName);
	computeShader = resources->GetComputeShader(csName);
	diffuseSRV = resources->GetShaderResourceView(mesh);
	vertexStride = resources->GetVertexStride(mesh);
	numVerts = resources->GetNumVertices(mesh);
	numIndices = resources->GetNumIndices(mesh);
	devResources = deviceResources;

	blender.SetAnimationSet(resources->GetAnimationSet(mesh));
	blender.SetCurAnimationIndex(curAnimationIndex);
}

void Renderer::Update(float dt)
{
	ID3D11DeviceContext* devContext = devResources->GetDeviceContext();

	////set shaders
	devContext->VSSetShader(vertexShader, NULL, NULL);
	devContext->PSSetShader(pixelShader, NULL, NULL);

	//set input layout
	devContext->IASetInputLayout(inputLayout);

	//update and set constant buffers
	ID3D11Buffer* mvpConstantBuffer = devResources->GetMVPConstantBuffer();
	devContext->UpdateSubresource(mvpConstantBuffer, NULL, NULL, &mvpData, NULL, NULL);
	devContext->VSSetConstantBuffers(0, 1, &mvpConstantBuffer);

	std::vector<DirectX::XMFLOAT4X4>* boneOffsets = &blender.GetBoneOffsets();
	if (!boneOffsets->empty())
	{
		ID3D11Buffer* boneOffsetConstantBuffer = devResources->GetBoneOffsetConstantBuffer();
		devContext->UpdateSubresource(boneOffsetConstantBuffer, NULL, NULL, boneOffsets, NULL, NULL);
		devContext->VSSetConstantBuffers(1, 1, &boneOffsetConstantBuffer);
	}

	//devContext->UpdateSubresource(lightMvpConstantBuffer.Get(), NULL, NULL, &lightMVPData, NULL, NULL);

	//devContext->VSSetConstantBuffers(2, 1, lightMvpConstantBuffer.GetAddressOf());

	//set vertex buffer
	UINT offset = 0;

	devContext->IASetVertexBuffers(0, 1, &vertexBuffer, &vertexStride, &offset);

	//set shader resource view
	devContext->PSSetShaderResources(0, 1, &diffuseSRV);
	//devContext->PSSetShaderResources(1, 1, normalSRV.GetAddressOf());
	//devContext->PSSetShaderResources(2, 1, specSRV.GetAddressOf());
	//devContext->PSSetShaderResources(3, 1, devResources->GetShadowMapSRVAddress());

	//Draw!
	if (indexBuffer)
	{
		devContext->IASetIndexBuffer(indexBuffer, DXGI_FORMAT_R32_UINT, 0);

		//and finally... draw model
		devContext->DrawIndexed(numIndices, 0, 0);
	}
	else
	{
		devContext->Draw(numVerts, 0);
	}
}

//getters//

std::vector<DirectX::XMFLOAT4X4>& Renderer::GetBoneOffsets()
{ 
	return blender.GetBoneOffsets();
}

std::vector<DirectX::XMFLOAT4X4>& Renderer::GetBonesWorlds()
{ 
	return blender.GetBonesWorlds();
}