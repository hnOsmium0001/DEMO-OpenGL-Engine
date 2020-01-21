#include <iostream>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include "../src/HOEngine.h"

void keyCallback(GLFWwindow* window, i32 key, i32 scancode, i32 action, i32 modss) {
	std::cout << key << "\n";
}

int main(void) {
	glfwInit();

	HOEngine::Window window = {};
	HOEngine::WindowCallbacks callbacks = {
		std::optional(keyCallback),
		std::nullopt,
		std::nullopt,
		std::nullopt,
		std::nullopt
	};
	window.Init({ 600, 400 }, "Test window", callbacks);

	while (!window.ShouldClose()) {
		glfwPollEvents();
	}

	return 0;
}
