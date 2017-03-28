#pragma once
#include <DirectXMath.h>
#include <vector>
#include <string>
#include "Collider.h"
#include "EDMesh.h"

class GameObject;
class CapsuleCollider;
class SphereCollider;
class FloorController;

class HexagonCollider : public Collider
{
private:
	ED2Mesh* mesh;
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
	bool CheckFloor2Capsule(CapsuleCollider* cap, int f, float dt);
	bool CheckCapsule(GameObject* tg, GameObject* ob, int _min, int _max, CapsuleCollider* cap, Capsule c, float3 pastPos, int f, float dt, float& St);
	bool CheckFloor2Sphere(SphereCollider* sphere, int f, float dt);
	bool CheckSphere(GameObject* tg, GameObject* ob, int _min, int _max, SphereCollider* sphere, Sphere s, float3 pastPos, int f, float dt, float& St);
	FloorController* floorController;
	unsigned int* originalColor;

public:
	HexagonCollider(GameObject* o, float v, float height, ED2Mesh* colmesh = nullptr);
	HexagonCollider(int row, int col, float3* posistions, float height, float v, GameObject* o, FloorController* FC, ED2Mesh* colmesh = nullptr);
	Hexagon* GetWorldHex();
	Hexagon* GetWorldHex(int i);
	void FixedUpdate(float dt) override;
};