#pragma once
#include <DirectXMath.h>
#include <vector>
#include <string>
#include "Collider.h"

class CapsuleCollider : Collider
{
private:
	float radius;
	float length;
public:
	void Update(float dt) override;
	void setRadius(float r);
	float GetRadius();
};