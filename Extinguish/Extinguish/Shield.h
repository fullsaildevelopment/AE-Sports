#pragma once
#include "PowerUp.h"

class Scene;
class GameObject;
class SphereCollider;
class DeviceResources;
class Collider;

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
	Shield(Scene* scene, XMFLOAT4X4 projection, DeviceResources* devResources);

	//basic
	void Activate() override;
	void Deactivate() override;
	void Enable() override;
	void Disable() override;

	//collision
	void OnCollisionEnter(Collider* collider) override;

	//getters
	std::string GetName() override;
};