#include "missile.hpp"

#include <iostream>
#include <chrono>
#include <SFML/Graphics.hpp>

#include "gamefunctions.hpp"

Missile::Missile(sf::Texture* missileTexture, double spawnX, double spawnY, double missileSpeed)
	: texture(missileTexture), x(spawnX), y(spawnY), speed(missileSpeed)
{
	spawnAnimationOriginalY = y;
	y -= spawnAnimationHeightSpacing;

	missile.setTexture(*texture);
	missile.setTextureRect(spriteSheetFrame(missileFrameWidth, missileFrameHeight, 0));
	missile.setOrigin(missile.getLocalBounds().width / 2, missile.getLocalBounds().height / 2);
	missile.setScale(sf::Vector2f(2, 2));

	std::cout << "missile created!" << std::endl;
}
Missile::~Missile()
{
	std::cout << "missile destroyed!" << std::endl;
}

void Missile::move(double deltaTime)
{
	animationFrameDeltaTime += std::chrono::duration<double>(deltaTime);

	if(animationFrameDeltaTime.count() >= animationFrameTimeBound && y >= spawnAnimationOriginalY - (((spawnAnimationHeightSpacing) / 6) * 5))
	{
		animationFrameDeltaTime = std::chrono::seconds::zero();
		animationCurrentFrame++;
		if(animationCurrentFrame >= animationFrameUpperBound)
		{
			animationCurrentFrame = animationFrameLowerbound;
			spawnAnimationFinished = true;
		}
	}

	missile.setTextureRect(spriteSheetFrame(missileFrameWidth, missileFrameHeight, animationCurrentFrame));

	if(spawnAnimationFinished)
	{
		xVelocity -= speed * deltaTime;
		x += xVelocity * deltaTime;
	} else
	{
		yVelocity += missileFallingSpeed * deltaTime;
		y += yVelocity * deltaTime;

		if(y >= spawnAnimationOriginalY)
		{
			y = spawnAnimationOriginalY;
			spawnAnimationFinished = true;
		}
	}

	missile.setPosition(x, y);
}

bool Missile::isOffScreen(double left, double width, double top, double height)
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

bool Missile::isOffScreenLeftRight(double left, double width)
{
	if(x <= left || x >= width)
	{
		return true;
	}

	return false;
}

bool Missile::isOffScreenBottomTop(double top, double height)
{
	if(y <= top || y >= height)
	{
		return true;
	}

	return false;
}

sf::Sprite Missile::getMissile()
{
	return missile;
}
