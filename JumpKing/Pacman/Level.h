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
	// Represents the tiles
	vector<vector<Tile*>>* _tiles;
	// Represents a what the player can see.
	Rect* _camera;

	//Background values;
	Texture2D* _backgroundTex;
	Rect* _backgroundRect;
	int _bgCurrentFrame;
	int _bgCurrentFrameTime;

	SoundEffect* _bgTrack;

public:
	Level();
	~Level(void);

	/// <summary>Gets the height of the level relivent to the tiles</summary>
	/// <returns>Returns the level height</returns>
	int GetHeight();
	/// <summary>Gets the width of the level relivent to the tiles</summary>
	/// <returns>Returns the level width</returns>
	int GetWidth();

	void LoadTiles();
	Tile* LoadTile(const char _tileType, int x, int y);
	Tile* LoadTile(const char* name, tileCollision collision);

	// Gets the bounds of a tile
	Rect GetBounds(int x, int y);
	// Gets the collision type of a tile
	tileCollision GetCollision(int x, int y);

	/// <summary>Finds the bottom of the screen's bounds</summary>
	/// <returns>Returns the location of the of the screen bottom</returns>
	int GetBoundBottom();
	/// <summary>Finds the top of the screen's bounds</summary>
	/// <returns>Returns the location of the of the screen top</returns>
	int GetBoundTop();

	/// <summary>Finds the top poistion of the camera in realation to the tile's size</summary>
	/// <returns>Camera's top positon</returns>
	int CameraTop();
	/// <summary>Finds the bottom poistion of the camera in realation to the tile's size</summary>
	/// <returns>Camera's bottom positon</returns>
	int CameraBottom();

	/// <summary>Loads the content</summary>
	void LoadContent();

	/// <summary>Draws to screen every fram</summary>
	/// <param name="elapsedTime:">The time in seconds it take to load next fram</param>
	void Draw(int elapsedTime);
	/// <summary>Draws tiles to screen</summary>
	void DrawTiles();
	/// <summary>Animate the background</summary>
	void AnimateBackground();
};
