#pragma once
#include <DirectXMath.h>
#include <vector>
#include <string>
#include "Collider.h"

class GameObject;
class BoxCollider;

class CapsuleCollider : public Collider
{
private:
	float radius;
	DirectX::XMFLOAT3 Start;
	DirectX::XMFLOAT3 End;
	CapsuleCollider* otherCapsule;
	BoxCollider* otherBox;

public:
	CapsuleCollider(float r, DirectX::XMFLOAT3 s, DirectX::XMFLOAT3 e, GameObject* o, bool t);
	void FixedUpdate(float _dt) override;
	void setRadius(float r);
	void SetSegment(DirectX::XMFLOAT3 s, DirectX::XMFLOAT3 e);
	float GetRadius();
	Capsule GetWorldCapsule();
	Capsule GetCapsule();
};