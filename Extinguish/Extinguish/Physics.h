#pragma once
#include "Component.h"
#include "vec3.h"

class GameObject;
class Transform;

class Physics : public Component
{
private:
	//const
	float gravity = -9.8f;
	//cache
	Transform* transform;

	bool stillapplygravity = true;

	// 0-1 1 being complete reflection 0 no reflection
	float bounce = 1.0f;
	// 0-1  1 being all the friction 0 being no friction
	float friction = 0.00f;
	// 0-1 same as others
	float airdrag = 0.00f;

	bool hasMaxSpeed;
	float maxMoveSpeed = 50;

	bool colliding = false;
	bool resting = false;
	//other
	//TODO: incorporate mass. float mass;
	bool isKinematic;
public:
	Physics() {};
	//original values Bounce = 1.06 friction = 0.23 airdrag = 0.08
	Physics(float _bounce, float _friction = 0.23f, float _airdrag = 0.08f, float _maxMoveSpeed = 20, float _grav = -9.8f) { bounce = _bounce; friction = _friction; airdrag = _airdrag; maxMoveSpeed = _maxMoveSpeed; gravity = _grav; };
	void Init();
	void FixedUpdate(float _dt) override;

	void HandlePhysics(Transform* tt, float3 nV, float3 nP, bool _bounce = false, float3 bounceNormal = float3(0,0,0), bool stillApplyGravity = false, bool stopMovement = false);

	//getters//
	bool IsKinematic();
	bool HasMaxSpeed();
	float GetMaxSpeed();

	//setters//
	void SetIsKinematic(bool toggle);
	void SetHasMaxSpeed(bool toggle);
	void SetMaxSpeed(float newMaxSpeed);
};