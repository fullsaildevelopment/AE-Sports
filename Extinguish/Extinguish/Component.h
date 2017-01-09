#pragma once
//#include "GameObject.h"
#include "InputManager.h"

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
	Component() {}
	~Component() {}

	//basic
	virtual void Init(GameObject* obj) { Object = obj; };
	virtual void Update(float dt, InputManager* input) {};
	virtual void Shutdown() {};

	//getters
	GameObject* const GetGameObject(void) { return Object; };
	CompType GetCompType() { return type; };
	Component(GameObject* o) { Object = o; };

	//setters
	void SetGameObject(GameObject* object) { Object = object; }
	//Component(GameObject* o) { Object = o; };
protected:
	void SetCompType(CompType t) { type = t; };
	void SetGameObject(GameObject* g) { Object = g; };
};