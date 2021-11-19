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
	Animation(Mesh* mesh_in, glm::vec3 vDeltaPosIn) {
		this->vDeltaPos = 	vDeltaPosIn;
		this->keepRot	=	true;
		this->tickSteps = 	DEFAULT.steps;
		this->targetMesh=	mesh_in;
		this->progress  =	0.0f;
	}
	Animation(Mesh* mesh_in, glm::vec3 vDeltaRotIn, float fDeltaRotIn) {
		this->vDeltaPos = 	glm::vec3(0.0f, 0.0f, 0.0f);
		this->vDeltaRot = 	vDeltaRotIn;
		this->fDeltaRot = 	fDeltaRotIn;
		this->tickSteps = 	DEFAULT.steps;
		this->targetMesh=	mesh_in;
		this->progress  =	0.0f;
		this->keepRot	=	false;
	}
	Animation(Mesh* mesh_in, glm::vec3 vDeltaPosIn, glm::vec3 vDeltaRotIn, float fDeltaRotIn) {
		this->vDeltaPos = 	vDeltaPosIn;
		this->vDeltaRot = 	vDeltaRotIn;
		this->fDeltaRot = 	fDeltaRotIn;
		this->tickSteps = 	DEFAULT.steps;
		this->targetMesh=	mesh_in;
		this->progress  =	0.0f;
		this->keepRot	=	false;
	}
	
	void set_ticks(float ticks_in) {
		this->tickSteps = ticks_in;
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
		if(this->keepRot) {
			this->vDeltaRot = this->targetMesh->get_rotation_vec();
			this->fDeltaRot = 0;
		}

		this->targetMesh->set_rotation_vec(this->vDeltaRot);

		this->vStartPos  = this->targetMesh->get_position();
//		this->vStartRot  = this->targetMesh->get_rotation_vec();
		this->fStartRot  = this->targetMesh->rotation_degree;
		this->state = 1;
	}

	void change_mesh(Mesh* new_mesh) {
		this->targetMesh = new_mesh;
	}

private:
	Mesh* targetMesh;
	float tickSteps;
	float progress;
	bool keepRot;
	
	glm::vec3 vStartPos;
	glm::vec3 vStartRot;
	float 	  fStartRot;
	
	glm::vec3 vDeltaPos;
	glm::vec3 vDeltaRot;
	float	  fDeltaRot;

	void apply_animation() {
		this->targetMesh->set_position(this->vStartPos + (this->vDeltaPos*easeInOutBack(this->progress, 0, 1, 1)));
//		this->targetMesh->set_rotation_vec(this->vStartRot + (this->vDeltaRot*(this->progress)));
		this->targetMesh->rotation_degree = this->fStartRot + (this->fDeltaRot*(this->progress));
	}

	bool normalize(float* val) {
		if (*val > 1.0f) {
			*val = 1.0f;
			return true;
		} else
			return false;
	}

	float easeInOutBack(float t,float b , float c, float d) {
		float sval = 1.70158f;
		if ((t/=d/2) < 1) 
			return c/2*(t*t*(((sval*(1.525f))+1)*t - sval*(1.525f))) + b;
		float postFix = t-=2;
		return c/2*((postFix)*t*(((sval*(1.525f))+1)*t + sval*(1.525f)) + 2) + b;
	}
};
#endif