#pragma once
#include <DirectXMath.h>
#include <vector>
#include <string>
#include "Collider.h"
#include "CollisionLibrary.h"

class BoxCollider : public Collider
{
private:
	DirectX::XMFLOAT3 min;
	DirectX::XMFLOAT3 max;
public:
	void Update(float dt) override;
	DirectX::XMFLOAT3 GetMax() { return max; };
	DirectX::XMFLOAT3 GetMin() { return min; };
	AABB GetAABB();
	AABB GetWorldAABB();
};