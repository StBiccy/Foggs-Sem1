#pragma once

// If Windows and not in Debug, this will run without a console window
// You can use this to output information when debugging using cout or cerr
#ifdef WIN32 
	#ifndef _DEBUG
		#pragma comment(linker, "/SUBSYSTEM:windows /ENTRY:mainCRTStartup")
	#endif
#endif

// Just need to include main header file
#include "S2D/S2D.h"

// Reduces the amount of typing by including all classes in S2D namespace
using namespace S2D;

// Declares the Pacman class which inherits from the Game class.
// This allows us to overload the Game class methods to help us
// load content, draw and update our game.
class Player : public Game
{
private:
	// Data to represent Player
	Vector2* _playerPosition;
	Rect* _playerSourceRect;
	Texture2D* _playerTexture;
	const float _cPlayerSpeed;

	// Data to represent Munchie
	int _frameCount;
	Rect* _munchieRect;
	Texture2D* _munchieBlueTexture;
	Texture2D* _munchieInvertedTexture;

	// Data to represent pause/start menu
	Texture2D* _menuBackground;
	Rect * _menuRectangle;
	Vector2* _menuStringPosition;
	bool _paused;

	// Position for String
	Vector2* _stringPosition;

	bool _pKeyDown;
	
	// Physics values
	Vector2* _velocity;
	static const float _gravity;
	static const float _maxFallSpeed;


public:
	/// <summary> Constructs the Pacman class. </summary>
	Player(int argc, char* argv[]);

	/// <summary> Destroys any data associated with Pacman class. </summary>
	virtual ~Player();

	//void SetVelocity(Vector2 velocity);
	//Vector2* GetVelocity();

	/// <summary> All content should be loaded in this method. </summary>
	void virtual LoadContent();

	void virtual Update(int elapsedTime);
	void virtual Draw(int elapsedTime);
	void virtual PhysicsUpdate(int elapsedTime);
};