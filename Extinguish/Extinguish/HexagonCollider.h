#pragma once
#include <DirectXMath.h>
#include <vector>
#include <string>
#include "Collider.h"

class GameObject;
class CapsuleCollider;
class SphereCollider;

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
	bool CheckFloor2Capsule(CapsuleCollider* cap, int f);
	bool CheckCapsule(GameObject* tg, int _min, int _max, CapsuleCollider* cap, Capsule c, float3 vel, int f);
	bool CheckFloor2Sphere(SphereCollider* sphere, int f);
	bool CheckSphere(GameObject* tg, int _min, int _max, SphereCollider* sphere, Sphere s, float3 vel, int f);

public:
	HexagonCollider(GameObject* o, float v, float height);
	HexagonCollider(int row, int col, float3* posistions, float height, float v, GameObject* o);
	Hexagon* GetWorldHex();
	Hexagon* GetWorldHex(int i);
	void FixedUpdate(float dt) override;
};