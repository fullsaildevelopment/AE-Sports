#include "Button.h"
#include "Game.h"
#include "Server Wrapper.h"
#include "Client Wrapper.h"
#include "EventDispatcher.h"
#include "LoadSceneEvent.h"

void StartGame()
{
	LoadSceneEvent* event = new LoadSceneEvent();
	event->Init("FirstLevel");
	EventDispatcher::GetSingleton()->DispatchTo(event, "Game");
	delete event;
}

void StartServer()
{
	if (Game::server.init("127.0.0.1", 60000) == 1)
	{
		ResourceManager::GetSingleton()->SetMultiplayer(true);
		ResourceManager::GetSingleton()->SetServer(true);

		Game::client.init("127.0.0.1", 60001);

		LoadSceneEvent* event = new LoadSceneEvent();
		event->Init("Lobby");
		EventDispatcher::GetSingleton()->DispatchTo(event, "Game");
		delete event;
	}
}

void JoinServer()
{
	ResourceManager::GetSingleton()->SetMultiplayer(true);
	ResourceManager::GetSingleton()->SetServer(false);
	Game::client.init("127.0.0.1", 60001);

	LoadSceneEvent* event = new LoadSceneEvent();
	event->Init("Lobby");
	EventDispatcher::GetSingleton()->DispatchTo(event, "Game");
	delete event;
}

void ShutdownGame()
{
	// shutdown game
}

void ReturnToMenu()
{
	if (ResourceManager::GetSingleton()->IsServer())
	{
		//Game::server.closeServer();
	}
	else
	{
		//Game::client.leaveLobby();
	}

	LoadSceneEvent* event = new LoadSceneEvent();
	event->Init("Menu");
	EventDispatcher::GetSingleton()->DispatchTo(event, "Game");
	delete event;
}

Button::Button(bool active, bool clickable, wchar_t * newText, unsigned int length, float _width, float _height,
	DeviceResources * resources, unsigned int type) {
	isActive = active;
	isClickable = clickable;
	text = newText;
	textLength = length;
	height = _height;
	width = _width;
	//eventFunction = func;
	buttonType = (BUTTON_TYPE)type;

	HRESULT hr = resources->GetWriteFactory()->CreateTypography(pTypography.GetAddressOf());
	if (SUCCEEDED(hr))
	{
		DWRITE_FONT_FEATURE fontFeature = { DWRITE_FONT_FEATURE_TAG_STYLISTIC_SET_7,
			1 };
		hr = pTypography->AddFontFeature(fontFeature);
	}

	setButtonType();
}

void Button::MakeHandler()
{
	GameObject * object = GetGameObject();
	EventDispatcher::GetSingleton()->RegisterHandler(this, object->GetName());
}

Button::Button(bool active, bool clickable, char * newText, unsigned int length) {
	isActive = active;
	isClickable = clickable;
	ctext = newText;
	textLength = length;
}

void Button::Update(float dt)
{
	time -= dt;

	if (clickCooldown >= 0.0f)
		clickCooldown -= dt;

	if (time < 0)
	{
		time = 300.0f;
	}



	if (showFps)
	{
		int _fps = (int)(1 / dt);

		fps = to_string(_fps);
		fps += " FPS";
	}
}


void Button::HandleEvent(Event* e)
{
	if (isClickable && isActive && sceneIndex == Game::currentScene)
	{
		InputDownEvent* inputDownEvent = dynamic_cast<InputDownEvent*>(e);

		if (inputDownEvent)
		{
			InputManager * input = inputDownEvent->GetInput();

			int mouseX = input->GetMouseX();
			int mouseY = input->GetMouseY();

			if (mouseX > (int)rect.left && mouseX < (int)rect.right && mouseY > (int)rect.top && mouseY < (int)rect.bottom)
			{

				if (input->GetMouseDown()[0] && clickCooldown <= 0.0f)
				{
					eventFunction();
				}
				else
				{
					hovered = true;
				}
			}
			else
			{
				hovered = false;
			}
		}
	}
}

void Button::setButtonType()
{
	switch (buttonType)
	{
	case BUTTON_TYPE::HOST:
	{
		eventFunction = StartServer;
		break;
	}
	case BUTTON_TYPE::JOIN:
	{
		eventFunction = JoinServer;
		break;
	}
	case BUTTON_TYPE::PLAY:
	{
		eventFunction = StartGame;
		break;
	}
	case BUTTON_TYPE::CREDITS:
	{
		break;
	}
	case BUTTON_TYPE::EXIT:
	{
		eventFunction = ShutdownGame;
		break;
	}
	case BUTTON_TYPE::RETURN:
	{
		eventFunction = ReturnToMenu;
		break;
	}
	case BUTTON_TYPE::RESUME_GAME:
	{
		break;
	}
	}
}