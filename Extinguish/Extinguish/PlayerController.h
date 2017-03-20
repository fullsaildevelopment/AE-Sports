#pragma once
#include <string>
#include "Component.h"
//#include "vec3.h"

class GameObject;
class Camera;
class Transform;
class GamePadEvent;

class PlayerController : public Component
{
private:
	//temp cache
	InputManager* input;
	GameObject* otherPlayer; //is set when colliding with other player
	GameObject* floor;

	//cache
	Transform* transform;
	Camera* camera;
	Transform* cameraTransform;
	
	//reg members
	std::string name;
	unsigned int playerID = 0;
	float _dt;
	bool justJumped;
	bool isSprinting;
	//bool isCharging;
	float canSprint;
	float originalMaxSpeed;
	//float chargeTimer;
	bool footstepsPlayed;
	int footstepsSound;
	unsigned int teamID;

	//stat members for the player's whole history
	unsigned int totalGoals;
	unsigned int totalAssists;
	unsigned int totalSaves;
	unsigned int totalCatches;

	//stat members for current game
	unsigned int score;
	unsigned int goals;
	unsigned int saves;
	unsigned int assists;

	//const members
	const float sprintMultiplier = 1.75f;
	//const float chargeMultiplier = 2.25f;
	const float timeTilCharge = 2.0f;
	const float sprintCost = 0.15f;
	const float attackCost = 0.50f;

	const unsigned int goalPoints = 100;
	const unsigned int savePoints = 50;
	const unsigned int assistPoints = 50;

	//private helper functions
	void HandleInput();
	void HandleGamePad(GamePadEvent* gamePadEvent);
	void HandleSprintAndCharge();
	void PlayFootstepsSound();
	void StopFootstepsSound();
	void SetFootstepsSound(int sound);
public:
	PlayerController();
	~PlayerController();

	//basic
	void Init();
	void Update(float _dt) override;
	void Shutdown() override;
	void HandleEvent(Event* e) override;

	//misc
	void OnCollisionEnter(Collider* collider) override;
	void OnCollisionStay(Collider* collider) override;
	void OnCollisionExit(Collider* collider) override;
	void ResetPlayerGameStats();
	void AddGoal();
	void AddSave();
	void AddAssist();
	void AddCatch();
	void ReadInStats(std::string playerName);

	//actions
	void Jump();
	void Attack();
	void Sprint();

	//getters//
	std::string GetName();
	unsigned int GetTotalGoals();
	unsigned int GetTotalSaves();
	unsigned int GetTotalAssists();
	unsigned int GetScore();
	unsigned int GetGoals();
	unsigned int GetSaves();
	unsigned int GetAssists();
	unsigned int GetTeamID();

	//setters//
	void SetName(std::string name);
	void SetTotalGoals(unsigned int ttlGoals);
	void SetTotalSaves(unsigned int ttlSaves);
	void SetTotalAssists(unsigned int ttlAssists);
	void SetTeamID(unsigned int id);
	void SetScore(unsigned int amount);
	void SetGoals(unsigned int amount);
	void SetSaves(unsigned int amount);
	void SetAssists(unsigned int amount);
};