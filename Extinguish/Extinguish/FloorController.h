#pragma once
#include "Component.h"
#include "vec3.h"
#include "XTime.h"

class FloorController : public Component
{

	static bool score;
	static int scorer;
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
	void WavePattern(float _dt);
	void BigHexPattern(float _dt);
	void Strips(float _dt);
	void StripPattern(float _dt);
	void InitialPattern(float _dt);
	void ControlMovement(float _dt);
	void ControlColors(float _dt);
	void ScoreColor();
	float sinceScore = 0;
	bool StripColor();
	int StripCurr = 0;
	int StripMult = 100;
	unsigned int team1color = 0xAD000000;
	unsigned int team2color = 0x0000AD00;
	float colortiming = 0;

	int direction;
public:
	FloorController(float3* f, int rows, int cols, float _maxHeight, unsigned int* _colors);
	~FloorController();
	void MovePillar(int pillar, float ratio);
	void LevelFloor(float _dt);
	void SetState(int state, float _dt);
	void Update(float _dt) override;
	int GetState() { return transState; };
	static void Score(int t);
	float3* getFloor() { return floor; }
	int getCol() { return col; }
	int getRow() { return row; }
	float getTime() { return timeing; }
};