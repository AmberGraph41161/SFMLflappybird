#include <iostream>
#include <cstdlib>
#include <ctime>
#include <chrono>
#include <vector>
#include <SFML/Window/Keyboard.hpp>
#include <SFML/Window/VideoMode.hpp>
#include <SFML/System/Vector2.hpp>
#include <SFML/Graphics/CircleShape.hpp>
#include <SFML/Graphics/Rect.hpp>
#include <SFML/Graphics/RectangleShape.hpp>
#include <SFML/Graphics.hpp>
#include <SFML/Graphics/RenderWindow.hpp>
#include <SFML/Graphics/Color.hpp>
#include <SFML/Audio.hpp>
#include <SFML/Audio/Sound.hpp>

#include "gamefunctions.hpp"
#include "pipe.hpp"
#include "missile.hpp"

int main()
{
	std::cout << "[doing setup crap]" << std::endl;
	srand(time(0));

	const double screenWidth = 1080;
	const double screenHeight = 720;

	//load player stuff
	sf::Sprite player;
	sf::Texture playerTexture;
	std::string playerTexturePath = "resources/textures/flappybird.png";
	if(!playerTexture.loadFromFile(playerTexturePath))
	{
		std::cerr << "failed to load \"" << playerTexturePath << "\"" << std::endl;
		exit(EXIT_FAILURE);
	}
	player.setTexture(playerTexture);
	player.setOrigin(player.getLocalBounds().width / 2, player.getLocalBounds().height / 2);
	player.setScale(sf::Vector2f(4, 4));

	double defaultPlayerX = screenWidth / 4;
	double defaultPlayerY = screenHeight / 2;
	double playerX = defaultPlayerX;
	double playerY = defaultPlayerY;
	double playerXvelocity = 0;
	double playerYvelocity = 0;
	double gravity = 3000; //1.3
	double antiGravity = gravity * 0.2; //* 0.1
	double playerAngle = 0;
	double playerAngleMultiplier = 0.05;
	bool playerInitialJump  = false;
	bool playerJumpedLastTime = false;
	bool playerIsAlive = true;
	bool startMenu = true;
	bool pauseMenu = false;
	bool deathMenu = false;
	bool settingsMenu = false;
	
	int playerCurrentScore = 0; //(current score, during live gameplay and not saved score(s))
	
	std::map<std::string, int> playerScores;
	getSavedScores(playerScores);
	std::string playerName = getSavedPlayerName();

	//player settings...?
	bool displayHitboxes = false;
	double sfxVolume = 50;
	//double musicVolume = 100;

	//player hitbox?
	sf::RectangleShape playerHitbox(sf::Vector2f(player.getGlobalBounds().width * 0.5, player.getGlobalBounds().height * 0.7));
	playerHitbox.setOrigin(playerHitbox.getLocalBounds().width / 2, playerHitbox.getLocalBounds().height / 2);
	playerHitbox.setPosition(playerX, playerY);
	playerHitbox.setFillColor(sf::Color::Red);

	//player jump indicator
	sf::Sprite playerJumpIndicator;
	sf::Texture playerJumpIndicatorTexture;
	std::string playerJumpIndicatorTexturePath = "resources/textures/jumpindicator-Sheet.png";
	if(!playerJumpIndicatorTexture.loadFromFile(playerJumpIndicatorTexturePath))
	{
		std::cerr << "failed to load \"" << playerJumpIndicatorTexturePath << "\"" << std::endl;
		exit(EXIT_FAILURE);
	}
	const int playerJumpIndicatorFrameWidth = 25;
	const int playerJumpIndicatorFrameHeight = 37;
	const double playerJumpIndicatorYoffsetFix = -18;

	playerJumpIndicator.setTexture(playerJumpIndicatorTexture);
	playerJumpIndicator.setTextureRect(spriteSheetFrame(playerJumpIndicatorFrameWidth, playerJumpIndicatorFrameHeight, 0));
	playerJumpIndicator.setOrigin(playerJumpIndicator.getLocalBounds().width / 2, playerJumpIndicator.getLocalBounds().height / 2);
	playerJumpIndicator.setScale(sf::Vector2f(4, 4));
	playerJumpIndicator.setPosition(playerX, playerY + playerJumpIndicatorYoffsetFix);
	
	std::chrono::duration<double> animatePlayerJumpIndicatorDuration = std::chrono::seconds::zero();
	double animatePlayerJumpIndicatorDurationThreshold = 0.4;

	//load pipe stuff pipes
	sf::Texture pipeTexture;
	std::string pipeTexturePath = "resources/textures/longpipe.png";
	if(!pipeTexture.loadFromFile(pipeTexturePath))
	{
		std::cerr << "failed to load \"" << pipeTexturePath << "\"" << std::endl;
		exit(EXIT_FAILURE);
	}
	std::vector<Pipe> pipes;
	double defaultPipeSpacing = 100;
	double defaultPipeSpeed = 500; //0.2
	bool pipesSubroutine = false;

	pipes.emplace_back(Pipe(&pipeTexture, screenWidth - 100, screenHeight / 2, defaultPipeSpacing, defaultPipeSpeed));

	//load missile stuff
	sf::Texture missileTexture;
	std::string missileTexturePath = "resources/textures/missile-Sheet.png";
	if(!missileTexture.loadFromFile(missileTexturePath))
	{
		std::cerr << "Failed to load \"" << missileTexturePath << "\"" << std::endl;
		exit(EXIT_FAILURE);
	}
	double defaultMissileSpeed = 1500;
	std::vector<Missile> missiles;
	//missiles.emplace_back(Missile(&missileTexture, (screenWidth / 4) * 3, screenHeight / 2, defaultMissileSpeed));

	//load background and flooring stuff
	sf::Sprite background;
	sf::Texture backgroundTexture;
	std::string backgroundTexturePath = "resources/textures/slicebackground.png";
	if(!backgroundTexture.loadFromFile(backgroundTexturePath))
	{
		std::cerr << "failed to load \"" << backgroundTexturePath << "\"" << std::endl;
		exit(EXIT_FAILURE);
	}
	int backgroundTextureMultiplier = 8;
	backgroundTexture.setRepeated(true);
	background.setTextureRect(sf::IntRect(0, 0, backgroundTexture.getSize().x * backgroundTextureMultiplier, backgroundTexture.getSize().y));
	background.setTexture(backgroundTexture);
	background.setScale(sf::Vector2f(7.5, 7.5));
	background.setOrigin(background.getLocalBounds().width / 2, background.getLocalBounds().height / 2);
	background.setPosition(screenWidth / 2, screenHeight / 2);
	double backgroundOriginalX = background.getPosition().x;
	double backgroundOriginalY = background.getPosition().y;
	double backgroundSpeed = 50;

	//score
	sf::Text scoreText;
	sf::Font scoreTextFont;
	std::string scoreTextFontPath = "resources/fonts/Minecraftia-Regular.ttf";
	if(!scoreTextFont.loadFromFile(scoreTextFontPath))
	{
		std::cout << "failed to load\"" << scoreTextFontPath << "\"" << std::endl;
		exit(EXIT_FAILURE);
	}
	scoreText.setFont(scoreTextFont);
	scoreText.setCharacterSize(50);
	//scoreText.setScale(sf::Vector2f(2, 2));
	scoreText.setString(std::to_string(playerCurrentScore));
	scoreText.setOrigin(scoreText.getLocalBounds().width / 2, scoreText.getLocalBounds().height / 2);
	scoreText.setPosition(screenWidth / 2, screenHeight * 0.2);

	//ceiling
	sf::RectangleShape ceiling(sf::Vector2f(screenWidth, screenHeight * 0.1));
	ceiling.setFillColor(sf::Color::Cyan);

	//floor
	sf::RectangleShape floor(sf::Vector2f(screenWidth, screenHeight * 0.1));
	floor.setPosition(0, screenHeight - floor.getLocalBounds().height);
	floor.setFillColor(sf::Color::Cyan);
	
	//start menu
	sf::Sprite startButton;
	sf::Texture startButtonTexture;
	std::string startButtonTexturePath = "resources/textures/startbutton-Sheet.png";
	if(!startButtonTexture.loadFromFile(startButtonTexturePath))
	{
		std::cerr << "failed to load \"" << startButtonTexturePath << "\"" << std::endl;
		exit(EXIT_FAILURE);
	}
	const int startButtonFrameWidth = 33;
	const int startButtonFrameHeight = 13;
	bool startButtonHoveredOver = false;
	bool startButtonClicked = false;
	int startButtonFlashAnimationCount = 0;
	const int startButtonFlashAnimationCountThreshold = 10;
	std::chrono::duration<double> startButtonFlashAnimationTickDelta = std::chrono::seconds::zero();
	double startButtonFlashAnimationTickDeltaThreshold = 0.08;

	startButton.setTexture(startButtonTexture);
	startButton.setTextureRect(sf::IntRect(0, 0, startButtonFrameWidth, startButtonFrameHeight));
	startButton.setScale(sf::Vector2f(10, 10));
	startButton.setOrigin(startButton.getLocalBounds().width / 2, startButton.getLocalBounds().height / 2);
	startButton.setPosition(screenWidth / 2, (screenHeight / 5) * 2);

	sf::Sprite viewHighscoresButton;
	sf::Texture viewHighscoresButtonTexture;
	std::string viewHighscoresButtonTexturePath = "resources/textures/viewhighscoresbutton-Sheet.png";
	if(!viewHighscoresButtonTexture.loadFromFile(viewHighscoresButtonTexturePath))
	{
		std::cerr << "failed to load \"" << viewHighscoresButtonTexturePath << "\"" << std::endl;
		exit(EXIT_FAILURE);
	}
	const int viewHighscoresButtonFrameWidth = 93;
	const int viewHighscoresButtonFrameHeight = 13;
	bool viewHighscoresButtonHoveredOver = false;
	bool viewHighscoresButtonClicked = false;
	int viewHighscoresButtonFlashAnimationCount = 0;
	const int viewHighscoresButtonFlashAnimationCountThreshold = 10;
	std::chrono::duration<double> viewHighscoresButtonFlashAnimationTickDelta = std::chrono::seconds::zero();
	double viewHighscoresButtonFlashAnimationTickDeltaThreshold = 0.08;

	viewHighscoresButton.setTexture(viewHighscoresButtonTexture);
	viewHighscoresButton.setTextureRect(sf::IntRect(0, 0, viewHighscoresButtonFrameWidth, viewHighscoresButtonFrameHeight));
	viewHighscoresButton.setScale(sf::Vector2f(10, 10));
	viewHighscoresButton.setOrigin(viewHighscoresButton.getLocalBounds().width / 2, viewHighscoresButton.getLocalBounds().height / 2);
	viewHighscoresButton.setPosition(screenWidth / 2, (screenHeight / 5) * 3);

	//death menu & high score screen
	sf::Sprite playAgainButton;
	sf::Texture playAgainButtonTexture;
	std::string playAgainButtonTexturePath = "resources/textures/playagainbutton-Sheet.png";
	if(!playAgainButtonTexture.loadFromFile(playAgainButtonTexturePath))
	{
		std::cerr << "failed to load \"" << playAgainButtonTexturePath << "\"" << std::endl;
		exit(EXIT_FAILURE);
	}
	const int playAgainButtonFrameWidth = 68;
	const int playAgainButtonFrameHeight = 13;
	bool playAgainButtonHoveredOver = false;
	std::chrono::duration<double> playAgainButtonIdleAnimationTickDelta = std::chrono::seconds::zero();
	double playAgainButtonIdleAnimationTickDeltaThreshold = 0.6;

	playAgainButton.setTexture(playAgainButtonTexture);
	playAgainButton.setTextureRect(sf::IntRect(0, 0, playAgainButtonFrameWidth, playAgainButtonFrameHeight));
	playAgainButton.setScale(sf::Vector2f(10, 10));
	playAgainButton.setOrigin(playAgainButton.getLocalBounds().width / 2, playAgainButton.getLocalBounds().height / 2);
	playAgainButton.setPosition(screenWidth / 2, screenHeight / 2);

	//dimscreen rectangleshape object for death menu, pause menu, etc, etc
	int dimScreenShapeDefaultDim = 200;
	sf::RectangleShape dimScreenShape(sf::Vector2f(screenWidth, screenHeight));
	dimScreenShape.setFillColor(sf::Color(0, 0, 0, dimScreenShapeDefaultDim));
	sf::Text dimScreenText;
	dimScreenText.setFont(scoreTextFont); //note to self as ofThursday, October 17, 2024, 14:14:38 --> make this default font...? change def
	
	sf::Sprite quitButton;
	sf::Texture quitButtonTexture;
	std::string quitButtonTexturePath = "resources/textures/quitbutton-Sheet.png";
	if(!quitButtonTexture.loadFromFile(quitButtonTexturePath))
	{
		std::cerr << "failed to load \"" << quitButtonTexturePath << "\"" << std::endl;
		exit(EXIT_FAILURE);
	}
	const int quitButtonFrameWidth = 29;
	const int quitButtonFrameHeight = 13;
	bool quitButtonHoveredOver = false;

	quitButton.setTexture(quitButtonTexture);
	quitButton.setTextureRect(spriteSheetFrame(quitButtonFrameWidth, quitButtonFrameHeight, 0));
	quitButton.setScale(sf::Vector2f(10, 10));
	quitButton.setOrigin(quitButton.getLocalBounds().width / 2, quitButton.getLocalBounds().height / 2);
	quitButton.setPosition(screenWidth / 2, (screenHeight / 4) * 3);

	//load music and sound effects sfx
	float masterVolume = 40;
	std::vector<sf::Sound*> allSFXvector;

	sf::Sound scoreSFX;
	sf::SoundBuffer scoreSFXbuffer;
	std::string scoreSFXbufferPath = "resources/sounds/score1.wav";
	if(!scoreSFXbuffer.loadFromFile(scoreSFXbufferPath))
	{
		std::cerr << "failed to load \"" << scoreSFXbufferPath << "\"" << std::endl;
		exit(EXIT_FAILURE);
	}
	scoreSFX.setBuffer(scoreSFXbuffer);
	scoreSFX.setVolume(sfxVolume);
	allSFXvector.push_back(&scoreSFX);

	sf::Sound menu0SFX;
	sf::SoundBuffer menu0SFXbuffer;
	std::string menu0SFXbufferPath = "resources/sounds/menu3.wav";
	if(!menu0SFXbuffer.loadFromFile(menu0SFXbufferPath))
	{
		std::cerr << "failed to load \"" << menu0SFXbufferPath << "\"" << std::endl;
		exit(EXIT_FAILURE);
	}
	menu0SFX.setBuffer(menu0SFXbuffer);
	menu0SFX.setVolume(sfxVolume);
	allSFXvector.push_back(&menu0SFX);

	sf::Sound menu1SFX;
	sf::SoundBuffer menu1SFXbuffer;
	std::string menu1SFXbufferPath = "resources/sounds/menu4.wav";
	if(!menu1SFXbuffer.loadFromFile(menu1SFXbufferPath))
	{
		std::cerr << "failed to load \"" << menu1SFXbufferPath << "\"" << std::endl;
		exit(EXIT_FAILURE);
	}
	menu1SFX.setBuffer(menu1SFXbuffer);
	menu1SFX.setVolume(sfxVolume);
	allSFXvector.push_back(&menu1SFX);

	sf::Sound jumpSFX;
	sf::SoundBuffer jumpSFXbuffer;
	std::string jumpSFXbufferPath = "resources/sounds/jump0.wav";
	if(!jumpSFXbuffer.loadFromFile(jumpSFXbufferPath))
	{
		std::cerr << "failed to load \"" << jumpSFXbufferPath << "\"" << std::endl;
		exit(EXIT_FAILURE);
	}
	jumpSFX.setBuffer(jumpSFXbuffer);
	jumpSFX.setVolume(sfxVolume);
	allSFXvector.push_back(&jumpSFX);

	sf::Sound deadSFX;
	sf::SoundBuffer deadSFXbuffer;
	std::string deadSFXbufferPath = "resources/sounds/dead0.wav";
	if(!deadSFXbuffer.loadFromFile(deadSFXbufferPath))
	{
		std::cerr << "failed to load \"" << deadSFXbufferPath << "\"" << std::endl;
		exit(EXIT_FAILURE);
	}
	deadSFX.setBuffer(deadSFXbuffer);
	deadSFX.setVolume(sfxVolume);
	allSFXvector.push_back(&deadSFX);

	sf::Sound missileDroppingSFX;
	sf::SoundBuffer missileDroppingSFXbuffer;
	std::string missileDroppingSFXbufferPath = "resources/sounds/missiledropping0.wav";
	if(!missileDroppingSFXbuffer.loadFromFile(missileDroppingSFXbufferPath))
	{
		std::cerr << "failed to load \"" << missileDroppingSFXbufferPath << "\"" << std::endl;
		exit(EXIT_FAILURE);
	}
	missileDroppingSFX.setBuffer(missileDroppingSFXbuffer);
	missileDroppingSFX.setVolume(sfxVolume);
	allSFXvector.push_back(&missileDroppingSFX);
	
	sf::Sound missileLaunchingSFX;
	sf::SoundBuffer missileLaunchingSFXbuffer;
	std::string missileLaunchingSFXbufferPath = "resources/sounds/missilelaunching0.wav";
	if(!missileLaunchingSFXbuffer.loadFromFile(missileLaunchingSFXbufferPath))
	{
		std::cerr << "failed to load \"" << missileLaunchingSFXbufferPath << "\"" << std::endl;
		exit(EXIT_FAILURE);
	}
	missileLaunchingSFX.setBuffer(missileLaunchingSFXbuffer);
	missileLaunchingSFX.setVolume(sfxVolume);
	allSFXvector.push_back(&missileLaunchingSFX);

	for(int x = 0; x < allSFXvector.size(); x++)
	{
		allSFXvector[x]->setVolume(masterVolume);
	}

	//debug FPS font stuff
	bool drawFPS = false;
	sf::Text fps;
	fps.setFont(scoreTextFont);
	fps.setPosition(10, 100);
	fps.setString("0");

	//chrono delta time stuff
	std::chrono::time_point<std::chrono::system_clock> lastlastframe = std::chrono::high_resolution_clock::now();
	std::chrono::time_point<std::chrono::system_clock> lastframe = std::chrono::high_resolution_clock::now();
	std::chrono::duration<double> deltaTime = lastframe - lastlastframe;

	//view and window render stuff
	sf::View view(sf::FloatRect(0, 0, screenWidth, screenHeight));

	sf::RenderWindow window(sf::VideoMode(screenWidth, screenHeight), "title goes here", sf::Style::Default);
	window.setView(view);
	window.setKeyRepeatEnabled(false); //reread documentation for this one buddy Thursday, October 10, 2024, 10:34:00
	window.setFramerateLimit(200);

	while(window.isOpen())
	{
		sf::Event event;
		while(window.pollEvent(event))
		{
			if(event.type == sf::Event::Closed)
			{
				window.close();
			}
			if(event.type == sf::Event::Resized)
			{
				//as of Saturday, October 12, 2024, 12:48:52,
					//I have finally gotten the window resizing crap to work.
					//yes, I admit it, all the paranthese make it look garbage, but I don't care.
					//it works, and thats what matters

				if((((float)window.getSize().x / 16) * 9) > window.getSize().y)
				{
					view.setViewport(sf::FloatRect(0.5 - (((((float)window.getSize().y / 9) * 16) / (float)window.getSize().x) / 2), 0, (((float)window.getSize().y / 9) * 16) / (float)window.getSize().x, 1));
				} else
				{
					view.setViewport(sf::FloatRect(0, 0.5 - (((((float)window.getSize().x / 16) * 9) / (float)window.getSize().y) / 2), 1, (((float)window.getSize().x / 16) * 9) / (float)window.getSize().y));
				}
				window.setView(view);
			}
		}
		if(sf::Keyboard::isKeyPressed(sf::Keyboard::Q))
		{
			window.close();
		}

		//startscreen and or pause menu
		if(startMenu)
		{
			//draw and deltaTime
			lastlastframe = std::chrono::high_resolution_clock::now();
			window.clear(sf::Color::Black);
			
			//update background
			if(background.getPosition().x <= backgroundOriginalX - (background.getGlobalBounds().width / backgroundTextureMultiplier))
			{
				background.setPosition(backgroundOriginalX, backgroundOriginalY);
			}
			background.move(-1 * backgroundSpeed * deltaTime.count(), 0);
			window.draw(background);
	
			//startButton logic
				//I should prolly just throw all this into a single button class... kill me. Tuesday, November 26, 2024, 10:40:49
			if(startButton.getGlobalBounds().contains(window.mapPixelToCoords(sf::Mouse::getPosition(window), view))) //view is in coords, without view is in pixels
			{
				startButton.setTextureRect(spriteSheetFrame(startButtonFrameWidth, startButtonFrameHeight, 1));

				if(!startButtonHoveredOver && !startButtonClicked && !viewHighscoresButtonClicked)
				{
					menu0SFX.play();
				}
				startButtonHoveredOver = true;

				if(sf::Mouse::isButtonPressed(sf::Mouse::Left))
				{
					if(!startButtonClicked && !viewHighscoresButtonClicked)
					{
						startButtonClicked = true;
						menu1SFX.play();
					}
				}

			} else
			{
				startButtonHoveredOver = false;
				startButton.setTextureRect(spriteSheetFrame(startButtonFrameWidth, startButtonFrameHeight, 0));
			}

			if(startButtonClicked)
			{
				if(startButtonFlashAnimationCount >= startButtonFlashAnimationCountThreshold)
				{
					startButtonFlashAnimationTickDelta = std::chrono::seconds::zero();

					startMenu = false;
					playerIsAlive = true;
					continue;
				} else
				{
					if(startButtonFlashAnimationTickDelta.count() >= startButtonFlashAnimationTickDeltaThreshold)
					{
						startButtonFlashAnimationTickDelta = std::chrono::seconds::zero();
						startButtonFlashAnimationCount++;
					} else
					{
						startButtonFlashAnimationTickDelta += deltaTime;
					}

					startButton.setTextureRect(spriteSheetFrame(startButtonFrameWidth, startButtonFrameHeight, 2 + (startButtonFlashAnimationCount % 2)));
				}
			}

			//viewHighscoresButton logic
			if(viewHighscoresButton.getGlobalBounds().contains(window.mapPixelToCoords(sf::Mouse::getPosition(window), view)))
			{
				viewHighscoresButton.setTextureRect(spriteSheetFrame(viewHighscoresButtonFrameWidth, viewHighscoresButtonFrameHeight, 1));

				if(!viewHighscoresButtonHoveredOver && !viewHighscoresButtonClicked && !startButtonClicked)
				{
					menu0SFX.play();
				}
				viewHighscoresButtonHoveredOver = true;

				if(sf::Mouse::isButtonPressed(sf::Mouse::Left))
				{
					if(!viewHighscoresButtonClicked && !startButtonClicked)
					{
						viewHighscoresButtonClicked = true;
						menu1SFX.play();
					}
				}

			} else
			{
				viewHighscoresButtonHoveredOver = false;
				viewHighscoresButton.setTextureRect(spriteSheetFrame(viewHighscoresButtonFrameWidth, viewHighscoresButtonFrameHeight, 0));
			}

			if(viewHighscoresButtonClicked)
			{
				if(viewHighscoresButtonFlashAnimationCount >= viewHighscoresButtonFlashAnimationCountThreshold)
				{
					viewHighscoresButtonFlashAnimationTickDelta = std::chrono::seconds::zero();

					startMenu = false;
					playerIsAlive = true;
					continue;
				} else
				{
					if(viewHighscoresButtonFlashAnimationTickDelta.count() >= viewHighscoresButtonFlashAnimationTickDeltaThreshold)
					{
						viewHighscoresButtonFlashAnimationTickDelta = std::chrono::seconds::zero();
						viewHighscoresButtonFlashAnimationCount++;
					} else
					{
						viewHighscoresButtonFlashAnimationTickDelta += deltaTime;
					}

					viewHighscoresButton.setTextureRect(spriteSheetFrame(viewHighscoresButtonFrameWidth, viewHighscoresButtonFrameHeight, 2 + (viewHighscoresButtonFlashAnimationCount % 2)));
				}
			}

			window.draw(startButton);
			window.draw(viewHighscoresButton);

			window.display();
			lastframe = std::chrono::high_resolution_clock::now();
			deltaTime = lastframe - lastlastframe;

		} else if(pauseMenu || settingsMenu)
		{
			if(quitButton.getGlobalBounds().contains(window.mapPixelToCoords(sf::Mouse::getPosition(window), view)))
			{
				quitButton.setTextureRect(spriteSheetFrame(quitButtonFrameWidth, quitButtonFrameHeight, 1));

				if(!quitButtonHoveredOver)
				{
					menu0SFX.play();
				}
				quitButtonHoveredOver = true;
			} else
			{
				quitButtonHoveredOver = false;
				quitButton.setTextureRect(spriteSheetFrame(quitButtonFrameWidth, quitButtonFrameHeight, 0));
			}

			if(sf::Mouse::isButtonPressed(sf::Mouse::Left))
			{
				if(quitButtonHoveredOver)
				{
					playerIsAlive = false;
				}
				pauseMenu = false;
				settingsMenu = false;
				menu1SFX.play();
			}

			//draw and deltaTime
			lastlastframe = std::chrono::high_resolution_clock::now();
			window.clear(sf::Color::Black);
		
			window.draw(background);
			
			window.draw(player);
			if(!playerInitialJump)
			{
				window.draw(playerJumpIndicator);
			}
			
			for(int x = 0; x < pipes.size(); x++)
			{
				window.draw(pipes[x].getTopPipe());
				window.draw(pipes[x].getBottomPipe());
			}

			for(int x = 0; x < missiles.size(); x++)
			{
				window.draw(missiles[x].getMissile());
			}

			if(displayHitboxes)
			{
				window.draw(ceiling);
				window.draw(floor);
				window.draw(playerHitbox);
			}

			window.draw(scoreText);
			if(drawFPS)
			{
				window.draw(fps);
			}
			
			window.draw(dimScreenShape);
			dimScreenText.setString("Left Mouse Button Anywhere To Resume");
			dimScreenText.setOrigin(dimScreenText.getLocalBounds().width / 2, dimScreenText.getLocalBounds().height / 2);
			dimScreenText.setPosition(screenWidth / 2, screenHeight / 2);
			dimScreenText.setCharacterSize(30);
			window.draw(dimScreenText);
			window.draw(quitButton);

			window.display();
			lastframe = std::chrono::high_resolution_clock::now();
			deltaTime = lastframe - lastlastframe;

		} else if(deathMenu)
		{
			lastlastframe = std::chrono::high_resolution_clock::now();
			window.clear(sf::Color::Black);
			
			//update background
			if(background.getPosition().x <= backgroundOriginalX - (background.getGlobalBounds().width / backgroundTextureMultiplier))
			{
				background.setPosition(backgroundOriginalX, backgroundOriginalY);
			}
			background.move(-1 * backgroundSpeed * deltaTime.count(), 0);
			window.draw(background);

			//playAgainButton logic
			if(playAgainButton.getGlobalBounds().contains(window.mapPixelToCoords(sf::Mouse::getPosition(window), view))) //view is in coords, without view is in pixels
			{
				playAgainButton.setTextureRect(spriteSheetFrame(playAgainButtonFrameWidth, playAgainButtonFrameHeight, 2));

				if(!playAgainButtonHoveredOver)
				{
					menu0SFX.play();
				}
				playAgainButtonHoveredOver = true;

				if(sf::Mouse::isButtonPressed(sf::Mouse::Left))
				{
					menu1SFX.play();
					deathMenu = false;
					playerIsAlive = true;
					playerInitialJump = false;
					continue;
				}
			} else
			{
				playAgainButtonHoveredOver = false;
				if(playAgainButtonIdleAnimationTickDelta.count() >= playAgainButtonIdleAnimationTickDeltaThreshold * 2)
				{
					playAgainButtonIdleAnimationTickDelta = std::chrono::seconds::zero();
				} else if(playAgainButtonIdleAnimationTickDelta.count() >= playAgainButtonIdleAnimationTickDeltaThreshold)
				{
					playAgainButton.setTextureRect(spriteSheetFrame(playAgainButtonFrameWidth, playAgainButtonFrameHeight, 1));
				} else
				{
					playAgainButton.setTextureRect(spriteSheetFrame(playAgainButtonFrameWidth, playAgainButtonFrameHeight, 0));
				}
				playAgainButtonIdleAnimationTickDelta += deltaTime;
			}

			window.draw(playAgainButton);

			window.display();
			lastframe = std::chrono::high_resolution_clock::now();
			deltaTime = lastframe - lastlastframe;
		} else if(playerIsAlive && !deathMenu)//the game (maybe this is a bad idea...?)
		{
			//debug controls
			if(sf::Keyboard::isKeyPressed(sf::Keyboard::Up))
			{
				while(sf::Keyboard::isKeyPressed(sf::Keyboard::Up))
				{

				}
				pipeShrinkTunnel(pipes, &pipeTexture, 40, screenWidth - 100, screenHeight / 2);
			} else if(sf::Keyboard::isKeyPressed(sf::Keyboard::Down))
			{
				while(sf::Keyboard::isKeyPressed(sf::Keyboard::Down))
				{

				}
				pipeTunnel(pipes, &pipeTexture, 100, screenWidth - 100, screenHeight / 2);
			} else if(sf::Keyboard::isKeyPressed(sf::Keyboard::Left))
			{
			} else if(sf::Keyboard::isKeyPressed(sf::Keyboard::Right))
			{
			} else if(sf::Keyboard::isKeyPressed(sf::Keyboard::R))
			{
			} else if(sf::Keyboard::isKeyPressed(sf::Keyboard::A))
			{
				while(sf::Keyboard::isKeyPressed(sf::Keyboard::A))
				{

				}
				//pipes.emplace_back(Pipe(&pipeTexture, screenWidth - 100, RANDOMDOUBLE(300, screenHeight - 300), defaultPipeSpacing, defaultPipeSpeed));
				missiles.emplace_back(Missile(&missileTexture, (screenWidth / 4) * 3, screenHeight / 2, defaultMissileSpeed, &missileDroppingSFX, &missileLaunchingSFX));
			} else if(sf::Mouse::isButtonPressed(sf::Mouse::Left))
			{
				while(sf::Mouse::isButtonPressed(sf::Mouse::Left))
				{
					missiles.emplace_back(Missile(&missileTexture, window.mapPixelToCoords(sf::Mouse::getPosition(window), view).x, window.mapPixelToCoords(sf::Mouse::getPosition(window), view).y, defaultMissileSpeed, &missileDroppingSFX, &missileLaunchingSFX));
				}
			}

			//pause menu...?
			if(sf::Keyboard::isKeyPressed(sf::Keyboard::Escape))
			{
				pauseMenu = true;
			}

			//player controls
			if(!sf::Keyboard::isKeyPressed(sf::Keyboard::Space) && playerJumpedLastTime)
			{
				playerJumpedLastTime = false;
			}

			if(sf::Keyboard::isKeyPressed(sf::Keyboard::Space) && !playerJumpedLastTime && playerIsAlive)
			{
				playerInitialJump = true;
				playerJumpedLastTime = true;
				playerYvelocity = -1 * antiGravity;

				jumpSFX.play();
			} else if(!playerHitbox.getGlobalBounds().intersects(floor.getGlobalBounds()) && !playerHitbox.getGlobalBounds().intersects(ceiling.getGlobalBounds()) && playerInitialJump)
			{
				//move player down due to gravity (falling)
				playerYvelocity += gravity * deltaTime.count();
			} else
			{
				//player is touching either floor or ceiling maybe? so check?
				playerYvelocity = 0;

				//ceiling?
				if(playerHitbox.getGlobalBounds().intersects(ceiling.getGlobalBounds()))
				{
					playerY = ceiling.getGlobalBounds().height + player.getGlobalBounds().top;
				} else if(playerHitbox.getGlobalBounds().intersects(floor.getGlobalBounds()) || playerHitbox.getPosition().y >= floor.getPosition().y)
				{
					playerIsAlive = false;
					deadSFX.play();
					continue;
				}
			}

			//update player
			playerY += playerYvelocity * deltaTime.count();
			playerX += playerXvelocity * deltaTime.count();

			playerAngle = playerYvelocity * -1 * playerAngleMultiplier;

			playerHitbox.setPosition(playerX, playerY);
			player.setPosition(playerX, playerY);
			player.setRotation(playerAngle);

			//update background
			if(background.getPosition().x <= backgroundOriginalX - (background.getGlobalBounds().width / backgroundTextureMultiplier))
			{
				background.setPosition(backgroundOriginalX, backgroundOriginalY);
			}
			background.move(-1 * backgroundSpeed * deltaTime.count(), 0);

			//draw and deltaTime
			lastlastframe = std::chrono::high_resolution_clock::now();
			window.clear(sf::Color::Black);
		
			window.draw(background);
			
			window.draw(player);
			if(!playerInitialJump)
			{
				animatePlayerJumpIndicatorDuration += deltaTime;

				if(animatePlayerJumpIndicatorDuration.count() >= animatePlayerJumpIndicatorDurationThreshold * 3)
				{
					playerJumpIndicator.setTextureRect(spriteSheetFrame(playerJumpIndicatorFrameWidth, playerJumpIndicatorFrameHeight, 0));
					animatePlayerJumpIndicatorDuration = std::chrono::seconds::zero();
				}else if(animatePlayerJumpIndicatorDuration.count() >= animatePlayerJumpIndicatorDurationThreshold * 2)
				{
					playerJumpIndicator.setTextureRect(spriteSheetFrame(playerJumpIndicatorFrameWidth, playerJumpIndicatorFrameHeight, 2));
				} else if(animatePlayerJumpIndicatorDuration.count() >= animatePlayerJumpIndicatorDurationThreshold)
				{
					playerJumpIndicator.setTextureRect(spriteSheetFrame(playerJumpIndicatorFrameWidth, playerJumpIndicatorFrameHeight, 1));
				}
				window.draw(playerJumpIndicator);
			}
			
			if(displayHitboxes)
			{
				window.draw(ceiling);
				window.draw(floor);
				window.draw(playerHitbox);
			}

			//update pipes stuff
			if(pipes.size() <= 0 && pipesSubroutine)
			{
				pipesSubroutine = false;
				spawnDefaultRandomPipe(pipes, &pipeTexture, screenWidth, screenHeight);
			}
			for(int x = 0; x < pipes.size(); x++)
			{
				if(pipes[x].intersects(playerHitbox.getGlobalBounds()))
				{
					playerIsAlive = false;
					deadSFX.play();
					break;
				}

				if(playerInitialJump)
				{
					pipes[x].move(deltaTime.count());
				}
				if(!pipes[x].hasPastPlayer() && pipes[x].isPastPlayer(playerX))
				{
					playerCurrentScore++;
					scoreText.setString(std::to_string(playerCurrentScore));
					scoreSFX.play();
					if(RANDOM(0, 10) == 5 && !pipesSubroutine)
					{
						pipeTunnel(pipes, &pipeTexture, RANDOM(0, 20), screenWidth - 100, screenHeight / 2);
						pipesSubroutine = true;
					} else if(RANDOM(0, 15) == 5 && !pipesSubroutine)
					{
						pipeShrinkTunnel(pipes, &pipeTexture, RANDOM(0, 20), screenWidth - 100, screenHeight / 2);
						pipesSubroutine = true;
					} else if(RANDOM(0, 1) && !pipesSubroutine)
					{
						missiles.emplace_back(Missile(&missileTexture, (screenWidth / 4) * 3.5, playerY + RANDOM(-100, 100), defaultMissileSpeed, &missileDroppingSFX, &missileLaunchingSFX));
						missileDroppingSFX.play();
						missiles.emplace_back(Missile(&missileTexture, (screenWidth / 4) * 3.5, playerY + RANDOM(-100, 100), defaultMissileSpeed, &missileDroppingSFX, &missileLaunchingSFX));
						missileDroppingSFX.play();
						missiles.emplace_back(Missile(&missileTexture, (screenWidth / 4) * 3.5, playerY + RANDOM(-100, 100), defaultMissileSpeed, &missileDroppingSFX, &missileLaunchingSFX));
						missileDroppingSFX.play();
						missiles.emplace_back(Missile(&missileTexture, (screenWidth / 4) * 3.5, playerY + RANDOM(-100, 100), defaultMissileSpeed, &missileDroppingSFX, &missileLaunchingSFX));
						missileDroppingSFX.play();
						pipesSubroutine = true;
					} else if(!pipesSubroutine)
					{
						spawnDefaultRandomPipe(pipes, &pipeTexture, screenWidth, screenHeight);
					}
				}

				if(pipes[x].isOffScreenLeftRight(0 - 200, view.getSize().x + 1000))
				{
					pipes.erase(pipes.begin() + x);
					x--;
					continue;
				}

				window.draw(pipes[x].getTopPipe());
				window.draw(pipes[x].getBottomPipe());
			}

			//update missiles stuff
			for(int x = 0; x < missiles.size(); x++)
			{
				if(missiles[x].getMissile().getGlobalBounds().intersects(playerHitbox.getGlobalBounds()))
				{
					playerIsAlive = false;
					deadSFX.play();
					break;
				}

				missiles[x].move(deltaTime.count());
				if(missiles[x].isOffScreenLeftRight(0 - 200, view.getSize().x + 1000))
				{
					missiles.erase(missiles.begin() + x);
					x--;
					continue;
				}

				window.draw(missiles[x].getMissile());
			}

			window.draw(scoreText);
			if(drawFPS)
			{
				window.draw(fps);
			}
			
			window.display();
			lastframe = std::chrono::high_resolution_clock::now();
			deltaTime = lastframe - lastlastframe;

			fps.setString(std::to_string(1 / deltaTime.count()) + "\ndeltaTime(): " + std::to_string(deltaTime.count()));
		} else if(!playerIsAlive)
		{
			playerYvelocity += gravity * deltaTime.count();

			playerY += playerYvelocity * deltaTime.count();
			playerX += playerXvelocity * deltaTime.count();

			playerAngle = playerYvelocity * -1 * playerAngleMultiplier;

			playerHitbox.setPosition(playerX, playerY);
			player.setPosition(playerX, playerY);
			player.setRotation(playerAngle);

			//draw and deltaTime
			lastlastframe = std::chrono::high_resolution_clock::now();
			window.clear(sf::Color::Black);
		
			//update background
			if(background.getPosition().x <= backgroundOriginalX - (background.getGlobalBounds().width / backgroundTextureMultiplier))
			{
				background.setPosition(backgroundOriginalX, backgroundOriginalY);
			}
			background.move(-1 * backgroundSpeed * deltaTime.count(), 0);
			window.draw(background);

			for(int x = 0; x < pipes.size(); x++)
			{
				window.draw(pipes[x].getTopPipe());
				window.draw(pipes[x].getBottomPipe());
			}
			for(int x = 0; x < missiles.size(); x++)
			{
				window.draw(missiles[x].getMissile());
			}

			window.draw(player);

			window.draw(scoreText);
			if(drawFPS)
			{
				window.draw(fps);
			}
			
			window.display();
			lastframe = std::chrono::high_resolution_clock::now();
			deltaTime = lastframe - lastlastframe;

			if(player.getPosition().y >= screenHeight)
			{
				if(playerScores[playerName] < playerCurrentScore)
				{
					playerScores[playerName] = playerCurrentScore;
					saveScores(playerScores);
				}

				pipes.clear();
				missiles.clear();

				playerCurrentScore = 0;
				playerXvelocity = 0;
				playerYvelocity = 0;
				playerX = defaultPlayerX;
				playerY = defaultPlayerY;

				playerJumpedLastTime = false;

				scoreText.setString("0");
				playerHitbox.setPosition(playerX, playerY);
				player.setPosition(playerX, playerY);
				player.setRotation(playerAngle);

				spawnDefaultRandomPipe(pipes, &pipeTexture, screenWidth, screenHeight);
				deathMenu = true;
			}
		}
	}
	saveScores(playerScores);

	std::cout << "[done!]" << std::endl;
	return 0;
}
