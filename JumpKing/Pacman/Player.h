#pragma once

#ifdef WIN32 
	#ifndef _DEBUG
		#pragma comment(linker, "/SUBSYSTEM:windows /ENTRY:mainCRTStartup")
	#endif
#endif

#include "S2D/S2D.h"
#include <fstream>;
#include "Level.h"

using namespace S2D;

class Player : public Game
{
private:
	// reference to level class in Level.h file
	Level* _level;

	/// represents current scene the player is on (0 being the top)
	int _currentScene;

	// Save file values
	ifstream _recallSave;
	ofstream _sendSave;

	// Walking animation values
	bool _walking;
	int _walkFrame;
	int _walkFrameTime;
	// Jump/falling animation values
	bool _jumping;
	float _jumpAnimDelay;
	// Animation values for hitting wall
	bool _hitWall;
	int _wallHitDelayAnim;
	// Face flat animation values
	bool _faceFlat;
	float _fallTimer;
	float _standUpDelay;

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

	/// <summary> Update the game every frame</summary>
	/// <param name="elapsedTime:"> The time it took in miliseconds for the current frame to load</param>
	void virtual Update(int elapsedTime);

	/// <summary> Draw to the screen every frame</summary>
	/// <param name="elapsedTime:"> The time it took in miliseconds for the current frame to load</param>
	void virtual Draw(int elapsedTime);

	/// <summary> Updates the players physiscs every time it's called (Do in update function)</summary>
	/// <param name="elapsedTime:"> The time it took in miliseconds for the current frame to load</param>
	void PhysicsUpdate(int elapsedTime);

	/// <summary>Checks for colisions every time it's called (Be called after Physics Updated)</summary>
	void CollisionHandeler();

	/// <summary>Finds the bounding rectangle of the player</summary>
	/// <returns>Player's boudning rect</returns>
	Rect GetBoundingRect();

	/// <summary>Dose Input checks every time it's called (Do in Update function)</summary>
	/// <param name="keyboardState:">Current Key Inputs</param>
	/// <param name="elapsedTime:">The time it took in miliseconds for the current frame to load</param>
	void PlayerInput(Input::KeyboardState* keyboardState, int elapsedTime);

	/// <summary>Updates the players animation every time it's called (Do in update function)</summary>
	void PlayerAnim();

	/// <summary>Calls player related sound effects (Do in update function)</summary>
	void PlayerSound();

	/// <summary>Check if player has fallen face flat (Do in update function)</summary>
	void FaceFlatCheck(int elapsedTime);
};