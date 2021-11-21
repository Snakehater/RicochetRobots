#ifndef ROBOT_H
#define ROBOT_H
#include <iostream>
#include <string>
#include <fstream>
#include <sstream>
#include <vector>
#include <stdlib.h> /* for abs */

class Robot{
public:
	Mesh* mesh;

	Robot(){ }; // Null constructor
	Robot(Mesh* mesh_in) {
		this->mesh = mesh_in;
		this->move_debounce = false;
		if (!mesh->is_null())
			this->mesh->vPos.y = 1;
	}
	~Robot() {
		delete mesh;
	}

	Animation move(glm::vec3 direction, std::vector<glm::vec2>* walls) {
		if (abs(direction.x) + abs(direction.y) + abs(direction.z) != 1) {
			std::cout << "ERROR:ROBOT:INVALID_MOVE_DIRECTION" << std::endl;
			return Animation();
		}

		if (this->move_debounce)
			return Animation();

		return Animation();
	}
	bool is_available() {
		return !this->move_debounce;
	}
private:
	bool move_debounce;
};
#endif
