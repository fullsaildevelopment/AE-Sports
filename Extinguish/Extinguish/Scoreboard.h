#pragma once
#include "Component.h"
#include <vector>

class GameObject;
class Scene;
class DeviceResources;

class Scoreboard : public Component
{
private:
	std::vector<GameObject*> redPlayerBars;
	std::vector<GameObject*> bluePlayerBars;
	GameObject** teamScores;
	GameObject** labels;
	Scene* scene;
	DeviceResources* devResources;

	//const members
	const int numOfTeams = 2;
	const int numOfLabels = 10;
	const int sceneID = 2;

public:
	Scoreboard(Scene* scene, DeviceResources* devResources);
	~Scoreboard();

	void Init(int numRedPlayers, int numBluePlayers);
	void Update(float dt) override;
};