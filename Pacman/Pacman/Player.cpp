#include "Player.h"
#include "RectExtensions.h"
#include <iostream>

#include <sstream>

const float Player::_cGravity = 0.07f;
const float Player::_cMaxFallSpeed = 100;

Player::Player(int argc, char* argv[]) : Game(argc, argv), _cPlayerSpeed(0.1f)
{
	_paused = false;

	//Initialise important Game aspects
	Graphics::Initialise(argc, argv, this, 800, 800, false, 25, 25, "MAX", 60);
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

	// Set string position
	_stringPosition = new Vector2(10.0f, 25.0f);

	_velocity = new Vector2(0, 0);

	_level = new Level();
}

void Player::PhysicsUpdate(int elapsedTime)
{
	//_velocity->Y = MathHelper::Clamp(_velocity->Y + _cGravity * elapsedTime ,-_cMaxFallSpeed, _cMaxFallSpeed);
	//_velocity->X = 0;
	//_playerPosition->Y += _velocity->Y;
	
	CollisionHandeler();
}

Rect Player::GetBoundingRect()
{
	return Rect(_playerPosition->X, _playerPosition->Y, _playerSourceRect->Width, _playerSourceRect->Height);
}

void Player::CollisionHandeler()
{
	Rect _bounds = GetBoundingRect();
	int _leftTile = (int)floorf((float)_bounds.Left() / Tile::_cWidth);
	int _rightTile = (int)ceilf(((float)_bounds.Right() / Tile::_cWidth)) - 1;
	int _topTile = (int)floorf((float)_bounds.Top() / Tile::_cHeight);
	int _bottomTile = (int)ceilf(((float)_bounds.Bottom() / Tile::_cHeight)) - 1;
	
	for (int y = _topTile; y <= _bottomTile; ++y)
	{
		for (int x = _leftTile; x <= _rightTile; ++x)
		{
			tileCollision _collision = _level->GetCollision(x, y);
			{
				if (_collision != tileCollision::Passable)
				{
					Rect _tileBounds = _level->GetBounds(x, y);
					Vector2 _depth = RectangleExtensions::GetInersectionDepth(&_bounds, &_tileBounds);
					if (_depth != *Vector2::Zero)
					{
						float absDepthX = abs(_depth.X);
						float absDepthY = abs(_depth.Y);

						// Resolve the collision along the shallow axis.
						if (absDepthY < absDepthX)
						{
							// Ignore platforms, unless we are on the ground.
							if (_collision == tileCollision::Impassable)
							{
								// Resolve the collision along the Y axis.
								_playerPosition = new Vector2(_playerPosition->X, _playerPosition->Y + _depth.Y);

								// Perform further collisions with the new bounds.
								_bounds = GetBoundingRect();
							}
						}
						else if (_collision == tileCollision::Impassable) // Ignore platforms.
						{
							// Resolve the collision along the X axis.
							_playerPosition = new Vector2(_playerPosition->X + _depth.X, _playerPosition->Y);

							// Perform further collisions with the new bounds.
							_bounds = GetBoundingRect();
						}
					}
				}
			}
		}
	}
}

void Player::PlayerInput(Input::KeyboardState* keyboardState, int elapsedTime)
{
	if (!_paused)
	{
		// Checks if D key is pressed
		if (keyboardState->IsKeyDown(Input::Keys::D))
			_playerPosition->X += _cPlayerSpeed * elapsedTime; //Moves Pacman Towards the right
		if (keyboardState->IsKeyDown(Input::Keys::A))
			_playerPosition->X -= _cPlayerSpeed * elapsedTime; //Moves Pacman Towards the left
		if (keyboardState->IsKeyDown(Input::Keys::W))
			_playerPosition->Y -= _cPlayerSpeed * elapsedTime; //Moves Pacman upwards
		if (keyboardState->IsKeyDown(Input::Keys::S))
			_playerPosition->Y += _cPlayerSpeed * elapsedTime; //Moves Pacman downwards

		// if the Player hits a wall it stops in place
		if (_playerPosition->X > Graphics::GetViewportWidth() - 32) //Right wall
			_playerPosition->X = Graphics::GetViewportWidth() - 32;

		if (_playerPosition->X < 0) //Left wall
			_playerPosition->X = 0;

		if (_playerPosition->Y > Graphics::GetViewportHeight() - 32) //Bottom wall
			_playerPosition->Y = Graphics::GetViewportHeight() - 32;

		//if (_playerPosition->Y < 0) //Top Wall
			//_playerPosition->Y = 0;
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