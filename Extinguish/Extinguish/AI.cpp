#include "AI.h"

#define RunSpeed 20
#define AttackSpeed 35
#define StumbleSpeed 5

/////////////////////////////////////////////////////////////////////////////////////////
// 
// Goals will be tagged as Goal1 or Goal2
// 
// players/AI will be tagged as Team1 or Team2
// 
/////////////////////////////////////////////////////////////////////////////////////////


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

		float3 vel = obj->GetGameObject()->GetTransform()->GetForwardf3() * StumbleSpeed;
		me->GetTransform()->AddVelocity(vel);
	}
}

void AI::OnCollisionEnter(Collider *obj)
{
	SphereCollider *col = dynamic_cast<SphereCollider*>(obj);

	// if i bump into a player, they caught me
	if (col && !ballClass->GetIsHeld())
	{
		ballClass->SetHolder(me);
		ball->GetTransform()->SetPosition(float3(0, 3, 0));
	}
}

void AI::Init()
{
	listOfEnemies.reserve(4);
	listOfMates.reserve(3);

	// grabbing all of the game objects
	std::vector<GameObject*> tmp = *me->GetGameObjects();

	// for each game object
	for (int i = 0; i < tmp.size(); ++i)
	{
		if (tmp[i] != me)
		{
			// if it's the ball, do the thing
			if (tmp[i]->GetTag() == "Ball")
				ball = tmp[i];

			// if i'm team1 -> goal1 is mine
			else if (me->GetTag() == "Team1")
			{
				// if it's my goal
				if (tmp[i]->GetTag() == "Goal1")
					myGoal = tmp[i];

				// if it's the enemy goal
				else if (tmp[i]->GetTag() == "Goal2")
					enemyGoal = tmp[i];
			}

			// if i'm team2 -> goal2 is mine
			else if (me->GetTag() == "Team2")
			{
				// if it's my goal
				if (tmp[i]->GetTag() == "Goal2")
					myGoal = tmp[i];

				// if it's the enemy goal
				else if (tmp[i]->GetTag() == "Goal1")
					enemyGoal = tmp[i];
			}

			// if they're on my team
			else if (tmp[i]->GetTag() == me->GetTag())
			{
				listOfMates.push_back(tmp[i]);

				if (tmp[i]->GetComponent<AI>())
				{
					++fakeTeam;
					AIbuddies.push_back(tmp[i]);
				}

			}

			// if they're enemy team
			else if (tmp[i]->GetTag() != me->GetTag())
				listOfEnemies.push_back(tmp[i]);
		}
	}

	switch (fakeTeam)
	{
	case 0: // if I'm the only AI

		currState = tank;

		break;

	case 1: // if there is one other AI

		if (AIbuddies[0]->GetComponent<AI>()->GetCurrState() == tank)
			currState = goalie;

		else
			currState = tank;

		break;

	case 2: // if there are two other AI

		for (int i = 0; i < AIbuddies.size(); ++i)
		{
			if (AIbuddies[i]->GetComponent<AI>()->GetCurrState() == goalie)
				currState = playboy;

			else if (AIbuddies[i]->GetComponent<AI>()->GetCurrState() == playboy)
				currState = tank;

			else
				currState = goalie;
		}

		break;

	case 3: // if there are three other AI

		for (int i = 0; i < AIbuddies.size(); ++i)
		{
			if (AIbuddies[i]->GetComponent<AI>()->GetCurrState() == goalie)
				currState = playboy;

			else if (AIbuddies[i]->GetComponent<AI>()->GetCurrState() == playboy)
				currState = guy1;

			else if (AIbuddies[i]->GetComponent<AI>()->GetCurrState() == guy1)
				currState = guy2;

			else
				currState = goalie;
		}

		break;

	default: break;

	}

	ballClass = ball->GetComponent<BallController>();
	Idle();
}

void AI::Update(float dt, InputManager* input)
{
	// if I'm the goalie
	if (currState == goalie)
	{
		float3 dist = ball->GetTransform()->GetPosition() - me->GetTransform()->GetPosition();

		// if the ball gets close
		if (dist.magnitude() < 15)
		{
			// if no one is holding it or the enemies have it
			if (!ballClass->GetIsHeld() || ballClass->GetHolder()->GetTag() != me->GetTag())
				GetBall();
		}
		
		if (ballClass->GetIsHeld() && ballClass->GetHolder() == me)
			Paranoia();
		
		else
		{
			// run to our goal and stay there
			if (RunTo(enemyGoal))
				Idle();
		}
	}

	// if I'm the playboy
	else if (currState == playboy)
	{
		if (ballClass->GetIsHeld() && ballClass->GetHolder() == me)
		{
			bool trash = RunTo(enemyGoal);
			Score();
		}

		else
		{
			if (RunTo(enemyGoal))
				Idle();
		}
	}

	// if I'm guy1 || guy2
	else if (currState == guy1 || currState == guy2)
	{
		GetBall();
		Paranoia();
	}

	// if I'm the tank
	else if (currState == tank)
		DefendTeammate();
}

void AI::Idle()
{
	me->GetTransform()->SetVelocity(float3(0, 0, 0));
}

void AI::GetBall()
{
	// if someone has the ball
	if (ballClass->GetIsHeld() && !ballClass->GetIsThrown())
	{
		// if they're not on my team
		if (ballClass->GetHolder()->GetTag() != me->GetTag())
		{
			if (RunTo(ballClass->GetHolder()))
				Attack(ballClass->GetHolder());
		}

		else // he's my teammate
			DefendTeammate();
	}

	float3 dist = ball->GetTransform()->GetPosition() - me->GetTransform()->GetPosition();

	// if im right next to the ball
	if (RunTo(ball) && dist.magnitude() < 1)
	{
		// running into the ball should pick it up *******************************************************************************************
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
			// dist between enemy and my teammate
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

void AI::Paranoia()
{
	// if i have the ball
	if (ballClass->GetIsHeld() && ballClass->GetHolder() == me)
	{
		float edist = 789; // distance to enemies
		float mdist = 0; // distance to friends
		GameObject *target = nullptr; // the friend i'll throw to

		// for each enemy
		for (int i = 0; i < listOfEnemies.size(); ++i)
		{
			float3 tmp = listOfEnemies[i]->GetTransform()->GetPosition() - me->GetTransform()->GetPosition();

			// if their distance is closer, switch to them
			if (tmp.magnitude() < edist)
				edist = tmp.magnitude();
		}

		// if the enemy is close enough to hit me
		if (edist < 10)
		{
			// for each friend
			for (int i = 0; i < listOfMates.size(); ++i)
			{
				float3 tmp2 = listOfMates[i]->GetTransform()->GetPosition() - enemyGoal->GetTransform()->GetPosition();

				if (fakeTeam > 2 && listOfMates[i]->GetComponent<AI>()->GetCurrState() == playboy)
					target = listOfMates[i];

				// if they're closer to the goal
				else if (tmp2.magnitude() < mdist)
				{
					// switch to them
					mdist = tmp2.magnitude();
					target = listOfMates[i];
				}
			}

			// pass the ball
			ballClass->ThrowTo(target);
		}
	}
}

bool AI::RunTo(GameObject *target)
{
	//u - forward vector
	float3 u = (me->GetTransform()->GetForwardf3() * float3(1, 0, 1)).normalize();

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
	if (RunTo(enemyGoal))
		ballClass->ThrowTo(enemyGoal);

	// if the vector between me and the goal is clear
		// call the balls ThrowTo(goal) funtion

	// if (tmp)
		// run into the goal
}

AI::State AI::GetCurrState() { return currState; }

bool AI::GetIsAttacking() { return isAttacking; }