#pragma once
#include <DirectXMath.h>
#include <vector>
#include <string>
#include "Component.h"
#include "GameObject.h"
#include "Transform.h"
#include "CollisionLibrary.h"

class Collider : public Component
{
public:
	enum ColliderType
	{
		sphere,
		aabb,
		capsule,
		mesh
	};
private:
	bool IsTrigger;
	ColliderType Type;
	Transform transform;
	bool IsStatic;
protected:
	void SetType(ColliderType type) { Type = type; };
	std::vector<Collider*> checked;
public:
	Collider(GameObject* o, bool trigger);
	Transform GetTransform() { return transform; };
	ColliderType GetType() { return Type; };
	bool isTrigger(void) { return IsTrigger; };
	virtual void Update(float dt, InputManager* input) {};
	void SetTransform(Transform t) { transform = t; };
	void SetTrigger(bool trigger) { IsTrigger = trigger; };
	bool isColliding(Collider* c);
	bool isStatic() { return IsStatic; };
	void SetStatic(bool s) { IsStatic = s; };
};