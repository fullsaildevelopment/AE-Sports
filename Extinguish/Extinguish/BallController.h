#pragma once
#include "Component.h"
#include "CapsuleCollider.h"
#include "SphereCollider.h"
#include "vec3.h"
#include "XTime.h"

class Physics;
class GameObject;

class BallController : public Component
{
private:
	bool isHeld = false; // if someone is holding the ball
	bool isThrown = false; // is the holder threw the ball
	GameObject *me; // the ball
	GameObject *holder; // the crosse that holds the ball
	XTime timer; // timer until someone else can catch the ball
	Transform* transform;
	Physics* physics;
	
	std::vector<Transform*> nets;

	//members for scoring
	GameObject* prevThrower = nullptr;
	GameObject* thrower = nullptr;
	float timeSincePreviouslyThrown = 0;
	float timeSinceThrown = 0;

public:
	BallController(GameObject* obj);
	void Init();
	void LateInit();
	void Update(float _dt) override;
	void FixedUpdate(float _dt) override;
	void OnTriggerEnter(Collider *obj) override;
	void OnCollisionEnter(Collider* obj) override;
	void HandleEvent(Event* e) override;

	// actions
	void Throw(); 
	void DropBall(GameObject *holder); // pass in the holder and makes them drop the bal

	// accessors
	bool GetIsHeld(); // returns if someone is holding it
	bool GetIsThrown(); // returns if the holder just threw it
	GameObject *GetHolder(); // returns the Mage holding the ball
	GameObject *GetCrosseHolder(); // returns the crosse holding the ball
	GameObject *GetThrower(); // returns the last person who threw the ball
	GameObject *GetPreviousThrower(); // returns the previous to last person who threw the ball
	float GetTimeSinceThrown(); // returns how long it's been since the ball has been thrown
	float GetTimeSincePreviouslyThrown(); // returns how long it's been since the previous to last person threw the ball

	// mutators
	void SetIsHeld(bool ans); // sets if the ball is held
	void SetHolder(GameObject *person); // sets who is holding the ball
};