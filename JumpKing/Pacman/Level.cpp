#include "Level.h"

#include <string>
#include <sstream>
#include <iostream>
#include <fstream>

Level::Level()
{
	LoadTiles();
	LoadContent();
}

void Level::LoadContent()
{
	_camera = new Rect(0.0f, 480.0f,480, 480);
}

Level::~Level(void)
{	
	delete _camera;
	for (vector<vector<Tile*>>::iterator it = _tiles->begin(); it != _tiles->end(); it++)
	{
		for (vector<Tile*>::iterator it2 = it->begin(); it2 != it->end(); it2 ++)
		{
			delete *it2;
		}
	}
	delete _tiles;
}



int Level::GetWidth()
{
	return _tiles->size();
}

int Level::GetHeight()
{
	return _tiles->at(0).size();
}

int Level::GetBoundBottom()
{
	return (_camera->Y + _camera->Height) / 32;
}

int Level::GetBoundTop()
{
	return _camera->Y / 32;
}
// this assigness the width and hight by the size of the text
void Level::LoadTiles()
{
	int _width;
	vector<string>* _lines = new vector<string>();
	fstream _stream;
	stringstream _ss;
	_ss << "Content/Level/Level.txt";
	_stream.open(_ss.str(), fstream::in);

	char* _line = new char[256];
	_stream.getline(_line, 256);
	string* _sline = new string(_line);
	_width = _sline->size();
	while (!_stream.eof())
	{
		_lines->push_back(*_sline);
		_stream.getline(_line, 256);
		delete _sline;
		_sline = new string(_line);
	}

	delete [] _line;
	delete _sline;

	//_tiles has the first vector set to that of the width, through the use of the _width value which represents the ammout of characters in the first row.
	//the second vector is set to the size of _lines which represensts the ammout of lines in the text file.
	_tiles = new vector<vector<Tile*>>(_width, vector<Tile*>(_lines->size()));

	// this loops over every tile position and sets the character found there as the char _tileType
	for (int y = 0; y < GetHeight(); ++y)
	{
		for (int x = 0; x < GetWidth(); x++)
		{
			char _tileType = _lines->at(y)[x];
			(*_tiles)[x][y] = LoadTile(_tileType, x, y);
		}
	}

	delete _lines;
}

//check what type of tile is being held in the _tileType value
Tile* Level::LoadTile(const char _tileType, int x, int y)
{

	switch (_tileType)
	{
	case '.':
		return new Tile(nullptr, tileCollision::Passable);

	case '_':
		return LoadTile("Wall", tileCollision::Impassable);

	default:
		return nullptr;
	}
}

//stringsream stors the file location as a string. turns the with the texture and the tileCollision information that was passed down in the function's parameter
Tile* Level::LoadTile(const char* name, tileCollision collision)
{
	stringstream ss;
	ss << "content/Tiles/" << name << ".png";
	Texture2D* tex = new Texture2D();
	tex->Load(ss.str().c_str(), false);

	return new Tile(tex, collision);
}

Rect Level::GetBounds(int x, int y)
{
	return Rect((float)(x * Tile::_cWidth), (float)(y * Tile::_cHeight), Tile::_cWidth, Tile::_cHeight);
}

tileCollision Level::GetCollision(int x, int y)
{
	// Prevent escaping past the level ends.
	if (x < 0 || x >= GetWidth())
		return tileCollision::Impassable;
	// Allow jumping past the level top and falling through the bottom.
	if (y < GetBoundTop()) 
	{
		_camera->Y -= 480;
		return tileCollision::Passable;
	}
	if (y >= GetBoundBottom())
	{
		_camera->Y += 480;
		return tileCollision::Passable;
	}

	return _tiles->at(x).at(y)->_collision;
}

int Level::CameraTop()
{
	int topPos = _camera->Y/ 32;
	return topPos;
}

int Level::CameraBottom()
{
	int bottomPos = ((_camera->Y) + _camera->Height) / 32;
	return bottomPos;
}

void Level::Draw(int elapsedTime)
{
	DrawTiles();
}

void Level::DrawTiles()
{
	for (int y = CameraTop(); y < CameraBottom(); ++y)
	{
		for (int x = 0; x < GetWidth(); ++x)
		{
			// Checks if there is an assienged tile in the text file
			Texture2D* texture = _tiles->at(x).at(y)->_texture;
			if (texture != nullptr)
			{
				// Draw it in screen space.
				float yDraw;
				yDraw = y - _camera->Y/32;
				Vector2 position((float)x, yDraw);
				position *= *Tile::_cSize;
				SpriteBatch::Draw(texture, &position);
			}
		}
	}
}
