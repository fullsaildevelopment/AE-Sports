#include "Button.h"
#include "Game.h"
#include "Server Wrapper.h"
#include "Client Wrapper.h"

void StartGame()
{
	Game::currentScene = 1;
}

void StartServer()
{
	if (Game::server.init("127.0.0.1", 60000) == 1)
	{
		ResourceManager::GetSingleton()->SetMultiplayer(true);
		ResourceManager::GetSingleton()->SetServer(true);

		Game::client.init("127.0.0.1", 60001);

		StartGame();
	}
}

void JoinServer()
{
	ResourceManager::GetSingleton()->SetMultiplayer(true);
	ResourceManager::GetSingleton()->SetServer(false);
	Game::client.init("127.0.0.1", 60001);

	StartGame();
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

			if (input->GetMouseDown()[0])
			{
				int mouseX = input->GetMouseX();
				int mouseY = input->GetMouseY();

				if (mouseX > (int)rect.left && mouseX < (int)rect.right && mouseY > (int)rect.top && mouseY < (int)rect.bottom)
				{
					eventFunction();
				}
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
		break;
	}
	case BUTTON_TYPE::RESUME_GAME:
	{
		break;
	}
	}
}