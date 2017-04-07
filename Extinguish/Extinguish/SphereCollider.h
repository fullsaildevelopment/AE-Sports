#pragma once
#include <DirectXMath.h>
#include <vector>
#include <string>
#include "Collider.h"
class GameObject;

class SphereCollider : public Collider
{
private:
	float radius;
	float3 offset;
public:
	SphereCollider(float r, GameObject* o, bool t);
	SphereCollider(float r, GameObject* o, bool t, float3 v);
	void FixedUpdate(float _dt) override;
	Sphere GetSphere();
	Sphere GetWorldSphere();
	void SetOffset(float3 o);
	float3 GetOffset() { return offset; }
};