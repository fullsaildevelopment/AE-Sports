#pragma once
//#include "GameObject.h"
#include "InputManager.h"
#include "EventHandler.h"

class Collider;
class GameObject;

//This class will be used to make future components such as renderer, AI, etc.
class Component : public EventHandler
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
		CTBall,
	};
private:
	GameObject* Object;
	CompType type;
public:
	Component() {}
	Component(GameObject* o) { Object = o; };
	~Component() {}

	//basic
	virtual void Init(GameObject* obj) { Object = obj; };
	virtual void Update(float dt, InputManager* input) {};
	virtual void Shutdown() {};
	virtual void HandleEvent(Event* e) {};

	virtual void OnCollisionEnter(Collider* e) {};
	virtual void OnCollisionStay(Collider* e) {};
	virtual void OnCollisionExit(Collider* e) {};

	virtual void OnTriggerEnter(Collider* e) {};
	virtual void OnTriggerStay(Collider* e) {};
	virtual void OnTriggerExit(Collider* e) {};

	//getters
	GameObject* const GetGameObject(void) { return Object; };
	CompType GetCompType() { return type; };

	//setters
	void SetGameObject(GameObject* object) { Object = object; }
	//Component(GameObject* o) { Object = o; };
protected:
	void SetCompType(CompType t) { type = t; };
	//void SetGameObject(GameObject* g) { Object = g; };
};