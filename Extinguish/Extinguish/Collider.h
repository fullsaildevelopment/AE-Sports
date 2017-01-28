#pragma once
#include <DirectXMath.h>
#include <vector>
#include <string>
#include "GameObject.h"
#include "Component.h"
#include "Transform.h"
#include "CollisionLibrary.h"

class Collider : public Component
{
private:
	bool IsTrigger;
	Transform transform;
	bool IsStatic;
protected:
	std::vector<Collider*> checked;
public:
	Collider(GameObject* o, bool trigger);
	Transform GetTransform() { return transform; };
	bool isTrigger(void) { return IsTrigger; };
	virtual void Update(float dt, InputManager* input) {};
	void SetTransform(Transform t) { transform = t; };
	void SetTrigger(bool trigger) { IsTrigger = trigger; };
	bool isColliding(Collider* c);
	bool isStatic() { return IsStatic; };
	void SetStatic(bool s) { IsStatic = s; };
};