#pragma once
#include <DirectXMath.h>
#include <vector>
#include <string>
#include "Collider.h"

class SphereCollider : Collider
{
private:
	float radius;
public:
	void Update(float dt) override;
};