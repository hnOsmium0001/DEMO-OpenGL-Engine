#include "Window.h"

void HOEngine::Window::Init(HOEngine::Dimension dim, std::string& title) {
	window = glfwCreateWindow(dim.width, dim.height, title.data(), nullptr, nullptr);
	if (window == nullptr) {
		throw std::runtime_error("Unable to create window");
	}
	glfwGetFramebufferSize(window, &dim.width, &dim.height);
}
