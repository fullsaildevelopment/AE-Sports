#pragma once
#include "InputManager.h"
#include "EventHandler.h"

class Collider;
class GameObject;

//This class will be used to make future components such as renderer, AI, etc.
class Component : public EventHandler
{
private:
	GameObject* Object;
	bool enabled = true;
public:
	Component() {}
	Component(GameObject* o) { Object = o; };
	~Component() {}

	//basic
	virtual void Init(GameObject* obj) { Object = obj; };
	virtual void Update(float dt) {};
	virtual void FixedUpdate(float dt) {};
	virtual void Shutdown() {};
	virtual void HandleEvent(Event* e) {};

	virtual void OnCollisionEnter(Collider* e) {};
	virtual void OnCollisionStay(Collider* e) {};
	virtual void OnCollisionExit(Collider* e) {};

	virtual void OnTriggerEnter(Collider* e) {};
	virtual void OnTriggerStay(Collider* e) {};
	virtual void OnTriggerExit(Collider* e) {};

	//getters
	GameObject* GetGameObject(void) { return Object; };
	bool isEnabled() { return enabled; };
	
	//setters
	void SetGameObject(GameObject* object) { Object = object; }
	void SetEnabled(bool e) { enabled = e; };
	//Component(GameObject* o) { Object = o; };
protected:
	//void SetGameObject(GameObject* g) { Object = g; };
};