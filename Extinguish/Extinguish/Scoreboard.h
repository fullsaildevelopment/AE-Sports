#pragma once
#include "Component.h"
#include <vector>

class GameObject;
class Scene;
class DeviceResources;

class Scoreboard : public Component
{
private:
	//reg members
	//std::vector<GameObject*> redPlayerBars;
	//std::vector<GameObject*> bluePlayerBars;
	std::vector<GameObject*> playerBars;
	std::vector<GameObject*> playerNames;
	std::vector<GameObject*> playerScores;
	std::vector<GameObject*> playerGoals;
	std::vector<GameObject*> playerAssists;
	std::vector<GameObject*> playerSaves;
	GameObject** teamScores;
	GameObject** labels;
	GameObject* scoreboardBackground;
	Scene* scene;
	DeviceResources* devResources;

	//const members
	const int numOfTeams = 2;
	const int numOfLabels = 10;
	const int sceneID = 2;

	//private helper functions
	void CreateLabels(float position);
public:
	Scoreboard(Scene* scene, DeviceResources* devResources);
	~Scoreboard();

	void Init(int numRedPlayers, int numBluePlayers);
	void Update(float dt) override;
	void Toggle(bool toggle);
};