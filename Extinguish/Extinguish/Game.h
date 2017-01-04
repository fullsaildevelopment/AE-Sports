#pragma once
#include <vector>
#include "Scene.h"
#include "InputManager.h"
#include "HashString.h"

class Game
{
private:
	std::vector<Scene> scenes;
	HashString scenesNamesTable;

	InputManager input;

	//private helper functions
	void CreateScenes(DeviceResources const * devResources);
public:
	void Init(DeviceResources const * devResources);
	void Update(InputManager& inputManager);
	void Render();
	void Shutdown();
	void LoadScene(unsigned int index);
};