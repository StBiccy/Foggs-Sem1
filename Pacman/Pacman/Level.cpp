#include "Level.h"

#include <string>
#include <sstream>
#include <iostream>
#include <fstream>

Level::Level()
{
	LoadTiles();
}

Level::~Level(void)
{

	// i need to figure out how this works
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

void Level::LoadTiles()
{

	// all ik is this has something to do with loading the level
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

	//_tiles has the first vector set to that of the width 
	// //(i feel this should mean the width of the grid but idk)
	// 
	//the second vector is set to the size of _lines 
	//(i feel this should mean hight but idk rn)
	_tiles = new vector<vector<Tile*>>(_width, vector<Tile*>(_lines->size()));

	// this loops over every tile position
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
//the texture loads the file using the string constructed in the stringstream.
//re
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


void Level::Draw(int elapsedTime)
{
	DrawTiles();
}

void Level::DrawTiles()
{
	for (int y = 0; y < GetHeight(); ++y)
	{
		for (int x = 0; x < GetWidth(); ++x)
		{
			// Checks if there is an assienged tile in the text file
			Texture2D* texture = _tiles->at(x).at(y)->_texture;
			if (texture != nullptr)
			{
				// Draw it in screen space.
				Vector2 position((float)x, (float)y);
				position *= *Tile::_cSize;
				SpriteBatch::Draw(texture, &position);
			}
		}
	}
}
