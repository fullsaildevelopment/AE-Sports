#pragma once
#include "Includes.h"
#include "DeviceResources.h"
#include "Component.h"
#include "GameObject.h"
#include <vector>

class Button;
class UIRenderer;

class Credits : public Component
{
	std::vector<GameObject*> theObjs;
	std::vector<Button*> theText;
	// 0-5 for names
	// 6-10 for labels
	std::vector<UIRenderer*> textRenderers;
	std::vector<wstring> names;
	std::vector<wstring> labels;

	bool fadeIn = true;
	float fadeInOut = 0.0f;
	float waitForIt = 0.0f;
	unsigned int nextName = 0;

	void SetNames();
	void NextSet();
	void ReturnToMenu();

public:
	Credits();
	~Credits();
	void Init(DeviceResources * devResources);
	void Update(float _dt);
	void Shutdown();
	void HandleEvent(Event* e);
	void Render();

};

