#include "pipe.hpp"

#include <iostream>
#include <SFML/Graphics.hpp>

#include "gamefunctions.hpp"

Pipe::Pipe(sf::Texture* pipeTexture, double startX, double startY, double pipeSpacing, double pipeSpeed)
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
Pipe::~Pipe()
{
	std::cout << "pipe destroyed!" << std::endl;
}

void Pipe::move(double deltaTime)
{
	x -= speed * deltaTime;

	topPipe.setPosition(x, y - spacing);
	bottomPipe.setPosition(x, y + spacing);
}

void Pipe::setPosition(double X, double Y)
{
	x = X;
	y = Y;

	topPipe.setPosition(x, y - spacing);
	bottomPipe.setPosition(x, y + spacing);
}

bool Pipe::isOffScreen(double left, double width, double top, double height)
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

bool Pipe::isOffScreenLeftRight(double left, double width)
{
	if(x <= left || x >= width)
	{
		return true;
	}

	return false;
}

bool Pipe::isOffScreenBottomTop(double top, double height)
{
	if(y <= top || y >= height)
	{
		return true;
	}

	return false;
}

bool Pipe::isPastPlayer(double playerX)
{
	if(x <= playerX)
	{
		pipeHasPastPlayer = true;
		return true;
	}
	return false;
}

bool Pipe::hasPastPlayer()
{
	return pipeHasPastPlayer;
}

bool Pipe::intersects(sf::FloatRect objectGlobalBounds)
{
	if(topPipe.getGlobalBounds().intersects(objectGlobalBounds) || bottomPipe.getGlobalBounds().intersects(objectGlobalBounds))
	{
		return true;
	}

	return false;
}

sf::Sprite Pipe::getTopPipe()
{
	return topPipe;
}
sf::Sprite Pipe::getBottomPipe()
{
	return bottomPipe;
}

//non class functions...

void spawnDefaultRandomPipe(std::vector<Pipe> &pipes, sf::Texture *pipeTexture, double screenWidth, double screenHeight)
{
	pipes.emplace_back(Pipe(pipeTexture, screenWidth + RANDOM(0, 500), RANDOMDOUBLE((screenHeight / 2) - 200, (screenHeight / 2) + 200), RANDOMDOUBLE(70, 200), RANDOMDOUBLE(300, 700)));
}

void pipeTunnel(std::vector<Pipe> &pipes, sf::Texture *pipeTexture, int nPipes, double startX, double startY, double pipeSpacing, double pipeSpeed, double pipeXspacing)
{
	for(int x = 0; x < nPipes; x++)
	{
		pipes.emplace_back(Pipe(pipeTexture, startX + (x * pipeXspacing), startY, pipeSpacing, pipeSpeed));
	}
}

void pipeShrinkTunnel(std::vector<Pipe> &pipes, sf::Texture *pipeTexture, int nPipes, double startX, double startY, double pipeSpacing, double pipeSpeed, double pipeXspacing)
{
	for(int x = 0; x < nPipes; x++)
	{
		pipes.emplace_back(Pipe(pipeTexture, startX + (x * pipeXspacing), startY, pipeSpacing - (10 * x), pipeSpeed));
	}
}
