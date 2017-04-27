#pragma once
#include "vec3.h"
#include <vector>
#include <map>
#include "PriorityQueue.h"


class Map
{
public:

	struct ColumnRow
	{
		int Column, Row;
	};

	struct Node
	{
		float *weight; // point to the y of pillar
		float3 *pos; // where/which pillar it's attatched to
		ColumnRow colRow;
		std::vector<ColumnRow> neighbors;
	};

	struct PlannerNode
	{
		float heuristicCost = 0;
		float givenCost = 0;
		float finalCost = 0;
		Node *path = nullptr; // the path that we'll take
		PlannerNode *parent = nullptr; // my parent
	};

private:

	int numRows; // number of rows
	int numCols; // number of columns
	float hWeight = 1;
	float3 prevBallPos;
	Node *ballNode;
	PriorityQueue<PlannerNode *> que;
	std::vector<std::vector<Node *>> nodes; // all the nodes in the map
	std::map<Node *, PlannerNode *> visited; // map of tiles we've already seen
	std::vector<PlannerNode *> usedPlanners; // used to delete when game is done

public:
	
	Map(int rows, int cols, float3 *positions);
	~Map();

	Node *FindClosest(float3 pos);
	Node *FindBallNode(float3 ballPos);
	std::vector<Node *> CreatePath(Node *start, Node *end);
};