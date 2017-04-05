#pragma once
#include "PowerUp.h"

class Scene;
class GameObject;
class SphereCollider;

class Shield : public PowerUp
{
private:
	//basic members
	std::string name = "Shield";

	//object members
	GameObject* shieldBubble;
	//GameObject* shieldCollider;
	SphereCollider* collider;

public:
	//structors
	Shield(Scene* scene);

	//basic
	void Activate() override;
	void Deactivate() override;
	void Enable() override;
	void Disable() override;

	//getters
	std::string GetName() override;
};