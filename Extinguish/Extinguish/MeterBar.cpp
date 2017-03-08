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


void MeterBar::Update(float dt)
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
					empty = true;
				}
				else
				{
					empty = false;
				}
			}
			
			if ((drain && isActive) && !Game::client.getMeterDown())
			{
				rTime = dTime * (rechargeTime / drainTime);

				if (rTime > rechargeTime)
					rTime = rechargeTime;
			}

			if (isActive != Game::client.getMeterActive())
				isActive = Game::client.getMeterActive();
			if (drain != Game::client.getMeterDrain())
				drain = Game::client.getMeterDrain();
			if (empty)
				drain = false;

			//dTime = Game::client.getDrainTime();
			//rTime = Game::client.getRechargeTime();
		}
	}

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
		if (input->GetKey(16) && input->GetKey('W'))
		{
			Drain(inputDownEvent);
		}
		else
		{
			Recharge(inputDownEvent);

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
		else
		{
			if (drain && isActive == true)
			{
				drain = false;
				rTime = dTime * (rechargeTime / drainTime);

				if (rTime > rechargeTime)
					rTime = rechargeTime;
			}
		}
	}
}


void MeterBar::Drain(InputDownEvent* inputDownEvent)
{
	if (!ResourceManager::GetSingleton()->IsMultiplayer() || (ResourceManager::GetSingleton()->IsServer() && inputDownEvent->GetID() == Game::GetClientID())) {
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
	else if ((inputDownEvent->GetID() != Game::GetClientID()) && ResourceManager::GetSingleton()->IsServer())
	{
		if (Game::server.getMeterActive(inputDownEvent->GetID() - 1) == false)
		{
		    //	float tdTime = rTime * (drainTime / rechargeTime);
			Game::server.setMeterActive(true, inputDownEvent->GetID() - 1);
			//Game::server.setDrainTime(tdTime, inputDownEvent->GetID());
			Game::server.setMeterDown(true, inputDownEvent->GetID() - 1);
		}
		else if (Game::server.getMeterDrain(inputDownEvent->GetID() - 1) == false)
		{
			//Game::server.setMeterActive(true, inputDownEvent->GetID() - 1);
			Game::server.setMeterDrain(true, inputDownEvent->GetID() - 1);
			Game::server.setMeterDown(true, inputDownEvent->GetID() - 1);
		/*	float tdTime = rTime * (drainTime / rechargeTime);
			Game::server.setDrainTime(tdTime, inputDownEvent->GetID());
			if (tdTime <= 0.0f)
				Game::server.setMeterEmpty(true, inputDownEvent->GetID());
			else
				Game::server.setMeterDrain(false, inputDownEvent->GetID());*/
		}
	}
}

void MeterBar::Recharge(InputDownEvent* inputDownEvent)
{
	if (!ResourceManager::GetSingleton()->IsMultiplayer() || (ResourceManager::GetSingleton()->IsServer() && inputDownEvent->GetID() == Game::GetClientID())) {
		if (drain && isActive)
		{
			drain = false;
			rTime = dTime * (rechargeTime / drainTime);

			if (rTime > rechargeTime)
				rTime = rechargeTime;
		}
	}
	else if ((inputDownEvent->GetID() != Game::GetClientID()) && ResourceManager::GetSingleton()->IsServer())
	{
		if (Game::server.getMeterActive(inputDownEvent->GetID() - 1) && Game::server.getMeterDrain(inputDownEvent->GetID() - 1))
		{
			Game::server.setMeterDrain(false, inputDownEvent->GetID() - 1);
		}
			Game::server.setMeterDown(false, inputDownEvent->GetID() - 1);
	}
}