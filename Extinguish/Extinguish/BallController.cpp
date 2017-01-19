#include "BallController.h"

void BallController::Init()
{
//	me->OnCollisionEnter = Collide;
}

void BallController::Update()
{
	if (isHeld)
	{
		// stay with player who has me
	}

	else
	{
		SetHolder(nullptr);
	}
}

void Collide(Collider *obj)
{

}

void BallController::ThrowTo(GameObject *target)
{
	// do stuff

	isHeld = false;
}

bool  BallController::GetIsHeld()
{
	return isHeld;
}

/*GameObject* BallController::GetHolder()
{
	if (isHeld)
	{
		// return holder
	}
}*/

void  BallController::SetIsHeld(bool ans)
{
	isHeld = ans;
}

void BallController::SetHolder(GameObject *person)
{
	if (isHeld)
		holder = person;
}