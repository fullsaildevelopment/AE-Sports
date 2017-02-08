#pragma once
#include "Component.h"
#include "BoxCollider.h"

class Goal : public Component
{
	void OnTriggerEnter(Collider* c) override;
	//team 1 = 0  team 2 = 1
	void Score(int team);
public:
	Goal(GameObject* g);
};