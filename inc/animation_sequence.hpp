#ifndef ANIMATION_SEQUENCE_H
#define ANIMATION_SEQUENCE_H
#include <iostream>
#include <string>
#include <fstream>
#include <sstream>
#include <vector>

class AnimationSeq {
public:
	AnimationSeq(){ }; // Null constructor
	AnimationSeq( std::vector<Animation*> animations_in) {
		this->animations = animations_in;
	}

	void add_animation(Animation* animation_in) {
		this->animations.push_back(animation_in);
	}

	bool tick() {
		if (this->animations[0]->state == 0)
			this->animations[0]->init();
		if (this->animations.size() == 0)
			return false;
		if (!this->animations[0]->tick())
			this->animations.erase(animations.begin());
		return true;
	}	

private:
	std::vector<Animation*> animations;

};
#endif
