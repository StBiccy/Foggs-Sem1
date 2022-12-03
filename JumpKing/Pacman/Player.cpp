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
	// animation
	_hitWall = false;
	_walking = false;
	_jumping = false;
	_walkFrameTime = 0;
	_walkFrame = 1;
	_direction = 0;

	// Load Jump King
	_playerTexture = new Texture2D();
	_playerTexture->Load("Content/Textures/JumpKing.png", false);
	_playerPosition = new Vector2(350.0f, 350.0f);
	_playerSourceRect = new Rect(0.0f, 0.0f, 32, 32);

	//Set menu Paramters
	_menuBackground = new Texture2D();
	_menuBackground->Load("Content/Textures/Transparency.PNG", false);
	_menuRectangle = new Rect(0.0f, 0.0f, Graphics::GetViewportWidth(), Graphics::GetViewportHeight());
	_menuStringPosition = new Vector2(Graphics::GetViewportWidth() / 2.0f, Graphics::GetViewportHeight() / 2.0f);

	_jump = false;

	// Set string position
	_stringPosition = new Vector2(10.0f, 25.0f);

	_velocity = new Vector2(0, 0);
	_grounded = false;

	// Load soundFX
	_landSFX = new SoundEffect();
	_landSFX->Load("Content/Sounds/Land.wav");
	_jumpSFX = new SoundEffect();
	_jumpSFX->Load("Content/Sounds/Jump.wav");
	_hitWallSFX= new SoundEffect();
	_hitWallSFX->Load("Content/Sounds/HitWall.wav");


	_lastGroundedTime = 0;


	_level = new Level();
	
	if (!Audio::IsInitialised)
	{
		std::cout << "audio not initialsiesed" << std::endl;
	}
	if (!_landSFX->IsLoaded())
	{
		std::cout << "land not loaded" << std::endl;
	}
}

void Player::PhysicsUpdate(int elapsedTime)
{

	if (!_grounded)
	{
		_velocity->Y = MathHelper::Clamp(_velocity->Y + _cGravity * elapsedTime, -_cMaxFallSpeed, _cMaxFallSpeed);
	}
	else
	{
		_velocity->Y = 0;
	}

	if (_jump)
	{
		Audio::Play(_jumpSFX);
		_velocity->Y -= _jumpValue;
		_jump = false;
		_jumpValue = 0;
		_velocity->X *= _cHorizontalJumpVel;
	}

	_playerPosition->Y += _velocity->Y;
	_playerPosition->X += _velocity->X;

	CollisionHandeler();
}

void Player::PlayerInput(Input::KeyboardState* keyboardState, int elapsedTime)
{
	if (!_paused && _canMove)
	{
		// Checks if D key is pressed
		if (keyboardState->IsKeyDown(Input::Keys::D))
		{
			_velocity->X = _cPlayerSpeed * elapsedTime; //Moves Pacman Towards the right
			_direction = 0;
			_walking = true;
		}
		else if (keyboardState->IsKeyDown(Input::Keys::A))
		{
			_velocity->X = -_cPlayerSpeed * elapsedTime;
			_direction = 2;
			_walking = true;
		}
		else
		{
			_walking = false;
			_velocity->X = 0;
		}

		if (_grounded)
		{
			if (keyboardState->IsKeyDown(Input::Keys::SPACE))
			{
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

			if (_spaceKeyDown && _grounded && keyboardState->IsKeyUp(Input::Keys::SPACE))
			{
				_jumping = true;
				_jump = true;
				_spaceKeyDown = false;
			}
		}
	}


	// pauses the game on the P input
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
	return Rect(_playerPosition->X, _playerPosition->Y, _playerSourceRect->Width, _playerSourceRect->Height);
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
	_canMove = false;


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
						if (_preBottom <= _tileBounds.Bottom())
						{
							_grounded = true;
							_canMove = true;

						}


						if (_collision == tileCollision::Impassable)
						{
							_playerPosition = new Vector2(_playerPosition->X, _playerPosition->Y + _depth.Y);
							_bounds = GetBoundingRect();
							if (!_grounded)
							{
								_velocity->Y = -_velocity->Y / 2;
								_hitWall = true;
								_hitWallCheck = true;

							}
						}
					}
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

	if (_grounded && _hitWall)
	{
		_hitWallCheck = false;
		_wallHitDelaySound = 0;
		_hitWall = false;
		_wallHitDelayAnim = 0;
	}

	_preTop = _bounds.Top();
	_preBottom = _bounds.Bottom();
}

void Player::PlayerAnim()
{
	if (_grounded && !_walking && !_spaceKeyDown)
	{
		_playerSourceRect->X = 0;
		_playerSourceRect->Y = _playerSourceRect->Height * _direction;

	}
	else if (_walking && _grounded && !_spaceKeyDown)
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
	else if (!_grounded && _hitWall)
	{
		_wallHitDelayAnim++;
		if (_wallHitDelayAnim > 2)
		{
			_playerSourceRect->Y = _playerSourceRect->Height + _playerSourceRect->Height * _direction;
			_playerSourceRect->X = _playerSourceRect->Width * 2;
		}
	}
	else if (_grounded && _spaceKeyDown)
	{
		_playerSourceRect->Y = _playerSourceRect->Height * 4;
		_playerSourceRect->X = 0;
	}
	else if (!_grounded && _velocity->Y < 0)
	{
		_playerSourceRect->Y = _playerSourceRect->Height + _playerSourceRect->Height * _direction;
		_playerSourceRect->X = _playerSourceRect->Width * 0;
	}
	else if (!_grounded && _velocity->Y > 0)
	{
		_playerSourceRect->Y = _playerSourceRect->Height + _playerSourceRect->Height * _direction;
		_playerSourceRect->X = _playerSourceRect->Width * 1;
	}
}

void Player::Update(int elapsedTime)
{
	PhysicsUpdate(elapsedTime);
	PlayerInput(Input::Keyboard::GetState(), elapsedTime);
	PlayerAnim();

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

void Player::Draw(int elapsedTime)
{
	// Allows us to easily create a string
	std::stringstream stream;
	stream << "Player X: " << _playerPosition->X << " Y: " << _playerPosition->Y;

	SpriteBatch::BeginDraw(); // Starts Drawing
	SpriteBatch::Draw(_playerTexture, _playerPosition, _playerSourceRect); // Draws Pacman
	_level->Draw(elapsedTime);

	// Draws the pause menu when _paused is true
	if(_paused)
	{
		std::stringstream menuStream;
		menuStream << "PAUSED!";

		SpriteBatch::Draw(_menuBackground, _menuRectangle, nullptr);
		SpriteBatch::DrawString(menuStream.str().c_str(), _menuStringPosition, Color::Red);
	}

	// Draws String
	SpriteBatch::DrawString(stream.str().c_str(), _stringPosition, Color::Green);
	SpriteBatch::EndDraw(); // Ends Drawing

}