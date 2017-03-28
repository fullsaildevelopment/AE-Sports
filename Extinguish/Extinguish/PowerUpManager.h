#pragma once
#include <vector>
#include "Component.h"

class Scene;
class PowerUp;
class DeviceResources;

//this class will create all of the powerups in the beginning
//it will figure out where to spawn the powerups and when

class PowerUpManager : public Component
{
private:
	//object members
	std::vector<PowerUp*> powerUps;

	//basic members
	float roundTimer;

	//const members
	const float timeTilSpawn = 5.0f;

public:
	//basic
	void Init(Scene* scene, XMFLOAT4X4& projection, DeviceResources* devResources);
	void Update(float _dt) override;

};