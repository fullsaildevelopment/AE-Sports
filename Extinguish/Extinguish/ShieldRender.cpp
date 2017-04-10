#include "ShieldRender.h"

void ShieldRender::Init(std::string mesh, XMFLOAT4X4 projection, DeviceResources* deviceResources, float rate)
{
	indexBuffer = ResourceManager::GetSingleton()->GetIndexBuffer(mesh);
	vertexBuffer = ResourceManager::GetSingleton()->GetVertexBuffer(mesh);
	inputLayout = ResourceManager::GetSingleton()->GetInputLayout("ShieldBubble");
	pixelShader = ResourceManager::GetSingleton()->GetPixelShader("ShieldBubble");
	vertexShader = ResourceManager::GetSingleton()->GetVertexShader("ShieldBubble");
	diffuseSRV = ResourceManager::GetSingleton()->GetShaderResourceView(mesh);

	Noisemap1SRV = ResourceManager::GetSingleton()->GetShaderResourceView("NM_" + mesh);
	Noisemap2SRV = ResourceManager::GetSingleton()->GetShaderResourceView("Spec_" + mesh);

	vertexStride = ResourceManager::GetSingleton()->GetVertexStride(mesh);
	numVerts = ResourceManager::GetSingleton()->GetNumVertices(mesh);
	numIndices = ResourceManager::GetSingleton()->GetNumIndices(mesh);
	devResources = deviceResources;
	SetProjection(projection);

	D3D11_BLEND_DESC BlendStateDesc;
	ZeroMemory(&BlendStateDesc, sizeof(D3D11_BLEND_DESC));
	BlendStateDesc.RenderTarget[0].BlendEnable = TRUE;
	BlendStateDesc.RenderTarget[0].SrcBlend = D3D11_BLEND_SRC_ALPHA;
	BlendStateDesc.RenderTarget[0].DestBlend = D3D11_BLEND_INV_SRC_ALPHA;
	BlendStateDesc.RenderTarget[0].BlendOp = D3D11_BLEND_OP_ADD;
	BlendStateDesc.RenderTarget[0].SrcBlendAlpha = D3D11_BLEND_ONE;
	BlendStateDesc.RenderTarget[0].DestBlendAlpha = D3D11_BLEND_ZERO;
	BlendStateDesc.RenderTarget[0].BlendOpAlpha = D3D11_BLEND_OP_ADD;
	BlendStateDesc.RenderTarget[0].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;
	BlendStateDesc.AlphaToCoverageEnable = FALSE;
	deviceResources->GetDevice()->CreateBlendState(&BlendStateDesc, m_blendstate.GetAddressOf());

	D3D11_SAMPLER_DESC samplerDesc;
	ZeroMemory(&samplerDesc, sizeof(D3D11_SAMPLER_DESC));

	samplerDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
	samplerDesc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
	samplerDesc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
	samplerDesc.AddressW = D3D11_TEXTURE_ADDRESS_CLAMP;
	samplerDesc.MipLODBias = 1.0f;
	samplerDesc.MaxAnisotropy = 1;
	samplerDesc.ComparisonFunc = D3D11_COMPARISON_NEVER;
	samplerDesc.BorderColor[0] = 1.0f;
	samplerDesc.BorderColor[1] = 1.0f;
	samplerDesc.BorderColor[2] = 1.0f;
	samplerDesc.BorderColor[3] = 1.0f;
	samplerDesc.MinLOD = -FLT_MAX;
	samplerDesc.MaxLOD = FLT_MAX;

	deviceResources->GetDevice()->CreateSamplerState(&samplerDesc, m_samplerstate.GetAddressOf());

	scrollRate = rate;
}

void ShieldRender::SetProjection(XMFLOAT4X4 projection)
{
	mvpData.projection = projection;
}

void ShieldRender::Render()
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

	devContext->UpdateSubresource(noiseOffsetBuffer, NULL, NULL, &noiseOffsets, NULL, NULL);
	devContext->PSSetConstantBuffers(0, 1, &noiseOffsetBuffer);

	devContext->UpdateSubresource(shieldColorBuffer, NULL, NULL, &shieldColor, NULL, NULL);
	devContext->PSSetConstantBuffers(1, 1, &shieldColorBuffer);

	//set vertex buffer
	UINT offset = 0;

	devContext->IASetVertexBuffers(0, 1, &vertexBuffer, &vertexStride, &offset);

	//set shader resource view
	devContext->PSSetShaderResources(0, 1, &diffuseSRV);
	devContext->PSSetShaderResources(1, 1, &Noisemap1SRV);
	devContext->PSSetShaderResources(2, 1, &Noisemap2SRV);
	devContext->OMSetBlendState(m_blendstate.Get(), blendFactor, sampleMask);
	devContext->PSSetSamplers(0, 1, m_samplerstate.GetAddressOf());

	devContext->IASetIndexBuffer(indexBuffer, DXGI_FORMAT_R32_UINT, 0);

	//and finally... draw model
	devContext->DrawIndexed(numIndices, 0, 0);

}

void ShieldRender::SetModel(XMFLOAT4X4& model)
{
	mvpData.model = model;
}

void ShieldRender::SetView(XMFLOAT4X4 view)
{
	mvpData.view = view;
}

void ShieldRender::SetShieldColor(float4 c)
{
	shieldColor = c;
	if (shieldColorBuffer) shieldColorBuffer->Release();
	shieldColorBuffer = ResourceManager::GetSingleton()->CreateConstantBuffer(&shieldColor);
}

void ShieldRender::UpdateOffsets(float dt)
{
	noiseOffsets[0] += dt * scrollRate;
	noiseOffsets[1] += dt * scrollRate;
	if (noiseOffsetBuffer) noiseOffsetBuffer->Release();
	noiseOffsetBuffer = ResourceManager::GetSingleton()->CreateConstantBuffer(&noiseOffsets, 2, sizeof(float));
}