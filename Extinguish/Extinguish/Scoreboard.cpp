#include "Scoreboard.h"
#include "GameObject.h"
#include "Button.h"
#include "Scene.h"
#include "UIRenderer.h"

Scoreboard::Scoreboard(Scene* scene, DeviceResources* devResources)
{
	this->scene = scene;
	this->devResources = devResources;

	//the teamScores and labels size's never change so I can create them here
	teamScores = new GameObject*[numOfTeams];
	labels = new GameObject*[numOfLabels];

	float positions[] = { 0.24f, 0.30f };
	int labelIndex = 0;

	for (int i = 0; i < numOfTeams; ++i)
	{		
		scene->AddUIObject(labels[labelIndex]);
		labels[labelIndex]->Init("Scoreboard Name" + to_string(i));
		{
			// text
			Button * textbutton = new Button(true, true, L"RED", (unsigned int)strlen("RED"), 100.0f, 50.0f, devResources, 0);
			textbutton->setSceneIndex(sceneID);
			textbutton->SetGameObject(labels[labelIndex]);
			textbutton->showFPS(false);
			textbutton->setPositionMultipliers(0.40f, positions[i]);
			labels[labelIndex]->AddComponent(textbutton);
			UIRenderer * textrender = new UIRenderer();
			textrender->Init(true, 20.0f, devResources, textbutton, L"Consolas", D2D1::ColorF(0.0f, 0.0f, 0.0f, 1.0f));
			labels[labelIndex]->AddComponent(textrender);
			textrender->MakeRTSize();
			textbutton->MakeRect();
			textbutton->setOrigin();
			//button->MakeHandler();
			textrender->InitMetrics();
			textbutton->SetActive(true);
			textbutton->setHelper(scene->GetNumUIObjects());
		}

		++labelIndex;

		// first score
		scene->AddUIObject(labels[labelIndex]);
		labels[labelIndex]->Init("Scoreboard Name" + to_string(i));
		{
			// text
			Button * textbutton = new Button(true, true, L"SCORE", (unsigned int)strlen("SCORE"), 100.0f, 50.0f, devResources, 0);
			textbutton->setSceneIndex(sceneID);
			textbutton->SetGameObject(labels[labelIndex]);
			textbutton->showFPS(false);
			textbutton->setPositionMultipliers(0.50f, positions[i]);
			labels[labelIndex]->AddComponent(textbutton);
			UIRenderer * textrender = new UIRenderer();
			textrender->Init(true, 20.0f, devResources, textbutton, L"Consolas", D2D1::ColorF(0.0f, 0.0f, 0.0f, 1.0f));
			labels[labelIndex]->AddComponent(textrender);
			textrender->MakeRTSize();
			textbutton->MakeRect();
			textbutton->setOrigin();
			//button->MakeHandler();
			textrender->InitMetrics();
			textbutton->SetActive(true);
			textbutton->setHelper(scene->GetNumUIObjects());
		}

		++labelIndex;

		scene->AddUIObject(labels[labelIndex]);
		labels[labelIndex]->Init("Scoreboard Goal" + to_string(i));
		{
			// text
			Button * textbutton = new Button(true, true, L"GOALS", (unsigned int)strlen("GOALS"), 100.0f, 50.0f, devResources, 0);
			textbutton->setSceneIndex(sceneID);
			textbutton->SetGameObject(labels[labelIndex]);
			textbutton->showFPS(false);
			textbutton->setPositionMultipliers(0.59f, positions[i]);
			labels[labelIndex]->AddComponent(textbutton);
			UIRenderer * textrender = new UIRenderer();
			textrender->Init(true, 20.0f, devResources, textbutton, L"Consolas", D2D1::ColorF(0.0f, 0.0f, 0.0f, 1.0f));
			labels[labelIndex]->AddComponent(textrender);
			textrender->MakeRTSize();
			textbutton->MakeRect();
			textbutton->setOrigin();
			//button->MakeHandler();
			textrender->InitMetrics();
			textbutton->SetActive(true);
			textbutton->setHelper(scene->GetNumUIObjects());
		}

		++labelIndex;

		//assists
		scene->AddUIObject(labels[labelIndex]);
		labels[labelIndex]->Init("Scoreboard Assists" + to_string(i));
		{
			// text
			Button * textbutton = new Button(true, true, L"ASSISTS", (unsigned int)strlen("ASSISTS"), 100.0f, 50.0f, devResources, 0);
			textbutton->setSceneIndex(sceneID);
			textbutton->SetGameObject(labels[labelIndex]);
			textbutton->showFPS(false);
			textbutton->setPositionMultipliers(0.70f, positions[i]);
			labels[labelIndex]->AddComponent(textbutton);
			UIRenderer * textrender = new UIRenderer();
			textrender->Init(true, 20.0f, devResources, textbutton, L"Consolas", D2D1::ColorF(0.0f, 0.0f, 0.0f, 1.0f));
			labels[labelIndex]->AddComponent(textrender);
			textrender->MakeRTSize();
			textbutton->MakeRect();
			textbutton->setOrigin();
			//button->MakeHandler();
			textrender->InitMetrics();
			textbutton->SetActive(true);
			textbutton->setHelper(scene->GetNumUIObjects());
		}

		++labelIndex;

		// Saves label
		scene->AddUIObject(labels[labelIndex]);
		labels[labelIndex]->Init("Scoreboard saves" + to_string(i));
		{
			// text
			Button * textbutton = new Button(true, true, L"SAVES", (unsigned int)strlen("SAVES"), 100.0f, 50.0f, devResources, 0);
			textbutton->setSceneIndex(sceneID);
			textbutton->SetGameObject(labels[labelIndex]);
			textbutton->showFPS(false);
			textbutton->setPositionMultipliers(0.80f, positions[i]);
			labels[labelIndex]->AddComponent(textbutton);
			UIRenderer * textrender = new UIRenderer();
			textrender->Init(true, 20.0f, devResources, textbutton, L"Consolas", D2D1::ColorF(0.0f, 0.0f, 0.0f, 1.0f));
			labels[labelIndex]->AddComponent(textrender);
			textrender->MakeRTSize();
			textbutton->MakeRect();
			textbutton->setOrigin();
			//button->MakeHandler();
			textrender->InitMetrics();
			textbutton->SetActive(true);
			textbutton->setHelper(scene->GetNumUIObjects());
		}

		++labelIndex;
	}
}

Scoreboard::~Scoreboard()
{
	int i;

	for (i = 0; i < redPlayerBars.size(); ++i)
	{
		delete redPlayerBars[i];
	}

	for (i = 0; i < bluePlayerBars.size(); ++i)
	{
		delete bluePlayerBars[i];
	}
	
	for (i = 0; i < numOfTeams; ++i)
	{
		delete teamScores[i];
	}

	delete[] teamScores;
	
	for (i = 0; i < numOfLabels; ++i)
	{
		delete labels[i];
	}

	delete[] labels;
}

void Scoreboard::Init(int numRedPlayers, int numBluePlayers)
{
	redPlayerBars.resize(numRedPlayers);
	bluePlayerBars.resize(numBluePlayers);

	int i;
	for (i = 0; i < numRedPlayers; ++i)
	{
		redPlayerBars[i] = new GameObject();

	}
}

void Scoreboard::Update(float dt)
{
	int i;

	for (i = 0; i < redPlayerBars.size(); ++i)
	{
		redPlayerBars[i]->Update(dt);
	}

	for (i = 0; i < bluePlayerBars.size(); ++i)
	{
		bluePlayerBars[i]->Update(dt);
	}

	for (i = 0; i < numOfTeams; ++i)
	{
		teamScores[i]->Update(dt);
	}

	for (int i = 0; i < numOfLabels; ++i)
	{
		labels[i]->Update(dt);
	}
}