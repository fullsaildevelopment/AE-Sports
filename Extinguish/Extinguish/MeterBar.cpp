#include "MeterBar.h"





MeterBar::~MeterBar()
{
}


void MeterBar::MakeHandler()
{
	GameObject * object = GetGameObject();
	EventDispatcher::GetSingleton()->RegisterHandler(this, object->GetName());
}


void MeterBar::Update(float dt) 
{

	if (isActive)
	{
		if (drain)
		{
			dTime -= dt;

			rect2 = ShrinkRect(dTime, drainTime);

			if (dTime <= 0.0f)
			{
				drain = false;
				dTime = drainTime;
				rTime = 0.0f;
			}
		}
		else if (canRecharge)
		{
			rTime += dt;

			if (rTime >= rechargeTime)
			{
				drain = true;
				isActive = false;
			}
			else
				rect2 = ShrinkRect(rTime, rechargeTime);
		}
		else
			isActive = false;
	}
	else if (canRecharge)
	{
		if (rTime < rechargeTime)
		{
			rTime += dt;

			if (rTime >= rechargeTime)
			{
				drain = true;
				isActive = false;
			}
			else
				rect2 = ShrinkRect(rTime, rechargeTime);
		}
	}

}

void MeterBar::HandleEvent(Event* e)
{
	InputDownEvent* inputDownEvent = dynamic_cast<InputDownEvent*>(e);

	if (inputDownEvent && Game::currentScene == 2)
	{
		// check for specific key press (taken from powerup or something)
		InputManager * input = inputDownEvent->GetInput();
		if (input->GetKey(16))
		{
			if (isActive == false)
			{
				dTime = rTime * (drainTime / rechargeTime);
			}
			else if (drain == false)
			{
				drain = true;
				dTime = rTime * (drainTime / rechargeTime);
			}
			isActive = true;

		}
		else
		{
			if (drain && isActive == true)
			{
				drain = false;
				rTime = dTime * (rechargeTime / drainTime);

				if (rTime > rechargeTime)
					rTime = rechargeTime;
			}

		//	isActive = false;
		}
	}
}