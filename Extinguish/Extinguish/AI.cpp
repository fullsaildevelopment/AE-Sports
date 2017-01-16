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
	listOfEnemies.reserve(4);
	listOfMates.reserve(4); // I'll include myself for now*************************************
	
	// grabbing all of the game objects
	std::vector<GameObject*> tmp = *me->GetGameObjects();

	// assuming we name the individual AI&&Player as their team number***************************************
	string myTeam = me->GetName();

	// for each game object
	for (int i = 0; i < tmp.size(); ++i)
	{
		// if it's the ball, do the thing
		if (tmp[i]->GetTag() == "Ball")
			ball = tmp[i];

		// if it's a goal
		if (tmp[i]->GetTag() == "Goal")
		{
			// if it's my goal
			if (tmp[i]->GetName() == myTeam)
				myGoal = tmp[i];

			// if it's enemy's goal
			else
				enemyGoal = tmp[i];
		}

		// if it's AI
		else if (tmp[i]->GetTag() == "AI" || tmp[i]->GetTag() == "Player")
		{
			if (tmp[i]->GetName() == myTeam) // if they're on my team
				listOfMates.push_back(tmp[i]);// add them to my team list

			else // if they're not
				listOfEnemies.push_back(tmp[i]);// put them in enemy list
		}
	}

	currState = idle;
}

void AI::Update()
{
	// check events and UpdateState accordingly

	// check if i have a clear shot, toss that shit
	// 
}

void AI::Idle()
{
	// maybe something with power-ups
	// or tracking

	// do idle pose/anim?
}

void AI::GetBall()
{
	// find the ball

	for (int i = 0; i < listOfEnemies.size(); ++i)
	{
		// if person (who is enemy) already has ball
			// RunTo(that guy)
		
		//if ((*listOfEnemies)[i]->)

		// if person (who is team) has ball
			// if enemies are around him
				// Attack(closest enemy to me around my guy)
	}

	// RunTo(listOfEnemies[1], true, true);
}

void AI::Attack(GameObject target)
{
	if (target.GetName() != me->GetName()) // better than looking through my list to see if he's in there ?? still assuming we name all the AI the same if they're on the same team
	{
		// hit
		//target.GetTransform()->Translate(me->GetTransform()->GetPosition()) ***********
	}
}

void AI::RunTo(GameObject *target, bool isPerson, bool isEnemy)
{
	// Multiply what acceleration you want them to have WITH maxSpeed. Then add that to velocity???

	Transform *tmpLoc = me->GetTransform();
	
	float accel = 12 * maxSpeed;
	velocity += accel; // but then i'd need to apply that velocity to my person in the direction of the target??...

	if (isPerson && isEnemy)
	{
		// run to them
		
		// attack them
	}

	else if (!isEnemy)
	{
		// idk why i'd run to my teammate, but do it
	}

	else
	{
		// just run to the gameobject
	}
}

void AI::Toss(XMFLOAT3 location)
{
	//if the goal is a game object, idk why i'd need this function
}

void AI::TossTo(GameObject target)
{
	
}

void AI::Score() // maybe i wont need this, if i just check to see if i have a clear shot in update, i can call toss in there??
{
	// assuming i have the ball

	// if i have a clear shot to the goal
	// Toss(to goal)
}

AI::State AI::GetCurrState() { return currState; }
