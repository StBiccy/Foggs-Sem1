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
	vector<vector<Tile*>>* _tiles;

public:
	Level();
	~Level(void);

	int GetHeight();
	int GetWidth();

	void LoadTiles();
	Tile* LoadTile(const char _tileType, int x, int y);
	Tile* LoadTile(const char* name, tileCollision collision);
	tileCollision GetCollision(int x, int y);

	//void Update(int elapsedTime);
	void Draw(int elapsedTime);
	void DrawTiles();
};
