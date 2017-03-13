#include "Scoreboard.h"
#include "GameObject.h"
#include "Button.h"
#include "Scene.h"
#include "UIRenderer.h"
#include <string>

using namespace std;

Scoreboard::Scoreboard(Scene* scene, DeviceResources* devResources)
{
	this->scene = scene;
	this->devResources = devResources;

	//dynamically allocate all the memory needed
	teamScores = new GameObject*[numOfTeams];
	labels = new GameObject*[numOfLabels];
	scoreboardBackground = new GameObject();

	playerBars.resize(8);
	playerNames.resize(8);
	playerScores.resize(8);
	playerGoals.resize(8);
	playerAssists.resize(8);
	playerSaves.resize(8);

	int i;
	for (i = 0; i < 8; ++i) //pool all objects even if really only two players later on
	{
		playerBars[i] = new GameObject();
		playerNames[i] = new GameObject();
		playerScores[i] = new GameObject();
		playerGoals[i] = new GameObject();
		playerAssists[i] = new GameObject();
		playerSaves[i] = new GameObject();
	}

	for (i = 0; i < numOfTeams; ++i)
	{
		teamScores[i] = new GameObject();
	}

	for (i = 0; i < numOfLabels; ++i)
	{
		labels[i] = new GameObject();
	}

	int teamID = 0;
	//int totalNumPlayers = numRedPlayers + numBluePlayers;
	string teamNames[] = { "Red", "Blue" };
	PCWSTR scoreBarPath[] = { L"../Assets/UI/scorebarRed.png", L"../Assets/UI/scorebarBlue.png" };

	//float heightOfBackground = 100.0f + 40.0f * (numRedPlayers + numBluePlayers) + 80.0f * (numRedPlayers + numBluePlayers) / 8;//total height of 500 max
	//for above line... 100 for height of labels (50 each). 40 for player bar height. 80 for total amount of space between everything

	//create scoreboard background
	scene->AddUIObject(scoreboardBackground);
	scoreboardBackground->Init("Scoreboard Background");
	{
		Button * button = new Button(true, true, L"", (unsigned int)strlen(""), 700.0f, 500.0f, devResources, 0);
		button->setSceneIndex(sceneID);
		button->SetGameObject(scoreboardBackground);
		button->showFPS(false);
		button->setPositionMultipliers(0.50f, 0.45f); // second field as 0.5f if height changes to 500
		scoreboardBackground->AddComponent(button);
		UIRenderer * render = new UIRenderer();
		render->Init(true, 25.0f, devResources, button, L"Brush Script MT", D2D1::ColorF(0.196f, 0.804f, 0.196f, 1.0f));
		render->DecodeBitmap(L"../Assets/UI/scoreback.png");
		scoreboardBackground->AddComponent(render);
		render->MakeRTSize();
		button->MakeRect();
		//button->MakeHandler();
		render->InitMetrics();
		button->SetActive(true);
		button->setHelper(scene->GetNumUIObjects());
	}

	float yOffset = 0;
	//int playerIDOffset = 0, playerID = 0;
	//float newYPos, newYLabelPos;

	//newYPos = yPos - 0.16f * (totalNumPlayers / 8.0f);
	//newYLabelPos = yPosLabel - 0.16f * (totalNumPlayers / 8.0f);

	for (i = 0; i < 8; ++i)
	{
		if (i >= 4)
		{
			teamID = 1;
			yOffset = 0.9f;
		}

		scene->AddUIObject(playerBars[i]);
		playerBars[i]->Init(teamNames[teamID] + " Player Bar" + to_string(i % 4));
		{
			//bar image
			Button * button = new Button(true, true, L"", (unsigned int)strlen(""), 500.0f, 40.0f, devResources, 0);
			button->setSceneIndex(sceneID);
			button->SetGameObject(playerBars[i]);
			button->showFPS(false);
			button->setPositionMultipliers(0.60f, yPos + ySpacing * (i + yOffset));
			playerBars[i]->AddComponent(button);
			UIRenderer * render = new UIRenderer();
			render->Init(true, 25.0f, devResources, button, L"Brush Script MT", D2D1::ColorF(0.196f, 0.804f, 0.196f, 1.0f));
			render->DecodeBitmap(scoreBarPath[teamID]);
			playerBars[i]->AddComponent(render);
			render->MakeRTSize();
			button->MakeRect();
			//button->MakeHandler();
			render->InitMetrics();
			button->SetActive(true);
			button->setHelper(scene->GetNumUIObjects());

		}

		//name
		//playerNames[i] = new GameObject();
		scene->AddUIObject(playerNames[i]);
		playerNames[i]->Init(teamNames[teamID] + " Player Name" + to_string(i % 4));
		{
			// text
			Button * textbutton = new Button(true, true, L"Name", (unsigned int)strlen("Name"), 100.0f, 50.0f, devResources, 0);
			textbutton->setSceneIndex(sceneID);
			textbutton->SetGameObject(playerNames[i]);
			textbutton->showFPS(false);
			textbutton->setPositionMultipliers(0.40f, yPos + ySpacing * (i + yOffset));
			playerNames[i]->AddComponent(textbutton);
			UIRenderer * textrender = new UIRenderer();
			textrender->Init(true, 25.0f, devResources, textbutton, L"Consolas", D2D1::ColorF(0.0f, 0.0f, 0.0f, 1.0f));
			playerNames[i]->AddComponent(textrender);
			textrender->MakeRTSize();
			textbutton->MakeRect();
			textbutton->setOrigin();
			//button->MakeHandler();
			textrender->InitMetrics();
			textbutton->SetActive(true);
			textbutton->setHelper(scene->GetNumUIObjects());
		}

		// first score
		//playerScores[i] = new GameObject();
		scene->AddUIObject(playerScores[i]);
		playerScores[i]->Init(teamNames[teamID] + " Player Score" + to_string(i % 4));
		{
			// text
			Button * textbutton = new Button(true, true, L"0", (unsigned int)strlen("0"), 100.0f, 50.0f, devResources, 0);
			textbutton->setSceneIndex(sceneID);
			textbutton->SetGameObject(playerScores[i]);
			textbutton->showFPS(false);
			textbutton->setPositionMultipliers(0.50f, yPos + ySpacing * (i + yOffset));
			playerScores[i]->AddComponent(textbutton);
			UIRenderer * textrender = new UIRenderer();
			textrender->Init(true, 25.0f, devResources, textbutton, L"Consolas", D2D1::ColorF(0.0f, 0.0f, 0.0f, 1.0f));
			playerScores[i]->AddComponent(textrender);
			textrender->MakeRTSize();
			textbutton->MakeRect();
			textbutton->setOrigin();
			//button->MakeHandler();
			textrender->InitMetrics();
			textbutton->SetActive(true);
			textbutton->setHelper(scene->GetNumUIObjects());
		}

		//player's goals
		//playerGoals[i] = new GameObject();
		scene->AddUIObject(playerGoals[i]);
		playerGoals[i]->Init(teamNames[teamID] + " Player Goals" + to_string(i % 4));
		{
			// text
			Button * textbutton = new Button(true, true, L"0", (unsigned int)strlen("0"), 100.0f, 50.0f, devResources, 0);
			textbutton->setSceneIndex(sceneID);
			textbutton->SetGameObject(playerGoals[i]);
			textbutton->showFPS(false);
			textbutton->setPositionMultipliers(0.59f, yPos + ySpacing * (i + yOffset));
			playerGoals[i]->AddComponent(textbutton);
			UIRenderer * textrender = new UIRenderer();
			textrender->Init(true, 25.0f, devResources, textbutton, L"Consolas", D2D1::ColorF(0.0f, 0.0f, 0.0f, 1.0f));
			playerGoals[i]->AddComponent(textrender);
			textrender->MakeRTSize();
			textbutton->MakeRect();
			textbutton->setOrigin();
			//button->MakeHandler();
			textrender->InitMetrics();
			textbutton->SetActive(true);
			textbutton->setHelper(scene->GetNumUIObjects());
		}

		// first assists
		//playerAssists[i] = new GameObject();
		scene->AddUIObject(playerAssists[i]);
		playerAssists[i]->Init(teamNames[teamID] + " Player Assists" + to_string(i % 4));
		{
			// text
			Button * textbutton = new Button(true, true, L"0", (unsigned int)strlen("0"), 100.0f, 50.0f, devResources, 0);
			textbutton->setSceneIndex(sceneID);
			textbutton->SetGameObject(playerAssists[i]);
			textbutton->showFPS(false);
			textbutton->setPositionMultipliers(0.70f, yPos + ySpacing * (i + yOffset));
			playerAssists[i]->AddComponent(textbutton);
			UIRenderer * textrender = new UIRenderer();
			textrender->Init(true, 25.0f, devResources, textbutton, L"Consolas", D2D1::ColorF(0.0f, 0.0f, 0.0f, 1.0f));
			playerAssists[i]->AddComponent(textrender);
			textrender->MakeRTSize();
			textbutton->MakeRect();
			textbutton->setOrigin();
			//button->MakeHandler();
			textrender->InitMetrics();
			textbutton->SetActive(true);
			textbutton->setHelper(scene->GetNumUIObjects());
		}

		// first saves
		//playerSaves[i] = new GameObject();
		scene->AddUIObject(playerSaves[i]);
		playerSaves[i]->Init(teamNames[teamID] + " Player Saves" + to_string(i % 4));
		{
			// text
			Button * textbutton = new Button(true, true, L"0", (unsigned int)strlen("0"), 100.0f, 50.0f, devResources, 0);
			textbutton->setSceneIndex(sceneID);
			textbutton->SetGameObject(playerSaves[i]);
			textbutton->showFPS(false);
			textbutton->setPositionMultipliers(0.80f, yPos + ySpacing * (i + yOffset));
			playerSaves[i]->AddComponent(textbutton);
			UIRenderer * textrender = new UIRenderer();
			textrender->Init(true, 25.0f, devResources, textbutton, L"Consolas", D2D1::ColorF(0.0f, 0.0f, 0.0f, 1.0f));
			playerSaves[i]->AddComponent(textrender);
			textrender->MakeRTSize();
			textbutton->MakeRect();
			textbutton->setOrigin();
			//button->MakeHandler();
			textrender->InitMetrics();
			textbutton->SetActive(true);
			textbutton->setHelper(scene->GetNumUIObjects());
		}
	}

	wstring wideTeamNames[] = { L"RED", L"BLUE" };
	float labelYPositions[] = { yPosLabel, yPos + ySpacing * 4 }; //second position changes based on where thePlayersBars ends
	int labelIndex = 0;

	for (int i = 0; i < 2; ++i)
	{
		//name label
		scene->AddUIObject(labels[labelIndex]);
		labels[labelIndex]->Init("Scoreboard Name" + to_string(i % 4));
		{
			// text
			Button * textbutton = new Button(true, true, (wchar_t*)wideTeamNames[i].c_str(), wideTeamNames[i].size(), 100.0f, 50.0f, devResources, 0);
			textbutton->setSceneIndex(sceneID);
			textbutton->SetGameObject(labels[labelIndex]);
			textbutton->showFPS(false);
			textbutton->setPositionMultipliers(0.40f, labelYPositions[i]);
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
		labels[labelIndex]->Init("Scoreboard Name" + to_string(i % 4));
		{
			// text
			Button * textbutton = new Button(true, true, L"SCORE", (unsigned int)strlen("SCORE"), 100.0f, 50.0f, devResources, 0);
			textbutton->setSceneIndex(sceneID);
			textbutton->SetGameObject(labels[labelIndex]);
			textbutton->showFPS(false);
			textbutton->setPositionMultipliers(0.50f, labelYPositions[i]);
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
		labels[labelIndex]->Init("Scoreboard Goal" + to_string(i % 4));
		{
			// text
			Button * textbutton = new Button(true, true, L"GOALS", (unsigned int)strlen("GOALS"), 100.0f, 50.0f, devResources, 0);
			textbutton->setSceneIndex(sceneID);
			textbutton->SetGameObject(labels[labelIndex]);
			textbutton->showFPS(false);
			textbutton->setPositionMultipliers(0.59f, labelYPositions[i]);
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
		labels[labelIndex]->Init("Scoreboard Assists" + to_string(i % 4));
		{
			// text
			Button * textbutton = new Button(true, true, L"ASSISTS", (unsigned int)strlen("ASSISTS"), 100.0f, 50.0f, devResources, 0);
			textbutton->setSceneIndex(sceneID);
			textbutton->SetGameObject(labels[labelIndex]);
			textbutton->showFPS(false);
			textbutton->setPositionMultipliers(0.70f, labelYPositions[i]);
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
		labels[labelIndex]->Init("Scoreboard Saves" + to_string(i % 4));
		{
			// text
			Button * textbutton = new Button(true, true, L"SAVES", (unsigned int)strlen("SAVES"), 100.0f, 50.0f, devResources, 0);
			textbutton->setSceneIndex(sceneID);
			textbutton->SetGameObject(labels[labelIndex]);
			textbutton->showFPS(false);
			textbutton->setPositionMultipliers(0.80f, labelYPositions[i]);
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

	for (int i = 0; i < playerBars.size(); ++i)
	{
		delete playerBars[i];
		delete playerNames[i];
		delete playerScores[i];
		delete playerGoals[i];
		delete playerAssists[i];
		delete playerSaves[i];
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
	int i, teamID = 0;
	int totalNumPlayers = numRedPlayers + numBluePlayers;
	string teamNames[] = { "Red", "Blue" };
	PCWSTR scoreBarPath[] = { L"../Assets/UI/scorebarRed.png", L"../Assets/UI/scorebarBlue.png" };

	float heightOfBackground = 100.0f + 40.0f * (numRedPlayers + numBluePlayers) + 80.0f * (numRedPlayers + numBluePlayers) / 8;//total height of 500 max
	//for above line... 100 for height of labels (50 each). 40 for player bar height. 80 for total amount of space between everything

	//create scoreboard background
	Button* button = scoreboardBackground->GetComponent<Button>();
	button->setHeight(heightOfBackground);

	float yOffset = 0;
	float newYPos, newYLabelPos;

	newYPos = yPos - 0.16f * (totalNumPlayers / 8.0f);
	newYLabelPos = yPosLabel - 0.16f * (totalNumPlayers / 8.0f);

	int playerIDOffsest = 0;

	for (i = 0; i < totalNumPlayers; ++i)
	{
		if (i >= numRedPlayers)
		{
			teamID = 1;
			yOffset = 0.9f;
			playerIDOffsest = 4 - numRedPlayers;
		}

		//player's bar
		Button* button = playerBars[i + playerIDOffsest]->GetComponent<Button>();
		button->setPositionMultipliers(0.60f, newYPos + ySpacing * (i + yOffset));

		// player's name
		Button* textbutton = playerNames[i + playerIDOffsest]->GetComponent<Button>();
		textbutton->setPositionMultipliers(0.40f, newYPos + ySpacing * (i + yOffset));

		// player's score
		Button* textbutton = playerScores[i + playerIDOffsest]->GetComponent<Button>();
		textbutton->setPositionMultipliers(0.50f, newYPos + ySpacing * (i + yOffset));

		//player's goals
		Button* textbutton = playerGoals[i + playerIDOffsest]->GetComponent<Button>();
		textbutton->setPositionMultipliers(0.59f, newYPos + ySpacing * (i + yOffset));

		// player's assists
		Button* textbutton = playerAssists[i + playerIDOffsest]->GetComponent<Button>();
		textbutton->setPositionMultipliers(0.70f, newYPos + ySpacing * (i + yOffset));

		// player's saves
		Button* textbutton = playerSaves[i + playerIDOffsest]->GetComponent<Button>();
		textbutton->setPositionMultipliers(0.80f, newYPos + ySpacing * (i + yOffset));
	}

	wstring wideTeamNames[] = { L"RED", L"BLUE" };
	float labelYPositions[] = { newYLabelPos, newYPos + ySpacing * numRedPlayers }; //second position changes based on where thePlayersBars ends
	int labelIndex = 0;

	for (int i = 0; i < 2; ++i)
	{
		//name label
		Button* textbutton = labels[labelIndex]->GetComponent<Button>();
		textbutton->setPositionMultipliers(0.40f, labelYPositions[i]);

		++labelIndex;

		//score label
		Button* textbutton = labels[labelIndex]->GetComponent<Button>();
		textbutton->setPositionMultipliers(0.50f, labelYPositions[i]);

		++labelIndex;

		//goal label
		Button* textbutton = labels[labelIndex]->GetComponent<Button>();
		textbutton->setPositionMultipliers(0.59f, labelYPositions[i]);
		++labelIndex;

		//assists
		Button* textbutton = labels[labelIndex]->GetComponent<Button>();
		textbutton->setPositionMultipliers(0.70f, labelYPositions[i]);
		++labelIndex;

		// Saves label
		Button* textbutton = labels[labelIndex]->GetComponent<Button>();
		textbutton->setPositionMultipliers(0.80f, labelYPositions[i]);

		++labelIndex;
	}

	//disable every other player button that isn't being shown
	for (int i = 0; i < 8; ++i)
	{
		if (i >= numRedPlayers && i < 4)
		{

		}
		else if (i > numBluePlayers + 3)
		{

		}

		//player's bar
		Button* button = playerBars[i]->GetComponent<Button>();
		button->SetActive(false);

		// player's name
		Button* textbutton = playerNames[i]->GetComponent<Button>();
		textbutton->SetActive(false);

		// player's score
		Button* textbutton = playerScores[i]->GetComponent<Button>();
		textbutton->SetActive(false);

		//player's goals
		Button* textbutton = playerGoals[i]->GetComponent<Button>();
		textbutton->SetActive(false);

		// player's assists
		Button* textbutton = playerAssists[i]->GetComponent<Button>();
		textbutton->SetActive(false);

		// player's saves
		Button* textbutton = playerSaves[i]->GetComponent<Button>();
		textbutton->SetActive(false);
	}

	Toggle(false);
}

void Scoreboard::Update(float dt)
{
	int i;

	for (i = 0; i < playerBars.size(); ++i)
	{
		playerBars[i]->Update(dt);
		playerNames[i]->Update(dt);
		playerScores[i]->Update(dt);
		playerGoals[i]->Update(dt);
		playerAssists[i]->Update(dt);
		playerSaves[i]->Update(dt);
	}

	for (i = 0; i < numOfTeams; ++i)
	{
		teamScores[i]->Update(dt);
	}

	for (int i = 0; i < numOfLabels; ++i)
	{
		labels[i]->Update(dt);
	}

	//then render
	for (i = 0; i < playerBars.size(); ++i)
	{
		playerBars[i]->GetComponent<UIRenderer>();
		playerNames[i]->GetComponent<UIRenderer>();
		playerScores[i]->GetComponent<UIRenderer>();
		playerGoals[i]->GetComponent<UIRenderer>();
		playerAssists[i]->GetComponent<UIRenderer>();
		playerSaves[i]->GetComponent<UIRenderer>();
	}

	for (i = 0; i < numOfTeams; ++i)
	{
		teamScores[i]->GetComponent<UIRenderer>();
	}

	for (int i = 0; i < numOfLabels; ++i)
	{
		labels[i]->GetComponent<UIRenderer>();
	}
}

void Scoreboard::Toggle(bool toggle)
{
	Button * sbb = scoreboardBackground->GetComponent<Button>();
	sbb->SetActive(toggle);

	for (unsigned int i = 0; i < 8; ++i)
	{
		Button* temp1 = playerBars[i]->GetComponent<Button>();
		temp1->SetActive(toggle);

		Button* temp2 = playerNames[i]->GetComponent<Button>();
		temp2->SetActive(toggle);

		Button* temp3 = playerScores[i]->GetComponent<Button>();
		temp3->SetActive(toggle);

		Button* temp4 = playerGoals[i]->GetComponent<Button>();
		temp4->SetActive(toggle);

		Button* temp5 = playerAssists[i]->GetComponent<Button>();
		temp5->SetActive(toggle);

		Button* temp6 = playerSaves[i]->GetComponent<Button>();
		temp6->SetActive(toggle);
		
	}

	/*for (unsigned int i = 0; i < (unsigned int)numOfTeams; ++i)
	{
		Button* temp = teamScores[i]->GetComponent<Button>();
		temp->SetActive(toggle);
	}*/

	for (unsigned int j = 0; j < (unsigned int)numOfLabels; ++j)
	{
		Button* temp = labels[j]->GetComponent<Button>();
		temp->SetActive(toggle);
	}
}