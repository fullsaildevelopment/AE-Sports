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
		goalie,
		playboy,
		guy1,
		guy2,
		tank,
	};

	bool isAttacking = false;
	int fakeTeam = 0; // number of AI on my team
	State currState; // the AI's current state or position that they'll take
	GameObject *myGoal;
	GameObject *enemyGoal;
	GameObject *ball;
	BallController *ballClass; // gives me access to the balls script
	GameObject *me; // access to the gameObject stuff through me
	std::vector<GameObject*> listOfEnemies; // list of AI's enemy team
	std::vector<GameObject*> listOfMates; // list of AI's teammates
	std::vector<GameObject*> AIbuddies; // list of AI on my team

public:
	AI(GameObject *obj);
	void Init();
	void Update(float dt, InputManager* input) override;
	void OnTriggerEnter(Collider *obj) override;
	void OnCollisionEnter(Collider *obj) override;

	// States
	void Idle();
	void GetBall();
	void DefendTeammate();

	// Actions
	void Attack(GameObject *target); // pass in the person you want to attack
	void Paranoia(); // checks how close enemy is to them and acts on it
	bool RunTo(GameObject *target); // returns true when near target
	void Score(); // throw the ball into the goal

	// Accessors
	State GetCurrState();
	bool GetIsAttacking();

	// Mutators


};