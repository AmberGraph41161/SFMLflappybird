#ifndef GAMEFUNCTIONS_HPP
#define GAMEFUNCTIONS_HPP

#include <map>
#include <SFML/Graphics.hpp>

int RANDOM(int minimum, int maximum);
double RANDOMDOUBLE(double minimum, double maximum);

sf::IntRect spriteSheetFrame(int spriteFrameWidth, int spriteFrameHeight, int frameNumber); //this is only for sprite sheet left to right horiztonal etc etc;

void savePlayerName(const std::filesystem::path savedPlayerNamePath, std::string playerName);
std::string getSavedPlayerName(const std::filesystem::path savedPlayerNamePath);
void saveScores(const std::filesystem::path savedScoresPath, std::map<std::string, int> &playerScores);
void getSavedScores(const std::filesystem::path savedScoresPath, std::map<std::string, int> &playerscores);

#endif
