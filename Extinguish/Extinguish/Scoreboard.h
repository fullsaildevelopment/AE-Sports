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
	bool alreadyCreated;

	//const members
	const int numOfTeams = 2;
	const int numOfLabels = 10;
	const int sceneID = 2;
	const float ySpacing = 0.06f;
	const float yPos = 0.40f;
	const float yPosLabel = 0.35f;
	//const float yPos = 0.23f;
	//const float yPosLabel = 0.18f;

	//private helper functions
	void CreateLabels(float position);
public:
	Scoreboard(Scene* scene, DeviceResources* devResources);
	~Scoreboard();

	void Init(int numRedPlayers, int numBluePlayers);
	void Update(float dt) override;
};