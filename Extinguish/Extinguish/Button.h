#pragma once
#include "Includes.h"
#include "DeviceResources.h"
#include "ResourceManager.h"
#include "Component.h"
#include "GameObject.h"
#include "InputManager.h"
#include "InputDownEvent.h"
#include "EventDispatcher.h"

class ClientWrapper;
class ServerWrapper;

class Button : public Component
{
private:
	enum BUTTON_TYPE {
		NONE,
		HOST,
		JOIN,
		PLAY,
		CREDITS,
		EXIT,
		RETURN,
		RESUME_GAME,
		CHANGE_TEAM_A,
		CHANGE_TEAM_B, 
		LOBBY,
		NEW_GAME
	};


	// FOR RESIZING:
	// setPositionMultipliers(x, y);
	// MakeRect();
	// if text
	//		setOrigin();


	// for rendering
	Microsoft::WRL::ComPtr<IDWriteTypography> pTypography;
	float height, width, fontSize;
	float originX, originY;
	float widthMult, heightMult;
	D2D1_SIZE_F rtSize, oldSize;
	D2D1_RECT_F rect;
	BUTTON_TYPE buttonType;
	GameObject * object;

	// text and functionality
	unsigned int textLength;
//	wchar_t * text;
	char * ctext;
	string fps;
	wstring text;
	float time = 300.0f;
	float clickCooldown = 0.2f;
	bool isActive;
	bool isClickable;
	bool isTimer = false;
	bool showFps = false;
	bool hovered = false;
	bool selected = false;
	float justSelectedCountdown = 0.0f;
	bool remainSelected = false;
	bool gamePadConnected = false;
	unsigned int sceneIndex;
	std::vector<int> helperIndex;

	float ratio = 1.0f;

	// for gamepad
	Button * up, * down, * left, * right;

	// some function pointer for event
	void (*eventFunction)(void);

	void DoEvent();

public:
	//~Button() {}
	Button(bool active, bool clickable, wchar_t * newText, unsigned int length, float _width, float _height,
		DeviceResources * resources, unsigned int type);
	
	Button(bool active, bool clickable, char * newText, unsigned int length);

	void Update(float _dt) override;

	void HandleEvent(Event* e) override;

	void showFPS(bool fps) { showFps = fps; }
	bool iShowFPS() { return showFps; }

	/* GETTERS */
	float getWidth() { return width; };
	float getHeight() { return height; };
	string getFPS() { return fps; }
	string getTime() {
		string theTime = "Countdown: ";
		theTime += to_string((time / 60.0f));
		return theTime;
	}


	wstring getWTime() {
		wstring theTime = L"0";
		int min = (int)time / 60;
		theTime += to_wstring(min);
		theTime += L":";
		float secf = time - (float)(min * 60);
		int sec = (int)time - (min * 60);
		if (sec > 9)
		theTime += to_wstring(sec);
		else
		{
			theTime += L"0";
			theTime += to_wstring(sec);
		}

		return theTime;
	}
	float* GetTime() { return &time; }

	unsigned int getLength() { return textLength; }
	IDWriteTypography * getTypography() { return pTypography.Get(); }

	wstring getText() { return text; }
	char * getCharText() { return ctext; }

	D2D1_RECT_F getRect() {return rect;}

	float getOriginX() { return originX; }
	float getOriginY() { return originY; }
	float GetRatio() { return ratio; }

	bool isHovered() { return hovered; }
	bool isSelected() { return selected; }
	bool getActive() { return isActive; }
	bool getRemainSelected() { return remainSelected; }
	Button* getAbove() { return up; }

	/* SETTERS */
	void setHeight(float _height) { height = _height; }
	void setWidth(float _width) { width = _width; }
	void setText(wstring _text) {
		text = _text; textLength = (unsigned int)text.length();
	}
	void setTexture(char * _textureAddress) {}
	void setOrigin() { originX = rect.left; originY = rect.top; }
	void setPositionMultipliers(float w, float h) { widthMult = w; heightMult = h; }
	void setRT(D2D1_SIZE_F _rtSize) { oldSize = rtSize; rtSize = _rtSize; }
	void setButtonType();
	void setSceneIndex(unsigned int i) { sceneIndex = i; }
	void SetActive(bool active) { isActive = active; }
	void setTimer(bool active) { isTimer = active; }
	void setHelper(int index) { helperIndex.push_back(index); }
	void setStayHovered(bool b) { remainSelected = b; }
	void setGameObject(GameObject* obj) { object = obj; }
	void resetTime() { time = 10.0f; }
	void setBelow(Button * below) { down = below; }
	void setAbove(Button * above) { up = above; }
	void setLeft(Button * left) { this->left = left; }
	void setRight(Button * right) { this->right = right; }
	void setSelected(bool tf = true) { selected = tf; if (tf) justSelectedCountdown = 0.2f; } // only use if the first button


	/* HELPERS*/

	void MakeRect() {
		rect = D2D1::RectF(
			(rtSize.width * widthMult) - (width * 0.5f),
			(rtSize.height * heightMult) - (height * 0.5f),
			(rtSize.width * widthMult) + (width * 0.5f),
			(rtSize.height * heightMult) + (height * 0.5f)
		);

		if (widthMult <= 0.0f)
		{
			rect.left = 0.0f;
			rect.right = width;
		}

		if (heightMult <= 0.0f)
		{
			rect.top = 0.0f;
			rect.bottom = height;
		}
	}

	void MakeHandler();

	void AdjustSize()
	{
		float tWidth, tHeight;
		ratio = (rtSize.width / oldSize.width);
		tWidth = width * (rtSize.width / oldSize.width);
		tHeight = height * (rtSize.height / oldSize.height);

		width = tWidth;
		height = tHeight;
	}

};