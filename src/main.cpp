#include <cstdlib>
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

	void move()
	{
		x -= speed;

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
	double speed = 1;
	double defaultPipeScale = 5;
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
	double gravity = 1.3; 
	double antiGravity = gravity * 0.1;
	double playerAngle = 0;
	double playerAngleMultiplier = 100;
	bool playerJumpedLastTime = false;
	bool playerIsAlive = true;

	//load pipe stuff
	sf::Texture pipeTexture;
	if(!pipeTexture.loadFromFile("resources/longpipe.png"))
	{
		std::cerr << "failed to load \"resources/longpipe.png\"" << std::endl;
		exit(EXIT_FAILURE);
	}
	std::vector<Pipe> pipes;
	double defaultPipeSpacing = 100;
	double defaultPipeSpeed = 0.5;

	pipes.push_back(Pipe(&pipeTexture, screenWidth - 100, screenHeight / 2, defaultPipeSpacing, defaultPipeSpeed));



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
	startButton.setTexture(startButtonTexture);
	startButton.setTextureRect(sf::IntRect(0, 0, 33, 13));
	startButton.setScale(sf::Vector2f(10, 10));
	startButton.setOrigin(startButton.getLocalBounds().width / 2, startButton.getLocalBounds().height / 2);
	startButton.setPosition(screenWidth / 2, screenHeight / 2);

	std::chrono::time_point<std::chrono::system_clock> lastlastframe = std::chrono::high_resolution_clock::now();
	std::chrono::time_point<std::chrono::system_clock> lastframe = std::chrono::high_resolution_clock::now();
	std::chrono::duration<double> deltaTime = lastframe - lastlastframe;

	sf::RenderWindow window(sf::VideoMode(screenWidth, screenHeight), "title goes here", sf::Style::Default);
	window.setKeyRepeatEnabled(false);
	while(window.isOpen())
	{
		sf::Event event;
		while(window.pollEvent(event))
		{
			if(event.type == sf::Event::Closed)
			{
				window.close();
			}
		}
		if(sf::Keyboard::isKeyPressed(sf::Keyboard::Q))
		{
			window.close();
		}

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
			pipes.push_back(Pipe(&pipeTexture, screenWidth - 100, RANDOMDOUBLE(300, screenHeight - 300), defaultPipeSpacing, defaultPipeSpeed));
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
			//move player
			playerYvelocity += gravity * deltaTime.count();
		} else
		{
			playerYvelocity = 0;
			if(player.getGlobalBounds().intersects(ceiling.getGlobalBounds()))
			{
				playerY = ceiling.getGlobalBounds().height + player.getGlobalBounds().top;
			}
		}

		playerY += playerYvelocity;
		playerX += playerXvelocity;

		playerAngle = playerYvelocity * -1 * playerAngleMultiplier;

		player.setPosition(playerX, playerY);
		player.setRotation(playerAngle);

		//draw and deltaTime
		lastlastframe = std::chrono::high_resolution_clock::now();
		window.clear(sf::Color::Black);
	
		window.draw(ceiling);
		window.draw(floor);
		window.draw(background);
		
		window.draw(player);
		for(int x = 0; x < pipes.size(); x++)
		{
			window.draw(pipes[x].getTopPipe());
			window.draw(pipes[x].getBottomPipe());
			pipes[x].move();
		}

		window.draw(startButton);

		window.display();
		lastframe = std::chrono::high_resolution_clock::now();
		deltaTime = lastframe - lastlastframe;

		std::cout << "playerX" << playerX << std::endl;
		std::cout << "playerY" << playerY << std::endl;
		std::cout << "playerXvelocity" << playerXvelocity << std::endl;
		std::cout << "playerYvelocity" << playerYvelocity << std::endl;
		std::cout << "playerAngle" << playerAngle << std::endl;
	}


	std::cout << "[done!]" << std::endl;
	return 0;
}
