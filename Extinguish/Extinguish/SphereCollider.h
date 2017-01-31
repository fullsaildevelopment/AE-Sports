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
	SphereCollider(float r, GameObject* o, bool t, float3 v);
	void Update(float dt) override;
	Sphere GetSphere();
	Sphere GetWorldSphere();
};