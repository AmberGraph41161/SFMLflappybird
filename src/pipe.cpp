#include "pipe.hpp"

#include <SFML/Graphics.hpp>
#include <SFML/Graphics/RectangleShape.hpp>
#include <SFML/System/Vector2.hpp>

#include "gamefunctions.hpp"

Pipe::Pipe(sf::Texture &pipeTexture, double startX, double startY, double pipeSpacing, double pipeSpeed)
	: x(startX), y(startY), spacing(pipeSpacing), speed(pipeSpeed), topPipe(pipeTexture), bottomPipe(pipeTexture)
{
	topPipe.rotate(sf::Angle(sf::degrees(180)));
	topPipe.setOrigin(sf::Vector2f(topPipe.getLocalBounds().size.x / 2, topPipe.getLocalBounds().position.y));
	bottomPipe.setOrigin(sf::Vector2f(bottomPipe.getLocalBounds().size.x / 2, bottomPipe.getLocalBounds().position.y));

	topPipeHitbox = sf::RectangleShape(sf::Vector2f(topPipe.getLocalBounds().size.x - 4, topPipe.getLocalBounds().size.y));
	bottomPipeHitbox = sf::RectangleShape(sf::Vector2f(bottomPipe.getLocalBounds().size.x - 4, bottomPipe.getLocalBounds().size.y));
	topPipeHitbox.rotate(sf::Angle(sf::degrees(180)));

	topPipeHitbox.setFillColor(sf::Color::Cyan);
	bottomPipeHitbox.setFillColor(sf::Color::Cyan);
	
	topPipeHitbox.setOrigin(sf::Vector2f(topPipeHitbox.getLocalBounds().size.x / 2, topPipeHitbox.getLocalBounds().position.y));
	bottomPipeHitbox.setOrigin(sf::Vector2f(bottomPipeHitbox.getLocalBounds().size.x / 2, bottomPipeHitbox.getLocalBounds().position.y));

	topPipe.setScale(sf::Vector2f(defaultPipeScale, defaultPipeScale));
	bottomPipe.setScale(sf::Vector2f(defaultPipeScale, defaultPipeScale));
	
	topPipeHitbox.setScale(sf::Vector2f(defaultPipeScale, defaultPipeScale));
	bottomPipeHitbox.setScale(sf::Vector2f(defaultPipeScale, defaultPipeScale));

	topPipe.setPosition(sf::Vector2f(x, y - spacing));
	bottomPipe.setPosition(sf::Vector2f(x, y + spacing));
	
	topPipeHitbox.setPosition(topPipe.getPosition());
	bottomPipeHitbox.setPosition(bottomPipe.getPosition());
}

Pipe::~Pipe()
{
}

void Pipe::move(double deltaTime)
{
	x -= speed * deltaTime;

	topPipe.setPosition(sf::Vector2f(x, y - spacing));
	bottomPipe.setPosition(sf::Vector2f(x, y + spacing));

	topPipeHitbox.setPosition(topPipe.getPosition());
	bottomPipeHitbox.setPosition(bottomPipe.getPosition());
}

void Pipe::setPosition(double X, double Y)
{
	x = X;
	y = Y;

	topPipe.setPosition(sf::Vector2f(x, y - spacing));
	bottomPipe.setPosition(sf::Vector2f(x, y + spacing));
	
	topPipeHitbox.setPosition(topPipe.getPosition());
	bottomPipeHitbox.setPosition(bottomPipe.getPosition());
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
	if(
		topPipeHitbox.getGlobalBounds().findIntersection(objectGlobalBounds).has_value() ||
		bottomPipeHitbox.getGlobalBounds().findIntersection(objectGlobalBounds).has_value()
	)
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

sf::RectangleShape Pipe::getTopPipeHitbox()
{
	return topPipeHitbox;
}

sf::RectangleShape Pipe::getBottomPipeHitbox()
{
	return bottomPipeHitbox;
}

//non class functions...

void spawnDefaultRandomPipe(std::vector<Pipe> &pipes, sf::Texture &pipeTexture, double screenWidth, double screenHeight)
{
	pipes.emplace_back(Pipe(pipeTexture, screenWidth + RANDOM(0, 500), RANDOMDOUBLE((screenHeight / 2) - 200, (screenHeight / 2) + 200), RANDOMDOUBLE(70, 200), RANDOMDOUBLE(300, 700)));
}

void pipeTunnel(std::vector<Pipe> &pipes, sf::Texture &pipeTexture, int nPipes, double startX, double startY, double pipeSpacing, double pipeSpeed, double pipeXspacing)
{
	for(int x = 0; x < nPipes; x++)
	{
		pipes.emplace_back(Pipe(pipeTexture, startX + (x * pipeXspacing), startY, pipeSpacing, pipeSpeed));
	}
}

void pipeShrinkTunnel(std::vector<Pipe> &pipes, sf::Texture &pipeTexture, int nPipes, double startX, double startY, double pipeSpacing, double pipeSpeed, double pipeXspacing)
{
	for(int x = 0; x < nPipes; x++)
	{
		pipes.emplace_back(Pipe(pipeTexture, startX + (x * pipeXspacing), startY, pipeSpacing - (10 * x), pipeSpeed));
	}
}
