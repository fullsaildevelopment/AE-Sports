#include "AI.h"
#include "GameObject.h"

#define     RunSpeed 10
#define  AttackSpeed 20
#define StumbleSpeed 10

/////////////////////////////////////////////////////////////////////////////////////////
// 
// players/AI will be tagged as Team1 or Team2
// 
/////////////////////////////////////////////////////////////////////////////////////////

AI::AI(GameObject* obj) : Component(obj)
{
	me = obj;
}

void AI::OnCollisionEnter(Collider *obj)
{
	CapsuleCollider *col = dynamic_cast<CapsuleCollider*>(obj);

	if (col)
	{
		if (obj->GetGameObject() == realTarget)
		{
			startTimer = true;

			// handle the target stumbling and dropping ball
			if (!ballClass->GetIsThrown() && ballClass->GetHolder() == realTarget)
				ballClass->DropBall(realTarget);

			realTarget->GetTransform()->AddVelocity(float3(0, 5, 0));
			realTarget->GetTransform()->AddVelocity(me->GetTransform()->GetForwardf3().negate() * (StumbleSpeed, 0, StumbleSpeed));
			realTarget = nullptr;

			//me->GetTransform()->AddVelocity(float3(0, 2, 0));
			//me->GetTransform()->AddVelocity(me->GetTransform()->GetForwardf3() * (2, 0, 2));
		}
	}
}

void AI::Init(GameObject *goal1, GameObject *goal2)
{
	listOfEnemies.reserve(4);
	listOfMates.reserve(3);
	AIbuddies.reserve(3);

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
			bool bguy = false;
			bool btank = false;

			for (int i = 0; i < AIbuddies.size(); ++i)
			{
				if (AIbuddies[i]->GetComponent<AI>()->GetCurrState() == goalie)
					bgoalie = true;

				else if (AIbuddies[i]->GetComponent<AI>()->GetCurrState() == playboy)
					bplayboy = true;

				else if (AIbuddies[i]->GetComponent<AI>()->GetCurrState() == guy)
					bguy = true;

				else if (AIbuddies[i]->GetComponent<AI>()->GetCurrState() == tank)
					btank = true;
			}

			if (!bgoalie) currState = goalie;
			else if (!bplayboy) currState = playboy;
			else if (!bguy) currState = guy;
			else if (!btank) currState = tank;

			break;
		}

		default: break;
	}

	ballClass = ball->GetComponent<BallController>();
	Idle();
}

void AI::Update(float _dt)
{
	if (startTimer)
		timer -= _dt;

	if (!crosse)
		crosse = me->GetTransform()->GetChild(0)->GetChild(0)->GetGameObject()->GetComponent<Crosse>();

	if (!camera)
	{
		camera = me->GetTransform()->GetChild(0)->GetGameObject()->GetTransform();
		camera->RotateX(-0.68f); // 345 -0.68f
	}

	if (!eTank)
	{
		for (int i = 0; i < listOfEnemies.size(); ++i)
		{
			if (listOfEnemies[i]->GetComponent<AI>() && listOfEnemies[i]->GetComponent<AI>()->GetCurrState() == tank)
				eTank = listOfEnemies[i];
		}
	}


#pragma region Goalie
	if (currState == goalie)
	{
		float3 dist = ball->GetTransform()->GetWorldPosition() - myGoal->GetTransform()->GetPosition();

		// if the ball gets close
		if (dist.magnitude() < 28)
		{
			// if no one is holding it or the enemies have it
			if (!ballClass->GetIsThrown() && (!ballClass->GetIsHeld() || ballClass->GetHolder()->GetTag() != me->GetTag()))
				GetBall();
		}

		// if i have the ball pass it off to someone
		if (ballClass->GetIsHeld() && !ballClass->GetIsThrown() && ballClass->GetHolder() == me)
		{
			GameObject *myGuy = nullptr;

			for (int i = 0; i < listOfMates.size(); ++i)
			{
				float mdist = 789; // distance to me
				float3 tmp2 = listOfMates[i]->GetTransform()->GetWorldPosition() - me->GetTransform()->GetPosition();

				if (fakeTeam == 3 && listOfMates[i]->GetComponent<AI>()->GetCurrState() == guy)
					myGuy = listOfMates[i];

				// if they're closer to me
				else if (tmp2.magnitude() < mdist)
				{
					// switch to them
					mdist = tmp2.magnitude();
					myGuy = listOfMates[i];
				}
			}

			if (myGuy && RunTo(myGuy, 10.0f))
				crosse->Throw();
		}

		// if the ball is too far from the goal
		else if (dist.magnitude() > 28)
		{
			if (RunTo(myGoal, 15.0f))
			{
				TurnTo(enemyGoal);
				Idle();
			}
		}
	}
#pragma endregion

#pragma region Goalie2
	else if (currState == playboy)
	{
		float3 pos1 = float3(-9, 0.5, -25);
		float3 pos2 = float3(-30, 0.5, -25);

		if (me->GetTag() == "Team2")
		{
			pos1.z *= -1;
			pos2.z *= -1;
		}

		if (ballClass->GetIsHeld() && !ballClass->GetIsThrown() && ballClass->GetHolder() == me)
			Score();

		// if the enemy team has the ball, attack their tank
		if (!ballClass->GetIsThrown() && ballClass->GetIsHeld() && ballClass->GetHolder()->GetTag() != me->GetTag())
			Attack(eTank);

		else
		{
			// if the ball gets close
			if (!ballClass->GetIsHeld() && (ball->GetTransform()->GetWorldPosition() - me->GetTransform()->GetPosition()).magnitude() < 15)
				GetBall();

			// run around enemys side
			else if (!at1 && RunTo(pos1, 1.5f))
				at1 = true;

			else if (at1 && RunTo(pos2, 1.5f))
				at1 = false;

			TurnTo(myGoal);
		}
	}

#pragma endregion

#pragma region Guy
	else if (currState == guy)
	{
		// if i have the ball or one of my teammates have the ball
		if (ballClass->GetIsHeld() && !ballClass->GetIsThrown())
		{
			if (ballClass->GetHolder() == me)
				Score();

			else if (ballClass->GetHolder()->GetTag() == me->GetTag())
				DefendTeammate();

			else
				Attack(ballClass->GetHolder());
		}

		else
			GetBall();
	}
#pragma endregion

#pragma region Tank
	else if (currState == tank)
	{
		// if someone has the ball
		if (ballClass->GetIsHeld() && !ballClass->GetIsThrown())
		{
			// if they're on my team
			if (ballClass->GetHolder()->GetTag() == me->GetTag())
			{
				// if it's me
				if (ballClass->GetHolder() == me)
					Score();

				// if it's my friend
				else
					DefendTeammate();
			}

			// if they're on the enemy team
			else
				Attack(ballClass->GetHolder());
		}

		// if nobody has the ball
		else
		{
			GameObject *myGuy = nullptr;
			
			for (int i = 0; i < listOfMates.size(); ++i)
			{
				if (listOfMates[i]->GetComponent<AI>() && listOfMates[i]->GetComponent<AI>()->GetCurrState() == guy)
					myGuy = listOfMates[i];

				else if (!listOfMates[i]->GetComponent<AI>())
					myGuy = listOfMates[i];
			}

			// hover around guy
			if (myGuy && RunTo(myGuy, 15.0f))
				Idle();
		}
	}
#pragma endregion
	
	if (timer <= 0)
	{
		timer = 2;
		startTimer = false;
	}
}

void AI::Idle()
{
	me->GetTransform()->AddVelocity(float3(0, 0, 0));
}

void AI::GetBall()
{
	float3 dist = ball->GetTransform()->GetPosition() - me->GetTransform()->GetPosition();

	// if someone has the ball
	if (ballClass->GetIsHeld() && !ballClass->GetIsThrown())
	{
		// if they're not on my team
		if (ballClass->GetHolder()->GetTag() != me->GetTag())
			Attack(ballClass->GetHolder());

		else // he's my teammate
			DefendTeammate();
	}

	// if im right next to the ball
	else if (RunTo(ball) && dist.magnitude() < 1)
	{
		// running into the ball should pick it up
	}
}

void AI::DefendTeammate()
{
	// double check that my teammate has the ball
	if (ballClass->GetIsHeld() && !ballClass->GetIsThrown() && ballClass->GetHolder()->GetTag() == me->GetTag())
	{
		// double check that it's not me who has the ball
		if (ballClass->GetHolder() != me)
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
}

void AI::Attack(GameObject *target)
{
	realTarget = target;

	// if they're not on my team and if the timer isn't going
	if (target->GetTag() != me->GetTag() && timer == 2)
	{
		TurnTo(target);
		me->GetTransform()->AddVelocity(((target->GetTransform()->GetWorldPosition() - me->GetTransform()->GetPosition()) * float3(1, 0, 1)).normalize() * AttackSpeed);
	}
}

void AI::Paranoia()
{
	// if i have the ball
	if (ballClass->GetIsHeld() && !ballClass->GetIsThrown() && ballClass->GetHolder() == me)
	{
		float edist = 789; // distance to enemies
		float mdist = 789; // distance to friends
		GameObject *target = nullptr; // the friend i'll throw to

		// for each enemy
		for (int i = 0; i < listOfEnemies.size(); ++i)
		{
			float3 tmp = listOfEnemies[i]->GetTransform()->GetWorldPosition() - me->GetTransform()->GetPosition();

			// if their distance is closer, switch to them
			if (tmp.magnitude() < edist)
				edist = tmp.magnitude();
		}

		// if the enemy is close enough to hit me
		if (edist < 5)
		{
			// for each friend
			for (int i = 0; i < listOfMates.size(); ++i)
			{
				float3 tmp2 = listOfMates[i]->GetTransform()->GetWorldPosition() - enemyGoal->GetTransform()->GetPosition();

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
			TurnTo(target);
			crosse->Throw();
		}
	}
}

bool AI::RunTo(GameObject *target)
{
	if (target)
	{
		if ((target->GetTransform()->GetPosition() - me->GetTransform()->GetPosition()).magnitude() < 5)
			return true;

		float3 v = ((target->GetTransform()->GetWorldPosition() - me->GetTransform()->GetPosition()) * float3(1, 0, 1)).normalize();
		TurnTo(target);
		me->GetTransform()->AddVelocity(v * RunSpeed);
	}

	return false;
}

bool AI::RunTo(GameObject *target, float dist)
{
	if (target)
	{
		if ((target->GetTransform()->GetPosition() - me->GetTransform()->GetPosition()).magnitude() < dist)
			return true;

		float3 v = ((target->GetTransform()->GetWorldPosition() - me->GetTransform()->GetPosition()) * float3(1, 0, 1)).normalize();
		TurnTo(target);
		me->GetTransform()->AddVelocity(v * RunSpeed);
	}

	return false;
}

bool AI::RunTo(float3 target, float dist)
{
	if ((target - me->GetTransform()->GetPosition()).magnitude() < dist)
		return true;

	float3 v = ((target - me->GetTransform()->GetPosition()) * float3(1, 0, 1)).normalize();
	TurnTo(target);
	me->GetTransform()->AddVelocity(v * RunSpeed);

	return false;
}

void AI::TurnTo(float3 target)
{
	//u - forward vector
	float3 u = (me->GetTransform()->GetRightf3() * float3(-1, 0, -1)).normalize(); //////////////////////////////////////////////////////////////////////////////////////////////////////

	//v - vector between me and destination
	float3 v = ((target - me->GetTransform()->GetPosition()) * float3(1, 0, 1)).normalize();

	//degRad - degrees/radians between me and target
	float degRad = dot_product(u, v);
	me->GetTransform()->RotateY(degRad);
}

void AI::TurnTo(GameObject *target)
{
	if (target)
	{
		//u - forward vector
		float3 u = (me->GetTransform()->GetRightf3() * float3(-1, 0, -1)).normalize(); //////////////////////////////////////////////////////////////////////////////////////////////////////

		//v - vector between me and destination
		float3 v = ((target->GetTransform()->GetPosition() - me->GetTransform()->GetPosition()) * float3(1, 0, 1)).normalize();

		//degRad - degrees/radians between me and target
		float degRad = dot_product(u, v);
		me->GetTransform()->RotateY(degRad);
	}
}

void AI::Score()
{
	Paranoia();

	if (RunTo(enemyGoal, 30.0f))
		crosse->Throw();
}

AI::State AI::GetCurrState() { return currState; }

bool AI::GetIsAttacking() { return isAttacking; }

GameObject * AI::GetTarget() { return (realTarget) ? realTarget : nullptr; }