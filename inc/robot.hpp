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

	Animation move(AnimationSeq* animationSeq, glm::vec3 direction, std::vector<glm::vec2>* walls, int board_size, std::vector<Robot*>* robots) {
		if (abs(direction.x) + abs(direction.y) + abs(direction.z) != 1) {
			std::cout << "ERROR:ROBOT:INVALID_MOVE_DIRECTION" << std::endl;
			return Animation();
		}

		if (this->move_debounce)
			return Animation();
		
		glm::vec3 deltaPos = calc_collPos(direction, this->mesh->get_position(), walls, board_size, robots);
		
		// this will be the main animation
		Animation animation(mesh, deltaPos, true);
		animation.set_ticks(0.05f);

		this->move_debounce = true;
		
		// also set the raise and lower animations to make a better transition for moving
		Animation raise(this->mesh, glm::vec3(0.0f, 1.0f, 0.0f), true);
		Animation lower(this->mesh, glm::vec3(0.0f, -1.0f, 0.0f), true, &this->move_debounce);
		
		animationSeq->add_animation(raise);
		animationSeq->add_animation(animation);
		animationSeq->add_animation(lower);
		animationSeq->set_ticks(0.05f); // set ticks on all animations
		return animation;
	}
	bool is_available() {
		return !this->move_debounce;
	}
	glm::vec3* getHitBox(){
		glm::vec3 hitpoint1 = glm::vec3(this->mesh->vPos + glm::vec3(-0.5f, -0.5f, -0.5f));
		glm::vec3 hitpoint2 = glm::vec3(this->mesh->vPos + glm::vec3( 0.5f,  1.5f,  0.5f));
		glm::vec3* output = new glm::vec3[2];
		output[0] = hitpoint1;
		output[1] = hitpoint2;
		return &output[0];
	}
private:
	bool move_debounce;
	bool cmpFloats(float a, float b) {
		float EPSILON = 0.005f;
		float diff = a-b;
		return (diff < EPSILON) && (-diff < EPSILON);
	}
	glm::vec3 calc_collPos(glm::vec3 direction, glm::vec3 posIn, std::vector<glm::vec2>* walls, int board_size, std::vector<Robot*>* robots) {
		glm::vec2 dir2D = glm::vec2(direction.x, direction.z);
		glm::vec2 pos = glm::vec2(posIn.x, posIn.z);
		glm::vec2 last_pos = glm::vec2(0.0f, 0.0f);
		
		last_pos += dir2D; 

		// this will set last_pos to the the position in the dimention that we got after multiplying vector posIn with direction
		// and then move in the direction of dir2D until it moves into a wall
		for (last_pos = pos; last_pos.x < board_size && last_pos.y < board_size && last_pos.x >= 0 && last_pos.y >= 0; last_pos += dir2D) {
			if (last_pos != pos) {
				for (long unsigned int i = 0; i < robots->size(); i++) {
					Robot* eachRobot = (*robots)[i];
					glm::vec2 robotpos = glm::vec2(eachRobot->mesh->vPos.x, eachRobot->mesh->vPos.z);
					if (	this->cmpFloats(last_pos.x, robotpos.x) && 
						this->cmpFloats(last_pos.y, robotpos.y)) {
						last_pos -= dir2D; // we will stop the detection when we are ON a wall or robot, so we need to go back one when hitting another robot
						goto collision_detected;
					}
				}
			}

			glm::vec2 checkPos = 
				(last_pos) + 
				(glm::vec2(0.5f, 0.5f) * dir2D);

			for (long unsigned int i = 0; i < walls->size(); i++) {
				glm::vec2 wall = (*walls)[i];

				if (
						this->cmpFloats(checkPos.x, wall.x) &&
						this->cmpFloats(checkPos.y, wall.y)) {
					goto collision_detected;
				}
			}
		}
		// if we didn't detect a collition on the last run, we moved out of bounds, lets fix that:
		last_pos -= dir2D;
	collision_detected:
		last_pos -= pos;

		return glm::vec3(last_pos.x, 0.0f, last_pos.y);
	}
};
#endif
