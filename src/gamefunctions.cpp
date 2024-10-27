#include "gamefunctions.hpp"

#include <iostream>
#include <map>
#include <fstream>
#include <chrono>

int RANDOM(int minimum, int maximum)
{
	return (rand() % (maximum - minimum + 1)) + minimum;
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

//playerName.txt file format:
	//playernamegoes here with spacesorwithnospacesatall or anythinglikethat

void savePlayerName(std::string playerName)
{
	std::fstream write;
	write.open("dat/playerName.txt", std::fstream::trunc);
	if(write.fail())
	{
		std::cerr << "failed to open \"dat/playerName.txt\"" << std::endl;
		write.close();
		return;
	}

	std::cout << "saving playerName..." << std::endl;
	write << playerName;
	write.close();
	std::cout << "done saving playerName [" << playerName << "] !" << std::endl;
}
std::string getSavedPlayerName()
{
	std::fstream read;
	read.open("dat/playerName.txt", std::fstream::in);
	if(read.fail())
	{
		std::cerr << "failed to open \"dat/playerName.txt\"" << std::endl;
		read.close();

		std::cerr << "creating default \"dat/playerName.txt\" file..." << std::endl;
		std::fstream write;
		write.open("dat/playerName.txt", std::fstream::out | std::fstream::trunc);
		if(write.fail())
		{
			std::cerr << "failed to create \"dat/playerName.txt\"" << std::endl;
			write.close();
			return "player0";
		}
		write << "player0";
		write.close();

		return "player0";
	}

	std::string playerName;
	std::cout << "getting playerName..." << std::endl;
	std::getline(read, playerName);
	read.close();
	std::cout << "done getting playerName [" << playerName << "] !" << std::endl;

	return playerName;
}

//scores.txt file format:
//hoping that \t will never be inserted by person
	//playerName0\tplayerScore0
	//playerName1\tplayerScore1
	//playerName2\tplayerScore2
//the scores are are mapped by std::string playername as the key value, so there are only unqiue players

void saveScores(std::map<std::string, int> &playerScores)
{
	std::fstream write;
	write.open("dat/scores.txt", std::fstream::out | std::fstream::trunc);
	if(write.fail())
	{
		std::cerr << "failed to open \"dat/scores.txt\"" << std::endl;
		write.close();

		return;
	}

	std::cout << "saving scores..." << std::endl;
	for(std::map<std::string, int>::iterator it = playerScores.begin(); it != playerScores.end(); ++it)
	{
		std::pair<std::string, int> nameScorePair = *it;
		write << nameScorePair.first << '\t' << nameScorePair.second << '\n';
	}
	write.close();
	std::cout << "done saving scores!" << std::endl;
}
void getSavedScores(std::map<std::string, int> &playerScores)
{
	std::fstream read;
	read.open("dat/scores.txt", std::fstream::in);
	if(read.fail())
	{
		std::cerr << "failed to open \"dat/scores.txt\"" << std::endl;
		read.close();
		return;
	}
	
	std::string getlinestring;
	while(std::getline(read, getlinestring))
	{
		if(getlinestring.find('\t') == std::string::npos)
		{
			continue;
		}

		std::string playerName = getlinestring.substr(0, getlinestring.find('\t'));
		getlinestring = getlinestring.substr(getlinestring.find('\t') + 1, getlinestring.size());

		int playerScore = 0;
		try
		{
			playerScore = std::stoi(getlinestring);
		} catch(...)
		{
			playerScore = 0;
		}
		playerScores.insert(std::make_pair(playerName, playerScore));
	}
}
