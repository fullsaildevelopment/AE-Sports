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
		defendTeammate,
	};

	bool isAttacking = false; // 
	State currState; // the AI's current state
	GameObject *myGoal;
	GameObject *enemyGoal;
	GameObject *ball;
	BallController *ballClass;
	GameObject *me; // access to the gameObject stuff through me
	std::vector<GameObject*> listOfEnemies; // list of AI's enemy team
	std::vector<GameObject*> listOfMates; // list of AI's teammates

public:
	AI(GameObject *obj);
	void Init();
	void Update(float dt) override;
	void UpdateState(State newState);
	void OnTriggerEnter(Collider *obj) override;
	void OnCollisionEnter(Collider *obj) override;

	// States
	void Idle();
	void GetBall();
	void DefendTeammate();

	// Actions
	void Attack(GameObject *target); // pass in the person you want to attack
	bool RunTo(GameObject *target); // returns true when near target
	void Score(); // throw the ball into the goal

	// Accessors
	State GetCurrState();
	bool GetIsAttacking();

	// Mutators


};