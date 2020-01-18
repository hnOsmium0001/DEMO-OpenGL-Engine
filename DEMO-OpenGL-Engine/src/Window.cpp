#include "Window.h"
#include <iostream>

HOEngine::Window::Window(Window&& source)
	: _dim{ std::move(source._dim) },
	_window{ std::move(source._window) } {
}

HOEngine::Window::~Window() {
	glfwDestroyWindow(_window);
}

void HOEngine::Window::Init(HOEngine::Dimension dim, const std::string& title, HOEngine::WindowCallbacks callbacks) {
	glfwWindowHint(GLFW_CLIENT_API, GLFW_OPENGL_API);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	_window = glfwCreateWindow(dim.width, dim.height, title.c_str(), nullptr, nullptr);
	if (_window == nullptr) {
		throw std::runtime_error("Unable to create window");
	}

	glfwMakeContextCurrent(_window);
	glfwGetFramebufferSize(_window, &_dim.width, &_dim.height);
	if (callbacks.keyCallback.has_value()) glfwSetKeyCallback(_window, callbacks.keyCallback.value().ptr);
	if (callbacks.charCallback.has_value()) glfwSetCharCallback(_window, callbacks.charCallback.value().ptr);
	if (callbacks.cursorPosCallback.has_value()) glfwSetCursorPosCallback(_window, callbacks.cursorPosCallback.value().ptr);
	if (callbacks.cursorButtonCallback.has_value()) glfwSetMouseButtonCallback(_window, callbacks.cursorButtonCallback.value().ptr);
	if (callbacks.scrollCallback.has_value()) glfwSetScrollCallback(_window, callbacks.scrollCallback.value().ptr);
}

bool HOEngine::Window::ShouldClose() const {
	return glfwWindowShouldClose(_window);
}
