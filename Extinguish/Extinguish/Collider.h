#pragma once
#include <DirectXMath.h>
#include <vector>
#include <string>
#include "Component.h"
#include "GameObject.h"

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
	float isTrigger;
	std::vector<Collider*> colliding;
public:
	virtual bool Update(float dt);
	bool isTrigger(void);
	void SetisTrigger(bool trigger);
};