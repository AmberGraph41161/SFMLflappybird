#include <SFML/Graphics/Rect.hpp>
#include <SFML/Graphics/RectangleShape.hpp>
#include <SFML/System/Vector2.hpp>
#include <cstdlib>
#include <iostream>
#include <chrono>
#include <SFML/Graphics.hpp>
#include <SFML/Graphics/RenderWindow.hpp>
#include <SFML/Window/Keyboard.hpp>
#include <SFML/Window/VideoMode.hpp>

class Pipe
{
public:
	Pipe(sf::Texture* pipeTexture, double pipeSpeed)
		: texture(pipeTexture), speed(pipeSpeed)
	{
		topPipe.setTexture(*texture);
		bottomPipe.setTexture(*texture);

		topPipe.rotate(180);

		topPipe.setPosition(x, y + spacing);
		bottomPipe.setPosition(x, y - spacing);

		std::cout << "pipe created!" << std::endl;
	}
	~Pipe()
	{
		std::cout << "pipe destroyed!" << std::endl;
	}

	void move()
	{
		x -= speed;
	}

	void setPosition()
	{

	}

	bool isOffScreen(double left, double right, double top, double bottom)
	{
		if(x <= left || x >= right)
		{
			return true;
		}

		if(y <= bottom || y >= top)
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
};

int main()
{
	std::cout << "[doing setup crap]" << std::endl;

	const int screenWidth = 1080;
	const int screenHeight = 720;

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

	//load pipe stuff
	sf::Texture pipeTexture;
	if(!pipeTexture.loadFromFile("resources/longpipe.png"))
	{
		std::cerr << "failed to load \"resources/longpipe.png\"" << std::endl;
		exit(EXIT_FAILURE);
	}

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
	
	std::vector<Pipe> pipes;
	pipes.push_back(Pipe(&pipeTexture, 1));

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
