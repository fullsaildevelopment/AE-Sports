#pragma once
#include "Component.h"
#include "GameObject.h"
#include "vec3.h"
#include "BallController.h"
#include "CapsuleCollider.h"

class AI : public Component
{
private:
	enum State
	{
		idle,
		getBall,
		defensive,
		offensive,
	};

	bool isAttacking = false; // 
	State currState; // the AI's current state
	GameObject *myGoal;
	GameObject *enemyGoal;
	GameObject *ball;
	BallController *ballClass;
	GameObject *me = GetGameObject(); // access to the gameObject stuff through me
	std::vector<GameObject*> listOfEnemies; // list of AI's enemy team
	std::vector<GameObject*> listOfMates; // list of AI's teammates

public:
	void Init();
	void Update();
	void UpdateState(State newState);
	void OnTriggerEnter(Collider *obj) override;

	// States
	void Idle();
	void GetBall();

	// Actions
	void Attack(GameObject *target); // pass in the person you want to attack
	bool RunTo(GameObject *target); // returns true when near target
	void Score(); // throw the ball into the goal
	void DropBall(); // 

	// Accessors
	State GetCurrState();
	bool GetIsAttacking();

	// Mutators


};