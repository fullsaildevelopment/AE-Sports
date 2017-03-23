#include "Goal.h"
#include "GameObject.h"
#include "EventDispatcher.h"
#include "ScoreEvent.h"
#include "BallController.h"
#include "FloorController.h"
#include "PlayerController.h"
#include "Game.h"

Goal::Goal(GameObject* g) : Component(g)
{
	ballController = GetGameObject()->FindGameObject("GameBall")->GetComponent<BallController>();
}

void Goal::OnTriggerEnter(Collider* c)
{
	if (c->GetGameObject()->GetName() == "GameBall")
	{
		if (GetGameObject()->GetName() == "RedGoal")
			Score(1);
		else
			Score(0);
	}
}

void Goal::Score(int team)
{
	//give the player who threw the ball a goal
	if (ballController->GetThrower())
	{
		//GetThrower returns crosse object. GetParent returns camera transform. GetParent again returns the mage transform
		PlayerController* throwerPC = ballController->GetThrower()->GetTransform()->GetParent()->GetParent()->GetGameObject()->GetComponent<PlayerController>();

		//only give person goal if it was your team
		if ((throwerPC->GetTeamID() == Game::PLAYER_TEAM::TEAM_A && !team) || (throwerPC->GetTeamID() == Game::PLAYER_TEAM::TEAM_B && team))
		{
			throwerPC->AddGoal();

			//check if the player was assisted
			if (ballController->GetPreviousThrower() != ballController->GetThrower() && ballController->GetPreviousThrower()) //can't assist yourself and mustn't be nullptr
			{
				//make sure the time isn't greater than the threshold
				if (ballController->GetTimeSincePreviouslyThrown() <= MAX_SECONDS_FOR_ASSIST)
				{
					PlayerController* prevThrowerPC = ballController->GetPreviousThrower()->GetTransform()->GetParent()->GetParent()->GetGameObject()->GetComponent<PlayerController>();

					//make sure thrower and prevthrower are on same team
					if (throwerPC->GetTeamID() == prevThrowerPC->GetTeamID())
					{
						prevThrowerPC->AddAssist();
					}
				}
			}
		}
	}

	ScoreEvent* se = new ScoreEvent();
	se->SetTeam(team);
	se->SetPlayerName(ballController->GetThrower()->GetTransform()->GetParent()->GetParent()->GetGameObject()->GetComponent<PlayerController>()->GetName());
	EventDispatcher::GetSingleton()->DispatchTo(se, "Game");
	delete se;
	//Transform* bt = GetGameObject()->FindGameObject("GameBall")->GetTransform();
	//bt->SetPosition({ -7,15,-20.5f });
	//bt->SetVelocity({ 0,0,0 });
	//bt->GetGameObject()->GetComponent<BallController>()->SetHolder(nullptr);
	FloorController::Score(team);
}