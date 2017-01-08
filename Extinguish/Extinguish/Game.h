#pragma once
#include <vector>
#include "Scene.h"
#include "InputManager.h"
#include "HashString.h"
#include "Transform.h"

class Game
{
private:
	std::vector<Scene> scenes;
	unsigned int currentScene;
	HashString scenesNamesTable;

	InputManager input;
	ResourceManager resourceManager;

	//private helper functions
	void CreateScenes(DeviceResources* devResources);
public:
	void Init(DeviceResources* devResources);
	void Update(InputManager& inputManager, float dt);
	void Render();
	void Shutdown();
	void LoadScene(unsigned int index);
};