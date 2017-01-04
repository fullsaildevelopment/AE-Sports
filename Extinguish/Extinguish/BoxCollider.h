#pragma once
#include <DirectXMath.h>
#include <vector>
#include <string>
#include "Collider.h"

class BoxCollider : Collider
{
private:
	DirectX::XMFLOAT3 min;
	DirectX::XMFLOAT3 max;
public:
	bool Update(float dt) override;
};