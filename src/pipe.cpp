#include "pipe.hpp"

#include <SFML/Graphics.hpp>
#include <SFML/Graphics/RectangleShape.hpp>
#include <SFML/System/Vector2.hpp>

#include "gamefunctions.hpp"

Pipe::Pipe(sf::Texture* pipeTexture, double startX, double startY, double pipeSpacing, double pipeSpeed)
	: texture(pipeTexture), x(startX), y(startY), spacing(pipeSpacing), speed(pipeSpeed)
{
	topPipe.setTexture(*texture);
	bottomPipe.setTexture(*texture);

	topPipe.rotate(180);
	topPipe.setOrigin(topPipe.getLocalBounds().width / 2, topPipe.getLocalBounds().top);
	bottomPipe.setOrigin(bottomPipe.getLocalBounds().width / 2, bottomPipe.getLocalBounds().top);

	topPipeHitbox = sf::RectangleShape(sf::Vector2f(topPipe.getLocalBounds().width - 4, topPipe.getLocalBounds().height));
	bottomPipeHitbox = sf::RectangleShape(sf::Vector2f(bottomPipe.getLocalBounds().width - 4, bottomPipe.getLocalBounds().height));
	topPipeHitbox.rotate(180);

	topPipeHitbox.setFillColor(sf::Color::Cyan);
	bottomPipeHitbox.setFillColor(sf::Color::Cyan);
	
	topPipeHitbox.setOrigin(topPipeHitbox.getLocalBounds().width / 2, topPipeHitbox.getLocalBounds().top);
	bottomPipeHitbox.setOrigin(bottomPipeHitbox.getLocalBounds().width / 2, bottomPipeHitbox.getLocalBounds().top);

	topPipe.setScale(sf::Vector2f(defaultPipeScale, defaultPipeScale));
	bottomPipe.setScale(sf::Vector2f(defaultPipeScale, defaultPipeScale));
	
	topPipeHitbox.setScale(sf::Vector2f(defaultPipeScale, defaultPipeScale));
	bottomPipeHitbox.setScale(sf::Vector2f(defaultPipeScale, defaultPipeScale));

	topPipe.setPosition(x, y - spacing);
	bottomPipe.setPosition(x, y + spacing);
	
	topPipeHitbox.setPosition(topPipe.getPosition());
	bottomPipeHitbox.setPosition(bottomPipe.getPosition());
}

Pipe::~Pipe()
{
}

void Pipe::move(double deltaTime)
{
	x -= speed * deltaTime;

	topPipe.setPosition(x, y - spacing);
	bottomPipe.setPosition(x, y + spacing);

	topPipeHitbox.setPosition(topPipe.getPosition());
	bottomPipeHitbox.setPosition(bottomPipe.getPosition());
}

void Pipe::setPosition(double X, double Y)
{
	x = X;
	y = Y;

	topPipe.setPosition(x, y - spacing);
	bottomPipe.setPosition(x, y + spacing);
	
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
	if(topPipeHitbox.getGlobalBounds().intersects(objectGlobalBounds) || bottomPipeHitbox.getGlobalBounds().intersects(objectGlobalBounds))
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
