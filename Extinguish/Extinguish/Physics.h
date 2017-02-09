#pragma once
#include "Component.h"
#include "vec3.h"

class GameObject;
class Transform;

class Physics : public Component
{
private:
	//const
	const float gravity = -9.8f;
	//const float friction 
	bool applyGravity = true;
	//cache
	Transform* transform;

	// 0-1 1 being complete reflection 0 no reflection
	float bounce = 1.5f;
	// 0-1  1 being all the friction 0 being no friction
	float friction = 0.06f;
	// 0-1 same as others
	float airdrag = 0.03f;

	float maxMoveSpeed = 30;

	bool colliding = false;
	//other
	//TODO: incorporate mass. float mass;
	bool isKinematic;
public:
	Physics() {};
	//original values Bounce = 1.06 friction = 0.23 airdrag = 0.08
	Physics(float _bounce, float _friction = 0.23f, float _airdrag = 0.08f, float _maxMoveSpeed = 10) { bounce = _bounce; friction = _friction; airdrag = _airdrag; maxMoveSpeed = _maxMoveSpeed; };
	void Init();
	void Update(float dt) override;

	void HandlePhysics(Transform* tt, float3 nV, float3 nP, bool b);
	//getters//
	bool IsKinematic();

	//setters//
	void SetIsKinematic(bool toggle);
	void ApplyGravity(bool g) { applyGravity = g; };
};