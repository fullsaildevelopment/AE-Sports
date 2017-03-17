#include "FloorController.h"
#include "GameObject.h"
#include <random>

bool FloorController::score = false;
int FloorController::scorer = 2;

FloorController::FloorController(float3* f, int rows, int cols, float _maxHeight, unsigned int* _colors)
{
	currPattern = 0;
	floor = f;
	row = rows;
	col = cols;
	maxHeight = _maxHeight;
	transSpeed = 0.25f;
	colors = _colors;
	timer = 0;
	ControlColors(0);
	direction = 1;
	timeing = 0;
}

void FloorController::WavePattern(float _dt)
{
	ratios += _dt * transSpeed;
	if (ratios > 1) ratios = 1;
	for (int i = 0; i < row; ++i)
	{
		for (int j = 0; j < col; ++j)
		{
			MovePillar(i * col + j, ratios * (sinf(i * 0.2f) + 1) * 0.5f);
		}
	}
	if (ratios >= 1.0f) currPattern = 2;
}

void FloorController::BigHexPattern(float _dt)
{
	ratios += _dt * transSpeed;
	for (int i = 0; i < row; ++i)
	{
		for (int j = 0; j < col; ++j)
		{
			if (i % 12 < 4 && j % 12 < 4)
				MovePillar(i * col + j, ratios);
		}
	}
	if (ratios >= 1.0f) currPattern = 3;
}

void FloorController::StripPattern(float _dt)
{
	ratios += _dt * transSpeed;
	for (int i = 0; i < row; ++i)
	{
		for (int j = 0; j < col; ++j)
		{
			if (i % 12 < 4)
				MovePillar(i * col + j, ratios);
		}
	}
	if (ratios >= 1.0f) currPattern = 1;
}

void FloorController::InitialPattern(float _dt)
{
	//decide which way to make the side pillars move up
	if (ratios > 1.0f)
	{
		ratios = 1.0f;
		direction = -1;
	}
	else if (ratios < 0.0f)
	{
		ratios = 0.0f;
		direction = 1;
	}

	_dt = fmodf(_dt, 1);
	ratios += _dt * transSpeed * direction;

	//red goal
	MovePillar(15, ratios);
	MovePillar(16, 1);

	MovePillar(19, 1);
	MovePillar(20, 1);
	MovePillar(21, 1);

	MovePillar(24, 1);
	MovePillar(25, ratios);

	//blue goal
	MovePillar((38 * 54) + 15, ratios);
	MovePillar((38 * 54) + 16, 1);
	MovePillar((38 * 54) + 19, 1);
	MovePillar((38 * 54) + 20, 1);
	MovePillar((38 * 54) + 21, 1);
	MovePillar((38 * 54) + 24, 1);
	MovePillar((38 * 54) + 25, ratios);
}

void FloorController::MovePillar(int pillar, float ratio)
{
	floor[pillar].y = ratio * maxHeight - 10;
	if (floor[pillar].y > -0.0001f) floor[pillar].y = 0;
}

void FloorController::LevelFloor(float _dt)
{
	ratios += _dt * transSpeed;
	for (int i = 0; i < row; ++i)
	{
		for (int j = 0; j < col; ++j)
		{
			if (floor[i * col + j].y != -10)
			{
				float r = 1 - floor[i * col + j].y * -0.1f;
				MovePillar(i * col + j, r - _dt * transSpeed);
				if (floor[i * col + j].y < -9.899f) floor[i * col + j].y = -10;
			}
		}
	}
	if (ratios >= 1.0f) currPattern = 0;
}

void FloorController::Strips(float _dt)
{

}

void FloorController::ControlMovement(float fullTime)
{
	timeing = fullTime;
	float dt = timeing - timer;
	timer = fullTime;

	InitialPattern(dt);

	//if (timeing < 10 && currPattern != 1)
	//{
	//	transState = 1;
	//	StripPattern(dt);
	//	if (currPattern == 1)
	//	{
	//		ratios = 0;
	//		transState = 10;
	//	}
	//}
	//else if (timeing >= 10 && timeing < 20 && currPattern != 0)
	//{
	//	transState = 0;
	//	LevelFloor(dt);
	//	if (currPattern == 0)
	//	{
	//		ratios = 0;
	//		transState = -1;
	//	}
	//}
	//else if (timeing >= 20 && timeing < 30 && currPattern != 2)
	//{
	//	transState = 2;
	//	WavePattern(dt);
	//	if (currPattern == 2)
	//	{
	//		ratios = 0;
	//		transState = 20;
	//	}
	//}
	//else if (timeing >= 30 && timeing < 40 && currPattern != 0)
	//{
	//	transState = 0;
	//	LevelFloor(dt);
	//	if (currPattern == 0)
	//	{
	//		ratios = 0;
	//		transState = -1;
	//	}
	//}
	//else if (timeing >= 40 && timeing < 50 && currPattern != 4)
	//{
	//	transState = 4;
	//	//RandomPattern(dt);
	//	if (currPattern == 4)
	//	{
	//		ratios = 0;
	//		transState = 40;
	//	}
	//}
	//if (timeing > 40)
	//	timeing = 0;
}

void FloorController::ControlColors(float _dt)
{
	score = false;
	int tr = (int)(row * 0.5f);
	for (int i = 0; i < tr; ++i)
	{
		for (int j = 0; j < col; ++j)
		{
			colors[i * col + j] = team1color;
		}
	}
	for (int i = tr; i < row; ++i)
	{
		for (int j = 0; j < col; ++j)
		{
			colors[i * col + j] = team2color;
		}
	}
}

bool FloorController::StripColor()
{
	for (int i = 0; i < 3; ++i)
	{
		unsigned int color = colors[StripCurr + i];
		unsigned int rc = ((color & 0xFF000000) >> 24);
		unsigned int gc = ((color & 0x00FF0000) >> 16);
		unsigned int bc = ((color & 0x0000FF00) >> 8);
		float3 rcf = float3((float)rc, (float)gc, (float)bc) * 0.003906f;
		rcf = rcf.normalize();
		rc = (unsigned int)min(max(rc + (rcf.x * StripMult), 0), 255);
		gc = (unsigned int)min(max(gc + (rcf.y * StripMult), 0), 255);
		bc = (unsigned int)min(max(bc + (rcf.z * StripMult), 0), 255);
		colors[StripCurr + i] = (rc << 24) | (gc << 16) | (bc << 8);
		if (StripCurr + i >= row * col)
		{
			StripCurr = 0;
			StripMult *= -1;
		}
	}
	StripCurr += 3;
	return false;
}

void FloorController::Score(int t)
{
	scorer = t;
	score = true;
}

void FloorController::ScoreColor()
{
	int tr = (int)(row * 0.5f);
	if (scorer == 1)
	{
		for (int i = 0; i < tr; ++i)
		{
			for (int j = 0; j < col; ++j)
			{
				colors[i * col + j] = 0x00FF0000;
			}
		}
	}
	else
	{
		for (int i = tr; i < row; ++i)
		{
			for (int j = 0; j < col; ++j)
			{
				colors[i * col + j] = 0x00FF0000;
			}
		}
	}
	scorer = 2;
}

void FloorController::Update(float _dt)
{
	timeing += _dt;
	ControlMovement(timeing);
	if (score)
	{
		if (scorer != 2)
			ScoreColor();
		sinceScore += _dt;
		if (sinceScore >= 2.5)
		{
			ControlColors(_dt);
			sinceScore = 0;
		}
	}
}

void FloorController::SetState(float fullTime)
{
	ControlMovement(fullTime);
}

FloorController::~FloorController()
{
	delete[] floor;
	delete[] colors;
}
