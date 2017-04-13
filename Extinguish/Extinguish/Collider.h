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
	std::vector<GameObject*> ignore;
	vector<GameObject*> objects;
	vector<bool> CollidingWith;
	bool ignoreAABB;
	bool ignoreCapsule;
	bool ignoreSphere;
	bool ignoreHex;
public:
	float3 collisionNormal;
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
	void IgnoreGameObject(GameObject* object);
	void UnIgnoreGameObject(GameObject* object);
	void IgnoreAABB(bool toggle) { ignoreAABB = toggle; };
	void IgnoreCapsule(bool toggle) { ignoreCapsule = toggle; };
	void IgnoreSphere(bool toggle) { ignoreSphere = toggle; };
	void IgnoreHex(bool toggle) { ignoreHex = toggle; };
	bool IgnoreAABB() { return ignoreAABB; };
	bool IgnoreCapsule() { return ignoreCapsule; };
	bool IgnoreSphere() { return ignoreSphere; };
	bool IgnoreHex() { return ignoreHex; };
};