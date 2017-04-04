#pragma once
#include <vector>
#include <string>
#include <map>
#include "Component.h"
#include "HashString.h"

class Scene;
class PowerUp;
class DeviceResources;
class UIRenderer;
class Button;

//this class will create all of the powerups in the beginning
//it will figure out where to spawn the powerups and when

class PowerUpManager : public Component
{
private:
	//object members
	std::vector<PowerUp*> powerUps;

	//ui for powerups
	// 0 == superjump
	// 1 == shield
	// 2 == magnet
	std::vector<GameObject*> powerUpUIobjs;
	std::vector<Button*> powerUpButtons;
	std::vector<UIRenderer*> powerUpRenderers;

	//basic members
	float roundTimer[4];
	bool posUsed[4];
	bool isSpawned[6];

	//const members
	const float TIME_TIL_SPAWN = 10.0f;
	const int NUM_OF_UPS = 6;
	const int NUM_OF_POS = 4;

public:
	//basic
	void Init(Scene* scene, XMFLOAT4X4& projection, DeviceResources* devResources);
	void Update(float _dt) override;

	//misc
	void HandleEvent(Event* e) override;

	//ui
	void Render();
	void UpdateSize(D2D1_SIZE_F rect);
};