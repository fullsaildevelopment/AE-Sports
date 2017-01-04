#pragma once
#include <DirectXMath.h>
#include <vector>
#include <string>
#include "Collider.h"

class CapsuleCollider : Collider
{
private:
	float radious;
	float length;
public:
	bool Update(float dt) override;
};