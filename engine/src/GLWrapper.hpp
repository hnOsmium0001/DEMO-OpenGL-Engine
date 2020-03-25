#pragma once

#include <cstring>
#include <string>
#include <optional>
#include <array>
#include <type_traits>
#include <stdexcept>
#include <GL/gl3w.h>
#include "Engine.hpp"

namespace HOEngine {

template <typename... As>
class VertexAttributes {
public:
	static constexpr size_t elements = sizeof...(As);
	static constexpr size_t bytes = (sizeof(As) + ... + 0);

	template <size_t n>
	using AttribAt = NthTypeOf<n, As...>;
	template <size_t n>
	using ElmAt = typename std::remove_all_extents<NthTypeOf<n, As...>>::type;
	template <size_t n>
	static constexpr size_t lenAt = std::extent<NthTypeOf<n, As...>>::value;

	template <size_t n>
	inline static constexpr size_t AttributeOffset() {
		if constexpr (n == 0) {
			return 0;
		} else {
			return AttributeOffset<n - 1>() + sizeof(AttribAt<n>);
		}
	}

	template <size_t nthAttribute, size_t nthElement>
	inline static constexpr size_t ElementOffset() {
		return AttributeOffset<nthAttribute>() + sizeof(ElmAt<nthAttribute>) * nthElement;
	}

	template <size_t n>
	inline static size_t ElementOffset(size_t index) {
		return AttributeOffset<n>() + sizeof(ElmAt<n>) * index;
	}

	inline static void SetupPointers() {
		SetupOne<0>(0);
	}

private:
	template <size_t n>
	inline static void SetupOne(size_t offset) {
		static_assert(n < elements, "Attribute index out of bounds!");
		static_assert(std::is_array<AttribAt<n>>::value, "AttributeLayout parameters must be arrays!");

		glEnableVertexAttribArray(n);
		glVertexAttribPointer(n, lenAt<n>, ToGL<ElmAt<n>>::value, GL_FALSE, bytes, (void*) offset);

		if constexpr (n < elements - 1) SetupOne<n + 1>(offset + sizeof(AttribAt<n>));
	}

private:
	uint8_t data[bytes];

public:
	VertexAttributes() : data{} {
	}

	template <int32_t n>
	class Attribute {
	private:
		VertexAttributes<As...>* vertex_;
		size_t currentIndex_ = 0;

	public:
		using Arr = AttribAt<n>;
		using Elm = ElmAt<n>;
		static constexpr size_t len = lenAt<n>;

		Attribute(VertexAttributes<As...>* vertex)
			: vertex_{ vertex }, currentIndex_{ 0 } {
		}

		Attribute& operator<<(Elm value) {
			if (currentIndex_ >= len) {
				throw std::runtime_error("Attribute array storage index out of bounds");
			}

			auto offset = ElementOffset<n>(currentIndex_);
			auto data = vertex_->data;
			std::memcpy(data + offset, &value, sizeof(Elm));
			++currentIndex_;
			return *this;
		}

		// Syntax sugar for using `v << 0.0f, 1.0f, 2.0f` instead of the weird-ish `v << 0.0f << 1.0f << 2.0f`
		// Note that `v << 0.0f, 1.0f, 2.0f` will be evaluated as `((v << 0.0f), 1.0f), 2.0f`
		Attribute& operator,(Elm value) {
			return operator<<(value);
		}
	};

	template <size_t n>
	Attribute<n> Attr() {
		return Attribute<n>(this);
	}

	template <size_t n, size_t i>
	ElmAt<n>* Get() {
		auto offset = ElementOffset<n, i>;
		return reinterpret_cast<ElmAt<n>*>(data + offset);
	}

	template <size_t n, size_t i>
	void Set(ElmAt<n> value) {
		auto offset = ElementOffset<n, i>;
		std::memcpy(data + offset, &value, sizeof(Attribute<n>::Elm));
	}
};

using GLGenBuf = void(*)(GLsizei, GLuint*);
using GLDelBuf = void(*)(GLsizei, GLuint*);

// template <GLuint count, GLGenBuf gen, GLDelBuf del>
template <size_t count, GLGenBuf gen, GLDelBuf del>
class GLObjects {
private:
	static_assert(count > 0, "GLObjects must contain at least one object. 0 is provided.");

	std::array<GLuint, count> handles;

public:
	GLObjects() noexcept { gen(count, handles.data()); }
	~GLObjects() noexcept { del(count, handles.data()); }
	GLuint handle(size_t index) const { return handles[index]; }
	GLuint handle() const { return handles[0]; }
	operator GLuint() const { return handles[0]; }
};

template <GLGenBuf gen, GLDelBuf del>
using GLObject = GLObjects<1, gen, del>;

// OpenGL objects are linked at runtime (thus their funcion pointers are not constexpr)
// we must wrap them in a determined compile time function for templates to work
inline void GenStateObjects_Internal_(GLsizei size, GLuint* ptr) { glGenVertexArrays(size, ptr); }
inline void DelStateObjects_Internal_(GLsizei size, GLuint* ptr) { glDeleteVertexArrays(size, ptr); }
inline void GenBufferObjects_Internal_(GLsizei size, GLuint* ptr) { glGenBuffers(size, ptr); }
inline void DelBufferObjects_Internal_(GLsizei size, GLuint* ptr) { glDeleteBuffers(size, ptr); }

/// Aka "vertex array object" which stores buffer binding and attribute
/// pointer states.
template <size_t count>
using StateObjects = GLObjects<count, GenStateObjects_Internal_, DelStateObjects_Internal_>;
/// A `StateObjects` alias with `count` defaulted to 1
using StateObject = StateObjects<1>;

/// Very primitive wrapper around a "vertex buffer object". This does not
/// manage OpenGL buffer usages, nor does it handle binding.
template <size_t count>
using BufferObjects = GLObjects<count, GenBufferObjects_Internal_, DelBufferObjects_Internal_>;
/// A `BufferObjects` alias with `count` defaulted to 1
using BufferObject = BufferObjects<1>;

class Shader {
private:
	GLuint handle_;

	Shader(GLuint handle) noexcept;

public:
	static std::optional<Shader> New(GLenum type, const std::string& source);
	Shader(const Shader&) = delete;
	Shader& operator=(const Shader&) = delete;
	Shader(Shader&& source) noexcept;
	Shader& operator=(Shader&& source) noexcept;
	~Shader() noexcept;
	
	operator GLuint() const { return handle_; }
	GLuint handle() const { return handle_; }
};

class ShaderProgram {
private:
	GLuint handle_;

	ShaderProgram(GLuint handle) noexcept;

public:
	static std::optional<ShaderProgram> New(const std::string& vshSource, const std::string& fshSource);
	static std::optional<ShaderProgram> New(const Shader& vsh, const Shader& fsh);
	ShaderProgram(const ShaderProgram&) = delete;
	ShaderProgram& operator=(const ShaderProgram&) = delete;
	ShaderProgram(ShaderProgram&& source) noexcept;
	ShaderProgram& operator=(ShaderProgram&& source) noexcept;
	~ShaderProgram() noexcept;

	operator GLuint() const { return handle_; }
	GLuint handle() const { return handle_; }
};

} // namespace HOEngine
