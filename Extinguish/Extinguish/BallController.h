#pragma once
#include "Component.h"
#include "CapsuleCollider.h"
#include "SphereCollider.h"
#include "vec3.h"
#include "XTime.h"

class Physics;
class GameObject;

class BallController : public Component
{
private:
	bool isHeld = false;
	bool isThrown = false;
	GameObject *me;
	GameObject *holder;
	XTime timer;
	Transform* transform;
	Physics* physics;
	void SetIsHeld(bool ans);
	std::vector<Transform*> nets;
	void SlightMagEff();

public:
	BallController(GameObject* obj);
	void Init();
	void LateInit();
	void Update(float dt) override;
	void OnTriggerEnter(Collider *obj) override;

	// actions
	void ThrowTo(GameObject *target);
	void DropBall(GameObject *holder);
	void Throw();

	// accessors
	bool GetIsHeld();
	bool GetIsThrown();
	GameObject* GetHolder();

	// mutators
	void SetHolder(GameObject *person);
};