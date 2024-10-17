#include <iostream>
#include <cstdlib>
#include <ctime>
#include <chrono>
#include <fstream>
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


int RANDOM(int minimum, int maximum)
{
	return (rand() % maximum - minimum + 1) + minimum;
}

double RANDOMDOUBLE(double minimum, double maximum)
{
	return (((double)rand() / RAND_MAX) * (maximum - minimum)) + minimum;
}

void SLEEP(double seconds)
{
	std::chrono::time_point<std::chrono::system_clock> START, END;
	START = std::chrono::high_resolution_clock::now();
	while(true)
	{
		END = std::chrono::high_resolution_clock::now();
		std::chrono::duration<double> duration = END - START;
		if(duration.count() >= seconds)
		{
			break;
		}
	}
}

sf::IntRect spriteSheetFrame(int spriteFrameWidth, int spriteFrameHeight, int frameNumber) //this is only for sprite sheet left to right horiztonal etc etc
{
	return sf::IntRect(frameNumber * spriteFrameWidth, 0, spriteFrameWidth, spriteFrameHeight); //x, y, width, height
}

void appendHighScore(int score, std::string playerName = "player")
{
	std::fstream write;
	write.open("dat/scores.txt", std::fstream::out | std::fstream::app);
	if(write.fail())
	{
		std::cerr << "failed to open \"dat/scores.txt\"" << std::endl;
		write.close();
		return;
	}

	std::cout << "saving score..." << std::endl;
	write << playerName << ' ' << score << '\n';
	write.close();
	std::cout << "done saving score!" << std::endl;
}

class Pipe
{
public:
	Pipe(sf::Texture* pipeTexture, double startX, double startY, double pipeSpacing, double pipeSpeed)
		: texture(pipeTexture), x(startX), y(startY), spacing(pipeSpacing), speed(pipeSpeed)
	{
		topPipe.setTexture(*texture);
		bottomPipe.setTexture(*texture);

		topPipe.rotate(180);
		topPipe.setOrigin(topPipe.getLocalBounds().width / 2, topPipe.getLocalBounds().top);
		bottomPipe.setOrigin(bottomPipe.getLocalBounds().width / 2, bottomPipe.getLocalBounds().top);

		topPipe.setScale(sf::Vector2f(defaultPipeScale, defaultPipeScale));
		bottomPipe.setScale(sf::Vector2f(defaultPipeScale, defaultPipeScale));

		topPipe.setPosition(x, y - spacing);
		bottomPipe.setPosition(x, y + spacing);

		std::cout << "pipe created!" << std::endl;
	}
	~Pipe()
	{
		std::cout << "pipe destroyed!" << std::endl;
	}

	void move(double deltaTime)
	{
		x -= speed * deltaTime;

		topPipe.setPosition(x, y - spacing);
		bottomPipe.setPosition(x, y + spacing);
	}

	void setPosition(double X, double Y)
	{
		x = X;
		y = Y;

		topPipe.setPosition(x, y - spacing);
		bottomPipe.setPosition(x, y + spacing);
	}

	bool isOffScreen(double left, double width, double top, double height)
	{
		if(x <= left || x >= width)
		{
			return true;
		}

		if(y >= height || y <= top)
		{
			return true;
		}

		return false;
	}

	bool isPastPlayer(double playerX)
	{
		if(x <= playerX)
		{
			pipeHasPastPlayer = true;
			return true;
		}
		return false;
	}

	bool hasPastPlayer()
	{
		return pipeHasPastPlayer;
	}

	bool intersects(sf::FloatRect objectGlobalBounds)
	{
		if(topPipe.getGlobalBounds().intersects(objectGlobalBounds) || bottomPipe.getGlobalBounds().intersects(objectGlobalBounds))
		{
			return true;
		}

		return false;
	}

	sf::Sprite getTopPipe()
	{
		return topPipe;
	}
	sf::Sprite getBottomPipe()
	{
		return bottomPipe;
	}

private:
	sf::Sprite topPipe;
	sf::Sprite bottomPipe; //is the "mother pipe" or the "master pipe". topPipe will be relative to bottomPipe

	sf::Texture* texture;
	double x, y;
	double spacing = 0;
	double speed = 500; //1
	double defaultPipeScale = 4;

	bool pipeHasPastPlayer = false;
};

void spawnDefaultRandomPipe(std::vector<Pipe> &pipes, sf::Texture *pipeTexture, double screenWidth, double screenHeight)
{
	pipes.emplace_back(Pipe(pipeTexture, screenWidth + RANDOM(0, 500), RANDOMDOUBLE((screenHeight / 2) - 200, (screenHeight / 2) + 200), RANDOMDOUBLE(70, 200), RANDOMDOUBLE(300, 700)));
}

void pipeTunnel(std::vector<Pipe> &pipes, sf::Texture *pipeTexture, int nPipes, double startX, double startY, double pipeSpacing = 120, double pipeSpeed = 500, double pipeXspacing = 200)
{
	for(int x = 0; x < nPipes; x++)
	{
		pipes.emplace_back(Pipe(pipeTexture, startX + (x * pipeXspacing), startY, pipeSpacing, pipeSpeed));
	}
}

void pipeShrinkTunnel(std::vector<Pipe> &pipes, sf::Texture *pipeTexture, int nPipes, double startX, double startY, double pipeSpacing = 250, double pipeSpeed = 500, double pipeXspacing = 100)
{
	for(int x = 0; x < nPipes; x++)
	{
		pipes.emplace_back(Pipe(pipeTexture, startX + (x * pipeXspacing), startY, pipeSpacing - (10 * x), pipeSpeed));
	}
}

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
	int playerScore = 0;

	//player settings...?
	bool displayHitboxes = false;
	double sfxVolume = 100;
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
	scoreText.setString(std::to_string(playerScore));
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
			if(sf::Mouse::isButtonPressed(sf::Mouse::Left))
			{
				pauseMenu = false;
				settingsMenu = false;
			}

			//draw and deltaTime
			lastlastframe = std::chrono::high_resolution_clock::now();
			window.clear(sf::Color::Black);
		
			window.draw(background);
			
			window.draw(player);
			
			for(int x = 0; x < pipes.size(); x++)
			{
				window.draw(pipes[x].getTopPipe());
				window.draw(pipes[x].getBottomPipe());
			}
			
			if(displayHitboxes)
			{
				window.draw(ceiling);
				window.draw(floor);
				window.draw(playerHitbox);
			}

			window.draw(scoreText);
			window.draw(fps);
			
			window.display();
			lastframe = std::chrono::high_resolution_clock::now();
			deltaTime = lastframe - lastlastframe;

		} else if(deathMenu)
		{
			animatePlayAgainButtonDuration += deltaTime;

			lastlastframe = std::chrono::high_resolution_clock::now();
			window.clear(sf::Color::Black);
			
			window.draw(background);
			
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
				pipes.emplace_back(Pipe(&pipeTexture, screenWidth - 100, RANDOMDOUBLE(300, screenHeight - 300), defaultPipeSpacing, defaultPipeSpeed));
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
					playerScore++;
					scoreText.setString(std::to_string(playerScore));
					scoreSFX.play();
					if(RANDOM(0, 10) == 5 && !pipesSubroutine)
					{
						pipeTunnel(pipes, &pipeTexture, RANDOM(0, 20), screenWidth - 100, screenHeight / 2);
						pipesSubroutine = true;
					} else if(RANDOM(0, 15) == 5 && !pipesSubroutine)
					{
						pipeShrinkTunnel(pipes, &pipeTexture, RANDOM(0, 20), screenWidth - 100, screenHeight / 2);
						pipesSubroutine = true;
					} else if(!pipesSubroutine)
					{
						spawnDefaultRandomPipe(pipes, &pipeTexture, screenWidth, screenHeight);
					}
				}

				if(pipes[x].isOffScreen(0 - 200, view.getSize().x + 10000, 0, screenHeight))
				{
					pipes.erase(pipes.begin() + x);
					x--;
					continue;
				}

				window.draw(pipes[x].getTopPipe());
				window.draw(pipes[x].getBottomPipe());
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
			window.draw(player);

			window.draw(scoreText);
			window.draw(fps);
			
			window.display();
			lastframe = std::chrono::high_resolution_clock::now();
			deltaTime = lastframe - lastlastframe;

			if(player.getPosition().y >= screenHeight)
			{
				pipes.clear();

				playerScore = 0;
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

	std::cout << "[done!]" << std::endl;
	return 0;
}
