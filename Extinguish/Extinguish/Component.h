#pragma once
//#include "GameObject.h"
//This class will be used to make future components such as renderer, AI, etc.
class GameObject;
class Component
{
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
private:
	GameObject* Object;
	CompType type;
public:
	//none of these necessarily need to be used, they're just there in case and for some unity
	virtual void Init() {};
	virtual void Update(float dt) {};
	virtual void Shutdown() {};
	GameObject* const GetGameObject(void) { return Object; };
	CompType GetCompType() { return type; };
	Component(GameObject* o) { Object = o; };
protected:
	void SetCompType(CompType t) { type = t; };
	void SetGameObject(GameObject* g) { Object = g; };
};