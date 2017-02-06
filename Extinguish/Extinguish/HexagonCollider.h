#pragma once
#include <DirectXMath.h>
#include <vector>
#include <string>
#include "Collider.h"

class HexagonCollider : public Collider
{
private:
	Hexagon hex;
	float height;
	float3* poses;
	int row;
	int col;
	bool CheckSphereAABB(int min, int max, Sphere s);
	bool CheckSphereAABB(AABB test, Sphere s);
	bool CheckCapsuleAABB(int _min, int _max, Capsule s);
	bool CheckCapsuleAABB(AABB test, Capsule s);
public:
	HexagonCollider(GameObject* o, float v, float height);
	HexagonCollider(int row, int col, float3* posistions, float height, float v, GameObject* o);
	Hexagon* GetWorldHex();
	Hexagon* GetWorldHex(int i);
	void Update(float dt) override;
};