#pragma once
#include "Component.h"
#include "vec3.h"
#include "BallController.h"
#include "Crosse.h"
#include "CapsuleCollider.h"
#include "Map.h"

class GameObject;
class AnimatorController;

class AI : public Component
{
private:
	enum State
	{
		goalie, // stays at teams goal
		playboy, // paces at enemy's goal
		guy, // runs after ball to score
		tank, // protects teammate with ball
	};

	bool at1 = false; // if goalie2 is at it's first checkpoint
	bool isAttacking = false; // bool to determine if AI is attacking
	bool startTimer = false; // bool for starting the timer for attacking
	bool canMove = false; //added by Tom for countdown portion
	bool validPath = false;
	float timer = 5.0f; // timer for attack
	int fakeTeam = 0; // number of AI on my team
	int pathIndex = 0;
	float moveSpeedMultiplier = 1.0f;

	State currState; // the AI's current state or position that they'll take
	Transform *camera; // gives me access to the camera transform

	GameObject *myGoal = nullptr; // my goal object
	GameObject *enemyGoal = nullptr; // the enemy's goal object
	GameObject *ball = nullptr; // the ball object
	GameObject *realTarget = nullptr; // holder for attacking
	GameObject *ogTarget = nullptr; // holder for attacking
	GameObject *pathTarget = nullptr;
	GameObject *eTank = nullptr; // enemy's tank
	GameObject *mGo2 = nullptr; // my goalie2
	GameObject *mGuy = nullptr; // my Guy
	GameObject *me = nullptr; // access to the gameObject stuff through me

	BallController *ballClass; // gives me access to the balls script
	Crosse *crosse; // gives me access to the crosse script
	AnimatorController* anim; // gives access to animations
	Map::Node *myNode;
	Map::Node *ballNode;
	Map::Node *tarNode;

	std::vector<Map::Node *> path;
	std::vector<GameObject*> listOfEnemies; // list of AI's enemy team
	std::vector<GameObject*> listOfMates; // list of AI's teammates
	std::vector<GameObject*> AIbuddies; // list of AI on my team


public:

	static Map *aiPath;

	AI(GameObject *obj);
	void OnCollisionEnter(Collider *obj) override;
	void HandleEvent(Event* e) override;
	void Init(GameObject *goal1, GameObject *goal2);
	void Update(float _dt) override;
	void FixedUpdate(float dt);

	// States
	void Idle(); // sets velocity to 0
	void GetBall(); // chases after ball
	void DefendTeammate(); // attacks enemy closest to teammate

	// Actions
	void Attack(GameObject *target); // pass in the person you want to attack
	void Paranoia(); // checks how close enemy is to them when they have the ball and acts on it
	bool RunTo(GameObject *target); // returns true when near target, but you can pass in how close you want to be to target
	bool RunTo(float3 target, float dist); // returns true when near position
	void TurnTo(float3 target); // makes the AI turn to the desired position
	void TurnTo(GameObject *target); // makes the AI turn to the desired object
	void Score(); // throw the ball into the goal

	// Accessors
	State GetCurrState(); // returns what position of the team the AI is
	GameObject *GetTarget(); // returns the target the AI is attacking
	bool GetIsAttacking(); // returns if im attacking
	bool GetCanMove();
	float GetMoveSpeedMultiplier();

	// Mutators
	void SetCanMove(bool ans);
	void SetMoveSpeedMultiplier(float multiplier);
};