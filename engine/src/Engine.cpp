#include <utility>
#include <stdexcept>
#include <iostream>

#include "Engine.hpp"

namespace HOEngine {

inline Window* Window::FromGLFW(GLFWwindow* handle) {
	return static_cast<Window*>(glfwGetWindowUserPointer(handle));
}

std::unique_ptr<Window> Window::New(const Dimension& dim, const std::string& title, const WindowCallbacks& callbacks) {
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_CLIENT_API, GLFW_OPENGL_API);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	glfwWindowHint(GLFW_RESIZABLE, GLFW_TRUE);
#ifdef __APPLE__
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GLFW_TRUE);
#endif // __APPLE__

	auto handle = glfwCreateWindow(dim.width, dim.height, title.c_str(), nullptr, nullptr);
	if (handle == nullptr) return {};
	auto window = std::unique_ptr<Window>(new Window(dim, handle));

	glfwMakeContextCurrent(handle);

	glfwGetFramebufferSize(handle, &window->dim_.width, &window->dim_.height);
	glfwSetWindowUserPointer(handle, window.get());
	if (callbacks.resizeCallback) {
		glfwSetFramebufferSizeCallback(handle, callbacks.resizeCallback.value());
	} else {
		// Fallback size callback to automatically update the dimension field
		glfwSetFramebufferSizeCallback(handle, Window::HandleResize);
	}
	if (callbacks.keyCallback) glfwSetKeyCallback(handle, callbacks.keyCallback.value());
	if (callbacks.charCallback) glfwSetCharCallback(handle, callbacks.charCallback.value());
	if (callbacks.cursorPosCallback) glfwSetCursorPosCallback(handle, callbacks.cursorPosCallback.value());
	if (callbacks.cursorButtonCallback) glfwSetMouseButtonCallback(handle, callbacks.cursorButtonCallback.value());
	if (callbacks.scrollCallback) glfwSetScrollCallback(handle, callbacks.scrollCallback.value());

	return window;
}

Window::Window(Dimension dim, GLFWwindow* handle) noexcept
	: dim_{ dim }, 
	handle_{ handle } {
}

Window::Window(Window&& source) noexcept
	: dim_{ std::move(source.dim_) },
	handle_{ std::move(source.handle_) } {
	source.handle_ = 0;
}

Window& Window::operator=(Window&& source) noexcept {
	dim_ = std::move(source.dim_);
	handle_ = std::move(source.handle_);
	source.handle_ = 0;
	return *this;
}

Window::~Window() noexcept {
	if(handle_ != 0) {
		glfwDestroyWindow(handle_);
	}
}

void Window::Resize(int32_t width, int32_t height) {
	dim_.width = width;
	dim_.height = height;
}

void Window::HandleResize(GLFWwindow* handle, int32_t width, int32_t height) {
	auto window = FromGLFW(handle);
	if (window == nullptr) return;
	window->Resize(width, height);
}

static void ApplicationBase_OnError(int32_t code, const char* msg) {
	std::cerr << "(" << code << ") Error: " << msg << "\n";
}
ApplicationBase::ApplicationBase() {
	if (!glfwInit()) {
		throw std::runtime_error("Unable to initialize GLFW");
	}
	glfwSetErrorCallback(ApplicationBase_OnError);
}

ApplicationBase::~ApplicationBase() noexcept {
	glfwTerminate();
}

} // namespace HOEngine
