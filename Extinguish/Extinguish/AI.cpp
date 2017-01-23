#include "AI.h"

AI::AI(GameObject* obj) : Component(obj)
{
	me = obj;
}

void AI::OnTriggerEnter(Collider *obj)
{
	CapsuleCollider *col = dynamic_cast<CapsuleCollider*>(obj);

	// if i bump into a player and they are intentionally attacking me
	if (col && obj->GetGameObject()->GetComponent<AI>()->GetIsAttacking())
	{
		// drop the ball and 'stumble' in the way they pushed me
		ballClass->DropBall(me);

		float num = 3;
		float3 vel = obj->GetGameObject()->GetTransform()->GetForwardf3() * num;
		me->GetTransform()->AddVelocity(vel);
	}
}

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
		else if (tmp[i]->GetTag() == "Goal")
		{
			// if it's my goal
			if (tmp[i]->GetName() == myTeam)
				myGoal = tmp[i];

			// if it's enemy's goal
			else
				enemyGoal = tmp[i];
		}

		else if (tmp[i]->GetTag() == "Team1")
			listOfMates.push_back(tmp[i]); // add them to my team list

		else if (tmp[i]->GetTag() == "Team2")
			listOfEnemies.push_back(tmp[i]); // put them in enemy list
	}

	ballClass = ball->GetComponent<BallController>();
	currState = idle;
}

void AI::Update()
{
	// check events and UpdateState accordingly

	// if i have the ball
	if (ballClass->GetIsHeld())
		if (ballClass->GetHolder() == me)
			Score();
	

	// for each enemy
		// if they're coming closer to me
			// pass the ball to a teammate
}

void AI::Idle()
{
	// maybe something with power-ups
	// or tracking

	// do idle pose/anim?
	UpdateState(getBall);
}

void AI::GetBall()
{
	// if someone has the ball
	if (ballClass->GetIsHeld())
	{
		if (ballClass->GetHolder()->GetName() != me->GetName()) // if they're not on my team********
		{
			if (RunTo(ballClass->GetHolder()))
				Attack(ballClass->GetHolder());
		}

		else // he's my teammate
		{
			// help him?
		}
	}

	float3 dist = ball->GetTransform()->GetPosition() - me->GetTransform()->GetPosition();
	
	// if im right next to the ball
	if (RunTo(ball) && dist.magnitude() < 1)
	{
		// grab it

		// the AI would need to have some way of 'hitting' the ball that way the ball could make them the holder
		// for now, i'll do it here manually
		ballClass->SetIsHeld(true);
		ballClass->SetHolder(me);
	}
}

void AI::Attack(GameObject *target)
{
	isAttacking = true;

	// if they're not on my team
	if (target->GetName() != me->GetName()) 
	{
		// hit
		// if they have the ball, make them drop it
	}

	isAttacking = false;
}

bool AI::RunTo(GameObject *target)
{
	// change this later to running with turning*********************************

	//u - forward vector
	float3 u = me->GetTransform()->GetForwardf3();

	//v - vector between me and destination
	float3 v = target->GetTransform()->GetPosition() - me->GetTransform()->GetPosition();

	//degRad - degrees/radians between me and target
	float degRad = (dot_product(u, u) / (u.magnitude() * v.magnitude()));

	me->GetTransform()->RotateX(degRad);

	// run to them
	me->GetTransform()->AddVelocity(float3(12, 12, 12));

	if (v.magnitude() < 3)
		return true;
	
	return false;
}

void AI::Score()
{
	bool tmp = RunTo(enemyGoal);


	// if the vector between me and the goal is clear
		// call the balls ThrowTo(goal) funtion

	// if (tmp)
		// run into the goal
}

AI::State AI::GetCurrState() { return currState; }

bool AI::GetIsAttacking() { return isAttacking; }