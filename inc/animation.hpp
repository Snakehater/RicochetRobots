#ifndef ANIMATION_H
#define ANIMATION_H
#include <iostream>
#include <string>
#include <fstream>
#include <sstream>

struct DEFAULTS{
	float steps = 0.0001f;
} DEFAULT;

class Animation {
public:
	Animation(){ }; // Null constructor
	Animation(Mesh* mesh_in, glm::vec3 vDeltaPosIn, glm::vec3 vDeltaRotIn, float fDeltaRotIn, float steps_in=DEFAULT.steps) {
		this->vDeltaPos = 	vDeltaPosIn;
		this->vDeltaRot = 	vDeltaRotIn;
		this->fDeltaRot = 	fDeltaRotIn;
		this->tickSteps = 	steps_in;
		this->targetMesh=	mesh_in;
		this->progress  =	0.0f;
	}

	bool tick() {
		if (this->state != 1) {
			std::cout << "ERROR::ANIMATION::NOT_INITIATED" << std::endl;
		}
		this->progress += this->tickSteps;
		this->normalize(&progress);
		this->apply_animation();
		if (this->progress >= 1.0f)
			return false;
		else
			return true;
	}
	int state = 0;
	void init() {
		this->vStartPos  = this->targetMesh->get_position();
		this->vStartRot  = this->targetMesh->get_rotation_vec();
		this->fStartRot  = this->targetMesh->rotation_degree;
		this->state = 1;
	}

private:
	Mesh* targetMesh;
	float tickSteps;
	float progress;
	
	glm::vec3 vStartPos;
	glm::vec3 vStartRot;
	float 	  fStartRot;
	
	glm::vec3 vDeltaPos;
	glm::vec3 vDeltaRot;
	float	  fDeltaRot;

	void apply_animation() {
		this->targetMesh->set_position(this->vStartPos + (this->vDeltaPos*(this->progress)));
		this->targetMesh->set_rotation_vec(this->vStartRot + (this->vDeltaRot*(this->progress)));
		this->targetMesh->rotation_degree = this->fStartRot + (this->fDeltaRot*(this->progress));
	}

	bool normalize(float* val) {
		if (*val > 1.0f) {
			*val = 1.0f;
			return true;
		} else
			return false;
	}
};
#endif
