#include "Map.h"

bool isGreater(Map::PlannerNode *const&lhs, Map::PlannerNode *const&rhs)
{
	return (lhs->finalCost > rhs->finalCost);
}

float distSquared(Map::Node *start, Map::Node *end)
{
	float3 vectorBetween = *start->pos - *end->pos;
	return dot_product(vectorBetween, vectorBetween);
}

Map::Map(int rows, int cols, float3 *positions) : que(isGreater)
{
	nodes.resize(rows);
	numRows = rows;
	numCols = cols;

	for (int r = 0; r < rows; ++r)
	{
		nodes[r].resize(cols);

		for (int c = 0; c < cols; ++c)
		{
			Node *newNode = new Node;
			//rowyoureat * numcol + columnyoureat
			newNode->weight = &positions[r * cols + c].y;
			newNode->pos = &positions[r * cols + c];
			newNode->colRow = { c, r };

			bool evenRow = (r % 2 == 0);
			ColumnRow rM = { c + 1, r };
			ColumnRow rB = { (evenRow) ? c : c + 1, r - 1 };
			ColumnRow lB = { (evenRow) ? c - 1 : c, r - 1 };
			ColumnRow lM = { c - 1, r };
			ColumnRow lT = { (evenRow) ? c - 1 : c, r + 1 };
			ColumnRow rT = { (evenRow) ? c : c + 1, r + 1 };

			std::vector<ColumnRow> neighbors({ NULL });
			neighbors.push_back(rM);
			neighbors.push_back(rB);
			neighbors.push_back(lB);
			neighbors.push_back(lM);
			neighbors.push_back(lT);
			neighbors.push_back(rT);

			for (int i = 0; i < 6; ++i)
			{
				if (neighbors[i].Column < 0 || neighbors[i].Column >= cols)
				{
					continue;
				}

				if (neighbors[i].Row < 0 || neighbors[i].Row >= rows)
				{
					continue;
				}

				newNode->neighbors.push_back(neighbors[i]);
			}

			nodes[r][c] = newNode;
		}
	}
}

Map::~Map()
{

}

Map::Node *Map::FindClosest(float3 pos)
{
	float dist = FLT_MAX;
	float3 h = float3(0, 10, 0);
	Node *closest = nullptr;

	for (int row = 0; row < numRows; ++row)
	{
		for (int col = 0; col < numCols; ++col)
		{
			float3 tmp = ((*nodes[row][col]->pos + h) - pos);
			float dot = dot_product(tmp, tmp);

			if (dot < dist)
			{
				dist = dot;
				closest = nodes[row][col];
			}
		}
	}

	return closest;
}

Map::Node *Map::FindBallNode(float3 ballPos)
{
	if (!ballPos.isEqual(prevBallPos))
	{
		prevBallPos = ballPos;
		ballNode = FindClosest(ballPos);
	}

	return ballNode;
}

std::vector<Map::Node *> Map::CreatePath(Node * start, Node *end)
{
	std::vector<Node *> path;
	PlannerNode *begin = new PlannerNode;
	begin->path = start;

	que.push(begin);
	visited[begin->path] = que.front();
	begin->givenCost = 0;
	begin->heuristicCost = distSquared(start, end);
	begin->finalCost = begin->givenCost + begin->heuristicCost * hWeight;

	while (!que.empty())
	{
		PlannerNode *curr = que.front();
		que.pop();

		if (curr->path == end)
		{
			PlannerNode *fin = curr;

			while (fin)
			{
				path.push_back(fin->path);
				fin = fin->parent;
			}

			que.clear();
			break;
		}

		int size = (int)curr->path->neighbors.size();

		for (int i = 0; i < size; ++i)
		{
			Node *next = nodes[curr->path->neighbors[i].Row][curr->path->neighbors[i].Column];
			float tmpCost = curr->givenCost + (*next->weight + 11.001f);
			PlannerNode *node = visited[next];

			if (node != nullptr)
			{
				if (tmpCost < node->givenCost)
				{
					que.remove(node);
					node->givenCost = tmpCost;
					node->heuristicCost = distSquared(next, end);
					node->finalCost = node->givenCost + node->heuristicCost * hWeight;
					node->parent = curr;

					que.push(node);
				}
			}

			else
			{
				PlannerNode *node = new PlannerNode;
				node->path = next;
				node->givenCost = tmpCost;
				node->heuristicCost = distSquared(next, end);
				node->finalCost = node->givenCost + node->heuristicCost * hWeight;
				node->parent = curr;

				visited[next] = node;
				que.push(node);
			}
		}
	}

	que.clear();
	visited.clear();

	return path;
}