#include <cstdlib>
#include <ctime>
#include <iostream>
#include <chrono>
#include <SFML/Graphics.hpp>
#include <SFML/Graphics/RenderWindow.hpp>
#include <SFML/Window/Keyboard.hpp>
#include <SFML/Window/VideoMode.hpp>
#include <SFML/Graphics/Rect.hpp>
#include <SFML/Graphics/RectangleShape.hpp>
#include <SFML/System/Vector2.hpp>

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
	double defaultPipeScale = 5;

	bool pipeHasPastPlayer = false;
};

int main()
{
	std::cout << "[doing setup crap]" << std::endl;

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

	double playerX = static_cast<double>(screenWidth) / 2;
	double playerY = static_cast<double>(screenHeight) / 2;
	double playerXvelocity = 0;
	double playerYvelocity = 0;
	double gravity = 3000; //1.3
	double antiGravity = gravity * 0.2; //* 0.1
	double playerAngle = 0;
	double playerAngleMultiplier = 0.05;
	bool playerJumpedLastTime = false;
	bool playerIsAlive = true;
	bool startMenu = false;
	bool pauseMenu = false;
	bool deathMenu = false;
	int playerScore = 0;

	//player hitbox?
	sf::RectangleShape playerHitbox(sf::Vector2f(player.getLocalBounds().width, player.getLocalBounds().height));
	playerHitbox.setPosition(playerX, playerY);
	playerHitbox.setFillColor(sf::Color::Red);

	//load pipe stuff
	sf::Texture pipeTexture;
	if(!pipeTexture.loadFromFile("resources/longpipe.png"))
	{
		std::cerr << "failed to load \"resources/longpipe.png\"" << std::endl;
		exit(EXIT_FAILURE);
	}
	std::vector<Pipe> pipes;
	double defaultPipeSpacing = 100;
	double defaultPipeSpeed = 500; //0.2

	pipes.emplace_back(Pipe(&pipeTexture, screenWidth - 100, screenHeight / 2, defaultPipeSpacing, defaultPipeSpeed));

	//load background and flooring stuff
	sf::Sprite background;
	sf::Texture backgroundTexture;
	if(!backgroundTexture.loadFromFile("resources/slicebackground.png"))
	{
		std::cerr << "failed to load \"resources/slicebackground.png\"" << std::endl;
		exit(EXIT_FAILURE);
	}
	backgroundTexture.setRepeated(true);
	background.setTextureRect(sf::IntRect(0, 0, backgroundTexture.getSize().x * 7, backgroundTexture.getSize().y));
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
	scoreText.setCharacterSize(20);
	scoreText.setString(std::to_string(playerScore));
	scoreText.setOrigin(scoreText.getLocalBounds().width / 2, scoreText.getLocalBounds().height / 2);

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
	int startButtonFrameWidth = 33;
	int startButtonFrameHeight = 13;

	startButton.setTexture(startButtonTexture);
	startButton.setTextureRect(sf::IntRect(0, 0, startButtonFrameWidth, startButtonFrameHeight));
	startButton.setScale(sf::Vector2f(10, 10));
	startButton.setOrigin(startButton.getLocalBounds().width / 2, startButton.getLocalBounds().height / 2);
	startButton.setPosition(screenWidth / 2, screenHeight / 2);

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
		if(startMenu || pauseMenu)
		{
			//draw and deltaTime
			lastlastframe = std::chrono::high_resolution_clock::now();
			window.clear(sf::Color::Black);
			
			window.draw(background);
			
			if(startButton.getGlobalBounds().contains(sf::Vector2f(window.mapPixelToCoords(sf::Mouse::getPosition(window), view)))) //view is in coords, without view is in pixels
			{
				startButton.setTextureRect(spriteSheetFrame(startButtonFrameWidth, startButtonFrameHeight, 1));
				if(sf::Mouse::isButtonPressed(sf::Mouse::Left))
				{
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
				}
			} else
			{
				startButton.setTextureRect(spriteSheetFrame(startButtonFrameWidth, startButtonFrameHeight, 0));
			}

			window.draw(startButton);

			window.display();
			lastframe = std::chrono::high_resolution_clock::now();
			deltaTime = lastframe - lastlastframe;

		} else //the game (maybe this is a bad idea...?)
		{
			//debug controls
			if(sf::Keyboard::isKeyPressed(sf::Keyboard::Up))
			{
				background.move(0, -1);
			} else if(sf::Keyboard::isKeyPressed(sf::Keyboard::Down))
			{
				background.move(0, 1);
			} else if(sf::Keyboard::isKeyPressed(sf::Keyboard::Left))
			{
				background.move(-1, 0);
			} else if(sf::Keyboard::isKeyPressed(sf::Keyboard::Right))
			{
				background.move(1, 0);
			} else if(sf::Keyboard::isKeyPressed(sf::Keyboard::R))
			{
				background.rotate(5);
			} else if(sf::Keyboard::isKeyPressed(sf::Keyboard::A))
			{
				while(sf::Keyboard::isKeyPressed(sf::Keyboard::A))
				{

				}
				pipes.emplace_back(Pipe(&pipeTexture, screenWidth - 100, RANDOMDOUBLE(300, screenHeight - 300), defaultPipeSpacing, defaultPipeSpeed));
			}

			//controls
			if(!sf::Keyboard::isKeyPressed(sf::Keyboard::Space) && playerJumpedLastTime)
			{
				playerJumpedLastTime = false;
			}

			if(sf::Keyboard::isKeyPressed(sf::Keyboard::Space) && !playerJumpedLastTime)
			{
				playerJumpedLastTime = true;
				playerYvelocity = -1 * antiGravity;
			} else if(!player.getGlobalBounds().intersects(floor.getGlobalBounds()) && !player.getGlobalBounds().intersects(ceiling.getGlobalBounds()))
			{
				//move player down due to gravity (falling)
				playerYvelocity += gravity * deltaTime.count();
			} else
			{
				playerYvelocity = 0;
				if(player.getGlobalBounds().intersects(ceiling.getGlobalBounds()))
				{
					playerY = ceiling.getGlobalBounds().height + player.getGlobalBounds().top;
				}
			}

			playerY += playerYvelocity * deltaTime.count();
			playerX += playerXvelocity * deltaTime.count();
			playerHitbox.setPosition(playerX, playerY);

			playerAngle = playerYvelocity * -1 * playerAngleMultiplier;// * deltaTime.count();

			player.setPosition(playerX, playerY);
			player.setRotation(playerAngle);

			if(background.getPosition().x <= backgroundOriginalX - (backgroundTexture.getSize().x * 7))
			{
				background.setPosition(backgroundOriginalX, backgroundOriginalY);
			}
			background.move(-1 * backgroundSpeed * deltaTime.count(), 0);

			//draw and deltaTime
			lastlastframe = std::chrono::high_resolution_clock::now();
			window.clear(sf::Color::Black);
		
			window.draw(ceiling);
			window.draw(floor);
			window.draw(background);
			
			window.draw(player);
			window.draw(playerHitbox);
			for(int x = 0; x < pipes.size(); x++)
			{
				if(pipes[x].intersects(player.getGlobalBounds()))
				{
					playerIsAlive = false;
				}

				pipes[x].move(deltaTime.count());
				if(pipes[x].isPastPlayer(playerX) && !pipes[x].hasPastPlayer())
				{
					playerScore++;
				}

				if(pipes[x].isOffScreen(0, view.getSize().x, 0, screenHeight))
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

			if(playerIsAlive)
			{
				std::cout << "ALIVE" << std::endl;
				std::cout << playerScore << std::endl;
			} else
			{
				std::cout << "DEAD" << std::endl;
			}

			//SLEEP(0.001);
			/*
			std::cout << "playerX" << playerX << std::endl;
			std::cout << "playerY" << playerY << std::endl;
			std::cout << "playerXvelocity" << playerXvelocity << std::endl;
			std::cout << "playerYvelocity" << playerYvelocity << std::endl;
			std::cout << "playerAngle" << playerAngle << std::endl;
			*/
		}
	}

	std::cout << "[done!]" << std::endl;
	return 0;
}
