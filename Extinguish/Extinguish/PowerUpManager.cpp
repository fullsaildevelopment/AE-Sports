#include <iostream>
#include "PowerUpManager.h"
#include "Scene.h"
#include "GameObject.h"
#include "PowerUp.h"
#include "SuperJump.h"
//#include "BoxCollider.h"
#include "Magnet.h"
#include "Shield.h"
#include "vec3.h"
#include "ScoreEvent.h"
#include "PowerUpEvent.h"
#include "EventDispatcher.h"
#include "SphereCollider.h"
#include "Button.h"
#include "UIRenderer.h"
#include "Game.h"

using namespace std;
float tempcooldown = 0.0f;
bool spawned = false;

void PowerUpManager::Init(Scene* scene, XMFLOAT4X4& projection, DeviceResources* devResources)
{
	//register as eventhandler
	EventDispatcher::GetSingleton()->RegisterHandler(this, GetGameObject()->GetName());


	XMFLOAT4X4 identity;
	XMStoreFloat4x4(&identity, DirectX::XMMatrixIdentity());


	//create power up objects
	int i;
	for (i = 0; i < 2; ++i)
	{
		string name = "Super Jump";

		GameObject* superJump = new GameObject();
		superJump->Init(name + to_string(i));
		scene->AddGameObject(superJump);
		superJump->InitTransform(identity, { 5, 1, 5 }, { 0, 0, 0 }, { 1, 1, 1 }, nullptr, nullptr, nullptr);
		Renderer* superJumpRenderer = new Renderer();
		superJump->AddComponent(superJumpRenderer);
		superJumpRenderer->Init("Super Jump", "PowerUp", "PowerUp", "Static", "", "", projection, devResources, false);
		superJumpRenderer->SetEmissiveColor(float4(1, 1, 0, 1));
		SuperJump* superJumpC = new SuperJump();
		superJump->AddComponent(superJumpC);
		SphereCollider* superJumpCollider = new SphereCollider(1.3f, superJump, true);
		superJump->AddSphereCollider(superJumpCollider);

		powerUps.push_back(superJumpC);

		name = "Magnet";
		GameObject* magnet = new GameObject();
		magnet->Init(name + to_string(i));
		scene->AddGameObject(magnet);
		magnet->InitTransform(identity, { 5, 1, 3 }, { 0, 0, 0 }, { 1, 1, 1 }, nullptr, nullptr, nullptr);
		Renderer* magnetRenderer = new Renderer();
		magnet->AddComponent(magnetRenderer);
		magnetRenderer->Init("Magnet", "PowerUp", "PowerUp", "Static", "", "", projection, devResources, false);
		magnetRenderer->SetEmissiveColor(float4(1, 1, 0, 1));
		Magnet* magnetC = new Magnet();
		magnet->AddComponent(magnetC);
		magnetC->Init(magnet);
		SphereCollider* magnetCollider = new SphereCollider(1.3f, magnet, true);
		magnet->AddSphereCollider(magnetCollider);

		powerUps.push_back(magnetC);

		name = "Shield";
		GameObject* shield = new GameObject();
		shield->Init(name + to_string(i));
		scene->AddGameObject(shield);
		shield->InitTransform(identity, { -50, 1, 1.8f }, { 0, 0, 0 }, { 1, 1, 1 }, nullptr, nullptr, nullptr);
		Renderer* shieldRenderer = new Renderer();
		shield->AddComponent(shieldRenderer);
		shieldRenderer->Init("Shield", "PowerUp", "PowerUp", "Static", "", "", projection, devResources, false);
		shieldRenderer->SetEmissiveColor(float4(1, 1, 0, 1));
		Shield* shieldC = new Shield(scene, projection, devResources);
		shield->AddComponent(shieldC);
		SphereCollider* shieldCollider = new SphereCollider(1.3f, shield, true);
		shield->AddSphereCollider(shieldCollider);

		powerUps.push_back(shieldC);
	}

	for (i = 0; i < powerUps.size(); ++i)
	{
		//disable them
		powerUps[i]->GetGameObject()->GetComponent<Renderer>()->SetEnabled(false);
		powerUps[i]->SetEnabled(false);
		powerUps[i]->GetGameObject()->GetComponent<SphereCollider>()->SetEnabled(false);
	}

	//initialize
	for (i = 0; i < NUM_OF_POS; ++i)
	{
		roundTimer[i] = 0.0f;
		posUsed[i] = false;
	}

	for (int i = 0; i < NUM_OF_UPS; ++i)
	{
		isSpawned[i] = false;
	}

	PCWSTR bitmaps[3] = {
	L"../Assets/UI/Icons/SuperJump.png",
	L"../Assets/UI/Icons/Shield.png",
	L"../Assets/UI/Icons/Magnet.png"
	};
#if !DEBUG_GRAPHICS
	for (unsigned int i = 0; i < 3; ++i)
	{
		GameObject * pUI = new GameObject();
		Button * theButton = new Button(true, true, L"", (unsigned int)strlen(""), 50.0f, 50.0f, devResources, 0);
		theButton->SetGameObject(pUI);
		theButton->setTimer(true);
		theButton->showFPS(false);
		theButton->setPositionMultipliers(0.75f + (0.07f * (float)i), 0.90f);
		pUI->AddComponent(theButton);

		UIRenderer * tRender = new UIRenderer();
		tRender->Init(true, 45.0f, devResources, theButton, L"Consolas", D2D1::ColorF(0.8f, 0.8f, 0.8f, 0.0f));
		pUI->AddComponent(tRender);
		tRender->MakeRTSize();
		theButton->MakeRect();
		tRender->InitMetrics();
		theButton->setOrigin();
		tRender->DecodeBitmap(bitmaps[i]);
		tRender->setOpacity(0.0f);

		powerUpUIobjs.push_back(pUI);
		powerUpButtons.push_back(theButton);
		powerUpRenderers.push_back(tRender);
	}
#endif
}

void PowerUpManager::Update(float _dt)
{
#if !DEBUG_GRAPHICS
	// update powerups via server, both
	if (ResourceManager::GetSingleton()->IsServer())
		ServerUpdate(_dt);

	//TODO: play a sound to indicate spawning


	// update ui if player has specific powerup, solo
	if (!ResourceManager::GetSingleton()->IsMultiplayer() || ResourceManager::GetSingleton()->IsServer())
		NormalUpdate(_dt);
	// update ui if player has specific powerup, client
	else
		ClientUpdate(_dt);



	if (spawned)
		tempcooldown -= _dt;
#endif
}



void PowerUpManager::ServerUpdate(float _dt)
{
	std::vector<int> indices;
	std::vector<float3> positions;

	for (int i = 0; i < NUM_OF_POS; ++i)
	{
		roundTimer[i] += _dt;
	}

	const float3 spawnPositions[] = { { -50.0f, 1.0f, 1.8f },{ 5.0f, 1.0f, 1.8f }, //middle positions
	{ -20.0f, 1.0f, 35.0f },{ -20.0f, 1.0f, -35.0f } }; //goal positions

	bool triedSpawn[6] = {}; //used to say whether a powerup has tried to been spawned or not so far
	bool triedPos[4] = {};

	int posCount = 0;

	for (int i = 0; i < NUM_OF_POS; ++i)
	{
		if (posUsed[i])
		{
			++posCount;
		}
	}

	if (posCount < NUM_OF_POS)
	{
		//spawn items
		for (int i = 0; i < NUM_OF_POS - posCount; ++i)
		{
			//figure which powerup
			int randPowIndex;

			do
			{
				randPowIndex = rand() % NUM_OF_UPS;
			} while (isSpawned[randPowIndex]);

			int randPosIndex = 0;

			//figure which position
			do
			{
				randPosIndex = rand() % NUM_OF_POS;
			} while (posUsed[randPosIndex]);

			//make sure this specific position can spawn
			if (roundTimer[randPosIndex] >= TIME_TIL_SPAWN)
			{
				//set position
				powerUps[randPowIndex]->GetGameObject()->GetTransform()->SetPosition(spawnPositions[randPosIndex]);
				// for networking
				positions.push_back(spawnPositions[randPosIndex]);
				indices.push_back(randPosIndex);

				//enable
				powerUps[randPowIndex]->Enable();

				//prevent pos from being used again
				posUsed[randPosIndex] = true;
				powerUps[randPowIndex]->SetSpawnIndex(randPowIndex);
				powerUps[randPowIndex]->SetPosIndex(randPosIndex);

				//cout << powerUps[randPowIndex]->GetGameObject()->GetName() << " spawned" << endl;

				//reset timer so it's every *blank* seconds til spawn for this position
				roundTimer[randPosIndex] = 0.0f;
			}
		}
	}

	if (ResourceManager::GetSingleton()->IsMultiplayer())
	{
		// send information to clients
		// send index & position
		if (positions.size() > 0) 
		{
			for (unsigned int j = 0; j < positions.size(); ++j)
			{
				// if it was spawned, update the information for the client
				Game::server.SetPowerUp(indices[j], positions[j], true);

			}

			Game::server.SendPowerUps();
			spawned = true;

		}

		for (unsigned int i = 0; i < powerUps.size(); ++i)
		{
			float t = powerUps[i]->GetElapsed();
			if (t < 0.0f)
				t = 0.0f;
			Game::server.setPowerUpTime(i, powerUps[i]->GetElapsed());
		}
		Game::server.SendPUTime();
	}
}

void PowerUpManager::ClientUpdate(float _dt)
{
	// grab powerup information from server
	if (Game::client.SpawnedPowerUps())
	{
		int amount = 6;

		for (int i = 0; i < amount; ++i)
		{
			bool active;
			float3 position;

			active = Game::client.getSpawnedPowerUpActive(i);
			// set that powerup active
			if (active)
			{
				position = Game::client.getSpawnedPowerUpPos(i);
				powerUps[i]->GetGameObject()->GetTransform()->SetPosition(position);
				powerUps[i]->Enable();

#if _DEBUG
				printf("SPAWNED: %s at %f x %f y %f z\n", powerUps[i]->GetName().c_str(), position.x, position.y, position.z);
#endif
				spawned = true;
			}
			else
			{
				powerUps[i]->Disable();
				powerUps[i]->GetGameObject()->GetTransform()->SetPosition({ 0,0,0 });
			}
		}
	}

	if (Game::client.RemovedPowerUp())
	{
		int index, id;

		index = Game::client.getRemovedPowerUpIndex();
		id = Game::client.getRemovedPlayerID();

		// set that powerup inactive
		powerUps[index]->Disable();

		// set that powerup's player
		powerUps[index]->SetID(id);

		if (id != Game::GetClientID())
		{
			powerUps[index]->SetID(0);
		}
		else
		{
#if _DEBUG
			printf("CLIENT PICKED UP: %s\n", powerUps[index]->GetName().c_str());
#endif
		}
	}


	// update ui
	int uiIndex = 0;
	bool sjump = false, shield = false, magnet = false;
	for (unsigned int i = 0; i < (unsigned int)NUM_OF_UPS; ++i)
	{
		if (powerUps[i]->GetName() == "SuperJump")
		{
			uiIndex = 0;
		}
		else if (powerUps[i]->GetName() == "Shield")
		{
			uiIndex = 1;
		}
		else if (powerUps[i]->GetName() == "Magnet")
		{
			uiIndex = 2;
		}

		if (powerUps[i]->GetID() == Game::GetClientID())
		{

			float newOpacity = Game::client.powerUpTime(i);
			if (newOpacity < 0.0f)
			{
				newOpacity = 0.0f;
				powerUps[i]->SetID(0);
			}
			powerUpRenderers[uiIndex]->setOpacity(newOpacity);

			if (uiIndex == 0)
				sjump = true;
			else if (uiIndex == 1)
				shield = true;
			else if (uiIndex == 2)
				magnet = true;
		}
		else
		{
			if ((uiIndex == 0 && !sjump) || (uiIndex == 1 && !shield) || (uiIndex == 3 && !magnet))
			{
				powerUpRenderers[uiIndex]->setOpacity(0.0f);
			}
		}
	}
}

void PowerUpManager::NormalUpdate(float _dt)
{
	int uiIndex = 0;
	bool sjump = false, shield = false, magnet = false;
	for (unsigned int i = 0; i < (unsigned int)NUM_OF_UPS; ++i)
	{
		if (powerUps[i]->GetName() == "SuperJump")
		{
			uiIndex = 0;
		}
		else if (powerUps[i]->GetName() == "Shield")
		{
			uiIndex = 1;
		}
		else if (powerUps[i]->GetName() == "Magnet")
		{
			uiIndex = 2;
		}

		if (powerUps[i]->GetPlayer())
		{
			if (powerUps[i]->GetPlayer()->GetPlayerID() == Game::GetClientID())
			{
				float newOpacity = powerUps[i]->GetElapsed();
				if (newOpacity < 0.0f)
					newOpacity = 0.0f;
				powerUpRenderers[uiIndex]->setOpacity(newOpacity);

				if (uiIndex == 0)
					sjump = true;
				else if (uiIndex == 1)
					shield = true;
				else if (uiIndex == 2)
					magnet = true;
			}
		}
		else
		{
			if ((uiIndex == 0 && !sjump) || (uiIndex == 1 && !shield) || (uiIndex == 3 && !magnet))
				powerUpRenderers[uiIndex]->setOpacity(0.0f);
		}
	}

}

//misc//
void PowerUpManager::HandleEvent(Event* e)
{
	ScoreEvent* scoreEvent = dynamic_cast<ScoreEvent*>(e);

	if (scoreEvent)
	{
		int i;

		for (i = 0; i < NUM_OF_POS; ++i)
		{
			roundTimer[i] = 0.0f;
		}

		//disable them
		for (i = 0; i < powerUps.size(); ++i)
		{
			powerUps[i]->Disable();
		}

		return;
	}

	PowerUpEvent* powerEvent = dynamic_cast<PowerUpEvent*>(e);

	if (powerEvent)
	{
		isSpawned[powerEvent->GetSpawnIndex()] = false;
		posUsed[powerEvent->GetPosIndex()] = false;
		roundTimer[powerEvent->GetPosIndex()] = 0.0f;

		//cout << powerEvent->GetName() << " picked up" << endl;

		if (ResourceManager::GetSingleton()->IsMultiplayer())
		{
			std::string n = powerEvent->GetName();
			int index = -1;
			for (unsigned int i = 0; i < powerUps.size(); ++i)
			{
				if (n == powerUps[i]->GetGameObject()->GetName())
				{
					index = i;
					break;
				}
			}
			// sent to client to despawn powerup
			if (index != -1) {
				Game::server.RemovePowerUp(index, powerEvent->GetClientID());
				Game::server.SendRemoved();
			}
		}

		return;
	}
}

void PowerUpManager::Render()
{
	float temp = 0.0f;

	for (unsigned int i = 0; i < powerUpRenderers.size(); ++i)
	{
		// to check positions
		powerUpRenderers[i]->Render();
	}

#if _DEBUG
	for (unsigned int i = 0; i < powerUps.size(); ++i)
	{
		if (powerUps[i]->isEnabled() && tempcooldown <= 0.0f && !ResourceManager::GetSingleton()->IsServer())
		{
			float3 position = powerUps[i]->GetGameObject()->GetTransform()->GetPosition();
			printf("POSITION CHECK: %s at %f x %f y %f z\n", powerUps[i]->GetName().c_str(), position.x, position.y, position.z);
		}
	}
#endif

	if (tempcooldown < 0.0f)
		tempcooldown = 2.0f;
}

void PowerUpManager::UpdateSize(D2D1_SIZE_F rect)
{
	float ratio = 1.0f;
	for (unsigned int i = 0; i < powerUpButtons.size(); ++i)
	{
		powerUpButtons[i]->setRT(rect);
		powerUpButtons[i]->AdjustSize();
		if (ratio == 1.0f)
			ratio = powerUpButtons[i]->GetRatio();
		powerUpButtons[i]->MakeRect();
		powerUpButtons[i]->setOrigin();
		powerUpRenderers[i]->ReInit(ratio);
	}
}