#include "Tile.h"

const int Tile::_cHight = 32;
const int Tile::_cWidth = 32;

const Vector2* Tile::_cSize = new Vector2((float)_cWidth, (float)_cHight);

Tile::Tile(Texture2D* _Texture)
{
	_texture = _Texture;
}

Tile::~Tile(void)
{
	delete _texture;
}