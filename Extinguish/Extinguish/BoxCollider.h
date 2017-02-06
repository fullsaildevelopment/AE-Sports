#pragma once
#include <DirectXMath.h>
#include <vector>
#include <string>
#include "Collider.h"

class GameObject;

class BoxCollider : public Collider
{
private:
	DirectX::XMFLOAT3 min;
	DirectX::XMFLOAT3 max;
public:
	BoxCollider(GameObject* g, bool trigger, DirectX::XMFLOAT3 _max, DirectX::XMFLOAT3 _min);
	void Update(float dt) override;
	DirectX::XMFLOAT3 GetMax() { return max; };
	DirectX::XMFLOAT3 GetMin() { return min; };
	AABB GetAABB();
	AABB GetWorldAABB();
	
};