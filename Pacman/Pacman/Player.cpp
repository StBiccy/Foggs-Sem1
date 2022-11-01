#include "Player.h"

#include <sstream>

const float Player::_cGravity = 0.07f;
const float Player::_cMaxFallSpeed = 100;

Player::Player(int argc, char* argv[]) : Game(argc, argv), _cPlayerSpeed(0.1f)
{
	_frameCount = 0;
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
	delete _munchieBlueTexture;
	delete _munchieInvertedTexture;
	delete _munchieRect;
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

	// Load Munchie
	_munchieBlueTexture = new Texture2D();
	_munchieBlueTexture->Load("Content/Textures/Munchie.tga", true);
	_munchieInvertedTexture = new Texture2D();
	_munchieInvertedTexture->Load("Content/Textures/MunchieInverted.tga", true);
	_munchieRect = new Rect(100.0f, 450.0f, 12, 12);

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
	_velocity->Y = MathHelper::Clamp(_velocity->Y + _cGravity * elapsedTime ,-_cMaxFallSpeed, _cMaxFallSpeed);
	_velocity->X = 0;
	_playerPosition->Y += _velocity->Y;
	
}

void Player::Update(int elapsedTime)
{
	PhysicsUpdate(elapsedTime);

	// Gets the current state of the keyboard
	Input::KeyboardState* keyboardState = Input::Keyboard::GetState();

	// pauses the game on the P input
	if (keyboardState->IsKeyDown(Input::Keys::P) && !_pKeyDown)
	{
		_pKeyDown = true;
		_paused = !_paused;
	}

	if (keyboardState->IsKeyUp(Input::Keys::P))
		_pKeyDown = false;

	//stops player input if paused
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


		_frameCount++;
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