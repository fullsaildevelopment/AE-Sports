#pragma once
#include "vec3.h"
#include <vector>
#include "PriorityQueue.h"

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

	/*
	struct Position
		{
			int row, col = 0;
		};

		struct SearchNode
		{
			std::vector<Tile *> edges; // vector of existing edges
			std::vector<Position> pEdges; // vector of existing edge positions
			Tile *curr = nullptr; // the tile it's representing
		};

		struct PlannerNode
		{
			float heuristicCost = 0; 
			float givenCost = 0; 
			float finalCost = 0; 
			SearchNode *path = nullptr; // the path that we'll take
			PlannerNode *parent = nullptr; // my parent
		};

		boolean isGreater(PlannerNode *lhs, PlannerNode *rhs)
		{
			return (lhs.finalCost > rhs.finalCost);
		}
	*/

	int numRows; // number of rows
	int numCols; // number of columns
	float3 prevBallPos;
	Node *ballNode;
	std::vector<std::vector<Node *>> nodes; // all the nodes in the map
	


public:
	
	Map(int rows, int cols, float3 *positions);
	~Map();

	Node *FindClosest(float3 pos);
	Node *FindBallNode(float3 ballPos);
	std::vector<Node *> CreatePath(Node * start, Node *end);
};