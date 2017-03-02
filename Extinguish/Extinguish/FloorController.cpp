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
	timer.Restart();
	ControlColors(0);
}

void FloorController::WavePattern(float dt)
{
	ratios += dt * transSpeed;
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

void FloorController::BigHexPattern(float dt)
{
	ratios += dt * transSpeed;
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

void FloorController::StripPattern(float dt)
{
	ratios += dt * transSpeed;
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

void FloorController::MovePillar(int pillar, float ratio)
{
	floor[pillar].y = ratio * maxHeight - 10;
	if (floor[pillar].y > -0.0001f) floor[pillar].y = 0;
}

void FloorController::LevelFloor(float dt)
{
	ratios += dt * transSpeed;
	for (int i = 0; i < row; ++i)
	{
		for (int j = 0; j < col; ++j)
		{
			if (floor[i * col + j].y != -10)
			{
				float r = 1 - floor[i * col + j].y * -0.1f;
				MovePillar(i * col + j, r - dt * transSpeed);
				if (floor[i * col + j].y < -9.899f) floor[i * col + j].y = -10;
			}
		}
	}
	if (ratios >= 1.0f) currPattern = 0;
}

void FloorController::Strips(float dt)
{

}

void FloorController::ControlMovement(float dt)
{
	timeing += dt;
	if (timeing < 10 && currPattern != 1)
	{
		transState = 1;
		StripPattern(dt);
		if (currPattern == 1)
		{
			ratios = 0;
			transState = 10;
		}
	}
	else if (timeing >= 10 && timeing < 20 && currPattern != 0)
	{
		transState = 0;
		LevelFloor(dt);
		if (currPattern == 0)
		{
			ratios = 0;
			transState = -1;
		}
	}
	else if (timeing >= 20 && timeing < 30 && currPattern != 2)
	{
		transState = 2;
		WavePattern(dt);
		if (currPattern == 2)
		{
			ratios = 0;
			transState = 20;
		}
	}
	else if (timeing >= 30 && timeing < 40 && currPattern != 0)
	{
		transState = 0;
		LevelFloor(dt);
		if (currPattern == 0)
		{
			ratios = 0;
			transState = -1;
		}
	}
	else if (timeing >= 40 && timeing < 50 && currPattern != 4)
	{
		transState = 4;
		//RandomPattern(dt);
		if (currPattern == 4)
		{
			ratios = 0;
			transState = 40;
		}
	}
	if (timeing > 40)
		timeing = 0;
}

void FloorController::ControlColors(float dt)
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

void FloorController::Update(float dt)
{
	ControlMovement(dt);
	//ControlColors(dt);
	if (score)
	{
		if (scorer != 2)
			ScoreColor();
		sinceScore += dt;
		if (sinceScore >= 2.5)
		{
			ControlColors(dt);
			sinceScore = 0;
		}
	}
}

void FloorController::SetState(int state, float dt)
{
	if (state == -1)
	{
		LevelFloor(1000);
	}
	else if (state == 0)
	{
		currPattern = 1;
		timeing = 10;
		Update(dt);
	}
	else if (state == 1)
	{
		currPattern = 0;
		timeing = 0;
		Update(dt);
	}
	else if (state == 10)
	{
		StripPattern(1000);
	}
	else if (state == 2)
	{
		currPattern = 0;
		timeing = 20;
		Update(dt);
	}
	else if (state == 20)
	{
		WavePattern(1000);
	}
	else if (state == 3)
	{
		currPattern = 0;
		timeing = 40;
		Update(dt);
	}
	else if (state == 30)
	{
		//RandomPattern(1000);
	}
}

FloorController::~FloorController()
{
	delete[] floor;
	delete[] colors;
}
