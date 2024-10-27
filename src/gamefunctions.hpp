#ifndef GAMEFUNCTIONS_HPP
#define GAMEFUNCTIONS_HPP

#include <map>
#include <SFML/Graphics.hpp>

int RANDOM(int minimum, int maximum);
double RANDOMDOUBLE(double minimum, double maximum);
void SLEEP(double seconds);

sf::IntRect spriteSheetFrame(int spriteFrameWidth, int spriteFrameHeight, int frameNumber); //this is only for sprite sheet left to right horiztonal etc etc;

void savePlayerName(std::string playerName);
std::string getSavedPlayerName();
void saveScores(std::map<std::string, int> &playerScores);
void getSavedScores(std::map<std::string, int> &playerscores);

#endif
