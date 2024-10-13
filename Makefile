all:
	clang++ -Wall -std=c++17 -lsfml-window -lsfml-system -lsfml-graphics -lsfml-audio -o main src/main.cpp
