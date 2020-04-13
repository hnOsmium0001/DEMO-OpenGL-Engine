#pragma once

#include <cstdint>
#include <cstddef>
#include <string>
#include <functional>
#include <memory>
#include <optional>
#include <stdexcept>
#include <iostream>
#include <GL/gl3w.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>

using u8 = uint8_t;
using u16 = uint16_t;
using u32 = uint32_t;
using u64 = uint64_t;
using i8 = int8_t;
using i16 = int16_t;
using i32 = int32_t;
using i64 = int64_t;
using f32 = float;
using f64 = double;
using usize = size_t;

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
	bool operator==(const UUID&) const;

	friend std::hash<UUID>;
};

} // namespace HOEngine

template<>
struct std::hash<HOEngine::UUID> {
	size_t operator()(const HOEngine::UUID& uuid) const;
};

namespace HOEngine {

struct Dimension {
	i32 width;
	i32 height;
};
std::ostream& operator<<(std::ostream& strm, const Dimension& dim);
std::string operator+(const char* prev, const Dimension& dim);
std::string operator+(const std::string& prev, const Dimension& dim);

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

inline void HashCombine(size_t& seed, size_t hash);

std::optional<std::string> ReadFileAsStr(const std::string& path);
std::optional<std::vector<std::string>> ReadFileLines(const std::string& path);

template<i32 n, typename... Ts>
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
template<> struct ToGL<f64> {
	static constexpr GLenum value = GL_DOUBLE;
	using Type = GLf64;
};
template<> struct ToGL<i32> {
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
template<> struct ToGL<u32> {
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
	std::optional<void(*)(GLFWwindow*, i32, i32)> resizeCallback;
	std::optional<void(*)(GLFWwindow*, i32, i32, i32, i32)> keyCallback;
	std::optional<void(*)(GLFWwindow*, u32)> charCallback;
	std::optional<void(*)(GLFWwindow*, f64, f64)> cursorPosCallback;
	std::optional<void(*)(GLFWwindow*, i32, i32, i32)> cursorButtonCallback;
	std::optional<void(*)(GLFWwindow*, f64, f64)> scrollCallback;
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
	static void HandleGLFWResize(GLFWwindow* handle, i32 width, i32 height);
	static std::unique_ptr<Window> New(const Dimension& dim, const std::string& title, const WindowCallbacks& callbacks);

	/// Constructor for creating from an already-created GLFWwindow. Main purpose is
	/// for internal usage, not recomandded for direct use.
	Window(Dimension, GLFWwindow*) noexcept;
	Window(const Window&) = delete;
	Window& operator=(const Window&) = delete;
	Window(Window&& source) noexcept;
	Window& operator=(Window&& source) noexcept;
	~Window() noexcept;

	void Resize(i32 width, i32 height);

	Dimension& dim() { return dim_; }
	const Dimension& dim() const { return dim_; }
	i32 width() const { return dim_.width; }
	i32 height() const { return dim_.height; }
	operator GLFWwindow*() const { return handle_; }
};

class ApplicationBase {
public:
	ApplicationBase();
	~ApplicationBase() noexcept;

	static void PrintGLFWError(i32 code, const char* msg);
};

class SimpleVertex {
public:
	static void SetupPointers();

	glm::vec3 pos;
	glm::vec3 normal;
	glm::vec2 uv;

	bool operator==(const SimpleVertex& that) const;
};

} // namespace HOEngine

template<>
struct std::hash<HOEngine::SimpleVertex> {
	size_t operator()(const HOEngine::SimpleVertex& vert) const;
};

constexpr f32 operator"" _deg(long double degrees) {
	return static_cast<float>(degrees * 3.14159265358979323846264l / 180);
}
