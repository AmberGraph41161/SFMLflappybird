#include "gamefunctions.hpp"

#include <iostream>
#include <fstream>
#include <chrono>

int RANDOM(int minimum, int maximum)
{
	return (rand() % maximum - minimum + 1) + minimum;
}

double RANDOMDOUBLE(double minimum, double maximum)
{
	return (((double)rand() / RAND_MAX) * (maximum - minimum)) + minimum;
}

void SLEEP(double seconds)
{
	std::chrono::time_point<std::chrono::system_clock> START, END;
	START = std::chrono::high_resolution_clock::now();
	while(true)
	{
		END = std::chrono::high_resolution_clock::now();
		std::chrono::duration<double> duration = END - START;
		if(duration.count() >= seconds)
		{
			break;
		}
	}
}

sf::IntRect spriteSheetFrame(int spriteFrameWidth, int spriteFrameHeight, int frameNumber) //this is only for sprite sheet left to right horiztonal etc etc
{
	return sf::IntRect(frameNumber * spriteFrameWidth, 0, spriteFrameWidth, spriteFrameHeight); //x, y, width, height
}

void appendHighScore(int score, std::string playerName)
{
	std::fstream write;
	write.open("dat/scores.txt", std::fstream::out | std::fstream::app);
	if(write.fail())
	{
		std::cerr << "failed to open \"dat/scores.txt\"" << std::endl;
		write.close();
		return;
	}

	std::cout << "saving score..." << std::endl;
	write << playerName << ' ' << score << '\n';
	write.close();
	std::cout << "done saving score!" << std::endl;
}
