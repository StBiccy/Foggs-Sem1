#pragma once

#include "S2D\S2D.h"

using namespace S2D;

/// <summaryRepresents Collision type</summary>
enum class tileCollision
{
	Passable = 0, // Player can go throug

	Impassable = 1, // Player can't go through
};

struct Tile
{
public:
	//Values for a type
	Texture2D* _texture;
	tileCollision _collision;

	//Represents the consistent mesuments of a tile
	static const int _cWidth;
	static const int _cHeight;
	static const Vector2* _cSize;

	Tile(Texture2D* _Texture, tileCollision _Collision);
	~Tile(void);
};