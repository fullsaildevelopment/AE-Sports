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
	std::vector<Collider*> colliding;
	ColliderType Type;
	Transform transform;
protected:
	void SetType(ColliderType type) { Type = type; };
public:
	Collider(GameObject* o, bool trigger);
	Transform GetTransform() { return transform; };
	ColliderType GetType() { return Type; };
	bool isTrigger(void) { return IsTrigger; };
	virtual void Update(float dt) {};
	void SetTransform(Transform t) { transform = t; };
	void SetTrigger(bool trigger) { IsTrigger = trigger; };
	bool isColliding(Collider* c);
};