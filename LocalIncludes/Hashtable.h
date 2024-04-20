#ifndef HASHTABLE_H
#define HASHTABLE_H

#include <cstdint>
#include <vector>
#include <iostream>
#include <glm/vec2.hpp>
#include "Settings.h"

const int P1 = 73856093;
const int P2 = 19349663;

//int Indices[20 * 40]; // table of all the cell indices

// hash: compute hash key from the position of a particle
static int tableHash(glm::vec2 cellPos)
{
	int x = cellPos.x;
	int y = cellPos.y;
	
	return abs(((x * P1) + ( y * P2)) % NUMCELLS);
}

// getCellPosition: convert world position to cell position
static glm::vec2 getCellPosition (double position[])
{
	// discretize particle position from float to int
	int x = floor(position[0] / IRADIUS);
	int y = floor(position[1] / IRADIUS);
	return glm::vec2(x, y);
}

// keyIndex = 
// getNeighborKeys: compute and return the keys (Table keys) of neighbor cells
static int* getNeighborKeysV1(int keyIndex, int orderedCellKeys[])
{
	int keyIndexDisplacement[] = { -xRange, -xRange - 1, -xRange - 2, -1, 0, 1, xRange, xRange - 1, xRange - 2 };
	int nKeys[9];
	for (int i = 0; i < 9; i++)
	{
		int key = keyIndexDisplacement[i] + keyIndex;
		nKeys[i] = orderedCellKeys[key];
	}
	return nKeys;
}

// getNeighborKeys: compute and return the keys (Table keys) of neighbor cells
static void getNeighborKeysV2(double position[], int keys[])
{
	double d = IRADIUS;
	double disp[9][2] = { {0.0, 0.0}, {-d, 0}, {-d, d}, {0, d}, {d, d}, {d, 0}, {d, -d}, {0, -d}, {-d, -d} };
	glm::vec2 cellPos;
	for (int i = 0; i < 9; i++)
	{
		double np[] = { position[0] + disp[i][0], position[1] + disp[i][1] };
		cellPos = getCellPosition(np);
		int key = tableHash(cellPos);
		keys[i] = key;
	}
}

// vector<int> (&gr)[10]
// insertInMap: inserts the particle into the hashtable
static void insertInCell(Particle *p, std::vector<Particle*> (& Table)[NUMCELLS])
{
	glm::vec2 cellPos = getCellPosition(p->pos);
	int key = tableHash(cellPos);
	Table[key].emplace_back(p);
	p->index = Table[key].size() - 1;
	//std::cout << p.index << std::endl;
	p->key = key;
}

static int computeKey(double position[])
{
	glm::vec2 cellPos = getCellPosition(position);
	int key = tableHash(cellPos);
	return key;
}

static void clearTable(std::vector<Particle*>(&Table)[NUMCELLS])
{
	for (int i = 0; i < NUMCELLS; i++)
	{
		Table[i].clear();
	}
}

static void getNeighbors(double position[], std::vector<Particle*>& nbs, std::vector<Particle*>(&Table)[NUMCELLS])
{
	int nkeys[] = { 0, 0, 0, 0, 0, 0, 0, 0, 0 };
	getNeighborKeysV2(position, nkeys);

	for (int i = 0; i < 9; i++)
	{
		int size = Table[nkeys[i]].size();
		if (size == 0)
			continue;
		for (int j = 0; j < size; j++)
		{
			nbs.emplace_back(Table[nkeys[i]][j]);
		}

	}
}



//static void remove(Particle& p, std::vector<Particle> cell)
//{
//	int key = std::find(0, cell.size(), p);
//	cell.erase(p);
//}

// generateBoundingBox: generates the bounding box around a particle
// it is within this bounding box that we search for the neighbors of
// the particle
//void generateBoundingBox(double position[])
//{
//	double bbMin[2] = { position[0] * (position[0] - IRADIUS), position[1] * (position[1] - IRADIUS) };
//	double bbMax[2] = { position[0] * (position[0] + IRADIUS), position[1] * (position[1] + IRADIUS) };
//
//}

#endif // !HASHTABLE_H
