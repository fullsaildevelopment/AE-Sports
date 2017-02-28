#pragma once
#include <DirectXMath.h>
#include <vector>
#include <string>
#include "Collider.h"
class GameObject;
class CapsuleCollider;

class HexagonCollider : public Collider
{
private:
	Hexagon hex;
	float height;
	float3* poses;
	int row;
	int col;
	CapsuleCollider* otherCapsule;
	bool CheckSphereAABB(int min, int max, Sphere s);
	bool CheckSphereAABB(AABB test, Sphere s);
	bool CheckCapsuleAABB(int _min, int _max, Capsule s);
	bool CheckCapsuleAABB(AABB test, Capsule s);
	float3 zeroF = float3(0, 0, 0);
	bool CheckFloor2Capsule(CapsuleCollider* cap, GameObject* OtherObject, int f);
	bool CheckCapsule(int _min, int _max, Capsule cap);

public:
	HexagonCollider(GameObject* o, float v, float height);
	HexagonCollider(int row, int col, float3* posistions, float height, float v, GameObject* o);
	Hexagon* GetWorldHex();
	Hexagon* GetWorldHex(int i);
	void FixedUpdate(float dt) override;
};