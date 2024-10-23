#ifndef PIPE_HPP
#define PIPE_HPP

#include <SFML/Graphics.hpp>

class Pipe
{
public:
	Pipe(sf::Texture* pipeTexture, double startX, double startY, double pipeSpacing, double pipeSpeed);
	~Pipe();

	void move(double deltaTime);
	void setPosition(double X, double Y);
	bool isOffScreen(double left, double width, double top, double height);
	bool isOffScreenLeftRight(double left, double width);
	bool isOffScreenBottomTop(double top, double height);
	bool isPastPlayer(double playerX);
	bool hasPastPlayer();
	bool intersects(sf::FloatRect objectGlobalBounds);

	sf::Sprite getTopPipe();
	sf::Sprite getBottomPipe();

private:
	sf::Sprite topPipe;
	sf::Sprite bottomPipe; //is the "mother pipe" or the "master pipe". topPipe will be relative to bottomPipe

	sf::Texture* texture;
	double x, y;
	double spacing = 0;
	double speed = 500; //1
	double defaultPipeScale = 4;

	bool pipeHasPastPlayer = false;
};

//non class functions...

void spawnDefaultRandomPipe(std::vector<Pipe> &pipes, sf::Texture *pipeTexture, double screenWidth, double screenHeight);
void pipeTunnel(std::vector<Pipe> &pipes, sf::Texture *pipeTexture, int nPipes, double startX, double startY, double pipeSpacing = 120, double pipeSpeed = 500, double pipeXspacing = 200);
void pipeShrinkTunnel(std::vector<Pipe> &pipes, sf::Texture *pipeTexture, int nPipes, double startX, double startY, double pipeSpacing = 250, double pipeSpeed = 500, double pipeXspacing = 100);

#endif
