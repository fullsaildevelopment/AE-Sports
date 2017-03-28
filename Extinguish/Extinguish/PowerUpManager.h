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
	std::vector<PowerUp*> powerUps;
public:
	//basic
	void Init(Scene* scene, XMFLOAT4X4& projection, DeviceResources* devResources);
	void Update(float _dt) override;

};