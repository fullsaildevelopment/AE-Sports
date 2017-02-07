#include "Goal.h"
#include "GameObject.h"
#include "EventDispatcher.h"
#include "ScoreEvent.h"

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
	std::vector<GameObject*>* go = GetGameObject()->GetGameObjects();
	Transform* bt = GetGameObject()->FindGameObject("GameBall")->GetTransform();
	bt->SetPosition({ 0,10,-20.5f });
	bt->SetVelocity({ 0,0,0 });
}