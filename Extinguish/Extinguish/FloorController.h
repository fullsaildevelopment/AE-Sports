#pragma once
#include "Component.h"
#include "vec3.h"
#include "XTime.h"
#include <vector>

class FloorController : public Component
{
	enum FloorStates
	{
		STRIPS = 0,
		GROUPS,
		NUM_OF_STATES,
		LEVEL, //level must come after num_of_states to prevent itself from being randomly chosen
	};

	static bool score;
	static int scorer;
	int currPattern;
	int transState;
	float3* floor;
	unsigned int* colors;
	float ratios, initialRatio;
	float transSpeed;
	int row;
	int col;
	float maxHeight;
	float timer;
	float timeing;
	void WavePattern(float _dt);
	void BigHexPattern(float _dt);
	void Strips(float _dt);
	void Groups(float _dt);
	void StripPattern(float _dt);
	void InitialPattern(float _dt);
	void ControlMovement(float fullTime);
	void ControlColors(float _dt);
	void ScoreColor();
	float sinceScore = 0;
	bool StripColor();
	int StripCurr = 0;
	int StripMult = 100;
	unsigned int team1color = 0x88000000;
	unsigned int team2color = 0x00008800;
	unsigned int* originalColor;
	float colortiming = 0;
	int randStateIndex = -1;
	int stateIndex = 0;
	float resetableTimer = 0.0f;
	bool cooldownDone = true;
	bool stateRunning = false;
	int* groupPositions;
	bool positionsGenerated = false;
	std::vector<int*> dontRaise;

	//const members
	const float TIME_TIL_COOLDOWN = 5.0f; //how long til after the state is finished til it starts another state
	const float TIME_TIL_STATE = 5.0f; //how long til the state runs
	const float TIME_TO_RUN_STATE = 5.0f; //how long to run the state
	const int NUM_OF_GROUPS = 27;

	struct pulseState
	{
		int pulseLevel;
		int pos;
		float pulseTimer;
		bool pulseing;
	};

	struct ColumnRow
	{
		int Column;
		int Row;
	};

	vector<pulseState> pulses;

	int direction;

	void FindEmptyPulseState(int _pos);
	void DoPulses();
	void NextLevel(int _pos, int _level);

	int ConvertTo1D(int x, int y);
	void ConvertTo2D(int num, int& x, int& y);
public:
	FloorController(float3* f, int rows, int cols, float _maxHeight, unsigned int* _colors);
	~FloorController();
	void MovePillar(int pillar, float ratio);
	void LevelFloor(float _dt);
	void SetState(float fullTime);
	void Update(float _dt) override;
	float GetState() { return timeing; };
	static void Score(int t);
	float3* getFloor() { return floor; }
	int getCol() { return col; }
	int getRow() { return row; }
	float getTime() { return timeing; }
	void setColor(int pos, float3 color);
	void setColor(int pos, unsigned int color);
	unsigned int* getColors() { return colors; };
	void PulseFloor(int pos);
	
};