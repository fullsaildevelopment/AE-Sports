#include <iostream>
#include "Scene.h"
#include "../Bin/FBXLoader/FBXLoader.h"
#include "Camera.h"
#include "Game.h"

Scene::Scene()
{
	
}

Scene::~Scene()
{

}

//basic//

void Scene::Init(DeviceResources const * devResources, InputManager* inputRef)
{
	//set camera initial position
	//XMVECTORF32 eye = { 0.0f, 0.7f, -1.5f, 0.0f };
	//XMVECTORF32 at = { 0.0f, -0.1f, 0.0f, 0.0f };
	//XMVECTORF32 up = { 0.0f, 1.0f, 0.0f, 0.0f };

	//XMStoreFloat4x4(&camera, XMMatrixInverse(nullptr, XMMatrixLookAtLH(eye, at, up)));

	//create all the device resources
	CreateDevResources(devResources);

	//create all lights
	CreateLights();

	//create models in scene
	CreateModels();

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

	//create lighting buffers and set them

	//create camera constat buffer
	CD3D11_BUFFER_DESC cameraConstantBufferDesc(sizeof(CameraPositionConstantBuffer), D3D11_BIND_CONSTANT_BUFFER);
	device->CreateBuffer(&cameraConstantBufferDesc, nullptr, &cameraConstantBuffer);
}

void Scene::CreateLights()
{
	//create only directional light
	dirLight.Create({ 0.577f, 0.577f, -0.577f, 0 }, { 0.75f, 0.75f, 0.94f, 1.0f }, { 0.6f, 0.6f, 0.6f, 0.6f });

	//create point lights
	PointLight pointLight0;
	pointLight0.Create({ -2.0f, 0.5f, 2.0f, 0 }, { 1, 0, 0, 1.0f }, 8.0f);

	PointLight pointLight1;
	pointLight1.Create({ -7.f, 1.0f, -7.0f, 0 }, { 0, 1.0f, 0, 1.0f }, 10.0f);

	PointLight pointLight2;
	pointLight2.Create({ 1.0f, 1.5f, -2.0f, 0 }, { 1, 0.0f, 1.0f, 1.0f }, 10.0f);

	PointLight pointLight3;
	pointLight3.Create({ 5.0f, 0.5f, 2.0f, 0 }, { 1.0f, 1.0f, 1.0f, 1.0f }, 7.0f);

	pointLights.push_back(pointLight0);
	pointLights.push_back(pointLight1);
	pointLights.push_back(pointLight2);
	pointLights.push_back(pointLight3);

	//create spot lights
	//Light spotLight;

	//spotLight.CreateSpotlight({ 2, 3.0f, 2, 0 }, { 1, 0, 0, 0 }, 5.0f);

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
void Scene::Update(float dt)
{
	//handle input
	//this->input = input;
	//HandleInput();

	//update lights
	//if (pointLights.size())
	//{

	//	pointLights[0].DoRadiusEffect(5.0f, radiusChange[0]);
	//	pointLights[1].DoRadiusEffect(7.0f, radiusChange[1]);

	//	devContext->UpdateSubresource(pointLightConstantBuffer.Get(), NULL, NULL, pointLights.data(), NULL, NULL);

	//	devContext->PSSetConstantBuffers(1, 1, pointLightConstantBuffer.GetAddressOf());
	//}

	//update camera (private function)
	//UpdateCamera(dt, 5.0f, 0.75f);

	//update camera constant buffer
	//cameraBufferData.cameraposW = XMFLOAT4(camera._41, camera._42, camera._43, 1);
	//devContext->UpdateSubresource(cameraConstantBuffer.Get(), NULL, NULL, &cameraBufferData, NULL, NULL);
	//devContext->PSSetConstantBuffers(2, 1, cameraConstantBuffer.GetAddressOf());


	//update view on every object
	//XMFLOAT4X4 sceneCam;
	//XMStoreFloat4x4(&sceneCam, XMMatrixTranspose(XMMatrixInverse(nullptr, XMLoadFloat4x4(&camera))));

	//update camera first because we need an accurate camera
	//gameObjects[0]->Update(dt, input);

	//error case... just in case client id isn't initialized and client id is 0
	string cameraName = "Camera";
	int id = Game::GetClientID();

	if (id == 0)
	{
		id = 1;
	}

	cameraName += to_string(id);

	//update cam to get accurate view
	XMFLOAT4X4 cameraCam;
	GameObject* camObject = gameObjects[0]->FindGameObject(cameraName);
	camObject->Update(dt);

	XMStoreFloat4x4(&cameraCam, XMMatrixTranspose(XMLoadFloat4x4(&camObject->GetComponent<Camera>()->GetView())));;

	for (int i = 0; i < gameObjects.size(); ++i)
	{
		if (id == 1)
		{
			gameObjects[i]->Update(dt);
		}

		Renderer* renderer = gameObjects[i]->GetComponent<Renderer>();

		if (renderer)
		{
			if (id != 1)
			{
				renderer->Update(dt);
			}

			renderer->SetView(cameraCam);
			
			Transform* transform = gameObjects[i]->GetTransform();

			if (transform)
			{
				XMFLOAT4X4 world;
				XMStoreFloat4x4(&world, XMMatrixTranspose(XMLoadFloat4x4(&transform->GetWorld())));
				renderer->SetModel(world);
			}

			renderer->Render();
		}
	}
}

void Scene::HandleInput()
{
	//if (input->GetKeyDown('O'))
	//{
	//	++curFrame;
	//}

	//if (input->GetKeyDown('P'))
	//{
	//	--curFrame;
	//}

	//if (input->GetKeyDown('K')) // go to idle
	//{
	//	//BlendInfo info;
	//	//info.totalBlendTime = 2.0f;

	//	//gameObjects[1]->SetBlendInfo(info);
	//	//gameObjects[1]->SetNextAnimation(2);
	//	//gameObjects[1]->CreateNextAnimation(true);
	//}

	//if (input->GetKeyDown('J')) //go to run
	//{
	//	//BlendInfo info;
	//	//info.totalBlendTime = 2.0f;

	//	//gameObjects[1]->SetNextAnimation(3);
	//	//gameObjects[1]->SetBlendInfo(info);
	//	//gameObjects[1]->CreateNextAnimation(true);
	//}

}

void Scene::UpdateCamera(float dt, const float moveSpeed, const float rotateSpeed)
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
	//render all models
	//for (size_t i = 0; i < models.size(); ++i)
	//{
	//	//if (i == 0) //to not render the idle test box
	//	{
	//		models[i].Render();
	//	}
	//}
}

void Scene::Shutdown()
{
	for (int i = 0; i < gameObjects.size(); ++i)
	{
		delete gameObjects[i];
	}
}

void Scene::CreateGameObject()
{

}

//this also sets game object's scene pointer
void Scene::AddGameObject(GameObject* gameObject)
{
	gameObject->SetScene(this);
	gameObjects.push_back(gameObject);
}