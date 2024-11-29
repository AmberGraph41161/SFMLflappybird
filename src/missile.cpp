#include "missile.hpp"

#include <SFML/Graphics/RectangleShape.hpp>
#include <SFML/System/Vector2.hpp>
#include <chrono>
#include <SFML/Graphics.hpp>

#include "gamefunctions.hpp"

Missile::Missile(sf::Texture* missileTexture, double spawnX, double spawnY, double missileSpeed, sf::Sound *droppingSFX, sf::Sound *launchingSFX)
	: texture(missileTexture), x(spawnX), y(spawnY), speed(missileSpeed), droppingSFX(droppingSFX), launchingSFX(launchingSFX)
{
	spawnAnimationOriginalY = y;
	y -= spawnAnimationHeightSpacing;

	missile.setTexture(*texture);
	missile.setTextureRect(spriteSheetFrame(missileFrameWidth, missileFrameHeight, 0));
	missile.setOrigin(missile.getLocalBounds().width / 2, missile.getLocalBounds().height / 2);
	missile.setScale(sf::Vector2f(2, 2));

	//(68 / 123) --> width of missile sprite in pixels excluding fire pixels
	hitbox = sf::RectangleShape(sf::Vector2f((missile.getGlobalBounds().width / 123) * 60, missile.getGlobalBounds().height / 2));
	hitbox.setOrigin(hitbox.getLocalBounds().width / 2, hitbox.getLocalBounds().height / 2);
	hitbox.setFillColor(sf::Color::Cyan);
}

Missile::~Missile()
{
}

void Missile::move(double deltaTime)
{
	if(!droppingSFXwasPlayed)
	{
		droppingSFX->play();
		droppingSFXwasPlayed = true;
	}
	if(!launchingSFXwasPlayed && spawnAnimationFinished)
	{
		launchingSFX->play();
		launchingSFXwasPlayed = true;
	}

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
	hitbox.setPosition(missile.getPosition());
	hitbox.move(hitboxXoffset, 0);
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

sf::RectangleShape Missile::getMissileHitbox()
{
	return hitbox;
}


bool Missile::isSpawnAnimationFinished()
{
	return spawnAnimationFinished;
}
