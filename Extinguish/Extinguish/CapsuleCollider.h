#pragma once
#include <DirectXMath.h>
#include <vector>
#include <string>
#include "Collider.h"

class CapsuleCollider : public Collider
{
private:
	float radius;
	DirectX::XMFLOAT3 Start;
	DirectX::XMFLOAT3 End;
public:
	CapsuleCollider(float r, DirectX::XMFLOAT3 s, DirectX::XMFLOAT3 e, GameObject* o, bool t);
	void Update(float dt) override;
	void setRadius(float r);
	void SetSegment(DirectX::XMFLOAT3 s, DirectX::XMFLOAT3 e);
	float GetRadius();
	Capsule GetWorldCapsule();
	Capsule GetCapsule();
};