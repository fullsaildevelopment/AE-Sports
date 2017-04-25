#include "AI.h"
#include "GameObject.h"
#include "ResourceManager.h"
#include "AnimatorController.h"
#include "EventDispatcher.h"
#include "CanPlayEvent.h"
#include "State.h"
#include "Movement.h"
#include "Trigger.h"
#include "Map.h"
#include "SoundEvent.h"
#include "SoundEngine.h"

#define     RunSpeed 10 //0.7f //0.475f //10

Map *AI::aiPath = nullptr;

/////////////////////////////////////////////////////////////////////////////////////////
// 
// players/AI will be tagged as Team1 or Team2
// 
/////////////////////////////////////////////////////////////////////////////////////////

AI::AI(GameObject* obj) : Component(obj)
{
	me = obj;

	//register as eventhandler
	EventDispatcher::GetSingleton()->RegisterHandler(this, GetGameObject()->GetName() + "AI");
}

void AI::OnCollisionEnter(Collider *obj)
{
	if (!ResourceManager::GetSingleton()->IsPaused())
	{
		if (obj->GetColliderType() == Collider::ColliderType::CTCapsule)
		{
			CapsuleCollider *col = (CapsuleCollider*)obj;

			if (obj->GetGameObject() == realTarget && !obj->GetGameObject()->GetComponent<AnimatorController>()->GetTrigger("Stumble")->GetTrigger())
			{
				if (!realTarget->GetComponent<PlayerController>()->IsInvincible() && realTarget->GetComponent<Movement>()->CanMove())
				{
					startTimer = true;

					// dropping the ball
					if (!ballClass->GetIsThrown() && ballClass->GetHolder() == realTarget)
						ballClass->DropBall(realTarget);

					// disabling movement
					if (realTarget->GetComponent<AI>())
						realTarget->GetComponent<AI>()->SetCanMove(false);

					else
					{
						realTarget->GetComponent<Movement>()->SetCanMove(false);

						//move the player's camera to match stumble
						Transform* otherCamera = realTarget->GetTransform()->GetChild(0);
						float3 translation = otherCamera->GetForwardf3();
						translation.x = -translation.x;
						translation.y = -3.0f;
						translation.z = -translation.z * 3.0f;
						otherCamera->MoveTo(otherCamera->GetPosition() + translation, 0.75f);
					}

					// triggering the animation
					realTarget->GetComponent<AnimatorController>()->SetTrigger("Stumble");
					ogTarget = realTarget;
					realTarget = nullptr;

					//play attack sound effect
					SoundEvent* soundEvent = new SoundEvent();
					soundEvent->Init(AK::EVENTS::PLAY_ATTACK, GetGameObject()->FindIndexOfGameObject(GetGameObject()));
					EventDispatcher::GetSingleton()->DispatchTo(soundEvent, "Game");
					delete soundEvent;
				}
			}
		}
	}
}

void AI::HandleEvent(Event* e)
{
	CanPlayEvent* playEvent = dynamic_cast<CanPlayEvent*>(e);

	if (playEvent)
	{
		canMove = playEvent->CanPlay();
		return;
	}
}

void AI::Init(GameObject *goal1, GameObject *goal2)
{
	listOfEnemies.reserve(4);
	listOfMates.reserve(3);
	AIbuddies.reserve(3);

	// grabbing all of the game objects
	std::vector<GameObject*> tmp = *me->GetGameObjects();

#pragma region Objects
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
#pragma endregion

#pragma region Switch
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
#pragma endregion

	ballClass = ball->GetComponent<BallController>();
	anim = me->GetComponent<AnimatorController>();

	myGoalNode = aiPath->FindClosest(myGoal->GetTransform()->GetPosition());
	enemyGoalNode = aiPath->FindClosest(enemyGoal->GetTransform()->GetPosition());

	Idle();
}

void AI::Update(float _dt)
{
	if (!ResourceManager::GetSingleton()->IsPaused())
	{

#pragma region Setting Objects
		if (ogTarget)
		{
			AnimatorController* animator = ogTarget->GetComponent<AnimatorController>();
			Param::Trigger *p = animator->GetTrigger("Stumble");
			bool a = animator->GetTrigger("Stumble")->GetTrigger();

			// if your current anim isn't stumble AND ???the the trigger isn't stumble???
			if (animator->GetState(animator->GetCurrentStateIndex())->GetName() != "Stumble" && !animator->GetTrigger("Stumble")->GetTrigger())
			{
				bool setCanMove = false;

				// if you have an anim queued
				if (animator->GetState(animator->GetNextStateIndex()))
				{
					// and it's not stumble
					if (animator->GetState(animator->GetNextStateIndex())->GetName() != "Stumble")
						setCanMove = true;
				}

				else //no animation queued at all
					setCanMove = true;

				if (setCanMove)
				{
					if (ogTarget->GetComponent<AI>())
						ogTarget->GetComponent<AI>()->SetCanMove(true);

					else
					{
						Movement* otherMovement = ogTarget->GetComponent<Movement>();
						otherMovement->SetCanMove(true);

						//move the player's camera to match getting up
						Transform* otherCamera = ogTarget->GetTransform()->GetChild(0);
						float3 translation = otherCamera->GetForwardf3();
						translation.x = translation.x;
						translation.y = 3.0f;
						translation.z = translation.z * 3.0f;

						otherCamera->MoveTo(otherCamera->GetPosition() + translation, 0.75f);
					}

					ogTarget = nullptr;
				}
			}
		}

		if (anim->GetState(anim->GetCurrentStateIndex())->GetName() != "Idle" && !anim->GetState(anim->GetNextStateIndex())) anim->SetTrigger("Idle");
		if (!canMove) Idle();
		if (!isAttacking) realTarget = nullptr;
		if (startTimer) timer -= _dt;
		if (!crosse) crosse = me->GetTransform()->GetChild(0)->GetChild(0)->GetGameObject()->GetComponent<Crosse>();
		if (!camera) camera = me->GetTransform()->GetChild(0)->GetGameObject()->GetTransform();

		if (!eTank || !mGuy || !mGo2)
		{
			for (int i = 0; i < listOfEnemies.size(); ++i)
			{
				if (listOfEnemies[i]->GetComponent<AI>() && listOfEnemies[i]->GetComponent<AI>()->GetCurrState() == tank)
					eTank = listOfEnemies[i];
			}

			for (int i = 0; i < listOfMates.size(); ++i)
			{
				if (listOfMates[i]->GetComponent<AI>() && listOfMates[i]->GetComponent<AI>()->GetCurrState() == playboy)
					mGo2 = listOfMates[i];

				else if (listOfMates[i]->GetComponent<AI>() && listOfMates[i]->GetComponent<AI>()->GetCurrState() == guy)
					mGuy = listOfMates[i];

				else if (!mGuy && !listOfMates[i]->GetComponent<AI>())
					mGuy = listOfMates[i];
			}
		}

#pragma endregion

		if (ballClass->GetIsHeld() && !ballClass->GetIsThrown() && ballClass->GetHolder() == me)
		{
			// if it's me
			if (currState != goalie)
				Score();

			else
			{
				camera->RotateX(-0.4f);
				crosse->Throw();
				camera->RotateX(0.4f);
			}
		}
		
#pragma region Goalie
		if (currState == goalie)
		{
			float3 dist = ball->GetTransform()->GetWorldPosition() - myGoal->GetTransform()->GetPosition();

			// if the ball gets close
			if (dist.magnitude() < 34)
			{
				// if no one is holding it or the enemies have it
				if (!ballClass->GetIsThrown() && (!ballClass->GetIsHeld() || ballClass->GetHolder()->GetTag() != me->GetTag()))
					GetBall();
			}

			// if the ball is too far from the goal
			else if (dist.magnitude() > 34)
			{
				float3 jik = myGoal->GetTransform()->GetWorldPosition() - me->GetTransform()->GetPosition();
				float diist = dot_product(jik, jik);
				
				if (RunTo(myGoal) || diist <= 370.0f)
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

			// if the enemy has the ball
			if (ballClass->GetIsHeld() && !ballClass->GetIsThrown() && ballClass->GetHolder()->GetTag() != me->GetTag())
			{
				if (eTank) Attack(eTank);
			}

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
			}
		}

#pragma endregion

#pragma region Guy || Tank
		else if (currState == guy || currState == tank)
		{
			// if i have the ball or one of my teammates have the ball
			if (ballClass->GetIsHeld() && !ballClass->GetIsThrown())
			{
				// if it's my teammate
				if (ballClass->GetHolder()->GetTag() == me->GetTag())
					DefendTeammate();

				// if it's enemy
				else Attack(ballClass->GetHolder());
			}

			else
			{
				if (currState == guy)
					GetBall();

				else
				{
					if (mGuy)
					{
						//hover around Guy
						float3 jik = mGuy->GetTransform()->GetWorldPosition() - me->GetTransform()->GetPosition();
						float dist = dot_product(jik, jik);

						if (RunTo(mGuy) || dist <= 300.0f)
							Idle();
					}
				}
			}
		}
#pragma endregion

		if (timer <= 0)
		{
			timer = 5.0f;
			startTimer = false;
		}
	}
}

void AI::FixedUpdate(float dt)
{
	if (pathTarget && pathTarget != myGoal && pathTarget != enemyGoal)
	{
		float3 h = float3(0, 10, 0);
		float3 jik = pathTarget->GetTransform()->GetWorldPosition() - (*tarNode->pos + h);
		float dist = dot_product(jik, jik);

		if (dist > 4.0f)
			validPath = false;
	}
}

void AI::Idle()
{
	me->GetTransform()->AddVelocity(float3(0, 0, 0));
}

void AI::GetBall()
{
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
	else
	{
		pathTarget = ball;
		
		// if it's not a valid path
		if (!validPath)
		{
			myNode = aiPath->FindClosest(me->GetTransform()->GetWorldPosition());
			tarNode = aiPath->FindBallNode(ball->GetTransform()->GetWorldPosition());
			path = aiPath->CreatePath(myNode, tarNode);
			pathIndex = path.size() - 2;
			validPath = true;
		}

		// if path actually has something
		if (path.size() > 1)
		{
			float3 h = float3(0, 10, 0);
			float3 dist = (*path[pathIndex]->pos + h) - me->GetTransform()->GetWorldPosition();
			float ballNodeToMe = dot_product(dist, dist);
			float3 v = dist.normalize();
			v.y = 0;

			if (path[pathIndex] == tarNode)
			{
				validPath = false;
				return;
			}

			if (ballNodeToMe < 1.25f)
			{
				--pathIndex;
				TurnTo(*path[pathIndex]->pos);
			}

			if (anim->GetState(anim->GetCurrentStateIndex())->GetName() != "Run" && !anim->GetState(anim->GetNextStateIndex()))
				anim->SetTrigger("Run");

			me->GetTransform()->AddVelocity(v * RunSpeed * moveSpeedMultiplier);
		}
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
	if (canMove)
	{
		// if they're not on my team and if the timer isn't going
		if (target->GetTag() != me->GetTag() && timer == 5.0f)
		{
			realTarget = target;
			isAttacking = true;
			RunTo(target);
			isAttacking = false;
		}
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

				if (mGo2) target = mGo2;

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
			camera->RotateX(-0.4f);
			crosse->Throw();
			camera->RotateX(0.4f);
		}
	}
}

bool AI::RunTo(GameObject *target)
{
	if (canMove)
	{
		if (target)
		{
			/*if ((target->GetTransform()->GetPosition() - me->GetTransform()->GetPosition()).magnitude() < dist)
				return true;

			float3 v = ((target->GetTransform()->GetWorldPosition() - me->GetTransform()->GetPosition())).normalize();
			v.y = 0;
			TurnTo(target);

			if (anim->GetState(anim->GetCurrentStateIndex())->GetName() != "Run" && !anim->GetState(anim->GetNextStateIndex()))
				anim->SetTrigger("Run");

			me->GetTransform()->AddVelocity(v * RunSpeed * moveSpeedMultiplier);*/
			pathTarget = target;

			// if it's not a valid path
			if (!validPath)
			{
				myNode = aiPath->FindClosest(me->GetTransform()->GetWorldPosition());

				if (pathTarget == myGoal) tarNode = myGoalNode;
				else if (pathTarget == enemyGoal) tarNode = enemyGoalNode;
				else tarNode = aiPath->FindClosest(target->GetTransform()->GetWorldPosition());

				path = aiPath->CreatePath(myNode, tarNode);
				pathIndex = path.size() - 2;
				validPath = true;
			}
			
			// if path actually has something
			if (path.size() > 1)
			{
				float3 h = float3(0, 10, 0);
				float3 dist = (*path[pathIndex]->pos + h) - me->GetTransform()->GetWorldPosition();
				float nodeToMe = dot_product(dist, dist);
				float3 v = dist.normalize();
				v.y = 0;

				if (path[pathIndex] == tarNode) return true;
				if (nodeToMe < 1.25f) // if im on the node
				{
					--pathIndex;
					TurnTo(*path[pathIndex]->pos);
				}

				if (anim->GetState(anim->GetCurrentStateIndex())->GetName() != "Run" && !anim->GetState(anim->GetNextStateIndex()))
					anim->SetTrigger("Run");

				me->GetTransform()->AddVelocity(v * RunSpeed * moveSpeedMultiplier);
			}
		}
	}

	return false;

}

bool AI::RunTo(float3 target, float dist)
{
	if (canMove)
	{
		if ((target - me->GetTransform()->GetPosition()).magnitude() < dist)
			return true;

		float3 v = ((target - me->GetTransform()->GetPosition())).normalize();
		v.y = 0;
		TurnTo(target);

		if (anim->GetState(anim->GetCurrentStateIndex())->GetName() != "Run" && !anim->GetState(anim->GetNextStateIndex()))
			anim->SetTrigger("Run");

		me->GetTransform()->AddVelocity(v * RunSpeed * moveSpeedMultiplier);
	}

	return false;
}

void AI::TurnTo(float3 target)
{
	//u - forward vector
	float3 u = (me->GetTransform()->GetRightf3() * (-1)).normalize(); //////////////////////////////////////////////////////////////////////////////////////////////////////

	//v - vector between me and destination
	float3 v = ((target - me->GetTransform()->GetPosition())).normalize();
	u.y = 0;
	v.y = 0;

	//degRad - degrees/radians between me and target
	float degRad = dot_product(u, v);
	me->GetTransform()->RotateY(degRad);
}

void AI::TurnTo(GameObject *target)
{
	if (target)
	{
		float3 u = (me->GetTransform()->GetRightf3() * (-1)).normalize(); //////////////////////////////////////////////////////////////////////////////////////////////////////
		float3 v = ((target->GetTransform()->GetPosition() - me->GetTransform()->GetPosition())).normalize();
		u.y = 0;
		v.y = 0;

		float degRad = dot_product(u, v);
		me->GetTransform()->RotateY(degRad);
	}
}

void AI::Score()
{
	float3 jik = enemyGoal->GetTransform()->GetWorldPosition() - me->GetTransform()->GetPosition();
	float dist = dot_product(jik, jik);

	if (dist <= 650.0f || RunTo(enemyGoal)) // 28.0f
	{
		TurnTo(enemyGoal);
		camera->RotateX(-0.98f);
		me->GetTransform()->AddVelocity(float3(0, 8, 0));
		crosse->Throw();
		camera->RotateX(0.98f);
	}

	Paranoia();
}

AI::State AI::GetCurrState() { return currState; }

GameObject * AI::GetTarget() { return (realTarget) ? realTarget : nullptr; }

bool AI::GetIsAttacking() { return isAttacking; }

float AI::GetMoveSpeedMultiplier() { return moveSpeedMultiplier; }

bool AI::GetCanMove() { return canMove; }

void AI::SetCanMove(bool ans) { canMove = ans; }

void AI::SetMoveSpeedMultiplier(float multiplier) { moveSpeedMultiplier = multiplier; }
