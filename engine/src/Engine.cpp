#include <optional>
#include <utility>
#include <iostream>
#include <fstream>
#include <sstream>
#include <random>
#include <limits>
#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/hash.hpp>
#include "Engine.hpp"
#include "GLWrapper.hpp"

using namespace HOEngine;

UUID UUID::Random() {
	auto rd = std::random_device{};
	auto seed = std::mt19937_64{rd()};
	auto gen = std::uniform_int_distribution<uint64_t>{};

	auto msbits = gen(seed);
	auto lsbits = gen(seed);
	// Byte #s: 7766554433221100 
	msbits &= 0xff0fffffffffffff; // Clear version on 6th byte
	msbits |= 0x0040000000000000; // Set to version 4
	lsbits &= 0xffffffffffffff3f; // Clear variant on (8 on msbits + 0th) = 8th byte 
	lsbits |= 0x0000000000000080; // Set to IETF variant
	return UUID{msbits, lsbits};
}

bool UUID::operator==(const UUID& that) const {
	return std::tie(msb_, lsb_) == std::tie(that.msb_, that.lsb_);
}

size_t std::hash<UUID>::operator()(const UUID& uuid) const {
	std::hash<uint64_t> hasher;
	size_t hash = 0;
	HashCombine(hash, hasher(uuid.msb_));
	HashCombine(hash, hasher(uuid.lsb_));
	return hash;
}

std::ostream& operator<<(std::ostream& strm, const Dimension& dim) {
	strm << "(" << dim.width << ", " << dim.height << ")";
	return strm;
}
std::string operator+(const char* prev, const Dimension& dim) {
	std::string result(prev);
	result += "(" + std::to_string(dim.width) + ", " + std::to_string(dim.height) + ")";
	return result;
}
std::string operator+(const std::string& prev, const Dimension& dim) {
	return prev + "(" + std::to_string(dim.width) + ", " + std::to_string(dim.height) + ")";
}

void HOEngine::HashCombine(size_t& seed, size_t hash) {
	seed ^= hash + 0x9e3779b9 + (seed << 6) + (seed >> 2);
}

std::optional<std::string> HOEngine::ReadFileAsStr(const std::string& path) {
	std::ifstream in;
	in.open(path);
	if (!in.is_open()) return {};

	std::stringstream buf;
	buf << in.rdbuf();
	return buf.str();
}

std::optional<std::vector<std::string>> HOEngine::ReadFileLines(const std::string &path) {
	std::ifstream in;
	in.open(path);
	if (!in.is_open()) return {};

	std::vector<std::string> lines;
	while (true) {
		std::string line;
		if (!std::getline(in, line)) break;
		lines.push_back(std::move(line));
	}
	return lines;
}

Window* Window::FromGLFW(GLFWwindow* handle) {
	return static_cast<Window*>(glfwGetWindowUserPointer(handle));
}

void Window::HandleGLFWResize(GLFWwindow* handle, int32_t width, int32_t height) {
	auto window = Window::FromGLFW(handle);
	if (window) {
		window->Resize(width, height);
	}
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
		glfwSetFramebufferSizeCallback(handle, Window::HandleGLFWResize);
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
	glfwSetErrorCallback(PrintGLFWError);
}

ApplicationBase::~ApplicationBase() noexcept {
	glfwTerminate();
}

void ApplicationBase::PrintGLFWError(int32_t code, const char* msg) {
	std::cerr << "(" << code << ") Error: " << msg << "\n";
}

void SimpleVertex::SetupPointers() {
	VertexAttributes<float[3], float[3], float[2]>::SetupPointers();
}

bool SimpleVertex::operator==(const SimpleVertex& that) const {
	return std::tie(pos, normal, uv) == std::tie(that.pos, that.normal, that.uv);
}

size_t std::hash<SimpleVertex>::operator()(const SimpleVertex& vert) const {
	size_t hash = 0;
	HashCombine(hash, std::hash<glm::vec3>()(vert.pos));
	HashCombine(hash, std::hash<glm::vec3>()(vert.normal));
	HashCombine(hash, std::hash<glm::vec2>()(vert.uv));
	return hash;
}
