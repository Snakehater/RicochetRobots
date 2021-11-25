#ifndef GAME_H
#define GAME_H
#include <iostream>
#include <string>
#include <fstream>
#include <sstream>
#include <vector>

class Game {
public:
	int selectedRobot;
	Game(){
		selectedRobot = -1;
	}; // Null constructor
};
#endif
