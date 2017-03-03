#pragma once
#include "Component.h"
#include "vec3.h"
#include "BallController.h"
#include "Crosse.h"
#include "CapsuleCollider.h"

class GameObject;

class AI : public Component
{
private:
	enum State
	{
		goalie, // stays at teams goal
		playboy, // stays at enemy's goal
		guy, // runs after ball to score
		tank, // protects teammate with ball
	};

	bool isAttacking = false; // bool to determine if AI is attacking
	bool startTimer = false; // bool for starting the timer for attacking
	float timer = 2; // timer for attack
	int fakeTeam = 0; // number of AI on my team

	State currState; // the AI's current state or position that they'll take

	GameObject *myGoal; // my goal object
	GameObject *enemyGoal; // the enemy's goal object
	GameObject *ball; // the ball object
	GameObject *realTarget; // holder for attacking
	GameObject *me; // access to the gameObject stuff through me

	BallController *ballClass; // gives me access to the balls script
	Crosse *crosse; // gives me access to the crosse script

	std::vector<GameObject*> listOfEnemies; // list of AI's enemy team
	std::vector<GameObject*> listOfMates; // list of AI's teammates
	std::vector<GameObject*> AIbuddies; // list of AI on my team



public:
	AI(GameObject *obj);
	void Init(GameObject *goal1, GameObject *goal2);
	void Update(float dt) override;
	void OnCollisionEnter(Collider *obj) override;

	// States
	void Idle();
	void GetBall();
	void DefendTeammate();

	// Actions
	void Attack(GameObject *target); // pass in the person you want to attack
	void Paranoia(); // checks how close enemy is to them when they have the ball and acts on it
	bool RunTo(GameObject *target); // returns true when near target
	bool RunTo(GameObject *target, float dist); // returns true when near target, but you can pass in how close you want to be to target
	void TurnTo(GameObject *target); // makes the AI turn to the desired object
	void Score(); // throw the ball into the goal

	// Accessors
	State GetCurrState();
	bool GetIsAttacking();

	// Mutators


};