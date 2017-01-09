#pragma once
#include "Component.h"

class AI : Component
{
private:
	enum State
	{
		idle,
		getBall,
		attack,
		score,
	};
	

	State currState;



public:
	void Update();
	void UpdateState(State newState);
	void Idle();
	void GetBall();
	void Attack();
	void Score();
};