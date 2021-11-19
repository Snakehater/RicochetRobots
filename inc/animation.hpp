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
	Animation(Mesh* mesh_in, glm::vec3 end_pos_vec_in, glm::vec3 end_rot_vec_in, float end_rot_deg_in, float steps_in=DEFAULT.steps) {
		this->vEndPos 	= 	end_pos_vec_in;
		this->vEndRot 	= 	end_rot_vec_in;
		this->fEndRot 	= 	end_rot_deg_in;
		this->tickSteps = 	steps_in;
		this->targetMesh=	mesh_in;
		this->progress  =	0.0f;

		this->vPosDiff  = end_pos_vec_in - mesh_in->get_position();
		this->vRotDiff  = end_rot_vec_in - mesh_in->get_rotation_vec();
		this->fRotDiff  = end_rot_deg_in - mesh_in->rotation_degree;
	}

	bool tick() {
		this->progress += this->tickSteps;
		this->normalize(&progress);
		this->apply_animation();
		if (this->progress >= 1.0f)
			return false;
		else
			return true;
	}

private:
	Mesh* targetMesh;
	glm::vec3 vEndPos;
	glm::vec3 vEndRot;
	float	  fEndRot;
	float tickSteps;
	float progress;
	glm::vec3 vPosDiff;
	glm::vec3 vRotDiff;
	float fRotDiff;

	void apply_animation() {
		targetMesh->set_position(this->vEndPos - (this->vPosDiff*(1-this->progress)));
		targetMesh->set_rotation_vec(this->vEndRot - (this->vRotDiff*(1-this->progress)));
		targetMesh->rotation_degree = this->fEndRot - (this->fRotDiff*(1-this->progress));
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
