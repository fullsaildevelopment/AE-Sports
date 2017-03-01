#include "Goal.h"
#include "GameObject.h"
#include "EventDispatcher.h"
#include "ScoreEvent.h"
#include "BallController.h"
#include "FloorController.h"

Goal::Goal(GameObject* g) : Component(g)
{

}

void Goal::OnTriggerEnter(Collider* c)
{
	if (c->GetGameObject()->GetName() == "GameBall")
	{
		if (GetGameObject()->GetName() == "Goal")
			Score(1);
		else
			Score(0);
	}
}

void Goal::Score(int team)
{
	ScoreEvent* se = new ScoreEvent();
	se->SetTeam(team);
	EventDispatcher::GetSingleton()->DispatchTo(se, "Game");
	delete se;
	Transform* bt = GetGameObject()->FindGameObject("GameBall")->GetTransform();
	bt->SetPosition({ -7,15,-20.5f });
	bt->SetVelocity({ 0,0,0 });
	bt->GetGameObject()->GetComponent<BallController>()->SetHolder(nullptr);
	FloorController::Score(team);
}