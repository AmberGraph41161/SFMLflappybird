#include <SFML/Graphics/RenderWindow.hpp>
#include <SFML/Graphics/Texture.hpp>
#include <SFML/System/Vector2.hpp>
#include <SFML/Window/Event.hpp>
#include <SFML/Window/Keyboard.hpp>
#include <SFML/Window/VideoMode.hpp>
#include <SFML/Window/WindowStyle.hpp>
#include <cstdlib>
#include <iostream>
#include <string>
#include <vector>
#include <thread>
#include <SFML/Graphics/Sprite.hpp>
#include <SFML/Graphics.hpp>
#include <SFML/Audio.hpp>

void MASTERESCAPE(bool *RUNNING)
{
	while(*RUNNING)
	{
		if(sf::Keyboard::isKeyPressed(sf::Keyboard::Escape))
		{
			exit(EXIT_FAILURE);
		}
	}
}

int main()
{
	//MASTERESCAPE
	bool RUNNING = true;
	std::thread MASTERESCAPETHREAD(MASTERESCAPE, &RUNNING);

	//vars to change later idk
	std::string pipeTextureFileLocation = "resources/pipe.png";
	std::string flappybirdTextureFileLocation = "resources/flappybird.png";
	int pipeLimit = 4;
	//load assets first
	sf::Sprite flappybird;
	std::vector<sf::Sprite> topPipes;
	std::vector<sf::Sprite> bottomPipes;

	sf::Texture flappybirdTexture;
	if(!flappybirdTexture.loadFromFile(flappybirdTextureFileLocation))
	{
		std::cerr << "[failed to load \"" << flappybirdTextureFileLocation << "\" ]" << std::endl;
		std::cerr << "[terminating program...]" << std::endl;
		exit(EXIT_FAILURE);
	}
	flappybird.setTexture(flappybirdTexture);

	sf::Texture pipeTexture;
	if(!pipeTexture.loadFromFile(pipeTextureFileLocation))
	{
		std::cerr << "[failed to load \"" << pipeTextureFileLocation << "\" ]" << std::endl;
		std::cerr << "[terminating program...]" << std::endl;
		exit(EXIT_FAILURE);
	}

	for(int x = 0; x < pipeLimit; x++)
	{
		bottomPipes.push_back(sf::Sprite(pipeTexture));
	}
	for(int x = 0; x < pipeLimit; x++)
	{
		topPipes.push_back(sf::Sprite(pipeTexture));
	}
	for(int x = 0; x < topPipes.size(); x++)
	{
		topPipes[x].setRotation(180); //flip pipes upside down for the top pipes
	}

	//preset sprite positions
	flappybird.setPosition(100, 100);
	for(int x = 0; x < bottomPipes.size(); x++)
	{
		bottomPipes[x].setPosition(x * 500, 700);
	}
	for(int x = 0; x < topPipes.size(); x++)
	{
		topPipes[x].setPosition(x * 500 + 400, 400);
	}

	//SFML STUFF YAY!!!!
	sf::RenderWindow window(sf::VideoMode(1920, 1080), "SFMLflappybird", sf::Style::Fullscreen);
	while (window.isOpen())
	{
		sf::Event event;
		while(window.pollEvent(event))
		{
			if(event.type == sf::Event::Closed)
			{
				window.close();
			}
		}

		flappybird.move(sf::Vector2f(0, 0.1));
		if(sf::Keyboard::isKeyPressed(sf::Keyboard::Space))
		{
			flappybird.move(sf::Vector2f(0, -1));
		}

		//draw stuff
		window.clear(sf::Color::Black);
		for(int x = 0; x < topPipes.size(); x++)
		{
			window.draw(topPipes[x]);
		}
		for(int x = 0; x < bottomPipes.size(); x++)
		{
			window.draw(bottomPipes[x]);
		}
		window.draw(flappybird);
		window.display();
	}

	//join MASTERESCAPETHREAD
	RUNNING = false;
	MASTERESCAPETHREAD.join();
	return 0;
}
