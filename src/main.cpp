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
	if(!playerTexture.loadFromFile("resources/flappybird.png"))
	{
		std::cerr << "failed to load \"resources/flappybird.png\"" << std::endl;
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
	if(!playerJumpIndicatorTexture.loadFromFile("resources/jumpindicator-Sheet.png"))
	{
		std::cerr << "failed to load \"resources/jumpindicator-Sheet.png\"" << std::endl;
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
	if(!pipeTexture.loadFromFile("resources/longpipe.png"))
	{
		std::cerr << "failed to load \"resources/longpipe.png\"" << std::endl;
		exit(EXIT_FAILURE);
	}
	std::vector<Pipe> pipes;
	double defaultPipeSpacing = 100;
	double defaultPipeSpeed = 500; //0.2
	bool pipesSubroutine = false;

	pipes.emplace_back(Pipe(&pipeTexture, screenWidth - 100, screenHeight / 2, defaultPipeSpacing, defaultPipeSpeed));

	//load missile stuff
	sf::Texture missileTexture;
	if(!missileTexture.loadFromFile("resources/missile-Sheet.png"))
	{
		std::cerr << "Failed to load \"resources/missile-Sheet.png\"" << std::endl;
		exit(EXIT_FAILURE);
	}
	double defaultMissileSpeed = 1500;
	std::vector<Missile> missiles;
	//missiles.emplace_back(Missile(&missileTexture, (screenWidth / 4) * 3, screenHeight / 2, defaultMissileSpeed));

	//load background and flooring stuff
	sf::Sprite background;
	sf::Texture backgroundTexture;
	if(!backgroundTexture.loadFromFile("resources/slicebackground.png"))
	{
		std::cerr << "failed to load \"resources/slicebackground.png\"" << std::endl;
		exit(EXIT_FAILURE);
	}
	int backgroundTextureMultiplier = 8;
	backgroundTexture.setRepeated(true);
	background.setTextureRect(sf::IntRect(0, 0, backgroundTexture.getSize().x * backgroundTextureMultiplier, backgroundTexture.getSize().y));
	background.setTexture(backgroundTexture);
	background.setScale(sf::Vector2f(7.5, 7.5));
	background.setOrigin(background.getLocalBounds().width / 2, background.getLocalBounds().height / 2);
	background.setPosition(static_cast<double>(screenWidth) / 2, static_cast<double>(screenHeight) / 2);
	double backgroundOriginalX = background.getPosition().x;
	double backgroundOriginalY = background.getPosition().y;
	double backgroundSpeed = 50;

	//score
	sf::Text scoreText;
	sf::Font scoreTextFont;
	if(!scoreTextFont.loadFromFile("resources/Minecraftia-Regular.ttf"))
	{
		std::cout << "failed to load\"resources/Minecraftia-Regular.ttf\"" << std::endl;
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
	if(!startButtonTexture.loadFromFile("resources/startbutton-Sheet.png"))
	{
		std::cerr << "failed to load \"resources/startbutton-Sheet.png\"" << std::endl;
		exit(EXIT_FAILURE);
	}
	const int startButtonFrameWidth = 33;
	const int startButtonFrameHeight = 13;
	bool startButtonHoveredOver = false;

	startButton.setTexture(startButtonTexture);
	startButton.setTextureRect(sf::IntRect(0, 0, startButtonFrameWidth, startButtonFrameHeight));
	startButton.setScale(sf::Vector2f(10, 10));
	startButton.setOrigin(startButton.getLocalBounds().width / 2, startButton.getLocalBounds().height / 2);
	startButton.setPosition(screenWidth / 2, screenHeight / 2);

	//death menu & high score screen
	sf::Sprite playAgainButton;
	sf::Texture playAgainButtonTexture;
	if(!playAgainButtonTexture.loadFromFile("resources/playagainbutton-Sheet.png"))
	{
		std::cerr << "failed to load \"resources/playagainbutton-Sheet.png\"" << std::endl;
		exit(EXIT_FAILURE);
	}
	const int playAgainButtonFrameWidth = 68;
	const int playAgainButtonFrameHeight = 13;
	bool playAgainButtonHoveredOver = false;
	std::chrono::duration<double> animatePlayAgainButtonDuration = std::chrono::seconds::zero();
	double animatePlayAgainButtonDurationThreshold = 0.4;

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
	if(!quitButtonTexture.loadFromFile("resources/quitbutton-Sheet.png"))
	{
		std::cerr << "failed to load \"resources/quitbutton-Sheet.png\"" << std::endl;
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
	std::vector<sf::Sound*> allSFXvector;

	sf::Sound scoreSFX;
	sf::SoundBuffer scoreSFXbuffer;
	if(!scoreSFXbuffer.loadFromFile("resources/score1.wav"))
	{
		std::cerr << "failed to load \"resources/score1.wav\"" << std::endl;
		exit(EXIT_FAILURE);
	}
	scoreSFX.setBuffer(scoreSFXbuffer);
	scoreSFX.setVolume(sfxVolume);
	allSFXvector.push_back(&scoreSFX);

	sf::Sound menuSFX;
	sf::SoundBuffer menuSFXbuffer;
	if(!menuSFXbuffer.loadFromFile("resources/menu0.wav"))
	{
		std::cerr << "failed to load \"resources/menu0.wav\"" << std::endl;
		exit(EXIT_FAILURE);
	}
	menuSFX.setBuffer(menuSFXbuffer);
	menuSFX.setVolume(sfxVolume);
	allSFXvector.push_back(&menuSFX);


	sf::Sound jumpSFX;
	sf::SoundBuffer jumpSFXbuffer;
	if(!jumpSFXbuffer.loadFromFile("resources/jump0.wav"))
	{
		std::cerr << "failed to load \"resources/jump0.wav\"" << std::endl;
		exit(EXIT_FAILURE);
	}
	jumpSFX.setBuffer(jumpSFXbuffer);
	jumpSFX.setVolume(sfxVolume);
	allSFXvector.push_back(&jumpSFX);

	sf::Sound deadSFX;
	sf::SoundBuffer deadSFXbuffer;
	if(!deadSFXbuffer.loadFromFile("resources/dead0.wav"))
	{
		std::cerr << "failed to load \"resources/dead0.wav\"" << std::endl;
		exit(EXIT_FAILURE);
	}
	deadSFX.setBuffer(deadSFXbuffer);
	deadSFX.setVolume(sfxVolume);
	allSFXvector.push_back(&deadSFX);

	//debug FPS font stuff
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
			
			window.draw(background);
			
			if(startButton.getGlobalBounds().contains(sf::Vector2f(window.mapPixelToCoords(sf::Mouse::getPosition(window), view)))) //view is in coords, without view is in pixels
			{
				startButton.setTextureRect(spriteSheetFrame(startButtonFrameWidth, startButtonFrameHeight, 1));

				if(!startButtonHoveredOver)
				{
					menuSFX.play();
				}
				startButtonHoveredOver = true;

				if(sf::Mouse::isButtonPressed(sf::Mouse::Left))
				{
					menuSFX.play();
					for(int x = 0 + 2; x < 8 + 2; x++)
					{
						startButton.setTextureRect(spriteSheetFrame(startButtonFrameWidth, startButtonFrameHeight, 2 + (x % 2)));
						window.clear(sf::Color::Black);
						window.draw(background);
						window.draw(startButton);
						window.display();
						SLEEP(0.1);
					}
					startMenu = false;
					playerIsAlive = true;
					continue;
				}
			} else
			{
				startButtonHoveredOver = false;
				startButton.setTextureRect(spriteSheetFrame(startButtonFrameWidth, startButtonFrameHeight, 0));
			}

			window.draw(startButton);

			window.display();
			lastframe = std::chrono::high_resolution_clock::now();
			deltaTime = lastframe - lastlastframe;

		} else if(pauseMenu || settingsMenu)
		{
			if(quitButton.getGlobalBounds().contains(sf::Vector2f(window.mapPixelToCoords(sf::Mouse::getPosition(window), view))))
			{
				quitButton.setTextureRect(spriteSheetFrame(quitButtonFrameWidth, quitButtonFrameHeight, 1));

				if(!quitButtonHoveredOver)
				{
					menuSFX.play();
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
				menuSFX.play();
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
			window.draw(fps);
			
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
			animatePlayAgainButtonDuration += deltaTime;

			lastlastframe = std::chrono::high_resolution_clock::now();
			window.clear(sf::Color::Black);
			
			window.draw(background);

			//TEMPORARY
			/*
			sf::Text highscores;
			highscores.setFont(scoreTextFont);
			std::string highscoresAppendString = "";
			for(std::map<int, std::string>::iterator it = savedPlayerScores.end(); it != savedPlayerScores.begin(); --it)
			{
				highscoresAppendString = it->second + " " + std::to_string(it->first) + '\n';
				std::cout << it->second << " " << it->first << std::endl;
			}
			highscores.setString(highscoresAppendString);
			highscores.setCharacterSize(24);
			highscores.setOrigin(highscores.getLocalBounds().width / 2, highscores.getLocalBounds().height / 2);
			highscores.setPosition(screenWidth / 2, (screenHeight / 4) * 1);
			window.draw(highscores);
			*/
			
			if(playAgainButton.getGlobalBounds().contains(sf::Vector2f(window.mapPixelToCoords(sf::Mouse::getPosition(window), view)))) //view is in coords, without view is in pixels
			{
				playAgainButton.setTextureRect(spriteSheetFrame(playAgainButtonFrameWidth, playAgainButtonFrameHeight, 2));

				if(!playAgainButtonHoveredOver)
				{
					menuSFX.play();
				}
				playAgainButtonHoveredOver = true;

				if(sf::Mouse::isButtonPressed(sf::Mouse::Left))
				{
					menuSFX.play();
					for(int x = 0 + 2; x < 8 + 2; x++)
					{
						playAgainButton.setTextureRect(spriteSheetFrame(playAgainButtonFrameWidth, playAgainButtonFrameHeight, 3 + (x % 2)));
						window.clear(sf::Color::Black);
						window.draw(background);
						window.draw(playAgainButton);
						window.display();
						SLEEP(0.1);
					}
					deathMenu = false;
					playerIsAlive = true;
					playerInitialJump = false;
					continue;
				}
			} else
			{
				playAgainButtonHoveredOver = false;
				if(animatePlayAgainButtonDuration.count() >= animatePlayAgainButtonDurationThreshold * 2)
				{
					playAgainButton.setTextureRect(spriteSheetFrame(playAgainButtonFrameWidth, playAgainButtonFrameHeight, 0));
					animatePlayAgainButtonDuration = std::chrono::seconds::zero();
				} else if(animatePlayAgainButtonDuration.count() >= animatePlayAgainButtonDurationThreshold)
				{
					playAgainButton.setTextureRect(spriteSheetFrame(playAgainButtonFrameWidth, playAgainButtonFrameHeight, 1));
				}
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
				missiles.emplace_back(Missile(&missileTexture, (screenWidth / 4) * 3, screenHeight / 2, defaultMissileSpeed));
			} else if(sf::Mouse::isButtonPressed(sf::Mouse::Left))
			{
				while(sf::Mouse::isButtonPressed(sf::Mouse::Left))
				{
					missiles.emplace_back(Missile(&missileTexture, window.mapPixelToCoords(sf::Mouse::getPosition(window), view).x, window.mapPixelToCoords(sf::Mouse::getPosition(window), view).y, defaultMissileSpeed));
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

			playerAngle = playerYvelocity * -1 * playerAngleMultiplier;// * deltaTime.count();

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
						missiles.emplace_back(Missile(&missileTexture, (screenWidth / 4) * 3.5, playerY + RANDOM(-100, 100), defaultMissileSpeed));
						missiles.emplace_back(Missile(&missileTexture, (screenWidth / 4) * 3.5, playerY + RANDOM(-100, 100), defaultMissileSpeed));
						missiles.emplace_back(Missile(&missileTexture, (screenWidth / 4) * 3.5, playerY + RANDOM(-100, 100), defaultMissileSpeed));
						missiles.emplace_back(Missile(&missileTexture, (screenWidth / 4) * 3.5, playerY + RANDOM(-100, 100), defaultMissileSpeed));
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
			window.draw(fps);
			
			window.display();
			lastframe = std::chrono::high_resolution_clock::now();
			deltaTime = lastframe - lastlastframe;

			fps.setString(std::to_string(1 / deltaTime.count()) + "\ndeltaTime(): " + std::to_string(deltaTime.count()));
		} else if(!playerIsAlive)
		{
			playerYvelocity += gravity * deltaTime.count();

			playerY += playerYvelocity * deltaTime.count();
			playerX += playerXvelocity * deltaTime.count();

			playerAngle = playerYvelocity * -1 * playerAngleMultiplier;// * deltaTime.count();

			playerHitbox.setPosition(playerX, playerY);
			player.setPosition(playerX, playerY);
			player.setRotation(playerAngle);

			//draw and deltaTime
			lastlastframe = std::chrono::high_resolution_clock::now();
			window.clear(sf::Color::Black);
		
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
			window.draw(fps);
			
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
