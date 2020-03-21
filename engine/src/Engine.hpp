#pragma once

#include <cstdint>
#include <string>
#include <functional>
#include <memory>
#include <optional>
#include <stdexcept>
#include <iostream>
#include <GL/gl3w.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>

namespace HOEngine {

class UUID {
public:
	const uint64_t lSig;
	const uint64_t mSig;

	/// Generate a type 1 UUID
	static UUID Chrono();
	/// Generate a type 4 UUID
	static UUID Random(); 
};

class Dimension {
public:
	int32_t width;
	int32_t height;
};

std::ostream& operator<<(std::ostream& strm, const Dimension& dim);

template <typename F>
class ScopeGuard {
private:
	F function_;

public:
	ScopeGuard() = delete;
	explicit ScopeGuard(F&& function) noexcept
		: function_{ std::move(function) } {
	}
	ScopeGuard(const ScopeGuard&) = delete;
	ScopeGuard& operator=(const ScopeGuard&) = delete;
	ScopeGuard(ScopeGuard&&) = delete;
	ScopeGuard& operator=(ScopeGuard&&) = delete;
	~ScopeGuard() noexcept {
		function_();
	}
};

std::optional<std::string> ReadFileAsStr(const std::string& path);
std::optional<std::vector<std::string>> ReadFileLines(const std::string& path);

template<int32_t n, typename... Ts>
using NthTypeOf = typename std::tuple_element<n, std::tuple<Ts...>>::type;

template <typename T>
struct ToGL {
  static constexpr GLenum value = 0;
  using Type = void;
};

template<> struct ToGL<float> {
  static constexpr GLenum value = GL_FLOAT;
  using Type = GLfloat;
};
template<> struct ToGL<double> {
  static constexpr GLenum value = GL_DOUBLE;
  using Type = GLdouble;
};
template<> struct ToGL<int32_t> {
  static constexpr GLenum value = GL_INT;
  using Type = GLint;
};
template<> struct ToGL<int16_t> {
  static constexpr GLenum value = GL_SHORT;
  using Type = GLshort;
};
template<> struct ToGL<int8_t> {
  static constexpr GLenum value = GL_BYTE;
  using Type = GLbyte;
};
template<> struct ToGL<uint32_t> {
  static constexpr GLenum value = GL_UNSIGNED_INT;
  using Type = GLuint;
};
template<> struct ToGL<uint16_t> {
  static constexpr GLenum value = GL_UNSIGNED_SHORT;
  using Type = GLushort;
};
template<> struct ToGL<uint8_t> {
  static constexpr GLenum value = GL_UNSIGNED_BYTE;
  using Type = GLubyte;
};

struct WindowCallbacks {
	std::optional<void(*)(GLFWwindow*, int32_t, int32_t)> resizeCallback;
	std::optional<void(*)(GLFWwindow*, int32_t, int32_t, int32_t, int32_t)> keyCallback;
	std::optional<void(*)(GLFWwindow*, uint32_t)> charCallback;
	std::optional<void(*)(GLFWwindow*, double, double)> cursorPosCallback;
	std::optional<void(*)(GLFWwindow*, int32_t, int32_t, int32_t)> cursorButtonCallback;
	std::optional<void(*)(GLFWwindow*, double, double)> scrollCallback;
};

class Window {
private:
	Dimension dim_;
	GLFWwindow* handle_;

	Window(Dimension, GLFWwindow*) noexcept;

public:
	static Window* FromGLFW(GLFWwindow* handle);
	static std::unique_ptr<Window> New(const Dimension& dim, const std::string& title, const WindowCallbacks& callbacks);
	Window(const Window&) = delete;
	Window& operator=(const Window&) = delete;
	Window(Window&& source) noexcept;
	Window& operator=(Window&& source) noexcept;
	~Window() noexcept;

	void Resize(int32_t width, int32_t height);

	Dimension& dim() { return dim_; }
	int32_t width() { return dim_.width; }
	int32_t height() { return dim_.height; }
	const Dimension& dim() const { return dim_; }
	operator GLFWwindow*() const { return handle_; }
};

class ApplicationBase {
public:
	ApplicationBase();
	~ApplicationBase() noexcept;

	virtual void Run() = 0;
};
void PrintGLFWError(GLFWWindow* handle, int32_t width, int32_t height();

// TODO
class Camera {
private:
	glm::vec3 look;
	float fov = 90.0f;
	float near = 0.1f;
	float far = 1000.0f;

public:
	void LookAt(glm::vec3 look);
	void SetFOV(float fov);
	void SetPanes(float near, float far);
};

} // namespace HOEngine


constexpr float operator"" _deg(long double degrees) {
  return static_cast<float>(degrees * 3.14159265358979323846264l / 180);
}
