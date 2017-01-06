#pragma once
//#include "GameObject.h"
//This class will be used to make future components such as renderer, AI, etc.
class GameObject;
class Component
{
private:
	GameObject* Object;
public:
	enum CompType
	{
		CTRenderer,
		CTBoxCollider,
		CTSphereCollider,
		CTCapsuleCollider,
		CTPhysics,
		CTAI,
	};
	//none of these necessarily need to be used, they're just there in case and for some unity
	virtual void Init() {};
	virtual void Update() {};
	virtual void Shutdown() {};
	GameObject* const GetGameObject(void) { return Object; };
};