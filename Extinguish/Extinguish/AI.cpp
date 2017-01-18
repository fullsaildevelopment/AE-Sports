#include "AI.h"

void AI::OnTriggerEnter(Collider *obj)
{
	SphereCollider *col = dynamic_cast<SphereCollider*>(obj);

	if (col)
	{
		// set everything to them being the holder
		//isHeld = true;
		//holder = obj->GetGameObject();
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

	
}

void AI::Attack(GameObject *target)
{
	// better than looking through my list to see if he's in there ?? still assuming we name all the AI the same if they're on the same team
	if (target->GetName() != me->GetName()) 
	{
		// hit
		// if they have the ball, make them drop it
	}
}

bool AI::RunTo(GameObject *target)
{
	// change this later to running with turning*********************************
	// turn to them

	//u - forward vector
	float3 u(me->GetTransform()->GetForward().x, me->GetTransform()->GetForward().y, me->GetTransform()->GetForward().z);

	//v - vector between me and destination
	float3 v = target->GetTransform()->GetPosition() - me->GetTransform()->GetPosition();

	//degRad - degrees/radians between me and target
	float degRad = (dot_product(u, u) / (u.magnitude() * v.magnitude()));

	me->GetTransform()->RotateX(degRad);

	// run to them
	me->GetTransform()->AddVelocity(XMFLOAT3(12, 12, 12));

	if (v.magnitude() < 5)
		return true;
}

void AI::Score()
{
	bool tmp = RunTo(enemyGoal);


	// if the vector between me and the goal is clear
		// call the balls ThrowTo(goal) funtion

	// if (tmp)
		// run into the goal
}

void AI::DropBall()
{
	ballClass->SetIsHeld(false);
	ballClass->SetHolder(nullptr);
}

AI::State AI::GetCurrState() { return currState; }
