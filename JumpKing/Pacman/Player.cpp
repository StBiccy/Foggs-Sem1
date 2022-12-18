#include "Player.h"
#include "RectExtensions.h"
#include <iostream>

#include <sstream>

const float Player::_cGravity = 0.02f;
const float Player::_cMaxFallSpeed = 20;
const float Player::_cMaxJumpValue = 11;
const float Player::_cHorizontalJumpVel = 2;


Player::Player(int argc, char* argv[]) : Game(argc, argv), _cPlayerSpeed(0.1f)
{
	_paused = false;

	//Initialise important Game aspects
	Audio::Initialise();
	Graphics::Initialise(argc, argv, this, 480, 480, false, 25, 25, "Babe of the the void", 60);
	Input::Initialise();

	// Start the Game Loop - This calls Update and Draw in game loop
	Graphics::StartGameLoop();
}

Player::~Player()
{
	
	delete _jumpSFX;
	delete _hitWallSFX;
	delete _landSFX;
	delete _playerTexture;
	delete _playerSourceRect;
	delete _menuBackground;
	delete _menuRectangle;

	delete _velocity;
}

void Player::LoadContent()
{

	// Set up animation values
	_hitWall = false;
	_walking = false;
	_jumping = false;
	_faceFlat = false;
	_fallTimer = 0;
	_standUpDelay = 0;
	_walkFrameTime = 0;
	_walkFrame = 1;
	_direction = 0;

	// Load Jump King
	_playerTexture = new Texture2D();
	_playerTexture->Load("Content/Textures/JumpKing.png", false);
	_playerPosition = new Vector2(32.0f, 416.0f);
	_playerSourceRect = new Rect(0.0f, 0.0f, 32, 32);
	_velocity = new Vector2(0, 0);


	//Set menu Paramters
	_menuBackground = new Texture2D();
	_menuBackground->Load("Content/Textures/Transparency.PNG", false);
	_menuRectangle = new Rect(0.0f, 0.0f, Graphics::GetViewportWidth(), Graphics::GetViewportHeight());
	_menuStringPosition = new Vector2(Graphics::GetViewportWidth() / 2.0f, Graphics::GetViewportHeight() / 2.0f);

	// Set up physics checks
	_jump = false;
	_grounded = false;

	// Load soundFX
	_landSFX = new SoundEffect();
	_landSFX->Load("Content/Sounds/Land.wav");
	_jumpSFX = new SoundEffect();
	_jumpSFX->Load("Content/Sounds/Jump.wav");
	_hitWallSFX= new SoundEffect();
	_hitWallSFX->Load("Content/Sounds/HitWall.wav");

	// Set up defualt start scene scene
	_currentScene = 5;


	_lastGroundedTime = 0;


	_level = new Level();

	// Reads last save file
	_recallSave.open("Save");
	if(_recallSave.is_open())
	{
		_recallSave >> _playerPosition->X;
		_recallSave >> _playerPosition->Y;
		_recallSave >> _currentScene;
	}
}

void Player::PhysicsUpdate(int elapsedTime)
{
	// Applies Gravity
	if (!_grounded)
	{
		_velocity->Y = MathHelper::Clamp(_velocity->Y + _cGravity * elapsedTime, -_cMaxFallSpeed, _cMaxFallSpeed);
	}
	else
	{
		_velocity->Y = 0;
	}

	// starts a jump
	if (_jump)
	{
		_velocity->Y -= _jumpValue;
		_jump = false;
		_jumpValue = 0;
		_velocity->X *= _cHorizontalJumpVel;
	}

	// Applies velocity to the players position;
	_playerPosition->Y += _velocity->Y;
	_playerPosition->X += _velocity->X;

	CollisionHandeler();
}

void Player::PlayerInput(Input::KeyboardState* keyboardState, int elapsedTime)
{
	// chekcs if the game is paused and the player is grounded.
	if (!_paused && _grounded)
	{
		// Checks if the player has a stand up delay
		if (_standUpDelay < 0)
		{
			// Applies Vectical movement to the player if A/D is held.
			if (keyboardState->IsKeyDown(Input::Keys::D))
			{
				_velocity->X = _cPlayerSpeed * elapsedTime;
				_direction = 0;
				_walking = true;
				_faceFlat = false;
			}
			else if (keyboardState->IsKeyDown(Input::Keys::A))
			{
				_velocity->X = -_cPlayerSpeed * elapsedTime;
				_direction = 2;
				_walking = true;
				_faceFlat = false;
			}
			else
			{
				_walking = false;
				_velocity->X = 0;
			}

			// If space is held then it loads a jump untill either the player lets go of space or the jump value reaches it's max value.
			if (keyboardState->IsKeyDown(Input::Keys::SPACE))
			{
				_faceFlat = false;
				_spaceKeyDown = true;
				if (_jumpValue < _cMaxJumpValue)
				{
					_jumpValue += 0.8f;
					_velocity->X = 0;
				}
				else
				{
					_jumping = true;
					_jumpValue = _cMaxJumpValue;
					_jump = true;
					_spaceKeyDown = false;
				}
			}
		}
		else
		{
			// stops the player in the x axis and winds dow the stand up delay
			_standUpDelay -= (float)elapsedTime / 1000;
			_velocity->X = 0;
		}
		// checks if the player let go of space and applies the related cehcks
		if (_spaceKeyDown && _grounded && keyboardState->IsKeyUp(Input::Keys::SPACE))
		{
			_jumping = true;
			_jump = true;
			_spaceKeyDown = false;
		}
	}

	// Saves the game when S is pressed and the player is grounded.
	if (_grounded && keyboardState->IsKeyDown (Input::Keys::S))
	{
		_sendSave.open("Save");

		_sendSave << _playerPosition->X << endl<< _playerPosition->Y << endl << _currentScene;

		_sendSave.close();
	}

	// pauses the game when p is pressed, and unpauses if p is pressed again.
	if (keyboardState->IsKeyDown(Input::Keys::P) && !_pKeyDown)
	{
		_pKeyDown = true;
		_paused = !_paused;
	}
	if (keyboardState->IsKeyUp(Input::Keys::P))
		_pKeyDown = false;
}

Rect Player::GetBoundingRect()
{
	// Finds the players position relitive to the current scene the player is on
	int camYPos = _currentScene * 480;
	return Rect(_playerPosition->X, _playerPosition->Y+camYPos, _playerSourceRect->Width, _playerSourceRect->Height);
}

void Player::CollisionHandeler()
{
	// finds the all tiles around the player
	Rect _bounds = GetBoundingRect();
	int _leftTile = (int)floorf((float)_bounds.Left() / Tile::_cWidth);
	int _rightTile = (int)ceilf(((float)_bounds.Right() / Tile::_cWidth)) - 1;
	int _topTile = (int)floorf((float)_bounds.Top() / Tile::_cHeight);
	int _bottomTile = (int)ceilf(((float)_bounds.Bottom() / Tile::_cHeight)) - 1;

	_grounded = false;


	// loop through all Tiles around player
	for (int y = _topTile; y <= _bottomTile; ++y)
	{
		for (int x = _leftTile; x <= _rightTile; ++x)
		{

			// finds the tile type
			tileCollision _collision = _level->GetCollision(x, y);
			if (_collision != tileCollision::Passable)
			{
				// finds how far into the tile the player is
				Rect _tileBounds = _level->GetBounds(x, y);
				Vector2 _depth = RectangleExtensions::GetInersectionDepth(&_bounds, &_tileBounds);

				// check if tiles is passable or not
				if (_depth != *Vector2::Zero)
				{
					// finds the abslout depth
					float _absDepthX = abs(_depth.X);
					float _absDepthY = abs(_depth.Y);

					// check if player is colldied more with the y axis or x axis of the tile
					// will push player back from the resulting side
					if (_absDepthY < _absDepthX)
					{
						// If the space bellow the player is the same space bellow them bellow them last frame then they are grounded
						if (_preBottom <= _tileBounds.Bottom())
						{
							_grounded = true;
						}

						// If the player hits an impassable object on the Y axis then the get pushed out of it and rebouned in the opposing direction
						if (_collision == tileCollision::Impassable)
						{
							_playerPosition = new Vector2(_playerPosition->X, _playerPosition->Y + _depth.Y);
							_bounds = GetBoundingRect();
							if (!_grounded)
							{
								_velocity->Y = -_velocity->Y;
								_hitWall = true;
								_hitWallCheck = true;
							}
						}
					}
					// if the player hits an impassable object in the X axis then they get pushed out of it and rebounded in the opposing direction
					else if (_collision == tileCollision::Impassable && _absDepthY > _absDepthX)
					{
						_playerPosition = new Vector2(_playerPosition->X + _depth.X, _playerPosition->Y);
						_bounds = GetBoundingRect();

						if (!_grounded)
						{
							_velocity->X = -_velocity->X;
							_hitWall = true;
							_hitWallCheck = true;

						}
					}

				}
			}
		}
	}

	// Resets hit wall checks if the player is grounded
	if (_grounded && _hitWall)
	{
		_hitWallCheck = false;
		_wallHitDelaySound = 0;
		_hitWall = false;
		_wallHitDelayAnim = 0;
	}

	// Finds the previouse space bellow the player
	_preBottom = _bounds.Bottom();
}

void Player::PlayerAnim()
{
	if (_grounded && !_walking && !_spaceKeyDown && !_faceFlat)
	{
		_playerSourceRect->X = 0;
		_playerSourceRect->Y = _playerSourceRect->Height * _direction;

	}
	else if (_walking && _grounded && !_spaceKeyDown && !_faceFlat)
	{
		_walkFrameTime++;

		if (_walkFrameTime == 6)
		{
			_walkFrame++;
			_playerSourceRect->Y = _playerSourceRect->Height * _direction;

			if (_walkFrame > 4)
				_walkFrame = 1;

			if (_walkFrame < 4)
				_playerSourceRect->X = _playerSourceRect->Width * _walkFrame;
			else if (_walkFrame = 4)
				_playerSourceRect->X = _playerSourceRect->Width * (_walkFrame - 2);

			_walkFrameTime = 0;
		}
	}
	else if (!_grounded && _hitWall && !_faceFlat)
	{
		_wallHitDelayAnim++;
		if (_wallHitDelayAnim > 2)
		{
			_playerSourceRect->Y = _playerSourceRect->Height + _playerSourceRect->Height * _direction;
			_playerSourceRect->X = _playerSourceRect->Width * 2;
		}
	}
	else if (_grounded && _spaceKeyDown && !_faceFlat)
	{
		_playerSourceRect->Y = _playerSourceRect->Height * 4;
		_playerSourceRect->X = 0;
	}
	else if (!_grounded && _velocity->Y < 0 && !_faceFlat)
	{
		_playerSourceRect->Y = _playerSourceRect->Height + _playerSourceRect->Height * _direction;
		_playerSourceRect->X = _playerSourceRect->Width * 0;
	}
	else if (!_grounded && _velocity->Y > 0 && !_faceFlat)
	{
		_playerSourceRect->Y = _playerSourceRect->Height + _playerSourceRect->Height * _direction;
		_playerSourceRect->X = _playerSourceRect->Width * 1;
	}
	else if (_faceFlat && _grounded)
	{
		_playerSourceRect->Y = _playerSourceRect->Height + _playerSourceRect->Height * _direction;
		_playerSourceRect->X = _playerSourceRect->Width * 3;
	}
}

void Player::PlayerSound()
{
	if (_jump)
	{
		Audio::Play(_jumpSFX);
	}
	if (!_grounded && _hitWallCheck)
	{
		_wallHitDelaySound++;
		if (_wallHitDelaySound > 2)
		{
			Audio::Play(_hitWallSFX);
			_wallHitDelaySound = 0;
			_hitWallCheck = false;
		}
	}
	if (!_grounded)
	{
		_lastGroundedTime++;
	}
	else if (_grounded && _lastGroundedTime > 2)
	{
		Audio::Play(_landSFX);
		_lastGroundedTime = 0;
	}
	else
	{
		_lastGroundedTime = 0;
	}
}

void Player::Update(int elapsedTime)
{
	PlayerInput(Input::Keyboard::GetState(), elapsedTime);

	// stops anything running if the game is paused to imitate the effect of no the world being freezed.
	if (!_paused)
	{
		PlayerSound();
		PhysicsUpdate(elapsedTime);
		PlayerAnim();

		// If the player is falling then it checks if the are going to fall face flat.
		if (!_grounded && _velocity->Y >= 0)
		{
			FaceFlatCheck(elapsedTime);
		}
		else 
		{
			_fallTimer = 0;
		}

		//Cheks if player is entering a new scene or not, and assigns the player possiton and current scene accordingly.
		if (_playerPosition->Y <= -32)
		{
			--_currentScene;
			_playerPosition->Y = 448;
		}
		if (_playerPosition->Y >= 480)
		{
			++_currentScene;
			_playerPosition->Y = 0;
		}
	}
}

void Player::FaceFlatCheck(int elapsedTime)
{
	// fallTimer represent the time spent falling in seconds.
	_fallTimer += (float)elapsedTime / 1000;

	if (_fallTimer >= 0.75)
	{
		_standUpDelay = .5f;
		_faceFlat = true;
	}
}

void Player::Draw(int elapsedTime)
{
	SpriteBatch::BeginDraw(); // Starts Drawing
	_level->Draw(elapsedTime); // Calls draw function in the level cpp
	SpriteBatch::Draw(_playerTexture, _playerPosition, _playerSourceRect); // Draws Pacman


	// Draws the pause menu when _paused is true
	if(_paused)
	{
		std::stringstream menuStream;
		menuStream << "PAUSED!";

		SpriteBatch::Draw(_menuBackground, _menuRectangle, nullptr);
		SpriteBatch::DrawString(menuStream.str().c_str(), _menuStringPosition, Color::Red);
	}

	SpriteBatch::EndDraw(); // Ends Drawing

}

