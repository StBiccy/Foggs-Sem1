#include "Player.h"

#include <sstream>

Player::Player(int argc, char* argv[]) : Game(argc, argv)
{
	_frameCount = 0;
	_paused = false;
	_player = new SPlayer();
	_player->_currentFrameTime = 0;
	_player->_frame = 0;
	_player->_frameTime = 250;
	_player->_speed = 0.1f;

	//Initialise important Game aspects
	Graphics::Initialise(argc, argv, this, 1024, 768, false, 25, 25, "Pacman", 60);
	Input::Initialise();
	// Start the Game Loop - This calls Update and Draw in game loop
	Graphics::StartGameLoop();
}

Player::~Player()
{
	delete _player->_texture;
	delete _player->_sourceRect;
	delete _player->_position;
	delete _player;
	delete _munchieBlueTexture;
	delete _munchieInvertedTexture;
	delete _munchieRect;
}

void Player::LoadContent()
{
	// Load Pacman
	_player->_texture = new Texture2D();
	_player->_texture->Load("Textures/Pacman.tga", false);
	_player->_position = new Vector2(350.0f, 350.0f);
	_player->_sourceRect = new Rect(0.0f, 0.0f, 32, 32);

	
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
	stream << "Pacman X: " << _player->_position->X << " Y: " << _player->_position->Y;

	SpriteBatch::BeginDraw(); // Starts Drawing
	SpriteBatch::Draw(_player->_texture, _player->_position, _player->_sourceRect); // Draws Pacman

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
		_player->_currentFrameTime += elapsedTime;

		if (_player->_currentFrameTime > _player->_frameTime)
		{
			_player->_frame++;

			if (_player->_frame >= 2)
				_player->_frame = 0;

			_player->_currentFrameTime = 0;

			_player->_sourceRect->X = _player->_sourceRect->Width * _player->_frame;
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
			_player->_position->Y += _player->_speed * elapsedTime;
			_player->_direction = 1;
			_player->_sourceRect->Y = _player->_sourceRect->Height * _player->_direction;
		}

		else if (state->IsKeyDown(Input::Keys::A))
		{
			_player->_position->X -= _player->_speed * elapsedTime;
			_player->_direction = 2;
			_player->_sourceRect->Y = _player->_sourceRect->Height * _player->_direction;
		}
		else if (state->IsKeyDown(Input::Keys::W))
		{
			_player->_position->Y -= _player->_speed * elapsedTime;
			_player->_direction = 3;
			_player->_sourceRect->Y = _player->_sourceRect->Height * _player->_direction;
		}
		else if (state->IsKeyDown(Input::Keys::D))
		{
			_player->_position->X += _player->_speed * elapsedTime;
			_player->_direction = 0;
			_player->_sourceRect->Y = _player->_sourceRect->Height * _player->_direction;
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
		if (_player->_position->X > Graphics::GetViewportWidth() - 32) //Right wall
			_player->_position->X = Graphics::GetViewportWidth() - 32;

		if (_player->_position->X < 0) //Left wall
			_player->_position->X = 0;

		if (_player->_position->Y > Graphics::GetViewportHeight() - 32) //Bottom wall
			_player->_position->Y = Graphics::GetViewportHeight() - 32;

		if (_player->_position->Y < 0) //Top Wall
			_player->_position->Y = 0;
	}
}
