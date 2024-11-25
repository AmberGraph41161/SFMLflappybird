#ifndef MISSILE_HPP
#define MISSILE_HPP

#include <chrono>
#include <SFML/Audio.hpp>
#include <SFML/Graphics.hpp>

class Missile
{
public:
	Missile(sf::Texture* missileTexture, double spawnX, double spawnY, double missileSpeed, sf::Sound *droppingSFX, sf::Sound *launchingSFX);
	~Missile();
	void move(double deltaTime);
	bool isOffScreen(double left, double width, double top, double height);
	bool isOffScreenLeftRight(double left, double width);
	bool isOffScreenBottomTop(double top, double height);
	sf::Sprite getMissile();

	bool isSpawnAnimationFinished();

private:
	sf::Sprite missile;
	sf::Texture* texture;

	int missileFrameWidth = 123;
	int missileFrameHeight = 36;

	//sf::RectangleShape hitbox(sf::Vector2f(1, 1));
	double x, y;
	double xVelocity = 0;
	double yVelocity = 0;
	double speed = 1000;
	double missileFallingSpeed = 1000;

	bool spawnAnimationFinished = false;
	double spawnAnimationHeightSpacing = 700;
	double spawnAnimationOriginalY;

	int animationCurrentFrame = 0;
	int animationFrameUpperBound = 15;
	int animationFrameLowerbound = 12;
	double animationFrameTimeBound = 0.08; //seconds
	std::chrono::duration<double> animationFrameDeltaTime = std::chrono::seconds::zero();

	sf::Sound *droppingSFX;
	sf::Sound *launchingSFX;

	bool droppingSFXwasPlayed = false;
	bool launchingSFXwasPlayed = false;
};

#endif
