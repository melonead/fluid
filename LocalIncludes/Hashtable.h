#ifndef HASHTABLE_H
#define HASHTABLE_H

#include <cstdint>
#include <vector>
#include <iostream>
#include <glm/vec2.hpp>
#include "Settings.h"

const int P1 = 73856093;
const int P2 = 19349663;

int cellDisplacements[9][2] = {{0, 0}, {-1, 0}, {1, 0}, {0, -1}, {0, 1}, {-1, 1}, {1, 1}, {-1, -1}, {1, -1}};

// hash: compute hash key from the position of a particle
static int tableHash(glm::vec2 cellPos)
{
	int x = cellPos.x;
	int y = cellPos.y;
	
	return abs(((x * P1) + ( y * P2)) % NUMCELLS);
}

// getCellPosition: convert world position to cell position
static glm::vec2 getCellPosition (double position[], double radius)
{
	// discretize particle position from float to int
	int x = floor(position[0] / radius);
	int y = floor(position[1] / radius);
	return glm::vec2(x, y);
}

// vector<int> (&gr)[10]
// insertInMap: inserts the particle into the hashtable
static void insertInCell(Particle &p, std::vector<Particle> (& Table)[NUMCELLS])
{
	glm::vec2 cellPos = getCellPosition(p.pos, IRADIUS);
	int key = tableHash(cellPos);
	Table[key].emplace_back(p);
	p.index = Table[key].size() - 1;
	//std::cout << p.index << std::endl;
	p.key = key;
}

static int computeKey(double position[])
{
	glm::vec2 cellPos = getCellPosition(position, IRADIUS);
	int key = tableHash(cellPos);
	return key;
}

static void clearTable(std::vector<Particle>(&Table)[NUMCELLS])
{
	for (int i = 0; i < NUMCELLS; i++)
	{
		Table[i].clear();
	}
}

static void getNeighbors(double position[], std::vector<Particle>& nbs, std::vector<Particle>(&Table)[NUMCELLS], double radius)
{
	glm::vec2 cellPos;
	glm::vec2 bcellPos = getCellPosition(position, radius);
	int key;
	for (int i = 0; i < 9; i++)
	{
		cellPos.x = bcellPos.x + cellDisplacements[i][0];
		cellPos.y = bcellPos.y + cellDisplacements[i][1];
		key = tableHash(cellPos);
		int size = Table[key].size();
		if (size == 0)
			continue;
		for (int j = 0; j < size; j++)
		{
			Particle &nb = Table[key][j];
			double xDist = nb.pos[0] - position[0];
			double yDist = nb.pos[1] - position[1];
			if ((xDist * xDist + yDist * yDist) < (radius * radius))
				nbs.push_back(nb);
		}

	}
}



#endif // !HASHTABLE_H
