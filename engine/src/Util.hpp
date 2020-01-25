#pragma once

#include <cstdint>
#include <string>
#include <utility>
#include <tuple>
#include <optional>

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

} // namespace HOEngine
