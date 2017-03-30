#pragma once
#include <vector>
#include <string>
#include <map>
#include "Component.h"
#include "HashString.h"

class Scene;
class PowerUp;
class DeviceResources;

//this class will create all of the powerups in the beginning
//it will figure out where to spawn the powerups and when

class PowerUpManager : public Component
{
private:
	//struct PowerUpInfo
	//{
	//	std::string name;
	//	int spawnPosIndex;
	//	bool isSpawned;
	//	bool triedToSpawn;
	//};

	//object members
	std::vector<PowerUp*> powerUps;
	//PowerUpInfo powerUpInfo[3];

	HashString powerUpsTable;

	//basic members
	float roundTimer;
	bool posUsed[4];
	bool isSpawned[3];
	std::string posUsedNames[4];

	//const members
	const float TIME_TIL_SPAWN = 5.0f;
	const int NUM_OF_UPS = 3;
	const int NUM_OF_POS = 4;

public:
	//basic
	void Init(Scene* scene, XMFLOAT4X4& projection, DeviceResources* devResources);
	void Update(float _dt) override;

	//misc
	void HandleEvent(Event* e) override;
};