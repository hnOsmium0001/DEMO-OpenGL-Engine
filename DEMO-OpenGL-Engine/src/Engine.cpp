#include <utility>
#include <stdexcept>
#include <iostream>
#include "Engine.hpp"

HOEngine::Window::Window(Window&& source) noexcept
	: dim_{ std::move(source.dim_) },
	window_{ std::move(source.window_) } {
}

HOEngine::Window::~Window() noexcept {
	glfwDestroyWindow(window_);
}

void HOEngine::Window::Init(HOEngine::Dimension dim, const std::string& title, HOEngine::WindowCallbacks callbacks) {
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_CLIENT_API, GLFW_OPENGL_API);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	window_ = glfwCreateWindow(dim.width, dim.height, title.c_str(), nullptr, nullptr);
	if (window_ == nullptr) {
		throw std::runtime_error("Unable to create window");
	}

	glfwMakeContextCurrent(window_);

	if (gl3wInit()) {
		throw std::runtime_error("Unable to initialize OpenGL");
	}

	glfwGetFramebufferSize(window_, &dim_.width, &dim_.height);
	if (callbacks.keyCallback.has_value()) glfwSetKeyCallback(window_, callbacks.keyCallback.value());
	if (callbacks.charCallback.has_value()) glfwSetCharCallback(window_, callbacks.charCallback.value());
	if (callbacks.cursorPosCallback.has_value()) glfwSetCursorPosCallback(window_, callbacks.cursorPosCallback.value());
	if (callbacks.cursorButtonCallback.has_value()) glfwSetMouseButtonCallback(window_, callbacks.cursorButtonCallback.value());
	if (callbacks.scrollCallback.has_value()) glfwSetScrollCallback(window_, callbacks.scrollCallback.value());
}

void ApplicationBase_OnError(i32 code, const char* msg) {
	std::cerr << "(" << code << ") Error: " << msg << "\n";
}
HOEngine::ApplicationBase::ApplicationBase() {
	if (!glfwInit()) {
		throw std::runtime_error("Unable to initialize GLFW");
	}
	glfwSetErrorCallback(ApplicationBase_OnError);
}

HOEngine::ApplicationBase::~ApplicationBase() noexcept {
	glfwTerminate();
}
