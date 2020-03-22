#pragma once

#include <cstdint>
#include <string>
#include <functional>
#include <memory>
#include <optional>
#include <compare>
#include <stdexcept>
#include <iostream>
#include <GL/gl3w.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>

namespace HOEngine {

class UUID {
private:
	uint64_t msb_;
	uint64_t lsb_;

public:
	/// Generate a type 4 UUID
	static UUID Random(); 
	UUID(uint64_t msb, uint64_t lsb) : msb_{msb}, lsb_{lsb} {}

	const uint64_t& msb() const { return msb_; }
	const uint64_t& lsb() const { return lsb_; }
	auto operator<=>(const UUID&) const = default;

	friend std::hash<UUID>;
};

} // namespace HOEngine

template<>
struct std::hash<HOEngine::UUID> {
	size_t operator()(const HOEngine::UUID& uuid) const;
};

namespace HOEngine {

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

public:
  /// Get the HOEngine::Window coutnerpart to the given GLFWwindow* if present,
  /// otherwise returns `nullptr`.
	static Window* FromGLFW(GLFWwindow* handle);
  /// Automatically handle fetching HOEngine::Window and internal resizing logic.
  /// Used by default callbacks when user-defined callbacks are not provided,
  /// not recommanded for direct usages.
  static void HandleGLFWResize(GLFWwindow* handle, int32_t width, int32_t height);
	static std::unique_ptr<Window> New(const Dimension& dim, const std::string& title, const WindowCallbacks& callbacks);

  /// Constructor for creating from an already-created GLFWwindow. Main purpose is
  /// for internal usage, not recomandded for direct use.
  Window(Dimension, GLFWwindow*) noexcept;
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
};

void PrintGLFWError(int32_t code, const char* msg);

} // namespace HOEngine


constexpr float operator"" _deg(long double degrees) {
  return static_cast<float>(degrees * 3.14159265358979323846264l / 180);
}
