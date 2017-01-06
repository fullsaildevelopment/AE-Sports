#pragma once
#include <DirectXMath.h>
#include <vector>
#include <string>
#include "Collider.h"

class BoxCollider : public Collider
{
private:
	DirectX::XMFLOAT3 min;
	DirectX::XMFLOAT3 max;
public:
	bool Update(float dt) override;
};