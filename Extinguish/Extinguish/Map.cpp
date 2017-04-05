#include "Map.h"

Map::Map(int rows, int cols, float3 *positions)
{
	nodes.resize(rows);

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
	

	return nullptr;
}