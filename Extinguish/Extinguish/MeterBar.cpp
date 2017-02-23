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
			}
		}
		else
		{
			rTime += dt;
			rect2 = ShrinkRect(rTime, rechargeTime);

			if (rTime >= rechargeTime)
			{
				rTime = 0.0f;
				drain = true;
				isActive = false;
			}
		}
	}
}

void MeterBar::HandleEvent(Event* e) 
{
	// check for specific key press (taken from powerup or something)
	// if true
		// isActive = true
}