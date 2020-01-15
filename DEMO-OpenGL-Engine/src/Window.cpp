#include "Window.h"

void HOEngine::Window::Init(HOEngine::Dimension dim, std::string& title) {
	glfwWindowHint(GLFW_CLIENT_API, GLFW_OPENGL_API);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	window = glfwCreateWindow(dim.width, dim.height, title.data(), nullptr, nullptr);
	if (window == nullptr) {
		throw std::runtime_error("Unable to create window");
	}

	glfwMakeContextCurrent(window);
	glfwGetFramebufferSize(window, &dim.width, &dim.height);
	glfwSetKeyCallback(window, this->keyCallback);
	glfwSetCharCallback(window, this->charCallback);
	glfwSetCursorPosCallback(window, this->cursorPosCallback);
	glfwSetScrollCallback(window, this->scrollCallback);
}
