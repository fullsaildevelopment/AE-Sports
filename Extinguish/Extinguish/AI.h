#pragma once
#include "Component.h"
#include "GameObject.h"

class AI : Component
{
private:
	enum State
	{
		idle,
		getBall,
		defensive,
		offensive,
	};
	

	State currState; // the AI's current state
	GameObject *goal1; // im not sure if i'll be able to get the goals location through this...
	GameObject *goal2;
	GameObject *ball;
	GameObject *me = GetGameObject(); // access to the gameObject stuff through me
	std::vector<GameObject*> *listOfEnemies; // list of AI's enemy team
	std::vector<GameObject*> *listOfMates; // list of AI's teammates

public:
	void Init();
	void Update();
	void UpdateState(State newState);

	// States
	void Idle();
	void GetBall();
	void Score();

	// Actions
	void Attack(GameObject target); // pass in the person you want to attack
	void RunTo(XMFLOAT4 location); // run to a certain place on the map
	void RunTo(GameObject target); // run to a person or object in the map
	void Toss(XMFLOAT4 location); // throw the ball to a certain location ie the goal
	void TossTo(GameObject target); // pass the ball off to a teammate
};