#ifndef COMMAND_BUFFER_H
#define COMMAND_BUFFER_H
#include <iostream>
#include <string>
#include <fstream>
#include <sstream>
#include <vector>

class CommandBuffer {
public:
	Mesh* mesh;
	bool enabled;
	CommandBuffer(){} // null constructor
	CommandBuffer(Mesh* meshIn, Game* gameIn, Camera* cameraIn){
		this->mesh = meshIn;
		this->game = gameIn;
		this->mesh->setSpecialColor(0.0f, 0.0f, 0.0f, 0.6f);
		this->enabled = false;
	};
	~CommandBuffer() {
		delete mesh;
	}
	void enable() {
		this->enabled = true;
		std::cout << "enable" << std::endl;
	}
	void update(GLFWwindow* window) {
		if (!this->enabled)
			return;
		glfwSetInputMode(window,GLFW_CURSOR,GLFW_CURSOR_NORMAL);
		std::cout << "Write your command here:" << std::endl << ">";
		std::string output;
		std::cin >> output;
		this->handle_command(output, window);
		glfwSetInputMode(window,GLFW_CURSOR,GLFW_CURSOR_DISABLED);
		this->enabled = false;
	}
	void handle_command(std::string command, GLFWwindow* window) {
		if (command.length() == 0)
			return;
		command.erase(0, 1);
		if (strcmp(command.c_str(), "exit"))
			glfwSetWindowShouldClose(window, true);
//		else if (strcmp(command, "lock"))
//			camera->lock();
//		else if (strcmp(command, "center"))
//			camera->center();
		else
			std::cout << "I don't recognize this command :(" << std::endl;
	}
private:
	Game* game;
};
#endif
