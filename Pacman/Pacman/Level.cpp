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

int Level::GetHight()
{
	return _tiles->at(0).size();
}

void Level::LoadTiles()
{
	// try to understand this a little more than you already do
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

	_tiles = new vector<vector<Tile*>>(_width, vector<Tile*>(_lines->size()));

	for (int y = 0; y < GetHight(); ++y)
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

Tile* Level::LoadTile(const char* name, tileCollision collision)
{
	stringstream ss;
	ss << "content/Tile/" << name << ".png";
	Texture2D* tex = new Texture2D();
	tex->Load(ss.str().c_str(), false);

	return new Tile(tex, collision);
}

//remember where you were


