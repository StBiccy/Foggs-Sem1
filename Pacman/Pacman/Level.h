#pragma once

#include "Tile.h"

#include "S2D\S2D.h"

//key note learn what these libaries include
#include <vector>
#include <stdlib.h>

using namespace S2D;
using namespace std;

struct Tile;

class Level
{
private:
	vector<vector<Tile*>>* _tile;

public:
	Level(int _levelIndex);
	~Level(void);

	void LoadTiles(int _levelIndex);
	Tile* LoadTile(const char _tileType, int x, int y);
	Tile* LoadTile(const char* name, tileCollision collision);
	// rememeber you where here biccy
};
