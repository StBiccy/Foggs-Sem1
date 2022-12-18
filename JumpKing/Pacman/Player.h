#pragma once

#include "Level.h"

// If Windows and not in Debug, this will run without a console window
// You can use this to output information when debugging using cout or cerr
#ifdef WIN32 
	#ifndef _DEBUG
		#pragma comment(linker, "/SUBSYSTEM:windows /ENTRY:mainCRTStartup")
	#endif
#endif

// Just need to include main header file
#include "S2D/S2D.h"
<<<<<<< HEAD
=======
#include <fstream>;
>>>>>>> parent of 8ca258e... Finished Portfolio fn

// Reduces the amount of typing by including all classes in S2D namespace
using namespace S2D;

// Declares the Pacman class which inherits from the Game class.
// This allows us to overload the Game class methods to help us
// load content, draw and update our game.
class Player : public Game
{
private:
	Level* _level;

	int _currentScene;

<<<<<<< HEAD
=======
	ifstream recallSave;
	ofstream sendSave;

>>>>>>> parent of 8ca258e... Finished Portfolio fn
	// Data to represent animation
	bool _walking;
	bool _jumping;
	float _preTop;
	int _walkFrame;
	float _walkFrameTime;
	bool _hitWall;
	int _wallHitDelayAnim;
	float _jumpAnimDelay;

	// Data to represent Player
	Vector2* _playerPosition;
	Rect* _playerSourceRect;
	Texture2D* _playerTexture;
	const float _cPlayerSpeed;
	short unsigned int _direction;

	// Data to represent pause/start menu
	Texture2D* _menuBackground;
	Rect* _menuRectangle;
	Vector2* _menuStringPosition;
	bool _paused;

	// Position for String
	Vector2* _stringPosition;

	// input checks
	bool _pKeyDown;
	bool _spaceKeyDown;

	// Physics values
	Vector2* _velocity;
	static const float _cGravity;
	static const float _cMaxFallSpeed;
	bool _grounded;
	bool _canMove;
	float _preBottom;

	// Jump values
	static const float _cHorizontalJumpVel;
	float _jumpValue;
	bool _jump;
	static const float _cMaxJumpValue;

	// Sound effects
	SoundEffect* _jumpSFX;
	SoundEffect* _landSFX;
	SoundEffect* _hitWallSFX;
	int _lastGroundedTime;
	int _wallHitDelaySound;
	bool _hitWallCheck;



public:
	/// <summary> Constructs the Pacman class. </summary>
	Player(int argc, char* argv[]);

	/// <summary> Destroys any data associated with Pacman class. </summary>
	virtual ~Player();

	/// <summary> All content should be loaded in this method. </summary>
	void virtual LoadContent();
	void virtual Update(int elapsedTime);
	void virtual Draw(int elapsedTime);
	void PhysicsUpdate(int elapsedTime);
	void CollisionHandeler();
	Rect GetBoundingRect();
	void PlayerInput(Input::KeyboardState* keyboardState, int elapsedTime);
	void PlayerAnim();
};