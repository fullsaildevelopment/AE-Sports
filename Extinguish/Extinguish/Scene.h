#pragma once
#include "../ShaderStructures/ShaderStructures.h"
#include "Model.h"
#include "DirectionalLight.h"
#include "PointLight.h"
#include "SpotLight.h"
#include "DeviceResources.h"
#include "ResourceManager.h"
#include "Renderer.h"
#include "InputManager.h"
#include "Transform.h"
#include "PlayerController.h"

#define NUMOFPOINTLIGHTS 1
#define NUMOFSPOTLIGHTS 0
// in regards to shaders and what not, if we want multiple ones, we can have an enum and a member to choose which num from enum we want

class GameObject;
class HashString;

class Scene
{
private:
	//buffer data
	//DirectX::XMFLOAT4X4 camera;
	//DirectX::XMFLOAT4X4 projection;

	//camera movement data
	//float camYaw;
	//float camPitch;

	//input data
	InputManager* input;
	//int prevMouseX, prevMouseY;

	//time_t previousTime; //deltaTime

	vector<Model> models;
	DirectionalLight dirLight;
	vector<PointLight> pointLights;
	vector<SpotLight> spotLights;

	//ResourceManager* resourceManager;
	//vector<AnimatedRenderNode*> renderNodes;

	vector<GameObject*> gameObjects;
	HashString* gameObjectsTable;
	vector<GameObject*> uiObjects;
	unsigned int curFrame;

	float radiusChange[2] = { 1.0f / 60.0f, 1.0f / 60.0f };

	//Device Resources
	const DeviceResources* deviceResources;
	ID3D11Device* device;
	ID3D11DeviceContext* devContext;
	//vector<Microsoft::WRL::ComPtr<ID3D11PixelShader>> pixelShaders;
	//vector<Microsoft::WRL::ComPtr<ID3D11VertexShader>> vertexShaders;
	//vector<Microsoft::WRL::ComPtr<ID3D11ComputeShader>> computerShaders;
	//vector<Microsoft::WRL::ComPtr<ID3D11InputLayout>> inputLayouts;
	Microsoft::WRL::ComPtr<ID3D11SamplerState> wrapSamplerState;
	Microsoft::WRL::ComPtr<ID3D11SamplerState> clampSamplerState;
	Microsoft::WRL::ComPtr<ID3D11Buffer> dirLightConstantBuffer;
	Microsoft::WRL::ComPtr<ID3D11Buffer> pointLightConstantBuffer;
	Microsoft::WRL::ComPtr<ID3D11Buffer> spotLightConstantBuffer;

	Microsoft::WRL::ComPtr<ID3D11Buffer> BallConstantBuffer;

	Microsoft::WRL::ComPtr<ID3D11DepthStencilState> depthStencilState;
	Microsoft::WRL::ComPtr<ID3D11DepthStencilState> depthDisabledStencilState;
	Microsoft::WRL::ComPtr<ID2D1DrawingStateBlock> stateBlock;

	ID2D1HwndRenderTarget * pRT;

	CameraPositionConstantBuffer cameraBufferData;
	Microsoft::WRL::ComPtr<ID3D11Buffer> cameraConstantBuffer;

	//private helper functions
	void CreateDevResources(DeviceResources const * devResources);
	void CreateLights();
	void UpdateCamera(float dt, const float moveSpeed, const float rotateSpeed);
	void CreateModels();
	void HandleInput();

public:
	Scene();
	~Scene();

	//basics
	void Init(DeviceResources const * devResources, InputManager* inputRef);
	void Update(float dt);
	void Render();
	void Shutdown();

	//misc
	void CreateGameObject();
	void AddGameObject(GameObject* gameObject);
	void AddUIObject(GameObject* gameObject);

	//getters//
	vector<GameObject*>* const GetGameObjects() { return &gameObjects; };
	GameObject* const GetGameObjects(int i) { return gameObjects[i]; };
	int GetNumObjects() { return (int)gameObjects.size(); };
	GameObject* GetGameObject(std::string name);
	GameObject* const GetUIByName(string name);

	//setters//
	void set2DRenderTarget(ID2D1HwndRenderTarget * renderTarget) { pRT = renderTarget; }


	//setters
	//void SetButtons(bool butts[256]) { memcpy(buttons, butts, sizeof(buttons)); }
	//void SetLeftClick(bool clicked) { leftClick = clicked; }
	//void SetRightClick(bool clicked) { rightClick = clicked; }
	//void SetMousePosition(int x, int y) { mouseX = x; mouseY = y; }
};