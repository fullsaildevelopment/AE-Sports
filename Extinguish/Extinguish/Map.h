#pragma once
#include "vec3.h"
#include <vector>

// class for the node map to help make the 
// path planning easier for AI around pillars

// each node has weight which is based off of the height(float)
// +10 because negative
// have xyz positions and know neighbors
class Map
{
private:
	struct Node
	{
		float *weight; // point to the y of pillar
		float3 *pos; // where/which pillar it's attatched to
		std::vector<int> neighbors;
	};

	struct ColumnRow
	{
		int Column;
		int Row;
	};


	std::vector<std::vector<Node *>> nodes; // will be an array
	int numRows; // number of rows
	int numCols; // number of columns


public:
	
	Map(int rows, int cols, float3 *positions);
	~Map();

	Node *FindClosest(float3 pos);

};