#include "MeterBar.h"
#include "GamePadEvent.h"
#include "GamePad.h"



MeterBar::~MeterBar()
{
}


void MeterBar::MakeHandler()
{
	GameObject * object = GetGameObject();
	EventDispatcher::GetSingleton()->RegisterHandler(this, object->GetName());
}


void MeterBar::Update(float _dt)
{
	if (ResourceManager::GetSingleton()->IsMultiplayer())
	{
		if (!ResourceManager::GetSingleton()->IsServer())
		{
			if (!isActive && Game::client.getMeterActive() && Game::client.getMeterDown())
			{
				dTime = rTime * (drainTime / rechargeTime);
			}
			else if (!drain && Game::client.getMeterDrain() && Game::client.getMeterDown())
			{
				dTime = rTime * (drainTime / rechargeTime);
				if (dTime <= 0.0f)
				{
					Game::client.sendEmpty(true);
					empty = true;
				}
				/*else
				{
					Game::client.sendEmpty(false);
					empty = false;
				}*/
			}
			
			if ((drain && isActive) && !Game::client.getMeterDown())
			{
				rTime = dTime * (rechargeTime / drainTime);

				if (rTime > rechargeTime)
				{
					rTime = rechargeTime; 
					Game::client.sendEmpty(false);
					empty = false;
				}
			}

			if (isActive != Game::client.getMeterActive())
				isActive = Game::client.getMeterActive();
			if (drain != Game::client.getMeterDrain())
				drain = Game::client.getMeterDrain();
			if (empty)
				drain = false;
		}
	}

	if (isActive)
	{
		if (drain)
		{
			dTime -= _dt;

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
			rTime += _dt;

			if (rTime >= rechargeTime)
			{
				drain = true;
				isActive = false;
				rTime = rechargeTime;
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
			rTime += _dt;

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

	if (!ResourceManager::GetSingleton()->IsPaused()) {
		InputDownEvent* inputDownEvent = dynamic_cast<InputDownEvent*>(e);

		if (inputDownEvent && Game::currentScene == 2)
		{
			// check for specific key press (taken from powerup or something)
			InputManager * input = inputDownEvent->GetInput();
			if (input->GetKey(16) && input->GetKey('W'))
			{
				Drain(inputDownEvent->GetID());
			}
			else
			{
				Recharge(inputDownEvent->GetID());

				//	isActive = false;
			}

			return;
		}

		GamePadEvent* gamePadEvent = dynamic_cast<GamePadEvent*>(e);

		if (gamePadEvent && Game::currentScene == 2)
		{
			// check for specific key press (taken from powerup or something)
			GamePad::State* state = gamePadEvent->GetState();
			GamePad::ButtonStateTracker tracker;

			tracker.Update(*state);

			if ((tracker.leftStick == GamePad::ButtonStateTracker::PRESSED && state->thumbSticks.leftY))
			{
				gamePadSprinting = true;
			}
			else if (tracker.leftStick == GamePad::ButtonStateTracker::RELEASED || !state->thumbSticks.leftY)
			{
				gamePadSprinting = false;
			}

			if (gamePadSprinting)
			{
				Drain(gamePadEvent->GetClientID());
			}
			else
			{
				Recharge(gamePadEvent->GetClientID());
			}
		}
	}
}


void MeterBar::Drain(unsigned int id)
{
	if (!ResourceManager::GetSingleton()->IsMultiplayer() || (ResourceManager::GetSingleton()->IsServer() && id == Game::GetClientID())) {
		if (isActive == false)
		{
			dTime = rTime * (drainTime / rechargeTime);
			isActive = true;
		}
		else if (drain == false)
		{
			drain = true;
			dTime = rTime * (drainTime / rechargeTime);
			if (dTime <= 0.0f)
				empty = true;
			else
				empty = false;
		}
	}
	else if ((id != Game::GetClientID()) && ResourceManager::GetSingleton()->IsServer())
	{
		if (Game::server.getMeterActive(id - 1) == false)
		{
			Game::server.setMeterActive(true, id - 1);
			Game::server.setMeterDown(true, id - 1);
		}
		else if (Game::server.getMeterDrain(id - 1) == false)
		{
			Game::server.setMeterDrain(true, id - 1);
			Game::server.setMeterDown(true, id - 1);
		}
	}
}

void MeterBar::Recharge(unsigned int id)
{
	if (!ResourceManager::GetSingleton()->IsMultiplayer() || (ResourceManager::GetSingleton()->IsServer() && id == Game::GetClientID())) {
		if (drain && isActive)
		{
			drain = false;
			rTime = dTime * (rechargeTime / drainTime);

			if (rTime > rechargeTime)
				rTime = rechargeTime;
		}
	}
	else if ((id != Game::GetClientID()) && ResourceManager::GetSingleton()->IsServer())
	{
		if (Game::server.getMeterActive(id - 1) && Game::server.getMeterDrain(id - 1))
		{
			Game::server.setMeterDrain(false, id - 1);
		}
			Game::server.setMeterDown(false, id - 1);
	}
}

//getters//
float MeterBar::GetPercentage()
{ 
	return dTime / drainTime; 
}

//setters//
void MeterBar::SetDTimeFromPercentage(float percentage)
{ 
	dTime = drainTime * percentage;
	rTime = dTime * (rechargeTime / drainTime);
	SetActive(true);
}
