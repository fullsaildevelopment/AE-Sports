#pragma once
#include "Component.h"

class Event;
class Collider;
class PlayerController;

class PowerUp : public Component
{
private:
	float duration; // how long the powerup lasts for
	float timer;
	bool isActivated;
	int spawnIndex, posIndex; //the manager needs this to help spawn
	int id; // for client-side only

	PlayerController* player;
public:
	//structors
	PowerUp();

	//basic
	void Update(float _dt) override;
	void Shutdown() override;
	void HandleEvent(Event* e) override;
	void OnTriggerEnter(Collider* collider) override;
	virtual std::string GetName() = 0;

	//misc
	virtual void Activate();
	virtual void Deactivate();

	//getters
	float GetDuration();
	bool IsDone();
	PlayerController* GetPlayer();
	int GetSpawnIndex();
	int GetPosIndex();
	float GetElapsed() { return (duration - timer) / duration; }
	int GetID() { return id; }

	//setters
	void SetDuration(float dur);
	void SetSpawnIndex(int index);
	void SetPosIndex(int index);
	void SetID(int _id) { id = _id; }
	void ResetTimer() { timer = 0.0f; }
	void SetActive(bool active) { isActivated = active; }
};