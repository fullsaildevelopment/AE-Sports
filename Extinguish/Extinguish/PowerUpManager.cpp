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
		Shield* shieldC = new Shield();
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

	if (ResourceManager::GetSingleton()->IsServer())
	{
		for (int i = 0; i < NUM_OF_POS; ++i)
		{
			roundTimer[i] += _dt;
		}

		const float3 spawnPositions[] = { {-50.0f, 1.0f, 1.8f}, {5.0f, 1.0f, 1.8f}, //middle positions
										  {-20.0f, 1.0f, 35.0f}, {-20.0f, 1.0f, -35.0f} }; //goal positions

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

					//enable
					powerUps[randPowIndex]->GetGameObject()->GetComponent<Renderer>()->SetEnabled(true);
					powerUps[randPowIndex]->SetEnabled(true);
					powerUps[randPowIndex]->GetGameObject()->GetComponent<SphereCollider>()->SetEnabled(true);

					//don't let it be spawned again til consumed
					isSpawned[randPowIndex] = true;

					//prevent pos from being used again
					posUsed[randPosIndex] = true;
					powerUps[randPowIndex]->SetSpawnIndex(randPowIndex);
					powerUps[randPowIndex]->SetPosIndex(randPosIndex);

					cout << powerUps[randPowIndex]->GetGameObject()->GetName() << " spawned" << endl;

					//reset timer so it's every *blank* seconds til spawn for this position
					roundTimer[randPosIndex] = 0.0f;
				}
			}
		}
	}
	//TODO: play a sound to indicate spawning


	// update ui if player has specific powerup
	int uiIndex = 0;
	bool sjump, shield, magnet;
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
			powerUps[i]->GetGameObject()->GetComponent<Renderer>()->SetEnabled(false);
			powerUps[i]->SetEnabled(false);
			powerUps[i]->GetGameObject()->GetComponent<SphereCollider>()->SetEnabled(false);
		}

		return;
	}

	PowerUpEvent* powerEvent = dynamic_cast<PowerUpEvent*>(e);

	if (powerEvent)
	{
		isSpawned[powerEvent->GetSpawnIndex()] = false;
		posUsed[powerEvent->GetPosIndex()] = false;
		roundTimer[powerEvent->GetPosIndex()] = 0.0f;

		cout << powerEvent->GetName() << " picked up" << endl;

		return;
	}
}


void PowerUpManager::Render()
{
	for (unsigned int i = 0; i < powerUpRenderers.size(); ++i)
	{
		powerUpRenderers[i]->Render();
	}
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