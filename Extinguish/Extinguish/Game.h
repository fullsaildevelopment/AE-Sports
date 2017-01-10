#pragma once
#include <vector>
#include "Scene.h"
#include "InputManager.h"
#include "HashString.h"
#include "Transform.h"
#include "Crosse.h"
#include "PlayerController.h"

class Game
{
private:
	std::vector<Scene*> scenes;
	unsigned int currentScene;
	HashString scenesNamesTable;

	//InputManager* input;
	ResourceManager resourceManager;

	//private helper functions
	void CreateScenes(DeviceResources* devResources, InputManager* inputManager);
public:
	void Init(DeviceResources* devResources, InputManager* inputManager);
	void Update(float dt);
	void Render();
	void Shutdown();
	void LoadScene(unsigned int index);
};