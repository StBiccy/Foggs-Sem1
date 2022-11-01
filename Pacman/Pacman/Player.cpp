#include "Player.h"

#include <sstream>

Player::Player(int argc, char* argv[]) : Game(argc, argv), _cPlayerSpeed(0.1f), _cPlayerFrameTime(250)
{
	_frameCount = 0;
	_paused = false;
	
	
	//Initialise important Game aspects
	Graphics::Initialise(argc, argv, this, 1024, 768, false, 25, 25, "Pacman", 60);
	Input::Initialise();

	// Start the Game Loop - This calls Update and Draw in game loop
	Graphics::StartGameLoop();
}

Player::~Player()
{
	delete _playerTexture;
	delete _playerSourceRect;
	delete _munchieBlueTexture;
	delete _munchieInvertedTexture;
	delete _munchieRect;
}

void Player::LoadContent()
{
	// Load Pacman
	_playerTexture = new Texture2D();
	_playerTexture->Load("Textures/Pacman.tga", false);
	_playerPosition = new Vector2(350.0f, 350.0f);
	_playerSourceRect = new Rect(0.0f, 0.0f, 32, 32);
	_playerCurrentFrameTime = 0;
	_playerFrame = 0;
	
	//_playerDirection = 0;

	// Load Munchie
	_munchieBlueTexture = new Texture2D();
	_munchieBlueTexture->Load("Textures/Munchie.tga", true);
	_munchieInvertedTexture = new Texture2D();
	_munchieInvertedTexture->Load("Textures/MunchieInverted.tga", true);
	_munchieRect = new Rect(100.0f, 450.0f, 12, 12);

	//Set menu Paramters

	_menuBackground = new Texture2D();
	_menuBackground->Load("Textures/Transparency.PNG", false);
	_menuRectangle = new Rect(0.0f, 0.0f, Graphics::GetViewportWidth(), Graphics::GetViewportHeight());
	_menuStringPosition = new Vector2(Graphics::GetViewportWidth() / 2.0f, Graphics::GetViewportHeight() / 2.0f);

	// Set string position
	_stringPosition = new Vector2(10.0f, 25.0f);
}

void Player::Update(int elapsedTime)
{
	Input::KeyboardState* keyboardState = Input::Keyboard::GetState();

	CheckPaused(keyboardState, Input::Keys::P);

	if (!_paused)
	{
		Input(elapsedTime, keyboardState);
		UpdatePlayer(elapsedTime);
		UpdateMunchie(elapsedTime);
		CheckViewportCollision();

		_frameCount++;
	}
}

void Player::Draw(int elapsedTime)
{
	// Allows us to easily create a string
	std::stringstream stream;
	stream << "Pacman X: " << _playerPosition->X << " Y: " << _playerPosition->Y;

	SpriteBatch::BeginDraw(); // Starts Drawing
	SpriteBatch::Draw(_playerTexture, _playerPosition, _playerSourceRect); // Draws Pacman

	if (_frameCount < 30)
	{
		// Draws Red Munchie
		SpriteBatch::Draw(_munchieInvertedTexture, _munchieRect, nullptr, Vector2::Zero, 1.0f, 0.0f, Color::White, SpriteEffect::NONE);

		
	}
	else
	{
		// Draw Blue Munchie
		SpriteBatch::Draw(_munchieBlueTexture, _munchieRect, nullptr, Vector2::Zero, 1.0f, 0.0f, Color::White, SpriteEffect::NONE);
		

		if (_frameCount >= 60)
			_frameCount = 0;
	}

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

void Player::UpdatePlayer(int elapsedTime)
{
	if (!_paused)
	{
		_playerCurrentFrameTime += elapsedTime;

		if (_playerCurrentFrameTime > _cPlayerFrameTime)
		{
			_playerFrame++;

			if (_playerFrame >= 2)
				_playerFrame = 0;

			_playerCurrentFrameTime = 0;

			_playerSourceRect->X = _playerSourceRect->Width * _playerFrame;
		}
	}
}

void Player::UpdateMunchie(int elapsedTime)
{

}

void Player::Input(int elapsedTime, Input::KeyboardState* state)
{
	if (!_paused)
	{
		if (state->IsKeyDown(Input::Keys::S))
		{
			_playerPosition->Y += _cPlayerSpeed * elapsedTime;
			_playerDirection = 1;
			_playerSourceRect->Y = _playerSourceRect->Height * _playerDirection;
		}

		else if (state->IsKeyDown(Input::Keys::A))
		{
			_playerPosition->X -= _cPlayerSpeed * elapsedTime;
			_playerDirection = 2;
			_playerSourceRect->Y = _playerSourceRect->Height * _playerDirection;
		}
		else if (state->IsKeyDown(Input::Keys::W))
		{
			_playerPosition->Y -= _cPlayerSpeed * elapsedTime;
			_playerDirection = 3;
			_playerSourceRect->Y = _playerSourceRect->Height * _playerDirection;
		}
		else if (state->IsKeyDown(Input::Keys::D))
		{
			_playerPosition->X += _cPlayerSpeed * elapsedTime;
			_playerDirection = 0;
			_playerSourceRect->Y = _playerSourceRect->Height * _playerDirection;
		}
	}
}

void Player::CheckPaused(Input::KeyboardState* state, Input::Keys pauseKey)
{
	// pauses the game on the P input
	if (state->IsKeyDown(pauseKey) && !_pKeyDown)
	{
		_pKeyDown = true;
		_paused = !_paused;
	}

	if (state->IsKeyUp(pauseKey))
		_pKeyDown = false;
}

void Player::CheckViewportCollision()
{
	if (!_paused)
	{
		// if the Player hits a wall it stops in place
		if (_playerPosition->X > Graphics::GetViewportWidth() - 32) //Right wall
			_playerPosition->X = Graphics::GetViewportWidth() - 32;

		if (_playerPosition->X < 0) //Left wall
			_playerPosition->X = 0;

		if (_playerPosition->Y > Graphics::GetViewportHeight() - 32) //Bottom wall
			_playerPosition->Y = Graphics::GetViewportHeight() - 32;

		if (_playerPosition->Y < 0) //Top Wall
			_playerPosition->Y = 0;
	}
}
