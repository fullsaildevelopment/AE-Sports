#pragma once
#include "Component.h"
#include "GameObject.h"
#include "vec3.h"

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

	//XMFLOAT3 velocity = XMFLOAT3(0, 0, 0);
	//float velocity = 0;
	//float maxSpeed = 100; // ??*******************************

	State currState; // the AI's current state
	GameObject *myGoal;
	GameObject *enemyGoal;
	GameObject *ball;
	GameObject *me = GetGameObject(); // access to the gameObject stuff through me
	std::vector<GameObject*> listOfEnemies; // list of AI's enemy team
	std::vector<GameObject*> listOfMates; // list of AI's teammates

public:
	void Init();
	void Update();
	void UpdateState(State newState);

	// States
	void Idle();
	void GetBall();

	// Actions
	void Attack(GameObject target); // pass in the person you want to attack
	void RunTo(GameObject *target, bool isPerson, bool isEnemy); // run to a person or object in the map
	void Toss(float3 location); // throw the ball to a certain location ie the goal
	void TossTo(GameObject target); // pass the ball off to a teammate
	void Score(); // throw the ball into the goal

	// Accessors
	State GetCurrState();

	// Mutators


};