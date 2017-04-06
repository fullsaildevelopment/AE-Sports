#include <iostream>
#include "Scene.h"
#include "../Bin/FBXLoader/FBXLoader.h"
#include "Camera.h"
#include "Game.h"
#include "GameObject.h"
#include "UIRenderer.h"
#include "Credits.h"
#include "HashString.h"
#include "AnimatorController.h"
#include "TrailRender.h"
#include "PowerUpManager.h"

Scene::Scene()
{
	gameObjectsTable = new HashString();
}

Scene::~Scene()
{
	//	depthDisabledStencilState.ReleaseAndGetAddressOf();
	delete gameObjectsTable;
}

//basic//

void Scene::Init(DeviceResources * devResources, InputManager* inputRef)
{
	//set camera initial position
	//XMVECTORF32 eye = { 0.0f, 0.7f, -1.5f, 0.0f };
	//XMVECTORF32 at = { 0.0f, -0.1f, 0.0f, 0.0f };
	//XMVECTORF32 up = { 0.0f, 1.0f, 0.0f, 0.0f };

	//XMStoreFloat4x4(&camera, XMMatrixInverse(nullptr, XMMatrixLookAtLH(eye, at, up)));

	//create all the device resources
	CreateDevResources(devResources);
	PostProcessing.CreateDevice(devResources);
	PostProcessing.CreateResources();
	//create all lights
	CreateLights();

	//create models in scene
	CreateModels();

	D3D11_RASTERIZER_DESC rastDesc = {};
	rastDesc.FillMode = D3D11_FILL_SOLID;
	rastDesc.CullMode = D3D11_CULL_BACK;
	rastDesc.FrontCounterClockwise = false;
	rastDesc.DepthBias = 0;
	rastDesc.SlopeScaledDepthBias = 0.0f;
	rastDesc.DepthBiasClamp = 0.0f;
	rastDesc.DepthClipEnable = true;
	rastDesc.ScissorEnable = false;
	rastDesc.MultisampleEnable = false;
	rastDesc.AntialiasedLineEnable = false;

	devResources->GetDevice()->CreateRasterizerState(&rastDesc, RasterizerStateBackCull.GetAddressOf());

	rastDesc.CullMode = D3D11_CULL_FRONT;

	devResources->GetDevice()->CreateRasterizerState(&rastDesc, RasterizerStateFrontCull.GetAddressOf());

	rastDesc.CullMode = D3D11_CULL_NONE;

	devResources->GetDevice()->CreateRasterizerState(&rastDesc, RasterizerStateNoCull.GetAddressOf());

	//temporary
	curFrame = 0;

	input = inputRef;
}

void Scene::CreateDevResources(DeviceResources const * devResources)
{
	deviceResources = devResources;
	device = devResources->GetDevice();
	devContext = devResources->GetDeviceContext();

	//compile shaders
	//Microsoft::WRL::ComPtr<ID3D10Blob> basicVSBuffer;
	//Microsoft::WRL::ComPtr<ID3D10Blob> depthPrePassVSBuffer;
	//Microsoft::WRL::ComPtr<ID3D10Blob> bindVSBuffer;
	//Microsoft::WRL::ComPtr<ID3D10Blob> vsDepthPreShadowBuffer;
	//Microsoft::WRL::ComPtr<ID3D10Blob> vsShadowBuffer;
	//Microsoft::WRL::ComPtr<ID3D10Blob> basicPSBuffer;
	//Microsoft::WRL::ComPtr<ID3D10Blob> normalPSBuffer;
	//Microsoft::WRL::ComPtr<ID3D10Blob> psDepthPreShadowBuffer;
	//Microsoft::WRL::ComPtr<ID3D10Blob> psShadowBuffer;

	//Microsoft::WRL::ComPtr<ID3D10Blob> basicCSBuffer;


	//UINT flags = D3DCOMPILE_DEBUG;
	//HRESULT result;
	//result = D3DCompileFromFile(L"VS_Basic.hlsl", NULL, NULL, "main", "vs_4_0", flags, NULL, basicVSBuffer.GetAddressOf(), NULL);
	//result = D3DCompileFromFile(L"PS_Basic.hlsl", NULL, NULL, "main", "ps_4_0", flags, NULL, basicPSBuffer.GetAddressOf(), NULL);
	//result = D3DCompileFromFile(L"PS_NormalMapped.hlsl", NULL, NULL, "main", "ps_4_0", flags, NULL, normalPSBuffer.GetAddressOf(), NULL);
	//result = D3DCompileFromFile(L"VS_Bind.hlsl", NULL, NULL, "main", "vs_4_0", flags, NULL, bindVSBuffer.GetAddressOf(), NULL);
	//result = D3DCompileFromFile(L"VS_Basic.hlsl", NULL, NULL, "PreDepthPass", "vs_4_0", flags, NULL, depthPrePassVSBuffer.GetAddressOf(), NULL);
	//result = D3DCompileFromFile(L"VS_DepthPreShadow.hlsl", NULL, NULL, "main", "vs_4_0", flags, NULL, vsDepthPreShadowBuffer.GetAddressOf(), NULL);
	//result = D3DCompileFromFile(L"PS_DepthPreShadow.hlsl", NULL, NULL, "main", "ps_4_0", flags, NULL, psDepthPreShadowBuffer.GetAddressOf(), NULL);
	//result = D3DCompileFromFile(L"VS_Shadow.hlsl", NULL, NULL, "main", "vs_4_0", flags, NULL, vsShadowBuffer.GetAddressOf(), NULL);
	//result = D3DCompileFromFile(L"PS_Shadow.hlsl", NULL, NULL, "main", "ps_4_0", flags, NULL, psShadowBuffer.GetAddressOf(), NULL);

	//result = D3DCompileFromFile(L"CS_Basic.hlsl", NULL, NULL, "main", "cs_4_0", flags, NULL, basicCSBuffer.GetAddressOf(), NULL);

	////create shaders
	//Microsoft::WRL::ComPtr<ID3D11VertexShader> basicVS;
	//Microsoft::WRL::ComPtr<ID3D11VertexShader> bindVS;
	//Microsoft::WRL::ComPtr<ID3D11VertexShader> depthPrePassVS;
	//Microsoft::WRL::ComPtr<ID3D11VertexShader> depthPreShadowVS;
	//Microsoft::WRL::ComPtr<ID3D11VertexShader> shadowVS;
	//Microsoft::WRL::ComPtr<ID3D11PixelShader> basicPS;
	//Microsoft::WRL::ComPtr<ID3D11PixelShader> normalPS;
	//Microsoft::WRL::ComPtr<ID3D11PixelShader> depthPreShadowPS;
	//Microsoft::WRL::ComPtr<ID3D11PixelShader> shadowPS;
	//Microsoft::WRL::ComPtr<ID3D11ComputeShader> basicCS;

	//result = device->CreateVertexShader(basicVSBuffer->GetBufferPointer(), basicVSBuffer->GetBufferSize(), NULL, basicVS.GetAddressOf());
	//result = device->CreateVertexShader(bindVSBuffer->GetBufferPointer(), bindVSBuffer->GetBufferSize(), NULL, bindVS.GetAddressOf());
	//result = device->CreateVertexShader(depthPrePassVSBuffer->GetBufferPointer(), depthPrePassVSBuffer->GetBufferSize(), NULL, depthPrePassVS.GetAddressOf());
	//result = device->CreateVertexShader(vsDepthPreShadowBuffer->GetBufferPointer(), vsDepthPreShadowBuffer->GetBufferSize(), NULL, depthPreShadowVS.GetAddressOf());
	//result = device->CreateVertexShader(vsShadowBuffer->GetBufferPointer(), vsShadowBuffer->GetBufferSize(), NULL, shadowVS.GetAddressOf());

	//result = device->CreatePixelShader(basicPSBuffer->GetBufferPointer(), basicPSBuffer->GetBufferSize(), NULL, basicPS.GetAddressOf());
	//result = device->CreatePixelShader(normalPSBuffer->GetBufferPointer(), normalPSBuffer->GetBufferSize(), NULL, normalPS.GetAddressOf());
	//result = device->CreatePixelShader(psDepthPreShadowBuffer->GetBufferPointer(), psDepthPreShadowBuffer->GetBufferSize(), NULL, depthPreShadowPS.GetAddressOf());
	//result = device->CreatePixelShader(psShadowBuffer->GetBufferPointer(), psShadowBuffer->GetBufferSize(), NULL, shadowPS.GetAddressOf());
	//result = device->CreateComputeShader(basicCSBuffer->GetBufferPointer(), basicCSBuffer->GetBufferSize(), NULL, basicCS.GetAddressOf());

	//vertexShaders.push_back(basicVS);
	//vertexShaders.push_back(bindVS);
	//vertexShaders.push_back(depthPrePassVS);
	//vertexShaders.push_back(depthPreShadowVS);
	//vertexShaders.push_back(shadowVS);

	//pixelShaders.push_back(basicPS);
	//pixelShaders.push_back(normalPS);
	//pixelShaders.push_back(depthPreShadowPS);
	//pixelShaders.push_back(shadowPS);

	//set up input layouts
	//Microsoft::WRL::ComPtr<ID3D11InputLayout> basicInput;

	//D3D11_INPUT_ELEMENT_DESC basicInputElementDescs[] =
	//{
	//	{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
	//	{ "NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
	//	{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
	//};

	////HRESULT inputResult = device->CreateInputLayout(basicInputElementDescs, ARRAYSIZE(basicInputElementDescs), basicVSBuffer->GetBufferPointer(), basicVSBuffer->GetBufferSize(), basicInput.GetAddressOf());

	//Microsoft::WRL::ComPtr<ID3D11InputLayout> bindInput;

	//D3D11_INPUT_ELEMENT_DESC bindInputElementDescs[] =
	//{
	//	{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
	//	{ "NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
	//	{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
	//	{ "BINORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
	//	{ "TANGENT", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
	//	{ "BLENDINDICES", 0, DXGI_FORMAT_R32G32B32A32_SINT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
	//	{ "BLENDWEIGHT", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
	//};

	////HRESULT bindInputResult = device->CreateInputLayout(bindInputElementDescs, ARRAYSIZE(bindInputElementDescs), bindVSBuffer->GetBufferPointer(), bindVSBuffer->GetBufferSize(), bindInput.GetAddressOf());

	////inputLayouts.push_back(basicInput);
	////inputLayouts.push_back(bindInput);

	//set topology
	devContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	//create sampler state
	CD3D11_SAMPLER_DESC samplerDesc = {};

	samplerDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR; // do D3D11_FILTER_ANISOTROPIC for better quality
	samplerDesc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
	samplerDesc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
	samplerDesc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
	samplerDesc.MaxAnisotropy = 1; //16 for anisotropic
	samplerDesc.ComparisonFunc = D3D11_COMPARISON_ALWAYS;
	samplerDesc.MaxLOD = D3D11_FLOAT32_MAX;
	samplerDesc.MipLODBias = 0.0f;
	samplerDesc.MinLOD = 0;
	samplerDesc.BorderColor[0] = 0;
	samplerDesc.BorderColor[1] = 0;
	samplerDesc.BorderColor[2] = 0;
	samplerDesc.BorderColor[3] = 0;

	CD3D11_SAMPLER_DESC samplerClampDesc = {};

	samplerClampDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR; // do D3D11_FILTER_ANISOTROPIC for better quality
	samplerClampDesc.AddressU = D3D11_TEXTURE_ADDRESS_CLAMP;
	samplerClampDesc.AddressV = D3D11_TEXTURE_ADDRESS_CLAMP;
	samplerClampDesc.AddressW = D3D11_TEXTURE_ADDRESS_CLAMP;
	samplerClampDesc.MaxAnisotropy = 1; //16 for anisotropic
	samplerClampDesc.ComparisonFunc = D3D11_COMPARISON_ALWAYS;
	samplerClampDesc.MaxLOD = D3D11_FLOAT32_MAX;
	samplerClampDesc.MipLODBias = 0.0f;
	samplerClampDesc.MinLOD = 0;
	samplerClampDesc.BorderColor[0] = 0;
	samplerClampDesc.BorderColor[1] = 0;
	samplerClampDesc.BorderColor[2] = 0;
	samplerClampDesc.BorderColor[3] = 0;

	HRESULT wrapSampleResult = device->CreateSamplerState(&samplerDesc, &wrapSamplerState);
	HRESULT clampSampleResult = device->CreateSamplerState(&samplerClampDesc, &clampSamplerState);

	devContext->PSSetSamplers(0, 1, wrapSamplerState.GetAddressOf());
	devContext->PSSetSamplers(1, 1, clampSamplerState.GetAddressOf());

	// create depth stencil state
	CD3D11_DEPTH_STENCIL_DESC depthStencilDesc;
	// Initialize the description of the stencil state.
	ZeroMemory(&depthStencilDesc, sizeof(depthStencilDesc));

	// Set up the description of the stencil state.
	depthStencilDesc.DepthEnable = true;
	depthStencilDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
	depthStencilDesc.DepthFunc = D3D11_COMPARISON_LESS;

	depthStencilDesc.StencilEnable = true;
	depthStencilDesc.StencilReadMask = 0xFF;
	depthStencilDesc.StencilWriteMask = 0xFF;

	// Stencil operations if pixel is front-facing.
	depthStencilDesc.FrontFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
	depthStencilDesc.FrontFace.StencilDepthFailOp = D3D11_STENCIL_OP_INCR;
	depthStencilDesc.FrontFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
	depthStencilDesc.FrontFace.StencilFunc = D3D11_COMPARISON_ALWAYS;

	// Stencil operations if pixel is back-facing.
	depthStencilDesc.BackFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
	depthStencilDesc.BackFace.StencilDepthFailOp = D3D11_STENCIL_OP_DECR;
	depthStencilDesc.BackFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
	depthStencilDesc.BackFace.StencilFunc = D3D11_COMPARISON_ALWAYS;
	device->CreateDepthStencilState(&depthStencilDesc, depthStencilState.GetAddressOf());


	// Clear the second depth stencil state before setting the parameters.

	CD3D11_DEPTH_STENCIL_DESC depthDisabledStencilDesc;
	ZeroMemory(&depthDisabledStencilDesc, sizeof(depthDisabledStencilDesc));

	// Now create a second depth stencil state which turns off the Z buffer for 2D rendering.  The only difference is 
	// that DepthEnable is set to false, all other parameters are the same as the other depth stencil state.
	depthDisabledStencilDesc.DepthEnable = false;
	depthDisabledStencilDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
	depthDisabledStencilDesc.DepthFunc = D3D11_COMPARISON_LESS;
	depthDisabledStencilDesc.StencilEnable = true;
	depthDisabledStencilDesc.StencilReadMask = 0xFF;
	depthDisabledStencilDesc.StencilWriteMask = 0xFF;
	depthDisabledStencilDesc.FrontFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
	depthDisabledStencilDesc.FrontFace.StencilDepthFailOp = D3D11_STENCIL_OP_INCR;
	depthDisabledStencilDesc.FrontFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
	depthDisabledStencilDesc.FrontFace.StencilFunc = D3D11_COMPARISON_ALWAYS;
	depthDisabledStencilDesc.BackFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
	depthDisabledStencilDesc.BackFace.StencilDepthFailOp = D3D11_STENCIL_OP_DECR;
	depthDisabledStencilDesc.BackFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
	depthDisabledStencilDesc.BackFace.StencilFunc = D3D11_COMPARISON_ALWAYS;

	device->CreateDepthStencilState(&depthDisabledStencilDesc, depthDisabledStencilState.GetAddressOf());

	//create lighting buffers and set them

	//create camera constat buffer
	CD3D11_BUFFER_DESC cameraConstantBufferDesc(sizeof(CameraPositionConstantBuffer), D3D11_BIND_CONSTANT_BUFFER);
	device->CreateBuffer(&cameraConstantBufferDesc, nullptr, &cameraConstantBuffer);
}

void Scene::CreateLights()
{
	//create only directional light
	dirLight.Create({ /*0.577f, 0.577f, -0.577f*/0,0,0, 0 }, { 0.20f, 0.30f, 0.20f, 1.0f }, { 0.099f, 0.099f, 0.099f, 0.099f });

	//create point lights
	PointLight pointLight0;
	pointLight0.Create({ -9, -13, -50.5f, 0 }, { 1.0f, 0, 0, 1.0f }, 145.0f);

	PointLight pointLight1;
	pointLight1.Create({ -9, -13, 10.0f, 0 }, { 0.0f, 0.0f, 1.0f, 1.0f }, 145.0f);

	PointLight pointLight2;
	pointLight2.Create({ -7, 10, -80.5f, 0 }, { 0.2f, 0.2f, 0.2f, 1.0f }, 100.0f);

	PointLight pointLight3;
	pointLight3.Create({ -7, 10, 20.5f, 0 }, { 0.2f, 0.2f, 0.2f, 1.0f }, 100.0f);

	pointLights.push_back(pointLight0);
	pointLights.push_back(pointLight1);
	pointLights.push_back(pointLight2);
	pointLights.push_back(pointLight3);

	//create spot lights
	//Light spotLight;

	//spotLight.CreateSpotlight({ 2, 3.0f, 2, 0 }, { 1, 0, 0, 0 }, 5.0f);

	CD3D11_BUFFER_DESC BallConstantBufferDesc(sizeof(float) * 4, D3D11_BIND_CONSTANT_BUFFER);
	device->CreateBuffer(&BallConstantBufferDesc, nullptr, &BallConstantBuffer);

	//spotLights.push_back(spotLight);

	//create directional light constant buffer
	CD3D11_BUFFER_DESC dirLightConstantBufferDesc(sizeof(DirectionalLightConstantBuffer), D3D11_BIND_CONSTANT_BUFFER);
	device->CreateBuffer(&dirLightConstantBufferDesc, nullptr, &dirLightConstantBuffer);

	devContext->UpdateSubresource(dirLightConstantBuffer.Get(), NULL, NULL, &dirLight, NULL, NULL);

	devContext->PSSetConstantBuffers(0, 1, dirLightConstantBuffer.GetAddressOf());

	//create point light constant buffer
	if (pointLights.size())
	{
		CD3D11_BUFFER_DESC pointLightConstantBufferDesc(sizeof(PointLightConstantBuffer) * (UINT)pointLights.size(), D3D11_BIND_CONSTANT_BUFFER);
		device->CreateBuffer(&pointLightConstantBufferDesc, nullptr, &pointLightConstantBuffer);

		devContext->UpdateSubresource(pointLightConstantBuffer.Get(), NULL, NULL, pointLights.data(), NULL, NULL);

		devContext->PSSetConstantBuffers(1, 1, pointLightConstantBuffer.GetAddressOf());
	}
	//Create spot light constant buffer
	if (NUMOFSPOTLIGHTS)
	{
		CD3D11_BUFFER_DESC spotLightConstantBufferDesc(sizeof(SpotLightConstantBuffer) * (UINT)spotLights.size(), D3D11_BIND_CONSTANT_BUFFER);
		device->CreateBuffer(&spotLightConstantBufferDesc, nullptr, &spotLightConstantBuffer);

		devContext->UpdateSubresource(spotLightConstantBuffer.Get(), NULL, NULL, spotLights.data(), NULL, NULL);

		devContext->PSSetConstantBuffers(2, 1, spotLightConstantBuffer.GetAddressOf());
	}
}

void Scene::CreateModels()
{
#if 0
	////ground plane
	//Model groundPlane;

	//vector<VS_BasicInput> basicVertices =
	//{
	//	{ XMFLOAT3(-5.5f, 0, -5.5f), XMFLOAT3(0, 1.0f, 0), XMFLOAT2(0.0f, 1.0f) }, //left bottom
	//	{ XMFLOAT3(5.5f, 0, -5.5f), XMFLOAT3(0, 1.0f, 0), XMFLOAT2(1.0f, 1.0f) }, //right bottom
	//	{ XMFLOAT3(-5.5f,  0, 5.5f), XMFLOAT3(0, 1.0f, 0), XMFLOAT2(0.0f, 0.0f) }, //left top
	//	{ XMFLOAT3(5.5f,  0,  5.5f), XMFLOAT3(0, 1.0f, 0), XMFLOAT2(1.0f, 0.0f) } //right top
	//};

	////clockwise
	//vector<unsigned int> indices =
	//{
	//	2, 1, 0,
	//	2, 3, 1
	//};
	//groundPlane.SetBasicVerts(basicVertices);
	//groundPlane.SetIndices(indices);

	//groundPlane.Init(VertexShaderTypes::vsBASIC, vertexShaders[VertexShaderTypes::vsBASIC].Get(), vertexShaders[VertexShaderTypes::vsDEPTHPREPASS].Get(), pixelShaders[PixelShaderTypes::psBASIC].Get(), inputLayouts[VertexShaderTypes::vsBASIC].Get(), "../Assets/Textures/DDS/FloorTexture.dds", XMMatrixIdentity(), camera, projection, L"");
	//groundPlane.CreateDevResources(deviceResources);
	//models.push_back(groundPlane);

	////test model for fbx loading 
	//Model testModel;
	//XMFLOAT4X4 identity;
	//XMStoreFloat4x4(&identity, XMMatrixIdentity());
	//XMFLOAT4X4 identities[4] = { identity, identity, identity, identity };

	//testModel.Init(VertexShaderTypes::vsBIND, vertexShaders[VertexShaderTypes::vsBIND].Get(), pixelShaders[PixelShaderTypes::psNORMALMAPPED].Get(), inputLayouts[VertexShaderTypes::vsBIND].Get(), "../Assets/Textures/DDS/TestCube.dds", "", XMMatrixIdentity(), camera, projection, identities, L"Box");
	//testModel.CreateDevResources(deviceResources);
	//models.push_back(testModel);

	////add bear
	//XMFLOAT4X4 lightView;

	//XMFLOAT4 lightPosition = pointLights[0].GetLight().pointLightPosition;

	//static const XMVECTORF32 eye = { lightPosition.x , lightPosition.y, lightPosition.z, lightPosition.w };
	//static const XMVECTORF32 at = { 0.0f, 0.0f, 0.0f, 0.0f };
	//static const XMVECTORF32 up = { 0.0f, 1.0f, 0.0f, 0.0f };

	//XMStoreFloat4x4(&lightView, XMMatrixInverse(nullptr, XMMatrixLookAtLH(eye, at, up)));

	//Model monokuma;
	////monokuma.Init(VertexShaderTypes::vsBIND, vertexShaders[VertexShaderTypes::vsBIND].Get(), pixelShaders[PixelShaderTypes::psNORMALMAPPED].Get(), inputLayouts[VertexShaderTypes::vsBIND].Get(), "../Assets/Textures/DDS/Teddy.dds", "", XMMatrixTranspose(XMMatrixMultiply(XMMatrixScaling(0.01f, 0.01f, 0.01f), XMMatrixTranslation(-3, 0, 3))), camera, projection, identities, L"Teddy");
	//monokuma.Init(VertexShaderTypes::vsBIND, vertexShaders[VertexShaderTypes::vsSHADOW].Get(), pixelShaders[PixelShaderTypes::psSHADOW].Get(), inputLayouts[VertexShaderTypes::vsBIND].Get(), "../Assets/Textures/DDS/Teddy.dds", "", XMMatrixTranspose(XMMatrixMultiply(XMMatrixScaling(0.01f, 0.01f, 0.01f), XMMatrixTranslation(-3, 0, 3))), camera, projection, identities, L"Teddy");
	//monokuma.SetShadowData(vertexShaders[VertexShaderTypes::vsDEPTHSHADOW].Get(), pixelShaders[PixelShaderTypes::psDEPTHSHADOW].Get(), XMMatrixTranspose(XMMatrixMultiply(XMMatrixScaling(0.01f, 0.01f, 0.01f), XMMatrixTranslation(-3, 0, 3))), lightView, projection);
	//monokuma.CreateDevResources(deviceResources);

	//models.push_back(monokuma);

	////add test box but with attack animation
	//Model testModelAttack;
	//testModelAttack.Init(VertexShaderTypes::vsBIND, vertexShaders[VertexShaderTypes::vsBIND].Get(), pixelShaders[PixelShaderTypes::psNORMALMAPPED].Get(), inputLayouts[VertexShaderTypes::vsBIND].Get(), "../Assets/Textures/DDS/TestCube.dds", "", XMMatrixTranspose(XMMatrixTranslation(3, 0, 0)), camera, projection, identities, L"Box");
	//testModelAttack.CreateDevResources(deviceResources);
	//models.push_back(testModelAttack);


	////add magician
	//Model mage;

	//mage.Init(VertexShaderTypes::vsBIND, vertexShaders[VertexShaderTypes::vsBIND].Get(), pixelShaders[PixelShaderTypes::psNORMALMAPPED].Get(), inputLayouts[VertexShaderTypes::vsBIND].Get(), "../Assets/Textures/DDS/Mage.dds", "../Assets/Textures/DDS/Mage_NM.dds", XMMatrixTranspose(XMMatrixTranslation(-3, 0, -3)), camera, projection, identities, L"Mage");
	//mage.SetSpecMap("../Assets/Textures/DDS/Mage_Spec.dds");
	//mage.CreateDevResources(deviceResources);

	//models.push_back(mage);

	////add four spheres. set postions at position in boneMats
	//for (int i = 0; i < 4; ++i)
	//{
	//	Model sphereModel;

	//	sphereModel.Init(VertexShaderTypes::vsBASIC, vertexShaders[VertexShaderTypes::vsBASIC].Get(), vertexShaders[VertexShaderTypes::vsDEPTHPREPASS].Get(), pixelShaders[PixelShaderTypes::psBASIC].Get(), inputLayouts[VertexShaderTypes::vsBASIC].Get(), "", XMMatrixIdentity(), camera, projection, L"Sphere");
	//	sphereModel.CreateDevResources(deviceResources);
	//	models.push_back(sphereModel);
	//}
#endif
}

//void Scene::Update(InputManager input, float dt)
void Scene::Update(float _dt)
{

	//ID3D11DepthStencilState * state = deviceResources->GetStencilState();
	devContext->OMSetDepthStencilState(depthStencilState.Get(), 1);
	transparentObjects.clear();
	opaqueObjects.clear();
	SLLIter<RenderItem> transparentIter(transparentObjects);
	SLLIter<RenderItem> opaqueIter(opaqueObjects);

	TrailRender* ballTrail = nullptr;
	//error case... just in case client id isn't initialized and client id is 0
	string cameraName = "Camera";

	int id = Game::GetClientID();

	if (id == 0 || Game::currentScene < 2)
	{
		id = 1;
	}

	//get camera object
	cameraName += to_string(id);
	GameObject* camObject = gameObjects[0]->FindGameObject(cameraName);
	float3 camPosition = camObject->GetTransform()->GetWorldPosition();
	camPosition.y += 1.85f;
	//get view
	XMFLOAT4X4 cameraCam;
	XMStoreFloat4x4(&cameraCam, XMMatrixTranspose(XMLoadFloat4x4(&camObject->GetComponent<Camera>()->GetView())));;


	//Renderer* renderer = gameObjects[gameObjects.size() - 1]->GetComponent<Renderer>();
	//ID2D1HwndRenderTarget * pRT = renderer->GetPRT();

	if (!DEBUG_GRAPHICS)
		ImGui_ImplDX11_NewFrame();

	for (int i = 0; i < gameObjects.size(); ++i)
	{
		//only update game objects if this is the server
		if (ResourceManager::GetSingleton()->IsServer())// || gameObjects[i]->GetName() == "HexFloor")
		{
			gameObjects[i]->Update(_dt);
		}

		Renderer* renderer = gameObjects[i]->GetComponent<Renderer>();

		if (renderer)
		{
			//client needs to update renderer but not any other component
			if (!ResourceManager::GetSingleton()->IsServer())
			{
				//renderer->Update(_dt);
			}

			renderer->SetView(cameraCam);

			Transform* transform = gameObjects[i]->GetTransform();

			if (gameObjects[i]->GetName() == "Mage1")
			{
				//cout << transform->GetRotationDeg().x << " " << transform->GetRotationDeg().y << " " << transform->GetRotationDeg().z << endl;
			}

			if (transform)
			{
				XMFLOAT4X4 world;
				XMStoreFloat4x4(&world, XMMatrixTranspose(XMLoadFloat4x4(&transform->GetWorld())));
				renderer->SetModel(world);
			}


			//don't render yourself
			if (i != (id - 1) * 3 + 2)
			{
				if (renderer->isEnabled())
				{
					float3 inBetween = gameObjects[i]->GetTransform()->GetWorldPosition() - camPosition;
					float dist = dot_product(inBetween, inBetween);
					if (renderer->GetTransparent())
					{
						RenderItem r;
						r.dist = dist;
						r.rend = renderer;
						if (transparentObjects.size() == 0)
						{
							transparentObjects.addHead(r);
						}
						else
						{
							for (transparentIter.begin(); true; ++transparentIter)
							{
								if (transparentIter.end())
								{
									transparentObjects.insert(transparentIter, r);
									break;
								}
								if (r.dist > transparentIter.current().dist)
								{
									transparentObjects.insert(transparentIter, r);
									break;
								}
							}
						}


					}
					else
					{
						RenderItem r;
						r.dist = dist;
						r.rend = renderer;
						if (opaqueObjects.size() == 0)
						{
							opaqueObjects.addHead(r);
						}
						else
						{
							for (opaqueIter.begin(); true; ++opaqueIter)
							{
								if (opaqueIter.end())
								{
									opaqueObjects.insert(opaqueIter, r);
									break;
								}
								if (r.dist < opaqueIter.current().dist)
								{
									opaqueObjects.insert(opaqueIter, r);
									break;
								}
							}
						}
					}
				}
			}

			else
			{
				float3 tp = transform->GetPosition();
				XMFLOAT4 cps = { tp.x,tp.y + 1.85f,tp.z + 1 ,1 };

				devContext->UpdateSubresource(BallConstantBuffer.Get(), NULL, NULL, &cps, NULL, NULL);
				devContext->PSSetConstantBuffers(2, 1, BallConstantBuffer.GetAddressOf());

				//cout << transform->GetForward().x << " " << transform->GetForward().y << " " << transform->GetForward().z << " ";
				//cout << camObject->GetTransform()->GetForward().x << " " << camObject->GetTransform()->GetForward().y << " " << camObject->GetTransform()->GetForward().z << endl;
			}

			AnimatorController* animator = gameObjects[i]->GetComponent<AnimatorController>();

			//don't animate yourself or animate server which has already been animated
			if (animator && i != (id - 1) * 3 + 2 && id != 1)
			{
				animator->Update(_dt);
			}
		}
		TrailRender* trender = gameObjects[i]->GetComponent<TrailRender>();
		if (trender)
		{
			trender->SetView(cameraCam);
			ballTrail = trender;
			trender->RenderUpdate(_dt);
		}
	}
	devContext->PSSetConstantBuffers(0, 1, dirLightConstantBuffer.GetAddressOf());
	///////////////Clear BackBuffer//////////////
	PostProcessing.Clear();
	//////////////////////////RenderOpaqueObjects/////////////////////////
	for (opaqueIter.begin(); !opaqueIter.end(); ++opaqueIter)
	{
		opaqueIter.current().rend->Update(_dt);
		opaqueIter.current().rend->Render();
	}
	//////////////////////////////////////////////////////////////////////
	if (ballTrail)
	{
		devContext->RSSetState(RasterizerStateNoCull.Get());
		ballTrail->Render();
	}
	////////////////////////RenderTransparentObjects//////////////////////
	for (transparentIter.begin(); !transparentIter.end(); ++transparentIter)
	{
		transparentIter.current().rend->Update(_dt);
		devContext->RSSetState(RasterizerStateFrontCull.Get());
		transparentIter.current().rend->Render();
		devContext->RSSetState(RasterizerStateBackCull.Get());
		transparentIter.current().rend->Render();
	}
	////////////////////////////DO POST PROCESSING////////////////////////
	PostProcessing.DoPostProcess();
	//////////////////////////////////////////////////////////////////////
	// commented out until we have the scoring
	// if score needs to be updated
	//GameObject * scoreUpdate = uiObjects[0]->FindGameObject("gameScore");
	//if (scoreUpdate)
	//{
	//	// something about if score isn't what the ui has, then update
	//	Button * scoreButton = scoreUpdate->GetComponent<Button>();
	//	wstring score = to_wstring(redScore) + L" : " + to_wstring(blueScore);
	//}
	if (!DEBUG_GRAPHICS) 
	{
		UIRenderer* uirend = uiObjects[0]->GetComponent<UIRenderer>();
		if (uirend)
			uirend->getUIDevCon()->BeginDraw();
		for (unsigned int i = 0; i < uiObjects.size(); ++i)
		{
			if (uiObjects[i]->GetName() != "Credits")
			{
				uiObjects[i]->Update(_dt);
				uiObjects[i]->GetComponent<UIRenderer>()->Render();
			}
			else
			{
				uiObjects[i]->Update(_dt);
				uiObjects[i]->GetComponent<Credits>()->Render();
			}
		}

		if (Game::currentScene == 2)
		{
			GetGameObject("PowerUpManager")->GetComponent<PowerUpManager>()->Render();
		}

		if (uirend)
			uirend->getUIDevCon()->EndDraw();


		ImGui::EndFrame();
	}
}

void Scene::FixedUpdate(float _dt)
{
	int id = Game::GetClientID();

	if (id == 0 || Game::currentScene < 2)
	{
		id = 1;
	}

	for (int i = 0; i < gameObjects.size(); ++i)
	{
		//only update game objects if this is the server
		if (ResourceManager::GetSingleton()->IsServer())
		{
			gameObjects[i]->FixedUpdate(_dt);
		}

		AnimatorController* animator = gameObjects[i]->GetComponent<AnimatorController>();

		//don't animate yourself or animate server which has already been animated
		if (animator && i != (id - 1) * 3 + 2 && !ResourceManager::GetSingleton()->IsServer())
		{
			animator->FixedUpdate(_dt);
		}

		/*TrailRender* trender = gameObjects[i]->GetComponent<TrailRender>();
		if (trender && !ResourceManager::GetSingleton()->IsServer())
		{
			trender->FixedUpdate(_dt);
		}*/
	}
}

void Scene::HandleInput()
{
}

void Scene::UpdateCamera(float _dt, const float moveSpeed, const float rotateSpeed)
{
	//	if (input->GetKey('W'))
	//	{
	//		XMMATRIX translation = XMMatrixTranslation(0.0f, 0.0f, moveSpeed * dt);
	//		XMMATRIX tempCamera = XMLoadFloat4x4(&camera);
	//		XMMATRIX newCamera = XMMatrixMultiply(translation, tempCamera);
	//		XMStoreFloat4x4(&camera, newCamera);
	//	}
	//
	//	if (input->GetKey('S'))
	//	{
	//		XMMATRIX translation = XMMatrixTranslation(0.0f, 0.0f, -moveSpeed * dt);
	//		XMMATRIX tempCamera = XMLoadFloat4x4(&camera);
	//		XMMATRIX newCamera = XMMatrixMultiply(translation, tempCamera);
	//		XMStoreFloat4x4(&camera, newCamera);
	//	}
	//
	//	if (input->GetKey('A'))
	//	{
	//		XMMATRIX translation = XMMatrixTranslation(-moveSpeed * dt, 0.0f, 0.0f);
	//		XMMATRIX tempCamera = XMLoadFloat4x4(&camera);
	//		XMMATRIX newCamera = XMMatrixMultiply(translation, tempCamera);
	//		XMStoreFloat4x4(&camera, newCamera);
	//	}
	//
	//	if (input->GetKey('D'))
	//	{
	//		XMMATRIX translation = XMMatrixTranslation(moveSpeed * dt, 0.0f, 0.0f);
	//		XMMATRIX tempCamera = XMLoadFloat4x4(&camera);
	//		XMMATRIX newCamera = XMMatrixMultiply(translation, tempCamera);
	//		XMStoreFloat4x4(&camera, newCamera);
	//	}
	//
	//	if (input->GetKey('Q')) //up
	//	{
	//		XMMATRIX translation = XMMatrixTranslation(0.0f, moveSpeed * dt, 0.0f);
	//		XMMATRIX tempCamera = XMLoadFloat4x4(&camera);
	//		XMMATRIX newCamera = XMMatrixMultiply(translation, tempCamera);
	//		XMStoreFloat4x4(&camera, newCamera);
	//	}
	//
	//	if (input->GetKey('E')) //down
	//	{
	//		XMMATRIX translation = XMMatrixTranslation(0.0f, -moveSpeed * dt, 0.0f);
	//		XMMATRIX tempCamera = XMLoadFloat4x4(&camera);
	//		XMMATRIX newCamera = XMMatrixMultiply(translation, tempCamera);
	//		XMStoreFloat4x4(&camera, newCamera);
	//	}
	//
	//#if _DEBUG
	//	if (input->GetMouseX() && input->GetMouseY())
	//	{
	//		if (input->GetMouseButton(1) && prevMouseX && prevMouseY)
	//		{
	//			float dx = (float)input->GetMouseX() - (float)prevMouseX;
	//			float dy = (float)input->GetMouseY() - (float)prevMouseY;
	//
	//			//store old cam position
	//			XMFLOAT3 camPosition = XMFLOAT3(camera._41, camera._42, camera._43);
	//
	//			camera._41 = 0;
	//			camera._42 = 0;
	//			camera._43 = 0;
	//
	//			XMMATRIX rotX = XMMatrixRotationX(dy * rotateSpeed * dt);
	//			XMMATRIX rotY = XMMatrixRotationY(dx * rotateSpeed * dt);
	//
	//			//apply rotations to camera
	//			XMMATRIX tempCamera = XMLoadFloat4x4(&camera);
	//			tempCamera = XMMatrixMultiply(rotX, tempCamera);
	//			tempCamera = XMMatrixMultiply(tempCamera, rotY);
	//
	//			//store new camera
	//			XMStoreFloat4x4(&camera, tempCamera);
	//
	//			//change position to where it was earlier
	//			camera._41 = camPosition.x;
	//			camera._42 = camPosition.y;
	//			camera._43 = camPosition.z;
	//		}
	//
	//		prevMouseX = input->GetMouseX();
	//		prevMouseY = input->GetMouseY();
	//	}
	//#else
	//	if (input->GetMouseX() && input->GetMouseY())
	//	{
	//		if (prevMouseX && prevMouseY)
	//		{
	//			float dx = (float)input->GetMouseX() - (float)prevMouseX;
	//			float dy = (float)input->GetMouseY() - (float)prevMouseY;
	//
	//			//store old cam position
	//			XMFLOAT3 camPosition = XMFLOAT3(camera._41, camera._42, camera._43);
	//
	//			camera._41 = 0;
	//			camera._42 = 0;
	//			camera._43 = 0;
	//
	//			XMMATRIX rotX = XMMatrixRotationX(dy * rotateSpeed * dt);
	//			XMMATRIX rotY = XMMatrixRotationY(dx * rotateSpeed * dt);
	//
	//			//apply rotations to camera
	//			XMMATRIX tempCamera = XMLoadFloat4x4(&camera);
	//			tempCamera = XMMatrixMultiply(rotX, tempCamera);
	//			tempCamera = XMMatrixMultiply(tempCamera, rotY);
	//
	//			//store new camera
	//			XMStoreFloat4x4(&camera, tempCamera);
	//
	//			//change position to where it was earlier
	//			camera._41 = camPosition.x;
	//			camera._42 = camPosition.y;
	//			camera._43 = camPosition.z;
	//		}
	//
	//		prevMouseX = input->GetMouseX();
	//		prevMouseY = input->GetMouseY();
	//	}
	//#endif
}

void Scene::Render()
{

}

void Scene::Shutdown()
{
	for (int i = 0; i < gameObjects.size(); ++i)
	{
		gameObjects[i]->Shutdown();
		delete gameObjects[i];
	}
	gameObjects.clear();
	for (int i = 0; i < uiObjects.size(); ++i)
	{
		delete uiObjects[i];
	}
	uiObjects.clear();
}

//misc//s
void Scene::CreateGameObject()
{

}

//this also sets game object's scene pointer
void Scene::AddGameObject(GameObject* gameObject)
{
	gameObject->SetScene(this);
	gameObjects.push_back(gameObject);
	gameObjectsTable->Insert(gameObject->GetName());
}

void Scene::AddUIObject(GameObject* gameObject)
{
	gameObject->SetScene(this);
	uiObjects.push_back(gameObject);
}

//getters//
GameObject* Scene::GetGameObject(std::string name)
{
	GameObject* result = nullptr;
	int index = gameObjectsTable->GetKey(name);

	if (index != -1)
	{
		result = gameObjects[index];
	}

	return result;
}

GameObject* const Scene::GetUIByName(string name)
{
	for (unsigned int i = 0; i < uiObjects.size(); ++i)
	{
		if (uiObjects[i]->GetName() == name)
		{
			return uiObjects[i];
		}
	}

	return nullptr;
}

void Scene::ResizeWindow(uint16_t w, uint16_t h)
{
	PostProcessing.ResizeWindow(w, h);
}