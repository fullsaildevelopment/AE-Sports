#pragma once
#include "Component.h"
#include "vec3.h"
#include "XTime.h"

class FloorController : public Component
{
	int currPattern;
	float3* floor;
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
public:
	FloorController(float3* f, int rows, int cols, float _maxHeight);
	void MovePillar(int pillar, float ratio);
	void LevelFloor(float dt);
	void Update(float dt) override;
};