#include <utility>
#include <iostream>
#include <random>
#include <limits>
#include "Engine.hpp"

namespace HOEngine {

UUID UUID::Chrono() {
	// TODO
	return UUID{0, 0};
}

UUID UUID::Random() {
	auto rd = std::random_device{};
	auto seed = std::mt19937_64{rd()};
	auto gen = std::uniform_int_distribution<uint64_t>{};

	auto lstBt = gen(seed);
	auto mstBt = gen(seed);
	// Byte #: 7766554433221100 
	lstBt &= 0xff0fffffffffffff; // Clear version on 6th byte
	lstBt |= 0x0040000000000000; // Set to version 4
	mstBt &= 0xffffffffffffff3f; // Clear variant on (8 on lstBt + 0th) = 8th byte 
	mstBt |= 0x0000000000000080; // Set to IETF variant
	return UUID{lower, higher};
}

std::ostream& operator<<(std::ostream& strm, const Dimension& dim) {
  strm << "(" << dim.width << ", " << dim.height << ")";
  return strm;
}

std::optional<std::string> ReadFileAsStr(const std::string& path) {
  std::ifstream in;
  in.open(path);
  if (!in.is_open()) return {};

  std::stringstream buf;
  buf << in.rdbuf();
  return buf.str();
}

std::optional<std::vector<std::string>> ReadFileLines(const std::string &path) {
	std::ifstream in;
	in.open(path);
  if (!in.is_open()) return {};

	std::vector<std::string> lines;
	while (true) {
		std::string line;
		std::getline(in, line);
		lines.push_back(std::move(line));
	}
	return lines;
}

Window* Window::FromGLFW(GLFWwindow* handle) {
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
	auto window = std::make_unique<Window>(dim, handle);

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

ApplicationBase::ApplicationBase() {
	if (!glfwInit()) {
		throw std::runtime_error("Unable to initialize GLFW");
	}
	glfwSetErrorCallback(ApplicationBase_OnError);
}

ApplicationBase::~ApplicationBase() noexcept {
	glfwTerminate();
}

void PrintGLFWError(int32_t code, const char* msg) {
	std::cerr << "(" << code << ") Error: " << msg << "\n";
}

} // namespace HOEngine
