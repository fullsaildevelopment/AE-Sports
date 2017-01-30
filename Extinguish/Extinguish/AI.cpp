#include "AI.h"

#define RunSpeed 10
#define AttackSpeed 20
#define StumbleSpeed 5


AI::AI(GameObject* obj) : Component(obj)
{
	me = obj;
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

	case defendTeammate: DefendTeammate(); break;

	default: break;

	}
}

void AI::OnTriggerEnter(Collider *obj)
{
	CapsuleCollider *col = dynamic_cast<CapsuleCollider*>(obj);

	// if i bump into a player and they are intentionally attacking me
	if (col && obj->GetGameObject()->GetComponent<AI>()->GetIsAttacking())
	{
		// drop the ball and 'stumble' in the way they pushed me
		ballClass->DropBall(me);

		float3 vel = obj->GetGameObject()->GetTransform()->GetForwardf3() * StumbleSpeed;
		me->GetTransform()->AddVelocity(vel);
	}
}

void AI::OnCollisionEnter(Collider *obj) // ***************************************************************************************
{
	SphereCollider *col = dynamic_cast<SphereCollider*>(obj);

	// if i bump into a player, they caught me
	if (col && !ballClass->GetIsHeld())
	{
		ballClass->SetHolder(me);
		printf("Ball Caught");
	}
}

void AI::Init()
{
	listOfEnemies.reserve(4);
	//listOfMates.reserve(4); // I'll include myself for now*************************************
	
	// grabbing all of the game objects
	std::vector<GameObject*> tmp = *me->GetGameObjects();

	// for each game object
	for (int i = 0; i < tmp.size(); ++i)
	{
		// if it's the ball, do the thing
		if (tmp[i]->GetTag() == "Ball")
			ball = tmp[i];

		// if it's my goal
		else if (tmp[i]->GetTag() == "Team1Goal")
			myGoal = tmp[i];

		// if it's the enemy goal
		else if (tmp[i]->GetTag() == "Team2Goal")
			myGoal = tmp[i];

		// if they're on my team
		//else if (tmp[i]->GetTag() == "Team1")
		//	listOfMates.push_back(tmp[i]);

		// if they're enemy team
		else if (tmp[i]->GetTag() == "Team2")
			listOfEnemies.push_back(tmp[i]);
	}

	ballClass = ball->GetComponent<BallController>();
	currState = idle;
}

void AI::Update(float dt, InputManager* input)
{
	// check events and UpdateState accordingly

	// if i have the ball
	if (ballClass->GetIsHeld())
	{
		if (ballClass->GetHolder() == me)
			Score();

		for (int i = 0; i < listOfEnemies.size(); ++i)
		{
			// if they're too close to me
				// pass ball
		}
	}

	else
		GetBall();

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
		// if they're not on my team
		if (ballClass->GetHolder()->GetTag() != me->GetTag()) 
		{
			if (RunTo(ballClass->GetHolder()))
				Attack(ballClass->GetHolder());
		}

		else // he's my teammate
		{
			UpdateState(defendTeammate);
		}
	}

	float3 dist = ball->GetTransform()->GetPosition() - me->GetTransform()->GetPosition();
	
	// if im right next to the ball
	if (RunTo(ball) && dist.magnitude() < 1)
	{
		// running into the ball should pick it up
	}
}

void AI::DefendTeammate()
{
	// double check that my teammate has the ball
	if (ballClass->GetIsHeld() && ballClass->GetHolder()->GetTag() == me->GetTag())
	{
		float dist = 785; // random high number
		GameObject *target = nullptr;

		// for each enemy
		for (int i = 0; i < listOfEnemies.size(); ++i)
		{
			// dist between enemy and my guy
			float3 tmp = listOfEnemies[i]->GetTransform()->GetPosition() - ballClass->GetHolder()->GetTransform()->GetPosition();

			// if this dist is less than last
			if (tmp.magnitude() < dist)
			{
				// make them my target
				dist = tmp.magnitude();
				target = listOfEnemies[i];
			}
		}

		if (target)
			Attack(target);
	}
}

void AI::Attack(GameObject *target)
{
	isAttacking = true;

	// if they're not on my team
	if (target->GetTag() != me->GetTag() && RunTo(target))
	{
		// run into target
		me->GetTransform()->AddVelocity(float3(AttackSpeed, AttackSpeed, AttackSpeed));
	}

	isAttacking = false;
}

bool AI::RunTo(GameObject *target)
{
	//u - forward vector
	float3 u = (me->GetTransform()->GetForwardf3() * float3(1,0,1)).normalize();

	//v - vector between me and destination
	float3 v = ((target->GetTransform()->GetPosition() - me->GetTransform()->GetPosition()) * float3(1, 0, 1)).normalize();

	//degRad - degrees/radians between me and target
	float degRad = dot_product(u, v);// / (u.magnitude() * v.magnitude());

	me->GetTransform()->RotateY(degRad);

	// run to them
	me->GetTransform()->SetVelocity(v * RunSpeed);

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