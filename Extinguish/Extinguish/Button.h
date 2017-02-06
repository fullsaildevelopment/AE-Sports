#pragma once
#include "Includes.h"
#include "DeviceResources.h"
#include "ResourceManager.h"
#include "Component.h"

class Button : public Component
{
private:
	// for rendering
	Microsoft::WRL::ComPtr<IDWriteTypography> pTypography;
	float height, width, fontSize;
	float originX, originY;

	// text and functionality
	unsigned int textLength;
//	wchar_t * text;
	char * ctext;
	string fps;
	wstring text;
	float time = 300.0f;
	bool isActive;
	bool isClickable;
	bool showFps = false;
	// some function pointer for event?

public:
	Button(bool active, bool clickable, wchar_t * newText, unsigned int length, float _width, float _height,
		DeviceResources * resources) { 
		isActive = active; 
		isClickable = clickable; 
		text = newText;
		textLength = length;
		height = _height;
		width = _width;
		HRESULT hr = resources->GetWriteFactory()->CreateTypography(pTypography.GetAddressOf());
		if (SUCCEEDED(hr))
		{
			DWRITE_FONT_FEATURE fontFeature = { DWRITE_FONT_FEATURE_TAG_STYLISTIC_SET_7,
			1 };
			hr = pTypography->AddFontFeature(fontFeature);
		}
	}


	Button(bool active, bool clickable, char * newText, unsigned int length) {
		isActive = active;
		isClickable = clickable;
		ctext = newText;
		textLength = length;
	}

	void Update(float dt) override 
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


	void HandleEvent(Event* e) override {};

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
	unsigned int getLength() { return textLength; }
	IDWriteTypography * getTypography() { return pTypography.Get(); }

	wstring getText() { return text; }
	char * getCharText() { return ctext; }

	D2D1_RECT_F getRect(D2D1_SIZE_F rtSize) {
		D2D1_RECT_F rect = D2D1::RectF(
			rtSize.width / 2.0f - (width / 2.0f),
			-height,
			rtSize.width / 2.0f + (width / 2.0f),
			height
		);

		return rect;
	}

	float getOriginX() { return originX; }
	float getOriginY() { return originY; }

	/* SETTERS */
	void setHeight(float _height) { height = _height; }
	void setWidth(float _width) { width = _width; }
	void setText(WCHAR * _text) { text = _text; }
	void setText(wstring _text) { text = _text; }
	void setTexture(char * _textureAddress) {}
	void setOrigin(float x, float y) { originX = x; originY = y; }





};