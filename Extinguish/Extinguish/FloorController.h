#pragma once
#include "Component.h"
#include "vec3.h"
#include "XTime.h"

class FloorController : public Component
{
	int currPattern;
	int transState;
	float3* floor;
	unsigned int* colors;
	float ratios;
	float transSpeed;
	int row;
	int col;
	float maxHeight;
	XTime timer;
	float timeing;
	void WavePattern(float dt);
	void BigHexPattern(float dt);
	void StripPattern(float dt);
	void RandomPattern(float dt);
	void ControlMovement(float dt);
	void ControlColors(float dt);
public:
	FloorController(float3* f, int rows, int cols, float _maxHeight, unsigned int* _colors);
	~FloorController();
	void MovePillar(int pillar, float ratio);
	void LevelFloor(float dt);
	void SetState(int state);
	void Update(float dt) override;
	int GetState() { return transState; };
};