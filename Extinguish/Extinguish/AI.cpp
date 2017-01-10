#include "AI.h"

void AI::UpdateState(State newState)
{
	switch (currState)
	{
	case idle: Idle(); break;

	case getBall: GetBall(); break;

	case defensive: 
		// do something
		break;

	case offensive:
		// do something
		break;

	default: break;

	}
}

void AI::Init()
{
	listOfEnemies->reserve(4);
	listOfMates->reserve(3);
	
	// grabbing all of the game objects
	std::vector<GameObject*>* tmp = me->GetGameObjects();

	// for each game object
	for (int i = 0; i < tmp->size(); ++i)
	{
		// go through and see what is
		// assign the ball
		// fill out my lists
	}

	currState = idle;
}

void AI::Update()
{
	// check events and UpdateState accordingly

	// if we do a starting timer to every game, ...
}

void AI::Idle()
{
	// maybe something with power-ups
	// or tracking

	// do idle pose/anim?
}

void AI::GetBall()
{
	// if person (who is enemy) already has ball
		// RunTo(that guy)

	// if person (who is team) has ball
		// if enemies are around him
			// Attack(closest enemy to me around my guy)
}

void AI::Score()
{
	// assuming i have the ball

	// if i have a clear shot to the goal
		// Toss(to goal)
}

void AI::Attack(GameObject target)
{
	// if (target == enemy/not teammate)
		// hit
}

void AI::RunTo(XMFLOAT4 location)
{

}

void AI::RunTo(GameObject Target)
{
	//...
	// if next to
		//Attack(Target)
}

void AI::Toss(XMFLOAT4 location)
{

}

void AI::TossTo(GameObject target)
{

}