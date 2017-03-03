#pragma once
#include "Includes.h"
#include "Component.h"
#include "DeviceResources.h"
#include "ResourceManager.h"
#include "GameObject.h"
#include "EventDispatcher.h"
#include "InputDownEvent.h"
#include "InputManager.h"
#include "Game.h"

class MeterBar : public Component
{
private:
	/* for the bar rendering */
	D2D1_SIZE_F rtSize;
	D2D1_RECT_F rect, rectInitial; // for shrinking bar
	D2D1_RECT_F rect2; // for bar border, stays the same
	float height, width;
	float widthMult, heightMult;
	bool isActive;

	/* for cooldowns */
	float rechargeTime, rTime; // rTime is the countdown time, rechargeTime doesn't change
	float drainTime, dTime; // dTime is the countdown time, drainTime doesn't change
	bool drain = true;
	bool canRecharge, empty;

	/* other */
	unsigned int sceneIndex;
	bool gamePadSprinting;

public:
	MeterBar(bool active, float _width, float _height, float multiplyX, float multiplyY) : isActive(active), width(_width), height(_height), widthMult(multiplyX), heightMult(multiplyY) { gamePadSprinting = false; }
	~MeterBar();

	/* overrides */
	void Update(float dt) override;
	void HandleEvent(Event* e) override;

	/* getters */
	float getWidth() { return width; };
	float getHeight() { return height; };
	D2D1_RECT_F getRect() { return rect; }
	D2D1_RECT_F getRect2() { return rect2; }
	bool getActive() { return isActive; } // if the bar is active | will render if true
	float getDrainTime() { return dTime; } // current drain time (dTime == 0.0f -> fully drained | no energy)
	float getRechargeTime() { return rTime; } // current time of recharge (rTime == rechargeTime -> fully charged)
	float getDrainTimeElapsed() { return drainTime - dTime; } // current time elapsed drain
	float getRechargeTimeElapsed() { return rechargeTime - rTime; } // current time elapsed recharge
	bool isDraining() { return drain; } // if false == recharge || false && !isActive == fully charged
	bool isEmpty() { return empty; }

	/* setters */
	void setHeight(float _height) { height = _height; }
	void setWidth(float _width) { width = _width; }
	void setPositionMultipliers(float w, float h) { widthMult = w; heightMult = h; }
	void setSceneIndex(unsigned int i) { sceneIndex = i; }
	void SetActive(bool active) { isActive = active; }
	void setRT(D2D1_SIZE_F _rtSize) { rtSize = _rtSize; }
	void setRechargeTime(float time) { rechargeTime = time; rTime = time; }
	void setDrainTime(float time) { drainTime = time; dTime = time; }
	void setCanRecharge(bool recharge) { canRecharge = recharge; }

	/* helpers */
	D2D1_RECT_F MakeRect() {
		D2D1_RECT_F _rect = D2D1::RectF(
			(rtSize.width * widthMult) - (width * 0.5f),
			(rtSize.height * heightMult) - (height * 0.5f),
			(rtSize.width * widthMult) + (width * 0.5f),
			(rtSize.height * heightMult) + (height * 0.5f)
		);

		if (widthMult <= 0.0f)
		{
			_rect.left = 0.0f;
			_rect.right = width;
		}

		if (heightMult <= 0.0f)
		{
			_rect.top = 0.0f;
			_rect.bottom = height;
		}

		return _rect;
	}

	D2D1_RECT_F ShrinkRect(float time, float fulltime) {

		D2D1_RECT_F _rect = D2D1::RectF(
			(rtSize.width * widthMult) - (width * 0.5f),
			(rtSize.height * heightMult) - (height * 0.5f),
			(rtSize.width * widthMult) + (width * 0.5f),
			(rtSize.height * heightMult) + (height * 0.5f)
		);

		if (widthMult <= 0.0f)
		{
			_rect.left = 0.0f;
			_rect.right = width;
		}

		if (heightMult <= 0.0f)
		{
			_rect.top = 0.0f;
			_rect.bottom = height;
		}

		float temp = _rect.right - _rect.left;
		temp *= (time / fulltime);
		_rect.right = temp + _rect.left;

		return _rect;
	}

	void MakeHandler();

	void MakeRects() { rect = rect2 = rectInitial = MakeRect(); }
};

