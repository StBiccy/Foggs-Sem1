#include "Player.h"
#include "RectExtensions.h"
#include <iostream>

#include <sstream>

const float Player::_cGravity = 0.02f;
const float Player::_cMaxFallSpeed = 20;
const float Player::_cMaxJumpValue = 13;

Player::Player(int argc, char* argv[]) : Game(argc, argv), _cPlayerSpeed(0.2f)
{
	_paused = false;

	//Initialise important Game aspects
	Graphics::Initialise(argc, argv, this, 800, 800, false, 25, 25, "Funny Name", 60);
	Input::Initialise();

	// Start the Game Loop - This calls Update and Draw in game loop
	Graphics::StartGameLoop();
}

Player::~Player()
{
	delete _playerTexture;
	delete _playerSourceRect;
	delete _menuBackground;
	delete _menuRectangle;

	delete _velocity;
}

void Player::LoadContent()
{
	// Load Pacman
	_playerTexture = new Texture2D();
	_playerTexture->Load("Content/Textures/Pacman.tga", false);
	_playerPosition = new Vector2(350.0f, 350.0f);
	_playerSourceRect = new Rect(0.0f, 0.0f, 32, 32);

	//Set menu Paramters
	_menuBackground = new Texture2D();
	_menuBackground->Load("Content/Textures/Transparency.PNG", false);
	_menuRectangle = new Rect(0.0f, 0.0f, Graphics::GetViewportWidth(), Graphics::GetViewportHeight());
	_menuStringPosition = new Vector2(Graphics::GetViewportWidth() / 2.0f, Graphics::GetViewportHeight() / 2.0f);

	_jump = false;
	_windJump = false;

	// Set string position
	_stringPosition = new Vector2(10.0f, 25.0f);

	_velocity = new Vector2(0, 0);
	_grounded = false;

	_level = new Level();
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
		_velocity->Y -= _jumpValue;
		_jump = false;
		_jumpValue = 0;
	}

	_playerPosition->Y += _velocity->Y;
	_playerPosition->X += _velocity->X;

	CollisionHandeler();
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
								_velocity->Y = -_velocity->Y/2;
							}
						}
					}
					else if (_collision == tileCollision::Impassable)
					{
						_playerPosition = new Vector2(_playerPosition->X + _depth.X, _playerPosition->Y);
						_bounds = GetBoundingRect();
						if (!_grounded)
						{
							_velocity->X = -_velocity->X;
						}
					}

				}
			}
		}
	}

	_preBottom = _bounds.Bottom();
}

void Player::PlayerInput(Input::KeyboardState* keyboardState, int elapsedTime)
{
	if (!_paused && _canMove)
	{
		// Checks if D key is pressed
		if (keyboardState->IsKeyDown(Input::Keys::D))
			_velocity->X = _cPlayerSpeed * elapsedTime; //Moves Pacman Towards the right
		else if (keyboardState->IsKeyDown(Input::Keys::A))
			_velocity->X = -_cPlayerSpeed * elapsedTime;
		else
			_velocity->X = 0;
	}


	// pauses the game on the P input
	if (keyboardState->IsKeyDown(Input::Keys::P) && !_pKeyDown)
	{
		_pKeyDown = true;
		_paused = !_paused;
	}
	if (keyboardState->IsKeyUp(Input::Keys::P))
		_pKeyDown = false;

	if (_grounded)
	{
		if (keyboardState->IsKeyDown(Input::Keys::SPACE))
		{
			_spaceKeyDown = true;
			if (_jumpValue < _cMaxJumpValue)
			{
				_jumpValue += 1;
				_windJump = true;
				_velocity->X = 0;

			}
			else
			{
				_jumpValue = _cMaxJumpValue;
				_windJump = false;
				_jump = true;
			}
		}
		if (keyboardState->IsKeyUp(Input::Keys::SPACE))
		{
			_jump = true;
			_windJump = false;
			_spaceKeyDown = false;
		}
	}
}

void Player::Update(int elapsedTime)
{
	PhysicsUpdate(elapsedTime);

	PlayerInput(Input::Keyboard::GetState(), elapsedTime);
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