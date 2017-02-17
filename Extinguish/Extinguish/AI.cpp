#include "AI.h"
#include "GameObject.h"

#define RunSpeed 5
#define AttackSpeed 15
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
		// IF I HAVE THE BALL  drop the ball and 'stumble' in the way they pushed me
		if (ballClass->GetIsHeld() && !ballClass->GetIsThrown() && ballClass->GetHolder() == me)//////////////////////////////////////////////////////////////////////////////////////////////////////
			ballClass->DropBall(me);

		float3 vel = obj->GetGameObject()->GetTransform()->GetForwardf3() * StumbleSpeed;
		me->GetTransform()->AddVelocity(vel);
	}
}

void AI::Init(GameObject *goal1, GameObject *goal2)
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
				myGoal = goal1;
				enemyGoal = goal2;

				// if they're on my team
				if (tmp[i]->GetTag() == "Team1")
				{
					listOfMates.push_back(tmp[i]);

					if (tmp[i]->GetComponent<AI>())
					{
						++fakeTeam;
						AIbuddies.push_back(tmp[i]);
					}
				}

				// if they're on the enemy team
				else if (tmp[i]->GetTag() == "Team2")
					listOfEnemies.push_back(tmp[i]);
			}

			// if i'm team2 -> goal2 is mine
			else if (me->GetTag() == "Team2")
			{
				myGoal = goal2;
				enemyGoal = goal1;

				// if they're on my team
				if (tmp[i]->GetTag() == "Team2")
				{
					listOfMates.push_back(tmp[i]);

					if (tmp[i]->GetComponent<AI>())
					{
						++fakeTeam;
						AIbuddies.push_back(tmp[i]);
					}
				}

				// if they're on the enemy team
				else if (tmp[i]->GetTag() == "Team1")
					listOfEnemies.push_back(tmp[i]);
			}
		}
	}

	switch (fakeTeam)
	{
		case 0: // if I'm the only AI
		{
			currState = tank;

			break;
		}

		case 1: // if there is one other AI
		{
			bool bgoalie = false;
			bool btank = false;

			if (AIbuddies[0]->GetComponent<AI>()->GetCurrState() == tank)
				btank = true;

			else if (AIbuddies[0]->GetComponent<AI>()->GetCurrState() == goalie)
				bgoalie = true;

			if (!bgoalie) currState = goalie;
			else if (!btank) currState = tank;

			break;
		}

		case 2: // if there are two other AI
		{
			bool bgoalie = false;
			bool bplayboy = false;
			bool btank = false;

			for (int i = 0; i < AIbuddies.size(); ++i)
			{
				if (AIbuddies[i]->GetComponent<AI>()->GetCurrState() == goalie)
					bgoalie = true;

				else if (AIbuddies[i]->GetComponent<AI>()->GetCurrState() == playboy)
					bplayboy = true;

				else if (AIbuddies[i]->GetComponent<AI>()->GetCurrState() == tank)
					btank = true;
			}

			if (!bgoalie) currState = goalie;
			else if (!bplayboy) currState = playboy;
			else if (!btank) currState = tank;

			break;
		}

		case 3: // if there are three other AI
		{
			bool bgoalie = false;
			bool bplayboy = false;
			bool bguy1 = false;
			bool btank = false;

			for (int i = 0; i < AIbuddies.size(); ++i)
			{
				if (AIbuddies[i]->GetComponent<AI>()->GetCurrState() == goalie)
					bgoalie = true;

				else if (AIbuddies[i]->GetComponent<AI>()->GetCurrState() == playboy)
					bplayboy = true;

				else if (AIbuddies[i]->GetComponent<AI>()->GetCurrState() == guy)
					bguy1 = true;

				else if (AIbuddies[i]->GetComponent<AI>()->GetCurrState() == tank)
					btank = true;
			}

			if (!bgoalie) currState = goalie;
			else if (!bplayboy) currState = playboy;
			else if (!bguy1) currState = guy;
			else if (!btank) currState = tank;

			break;
		}

		default: break;
	}

	ballClass = ball->GetComponent<BallController>();
	Idle();
}

void AI::Update(float dt)
{
	// if I'm the goalie
	if (currState == goalie || currState == playboy)
	{
		float3 dist = ball->GetTransform()->GetPosition() - myGoal->GetTransform()->GetPosition();

		// if the ball gets close
		if (dist.magnitude() < 2)
		{
			// if no one is holding it or the enemies have it
			if (!ballClass->GetIsHeld() || ballClass->GetHolder()->GetTag() != me->GetTag()) //////////////////////////////////////////////////////////////////////////////////////////////////////
				GetBall();
		}

		// if i have the ball
		if (ballClass->GetIsHeld() && !ballClass->GetIsThrown() && ballClass->GetHolder() == me) //////////////////////////////////////////////////////////////////////////////////////////////////////
		{
			Score();
			Paranoia();
		}

		// if the ball is too far from the goal
		else if (dist.magnitude() > 2)
		{
			if (currState == playboy)
			{
				if (RunTo(enemyGoal))
					Idle();
			}

			else if (currState == goalie)
			{
				// run to our goal and stay there
				if (RunTo(myGoal))
					Idle();
			}
		}
	}

	// if I'm guy1
	else if (currState == guy)
	{
		// if i have the ball or one of my teammates have the ball
		if (ballClass->GetIsHeld() && !ballClass->GetIsThrown())
		{
			if (ballClass->GetHolder() == me) //////////////////////////////////////////////////////////////////////////////////////////////////////
			{
				Score();
				Paranoia();
			}
			
			else if (ballClass->GetHolder()->GetTag() == me->GetTag()) //////////////////////////////////////////////////////////////////////////////////////////////////////
			{
				//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
			}
		}

		else
			GetBall();
	}

	// if I'm the tank
	else if (currState == tank)
	{
		// if someone has the ball
		if (ballClass->GetIsHeld() && !ballClass->GetIsThrown())
		{
			// if they're on my team
			if (ballClass->GetHolder()->GetTag() == me->GetTag()) //////////////////////////////////////////////////////////////////////////////////////////////////////
			{
				// if it's me
				if (ballClass->GetHolder() == me) //////////////////////////////////////////////////////////////////////////////////////////////////////
				{
					Score();
					Paranoia();
				}

				// if it's my friend
				else
					DefendTeammate();
			}

			// if they're on the enemy team
			else
			{
				Attack(ballClass->GetHolder()); //////////////////////////////////////////////////////////////////////////////////////////////////////
			}
		}

		// if nobody has the ball
		else
		{
			// Attack a random dude
		}
	}

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
		if (ballClass->GetHolder()->GetTag() != me->GetTag()) //////////////////////////////////////////////////////////////////////////////////////////////////////
		{
			if (RunTo(ballClass->GetHolder())) //////////////////////////////////////////////////////////////////////////////////////////////////////
				Attack(ballClass->GetHolder()); //////////////////////////////////////////////////////////////////////////////////////////////////////
		}

		else // he's my teammate
			DefendTeammate();
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
	if (ballClass->GetIsHeld() && !ballClass->GetIsThrown() && ballClass->GetHolder()->GetTag() == me->GetTag()) //////////////////////////////////////////////////////////////////////////////////////////////////////
	{
		// double check that it's not me who has the ball
		if (ballClass->GetHolder() != me) //////////////////////////////////////////////////////////////////////////////////////////////////////
		{
			float dist = 785; // random high number
			GameObject *target = nullptr;

			// for each enemy
			for (int i = 0; i < listOfEnemies.size(); ++i)
			{
				// dist between enemy and my teammate
				float3 tmp = listOfEnemies[i]->GetTransform()->GetPosition() - ballClass->GetHolder()->GetTransform()->GetPosition(); //////////////////////////////////////////////////////////////////////////////////////////////////////

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
}

void AI::Attack(GameObject *target)
{
	string tag = target->GetTransform()->GetParent()->GetParent()->GetGameObject()->GetTag();

	// if they're not on my team run into target
	if (tag != me->GetTag())
	{
		isAttacking = true;

		float3 u = (me->GetTransform()->GetForwardf3() * float3(1, 0, 1)).normalize();
		float3 v = ((float3(target->GetTransform()->GetWorld()._41, target->GetTransform()->GetWorld()._42, target->GetTransform()->GetWorld()._43) - me->GetTransform()->GetPosition()) * float3(1, 0, 1)).normalize();
		float degRad = dot_product(u, v);
		me->GetTransform()->RotateY(degRad);
		me->GetTransform()->SetVelocity(v * AttackSpeed);

		isAttacking = false;
	}
}

void AI::Paranoia()
{
	// if i have the ball
	if (ballClass->GetIsHeld() && !ballClass->GetIsThrown() && ballClass->GetHolder() == me) //////////////////////////////////////////////////////////////////////////////////////////////////////
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

	//float3 dist = ball->GetTransform()->GetPosition() - enemyGoal->GetTransform()->GetPosition();
	// if the vector between me and the goal is clear
		// call the balls ThrowTo(goal) funtion
}

AI::State AI::GetCurrState() { return currState; }

bool AI::GetIsAttacking() { return isAttacking; }