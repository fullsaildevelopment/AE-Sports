#pragma once
#include <DirectXMath.h>
#include <vector>
#include <string>
#include "Component.h"
#include "GameObject.h"
#include "Transform.h"

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
	std::vector<Collider*> colliding;
	ColliderType Type;
	Transform transform;
protected:
	void SetType(ColliderType type) { Type = type; };
public:
	Transform GetTransform() { return transform; };
	void SetTransform(Transform t) { transform = t; };
	virtual void Update(float dt) {};
	bool isTrigger(void) { return IsTrigger; };
	void SetTrigger(bool trigger) { IsTrigger = trigger; };
	ColliderType GetType() { return Type; };
};