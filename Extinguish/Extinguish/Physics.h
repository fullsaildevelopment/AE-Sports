#pragma once
#include "Component.h"

class Transform;

class Physics : public Component
{
private:
	//const
	const float gravity = -9.8f;
	//const float friction 

	//cache
	Transform* transform;

	//other
	//TODO: incorporate mass. float mass;
	bool isKinematic;
public:
	void Init();
	void Update(float dt) override;

	//getters//
	bool IsKinematic();

	//setters//
	void SetIsKinematic(bool toggle);
};