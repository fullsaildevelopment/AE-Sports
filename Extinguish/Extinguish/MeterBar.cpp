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

	//if (isActive)
	//{
	//	if (drain)
	//	{
	//		dTime -= dt;

	//		rect2 = ShrinkRect(dTime, drainTime);

	//		if (dTime <= 0.0f)
	//		{
	//			drain = false;
	//			dTime = drainTime;
	//		}
	//	}
	//	else
	//	{
	//		rTime += dt;
	//		rect2 = ShrinkRect(rTime, rechargeTime);

	//		if (rTime >= rechargeTime)
	//		{
	//			rTime = 0.0f;
	//			drain = true;
	//			isActive = false;
	//		}
	//	}
	//}

}

float MeterBar::GetPercentage()
{
	return percentage;
}

//bool MeterBar::IsEmpty()
//{
//	return (percentage == 0.0f) ? 1 : 0;
//}

//percentage should be from 0.0f - 1.0f
void MeterBar::UpdatePercentage(float newPercentage)
{
	if (newPercentage >= 0.0f)
	{
		percentage = newPercentage;

		rect2 = ShrinkRect(percentage, 1.0f);
	}
}

void MeterBar::HandleEvent(Event* e) 
{
	// check for specific key press (taken from powerup or something)
	// if true
		// isActive = true
}