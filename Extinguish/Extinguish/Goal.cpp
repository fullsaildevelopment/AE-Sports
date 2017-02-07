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
	for (int i = 0; i < go->size(); ++go)
	{
		if ((*go)[i]->GetName() == "GameBall")
		{
			(*go)[i]->GetTransform()->SetPosition({ 0,10,-25 });
		}
	}
}