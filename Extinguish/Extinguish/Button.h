#pragma once
#include "Includes.h"
#include "DeviceResources.h"
#include "ResourceManager.h"
#include "Component.h"

class Button : public Component
{
private:
	Microsoft::WRL::ComPtr<IDWriteTypography> pTypography;
	float height, width, fontSize;
	unsigned int textLength;
	wchar_t * text;
	char * ctext;
	string fps;
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

	/* SETTERS */
	void setHeight(float _height) { height = _height; }
	void setWidth(float _width) { width = _width; }
	void setText(WCHAR * _text) { text = _text; }
	void setTexture(char * _textureAddress) {}
	wchar_t * getText() { return text; }
	char * getCharText() { return ctext; }
};