#pragma once
#include "vec3.h"
#include <vector>

// class for the node map to help make the 
// path planning easier for AI around pillars

class Map
{
	struct Node
	{
		int weight;
		float3 pos;
		std::vector<Node*> neighbor;
	};

	// each node has weight which is based off of the height(float)
	// +10 because negative
	// have xyz positions and know neighbors

};