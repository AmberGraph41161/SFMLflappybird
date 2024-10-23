#ifndef GAMEFUNCTIONS_HPP
#define GAMEFUNCTIONS_HPP

#include <SFML/Graphics.hpp>

int RANDOM(int minimum, int maximum);
double RANDOMDOUBLE(double minimum, double maximum);
void SLEEP(double seconds);

sf::IntRect spriteSheetFrame(int spriteFrameWidth, int spriteFrameHeight, int frameNumber); //this is only for sprite sheet left to right horiztonal etc etc;
void appendHighScore(int score, std::string playerName = "player");

#endif
