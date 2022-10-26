#pragma once

#include "S2D\S2D.h"

using namespace S2D;

enum class tileCollision
{
	Normal = 0,
};

struct Tile
{
public:
	Texture2D* _texture;

	static const int _cWidth;
	static const int _cHight;

	static const Vector2* _cSize;

	Tile(Texture2D* _Texture);
	~Tile(void)
};