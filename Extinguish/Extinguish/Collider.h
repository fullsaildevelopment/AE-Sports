#pragma once
#include <DirectXMath.h>
#include <vector>
#include <string>
#include "Component.h"
#include "Transform.h"
#include "CollisionLibrary.h"
#include "Physics.h"

class GameObject;

class Collider : public Component
{
public:
	enum ColliderType
	{
		CTAABB,
		CTCapsule,
		CTSphere,
		CTHex
	};
private:
	bool IsTrigger;
	bool IsStatic;
	ColliderType type;
protected:
	Transform* transform;
	std::vector<Collider*> checked;
	vector<GameObject*> objects;
	vector<bool> CollidingWith;
public:
	Collider(GameObject* o, bool trigger);
	Collider(GameObject* o, bool trigger, ColliderType t);
	Transform* GetTransform() { return transform; };
	bool isTrigger(void) { return IsTrigger; };
	virtual void Update(float _dt) {};
	void SetTransform(Transform* t) { transform = t; };
	void SetTrigger(bool trigger) { IsTrigger = trigger; };
	bool isColliding(Collider* c);
	bool isStatic() { return IsStatic; };
	void SetStatic(bool s) { IsStatic = s; };
	ColliderType GetColliderType() { return type; };
};