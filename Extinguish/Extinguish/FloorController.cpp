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
	pulses.resize(4);

	for (int i = 0; i < 4; ++i)
	{
		pulses[i].pulseing = false;
		pulses[i].pulseLevel = 0;
		pulses[i].pulseTimer = 0;
		pulses[i].pos = -1;
	}

	originalColor = new unsigned int[row * col];
	int size = row * col;
	for (int i = 0; i < size; ++i)
	{
		originalColor[i] = colors[i];
	}

	dontRaise.resize((row - 5) / 5);
	for (int i = 0; i < dontRaise.size(); ++i)
	{
		dontRaise[i] = new int[3];
	}
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
	float TIME = timeing * transSpeed;

	direction = (int)(TIME) % 2;
	direction = (direction < 1) ? -1 : 1;
	float change = fmodf(TIME, 1.0f);
	ratios = (direction == 1) ? change : 1 - change;
	for (int i = 0; i < row; ++i)
	{
		for (int j = 0; j < col; ++j)
		{
			if (i % 12 > 4 && j % 12 > 4)
				MovePillar(i * col + j, ratios);
		}
	}
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
	float TIME = timeing * transSpeed;

	direction = (int)(TIME) % 2;
	direction = (direction < 1) ? -1 : 1;
	float change = fmodf(TIME, 1.0f);
	initialRatio = (direction == 1) ? change : 1 - change;

	//decide which way to make the side pillars move up
	if (initialRatio > 1.0f)
	{
		initialRatio = 1.0f;
	}
	else if (initialRatio < 0.0f)
	{
		initialRatio = 0.0f;
	}

	//red goal
	MovePillar(15, initialRatio);
	MovePillar(16, 1);
	MovePillar(17, 1);
	MovePillar(18, 1);
	MovePillar(19, 1);
	MovePillar(20, 1);
	MovePillar(21, 1);
	MovePillar(22, 1);
	MovePillar(23, 1);
	MovePillar(24, 1);
	MovePillar(25, initialRatio);

	//blue goal
	MovePillar((38 * 54) + 15, initialRatio);
	MovePillar((38 * 54) + 16, 1);
	MovePillar((38 * 54) + 17, 1);
	MovePillar((38 * 54) + 18, 1);
	MovePillar((38 * 54) + 19, 1);
	MovePillar((38 * 54) + 20, 1);
	MovePillar((38 * 54) + 21, 1);
	MovePillar((38 * 54) + 22, 1);
	MovePillar((38 * 54) + 23, 1);
	MovePillar((38 * 54) + 24, 1);
	MovePillar((38 * 54) + 25, initialRatio);
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
	//problem: the logic is there, but it doesn't keep track of the dontRaise the next time the function is called
	//solution: generate them the first time only (in a 2D array)
	//generate 2D array of dontRaise
	if (!dontRaiseGenerated)
	{
		dontRaiseGenerated = true;

		for (int i = 0; i < dontRaise.size(); ++i)
		{
			//decide columns not to raise
			int randIndex = rand() % col;

			//if at the end, move down randIndex
			if (randIndex > col - 3)
			{
				randIndex = col - 3;
			}

			//dont raise the random one and the next two
			for (int j = 0; j < 3; ++j)
			{
				dontRaise[i][j] = randIndex + j;
			}
		}
	}

	int dontI = 0, dontJ = 0;

	for (int r = 5; r < row - 5; r += 5, ++dontI) //i don't want the walls to be up near the goals... so 5 away from goal walls
	{
		//if (r % 5 == 0)
		{
			for (int c = 0; c < col; ++c)
			{
				if (dontRaise[dontI][dontJ] != c)
				{
					MovePillar(r * col + c, ratios);
				}
				else
				{
					++dontJ;
				}
			}
		}
	}
}

void FloorController::ControlMovement(float fullTime)
{
	timeing = fullTime;
	float dt = timeing - timer;

	ratios += dt * transSpeed;

	//always do this (at least for most part)
	InitialPattern(dt);

	timer = fullTime;
	resetableTimer += dt;

	//if we want random, we can use this code
	//resetableTimer += dt;

	//if (resetableTimer > TIME_TIL_COOLDOWN + TIME_TIL_STATE) //both times to take into consideration time 
	//{
	//	cooldownDone = true;
	//}

	//randStateIndex = -1;

	//if (resetableTimer > TIME_TIL_STATE && cooldownDone)
	//{
	//	randStateIndex = rand() % NUM_OF_STATES;
	//	resetableTimer = 0.0f;
	//	cooldownDone = false;
	//}
	//else if (resetableTimer > TIME_TO_RUN_STATE)
	//{

	//}


	//if (randStateIndex != -1)
	//{
	//	switch (randStateIndex)
	//	{
	//	case STRIPS:
	//		Strips(dt);
	//		break;
	//	case GROUPS:

	//		break;
	//	}
	//}
/*
	if (resetableTimer > TIME_TIL_STATE + TIME_TIL_COOLDOWN)
	{
		transState = -1;
		LevelFloor(dt);
	}
	else *//*if (resetableTimer > TIME_TIL_STATE)
	{
		Strips(dt);
	}*/

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
	PulseFloor(-1);
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

void FloorController::setColor(int pos, float3 color)
{
	unsigned int r = (unsigned int)min(color.x * 255, 255);
	unsigned int g = (unsigned int)min(color.y * 255, 255);
	unsigned int b = (unsigned int)min(color.z * 255, 255);
	colors[pos] = (r << 24) | (g << 16) | b;
}

void FloorController::setColor(int pos, unsigned int color)
{
	colors[pos] = color;
}

void FloorController::FindEmptyPulseState(int _pos)
{
	bool foundEmpty = false;
	int highestLevel = 0;
	int highestPos = -1;
	for (int i = 0; i < pulses.size(); ++i)
	{
		if (pulses[i].pulseing == false)
		{
			pulses[i].pulseing = true;
			pulses[i].pulseTimer = timeing;
			pulses[i].pos = _pos;
			pulses[i].pulseLevel = 1;
			foundEmpty = true;
			setColor(_pos, 0x00FF0000);
			break;
		}
		else if (highestLevel < pulses[i].pulseLevel)
		{
			highestLevel = pulses[i].pulseLevel;
			highestPos = i;
		}
	}
	if (!foundEmpty)
	{
		pulses[highestPos].pulseing = true;
		pulses[highestPos].pulseTimer = timeing;
		pulses[highestPos].pos = _pos;
		pulses[highestPos].pulseLevel = 1;
		setColor(_pos, 0x00FF0000);
	}
}

void FloorController::NextLevel(int _pos, int _level)
{
	int Row = (int)(_pos / col);
	int Column = _pos % col;

	switch (_level)
	{
	case 0:
	{
		bool evenRow = (Row % 2 == 0);
		bool evenColumn = (Column % 2 == 0);
		{
			ColumnRow rM = { Column + 1, Row };
			ColumnRow rB = { (evenRow) ? Column : Column + 1, Row - 1 };
			ColumnRow lB = { (evenRow) ? Column - 1 : Column, Row - 1 };
			ColumnRow lM = { Column - 1, Row };
			ColumnRow lT = { (evenRow) ? Column - 1 : Column, Row + 1 };
			ColumnRow rT = { (evenRow) ? Column : Column + 1, Row + 1 };
			vector<ColumnRow> positions;
			positions.push_back(rM);
			positions.push_back(rB);
			positions.push_back(lB);
			positions.push_back(lM);
			positions.push_back(lT);
			positions.push_back(rT);
			for (int i = 0; i < 6; ++i)
			{
				if (positions[i].Column < 0 || positions[i].Column >= col)
				{
					continue;
				}
				if (positions[i].Row < 0 || positions[i].Row >= row)
				{
					continue;
				}
				setColor((positions[i].Row * col + positions[i].Column), originalColor[positions[i].Row * col + positions[i].Column]);
			}
		}
	}
	break;
	case 2:
	{
		setColor(_pos, originalColor[_pos]);
		bool evenRow = (Row % 2 == 0);
		bool evenColumn = (Column % 2 == 0);
		ColumnRow rM = { Column + 1, Row };
		ColumnRow rB = { (evenRow) ? Column : Column + 1, Row - 1 };
		ColumnRow lB = { (evenRow) ? Column - 1 : Column, Row - 1 };
		ColumnRow lM = { Column - 1, Row };
		ColumnRow lT = { (evenRow) ? Column - 1 : Column, Row + 1 };
		ColumnRow rT = { (evenRow) ? Column : Column + 1, Row + 1 };
		vector<ColumnRow> positions;
		positions.push_back(rM);
		positions.push_back(rB);
		positions.push_back(lB);
		positions.push_back(lM);
		positions.push_back(lT);
		positions.push_back(rT);
		for (int i = 0; i < 6; ++i)
		{
			if (positions[i].Column < 0 || positions[i].Column >= col)
			{
				continue;
			}
			if (positions[i].Row < 0 || positions[i].Row >= row)
			{
				continue;
			}
			setColor((positions[i].Row * col + positions[i].Column), 0x00ff0000);
		}
	}
	break;
	default:
		break;
	}

}

void FloorController::DoPulses()
{
	for (int i = 0; i < pulses.size(); ++i)
	{
		if (pulses[i].pulseing == false)
			continue;
		//Get time since last change
		float betweenTime = timeing - pulses[i].pulseTimer;
		//If the time since last change is great then chage level up to 3
		if (betweenTime >= 0.13f)
		{
			if (pulses[i].pulseLevel == 2)
			{
				pulses[i].pulseing = false;
				pulses[i].pulseLevel = 0;
				pulses[i].pulseTimer = 0;
				NextLevel(pulses[i].pos, pulses[i].pulseLevel);
				pulses[i].pos = -1;
			}
			else
			{
				++pulses[i].pulseLevel;
				NextLevel(pulses[i].pos, pulses[i].pulseLevel);
				pulses[i].pulseTimer = timeing;
			}
		}
	}
}

void FloorController::PulseFloor(int _pos)
{
	//If there is a new pulse find a spot for it
	if (_pos >= 0)
	{
		FindEmptyPulseState(_pos);
	}
	//Do the pulses.. change states and timeing if need be
	DoPulses();
}

FloorController::~FloorController()
{
	delete[] floor;
	delete[] colors;

	for (int i = 0; i < row; ++i)
	{
		delete [] dontRaise[i];
	}
}
