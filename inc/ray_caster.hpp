#ifndef RAY_CASTER_H
#define RAY_CASTER_H
#include <iostream>
#include <string>
#include <fstream>
#include <sstream>
#include <vector>

class RayCaster {
public:
	int nx, ny, nz; //declared for near points
	int fx, fy, fz; //declared for far points
	// another way
	glm::vec3 near;
	glm::vec3 far;
	RayCaster(){ }; // Null constructor
	int cast(int mouseX, int mouseY, Camera* camera, std::vector<Robot*>* robots){
		glm::vec3 camvec = camera->position;
//		std::cout << camvec.x << ' ' << camvec.y << ' ' << camvec.z << std::endl;
		
		int robotHit = -1;

		for (float i = 0; i < 30; i += 0.001f) {
			glm::vec3 pos = camvec + (camera->cam_front * glm::vec3(i,i,i));
			for(long unsigned int j = 0; j < robots->size(); j++) {
				if (this->check_hit_box(pos, (*robots)[j])) {
					robotHit = j;
					goto hitDetected;
				}
			}
		}
	hitDetected:
		return robotHit;
				
	}
private:
	bool check_hit_box(glm::vec3 pos, Robot* robot) {
		glm::vec3* hitBox = robot->getHitBox();
		if (		hitBox[0].x < pos.x &&
				hitBox[0].y < pos.y &&
				hitBox[0].z < pos.z) {
			if (		hitBox[1].x > pos.x &&
					hitBox[1].y > pos.y &&
					hitBox[1].z > pos.z) {
				return true;
			}
		}
		return false;
	}
};
#endif
