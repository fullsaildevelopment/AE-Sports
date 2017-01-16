#pragma once
#include <DirectXMath.h>
#include <vector>
#include <string>
#include "Collider.h"

class SphereCollider : public Collider
{
private:
	float radius;
public:
	SphereCollider(float r, GameObject* o, bool t);
	void Update(float dt, InputManager* input) override;
	Sphere GetSphere();
	Sphere GetWorldSphere();
};