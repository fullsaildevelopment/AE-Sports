#pragma once
#include <DirectXMath.h>
#include <vector>
#include <string>
#include "Collider.h"

class SphereCollider : Collider
{
private:
	float radious;
public:
	bool Update(float dt) override;
};