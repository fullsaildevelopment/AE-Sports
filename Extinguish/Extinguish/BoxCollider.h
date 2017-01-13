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
	bool IsStatic;
public:
	BoxCollider(GameObject* g, bool trigger, DirectX::XMFLOAT3 _max, DirectX::XMFLOAT3 _min);
	void Update(float dt, InputManager* input) override;
	DirectX::XMFLOAT3 GetMax() { return max; };
	DirectX::XMFLOAT3 GetMin() { return min; };
	AABB GetAABB();
	AABB GetWorldAABB();
	bool isStatic() { return IsStatic; };
	void SetStatic(bool s) { IsStatic = s; };
	float3 XMtoF(DirectX::XMFLOAT3 m);
};