#pragma once
#include "Component.h"
#include <vector>

class GameObject;
class Scene;
class DeviceResources;
class PlayerController;

class Scoreboard : public Component
{
private:
	//reg members
	std::vector<GameObject*> playerBars;
	std::vector<GameObject*> playerNames;
	std::vector<GameObject*> playerScores;
	std::vector<GameObject*> playerGoals;
	std::vector<GameObject*> playerAssists;
	std::vector<GameObject*> playerSaves;
	std::vector<PlayerController*> players;
	GameObject** teamScores;
	GameObject** labels;
	GameObject* scoreboardBackground;
	Scene* scene;
	DeviceResources* devResources;
	bool alreadyCreated;
	int numRedPlayers, numBluePlayers;
	float originalYPosOfBack;

	//const members
	const int numOfTeams = 2;
	const int numOfLabels = 10;
	const int sceneID = 2;
	const float ySpacing = 0.06f;
	const float yPos = 0.23f;
	const float yPosLabel = 0.18f;

	// for resize
	float x, y;

	//private helper functions
	bool IsBeingUsed(int index);
	float Clamp(float num);
public:
	Scoreboard(Scene* scene, DeviceResources* devResources);
	~Scoreboard();

	void Init(int numRedPlayers, int numBluePlayers);
	void Update(float dt) override;
	void Toggle(bool toggle);
	void SendScoreboard();
	void ReceiveScoreboard();
	bool isActive();
	bool isInit() { return alreadyCreated; }
	void UpdateSize(D2D1_SIZE_F rect);
};