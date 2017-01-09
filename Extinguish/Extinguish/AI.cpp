#include "AI.h"

void AI::UpdateState(State newState)
{
	switch (currState)
	{
	case idle: Idle(); break;

	case getBall: GetBall(); break;

	case attack: Attack(); break;

	case score: Score(); break;

	default: break;

	}
}

void AI::Update()
{
	// check events and UpdateState accordingly
}

void AI::Idle()
{
	// im not sure why they'd be in idle...
	// maybe something with power-ups
	// or tracking
}

void AI::GetBall()
{
	// find ball

	// if person (who is enemy) already has it
		// RunTo(that guy)

	// if person (who is team) has it
		// if enemies are around him
			// attack them
}

void AI::Attack() //
{
	// if enemy
		// hit
}

void AI::Score()
{
	// assuming i have the ball

}

//RunTo(some location?)

//RunTo(gameObject Target)
	//...
	// if next to
		//Attack(Target)