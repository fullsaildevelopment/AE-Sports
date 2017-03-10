#pragma once
#include "Component.h"
#include "BoxCollider.h"

class BallController;

class Goal : public Component
{
	//cached members
	BallController* ballController;

	//const members
	const float MAX_SECONDS_FOR_ASSIST = 7.0f;

	void OnTriggerEnter(Collider* c) override;
	//team 1 = 0  team 2 = 1
	void Score(int team);
public:
	Goal(GameObject* g);
};