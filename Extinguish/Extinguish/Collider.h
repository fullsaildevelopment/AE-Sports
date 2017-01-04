#pragma once
#include <DirectXMath.h>
#include <vector>
#include <string>
#include "Component.h"

class Collider : Component
{
public:
	enum ColliderType
	{
		sphere,
		aabb,
		capsule,
		mesh
	};
private:
	//Sphere and Capsule
	float radious;
	//AABB
	DirectX::XMFLOAT3 min;
	DirectX::XMFLOAT3 max;
	//Capsule
	float length;
	//Mesh
		//triangle indexs to mesh
		//mesh points
public:
	bool Update(float dt);
};