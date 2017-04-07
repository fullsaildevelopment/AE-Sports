#include "Map.h"

Map::Map(int rows, int cols, float3 *positions)
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

				newNode->neighbors.push_back(neighbors[i].Row * cols + neighbors[i].Column);
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
	float dist = 98700;
	Node *closest = nullptr;

	for (int row = 0; row < nodes.size(); ++row)
	{
		for (int col = 0; col < nodes.size(); ++col)
		{
			float3 tmp = (*nodes[row][col]->pos - pos);
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

	/*
	  PriorityQueue open(isGreater);
      std::map visited;
      open.enqueue(new PlannerNode(start));
      visited[start] = open.front();
      
      open.front().givenCost = 0;
      open.front().heuristicCost = estimate(start, end);
      open.front().finalCost = givenCost + heuristicCost * hWeight;

	  while (!open.empty())
	  {
		PlannerNode *current = open.dequeue();
	    if current.vertex is the end, we’re done;
	    
	    for each edge of current.vertex:
	    {
	      SearchNode *successor = edge.endpoint;
	      float tempGivenCost = current.givenCost + edge.cost;
	      
	      if (visited[successor] != NULL)
	      {
	        PlannerNode *node = visted[successor];
	        
	        if (tempGivenCost < node.givenCost)
	        {
	          open.remove(node);
	          node.givenCost = tempGivenCost;
	          node.finalCost = node.givenCost + node.heuristicCost * hWeight;
	          node.parent = current;
	          open.enqueue(node);
	        }
	      }
	      
	      else
	      {
	        node = new PlannerNode(successor);
	        node.givenCost = tempGivenCost;
	        node.heuristicCost = estimate(successor, end);
	        node.finalCost = node.givenCost + node.heuristicCost * hWeight;
	        node.parent = current;
	        visited[successor] = node;
	        open.enqueue(node);
	      }
	    }
	  }

	*/

	return path;
}