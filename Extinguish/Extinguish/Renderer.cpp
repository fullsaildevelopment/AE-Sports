#include "Renderer.h"

//constructor
Renderer::Renderer() : Component(nullptr)
{
	blender = nullptr;
}

Renderer::~Renderer()
{
	delete blender;
	blender = nullptr;
}

void Renderer::Init(bool _isButton, ResourceManager* resources, DeviceResources* deviceResources, ID3D11DepthStencilState * state)
{
	pDWriteFactory = resources->GetWriteFactory();
	pTextFormat = resources->GetTextFormat();
	pD2DFactory = resources->GetID2D1Factory();
	pRT = resources->GetRenderTarget();
	pBrush = resources->GetBrush();
	layoutRect = resources->GetRect();
	//pTextLayout = resources->GetTextLayout();
	devResources = deviceResources;
	isButton = _isButton;
	depthStencilState = state;
	d2DevContext = deviceResources->Get2DContext();


	// to test
	pDWriteFactory->CreateTextLayout(
		L"I test good!",
		(unsigned int)strlen("I test good!"),
		pTextFormat,
		240.0f,
		100.0f,
		&pTextLayout
	);

	ZeroMemory(&textMetrics, sizeof(DWRITE_TEXT_METRICS));
	pTextLayout->GetMetrics(&textMetrics);

	resources->GetID2D1Factory()->CreateDrawingStateBlock(stateBlock.GetAddressOf());
}

//basic
void Renderer::Init(std::string mesh, std::string psName, std::string vsName, std::string csName, std::string curAnimName, XMFLOAT4X4 projection, ResourceManager* resources, DeviceResources* deviceResources)
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
	m_instanced = nullptr;
	numIns = 0;
	SetProjection(projection);

	if (!curAnimName.empty())
	{
		blender = new Blender();
		blender->SetAnimationSet(resources->GetAnimationSet(mesh));
		blender->Init(true, curAnimName, "");
	}
}

void Renderer::Init(int numInstences, float3* instanced, std::string mesh, std::string psName, std::string vsName, std::string csName, std::string curAnimName, XMFLOAT4X4 projection, ResourceManager* resources, DeviceResources* deviceResources)
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
	m_instanced = instanced;
	numIns = numInstences;
	instancedBuffer = resources->CreateInstancedBuffer(numInstences, instanced);

	SetProjection(projection);

	if (!curAnimName.empty())
	{
		blender = new Blender();
		blender->SetAnimationSet(resources->GetAnimationSet(mesh));
		blender->Init(true, curAnimName, "");
	}
}

void Renderer::Update(float dt, InputManager* input)
{
	ID3D11DeviceContext* devContext = devResources->GetDeviceContext();
	if (!isButton)
	{
	//update blender
	if (blender)
	{
		blender->Update(dt * 0.5f, 0);
	}

	////set shaders
	devContext->VSSetShader(vertexShader, NULL, NULL);
	devContext->PSSetShader(pixelShader, NULL, NULL);

	//set input layout
	devContext->IASetInputLayout(inputLayout);

	//update and set constant buffers
	ID3D11Buffer* mvpConstantBuffer = devResources->GetMVPConstantBuffer();
	devContext->UpdateSubresource(mvpConstantBuffer, NULL, NULL, &mvpData, NULL, NULL);
	devContext->VSSetConstantBuffers(0, 1, &mvpConstantBuffer);

	if (blender)
	{
		std::vector<DirectX::XMFLOAT4X4> boneOffsets = blender->GetBoneOffsets();

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
		if (numIns > 0)
		{
			devContext->IASetIndexBuffer(indexBuffer, DXGI_FORMAT_R32_UINT, 0);

			D3D11_MAPPED_SUBRESOURCE mapstuff;

			devContext->Map(instancedBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mapstuff);

			float3* mapyay = (float3*)mapstuff.pData;
			for (int i = 0; i < numIns; ++i)
			{
				mapyay[i] = m_instanced[i];
			}

			devContext->Unmap(instancedBuffer, 0);

			unsigned int strides[] = { vertexStride, sizeof(float3) };
			unsigned int offsets[] = { 0, 0 };
			ID3D11Buffer* buf_ptrs[] = { vertexBuffer, instancedBuffer };

			devContext->IASetVertexBuffers(0, 2, buf_ptrs, strides, offsets);
			//and finally... draw models
			devContext->DrawIndexedInstanced(numIndices, numIns, 0, 0, 0);
		}
		else if (indexBuffer)
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
	else
	{
	//	HRESULT hr;
	//	pDWriteFactory->CreateTextLayout(
	//		L"I test good!",
	//		(unsigned int)strlen("I test good!"),
	//		pTextFormat,
	//		300.0f,
	//		50.0f,
	//		&pTextLayout
	//	);

	//	pTextLayout->GetMetrics(&textMetrics);
	//	
	//	devContext->OMSetDepthStencilState(depthStencilState, 1);
	//	d2DevContext->SaveDrawingState(stateBlock.Get());
	//	d2DevContext->BeginDraw();
	//	d2DevContext->SetTransform(D2D1::IdentityMatrix());
	//	d2DevContext->Clear(D2D1::ColorF(D2D1::ColorF::White));
	////pRT->Clear(D2D1::ColorF(D2D1::ColorF::White));
		GameObject * temp = GetGameObject();
		Button * theButton = (Button*)temp->GetComponent(0);

	//	DWRITE_TEXT_RANGE textRange = { 0, theButton->getLength() };
	//	hr = pTextLayout->SetTypography(theButton->getTypography(), textRange);

	//	d2DevContext->DrawTextA(
	//		theButton->getText(),
	//		theButton->getLength(),
	//		pTextFormat,
	//		layoutRect,
	//		pBrush
	//	);

	//	//pTextFormat->SetTextAlignment(DWRITE_TEXT_ALIGNMENT_TRAILING);

	//	//d2DevContext->DrawTextLayout(
	//	//	D2D1::Point2F(0.5f, 0.5f),
	//	//	pTextLayout,
	//	//	pBrush
	//	//	);

	//	hr = d2DevContext->EndDraw();
	//	if (hr != D2DERR_RECREATE_TARGET && hr != S_OK)
	//	{
	//		float t = 0;
	//	}
	//	d2DevContext->RestoreDrawingState(stateBlock.Get());

		ImGui::BeginMainMenuBar();
		//ImGui::RenderText(ImVec2(0, 0), "I am test");
		ImGui::Text(theButton->getCharText());

		if (theButton->iShowFPS())
		{
			ImGui::RenderText(ImVec2(300.0f, 2.0f), theButton->getFPS().c_str());
		}

		ImGui::EndMainMenuBar();

		
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

//setters
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

void Renderer::SetNextAnimation(std::string animName)
{
	blender->SetNextAnimation(animName);
}

void Renderer::SetBlendInfo(BlendInfo info)
{
	blender->SetBlendInfo(info);
}