#include "BallController.h"

void Ball::Init()
{

}

void Ball::Update()
{
	if (isHeld)
	{
		// stay with player who has me
	}

	

	//else
	//{
	//	// do velocity stuff?
	//}
}

void Ball::OnCollisionEnter(Collider *obj)
{
	
}

void Ball::ThrowTo(GameObject *target)
{
	// do stuff

	isHeld = false;
}

bool  Ball::GetIsHeld()
{
	return isHeld;
}

void  Ball::SetIsHeld(bool ans)
{
	isHeld = ans;
}