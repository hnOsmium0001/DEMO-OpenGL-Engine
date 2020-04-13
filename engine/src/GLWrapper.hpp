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
	static constexpr usize elements = sizeof...(As);
	static constexpr usize bytes = (sizeof(As) + ... + 0);

	template <usize n>
	using AttribAt = NthTypeOf<n, As...>;
	template <usize n>
	using ElmAt = typename std::remove_all_extents<NthTypeOf<n, As...>>::type;
	template <usize n>
	static constexpr usize lenAt = std::extent<NthTypeOf<n, As...>>::value;

	template <usize n>
	inline static constexpr usize AttributeOffset() {
		if constexpr (n == 0) {
			return 0;
		} else {
			return AttributeOffset<n - 1>() + sizeof(AttribAt<n>);
		}
	}

	template <usize nthAttribute, usize nthElement>
	inline static constexpr usize ElementOffset() {
		return AttributeOffset<nthAttribute>() + sizeof(ElmAt<nthAttribute>) * nthElement;
	}

	template <usize n>
	inline static usize ElementOffset(usize index) {
		return AttributeOffset<n>() + sizeof(ElmAt<n>) * index;
	}

	inline static void SetupPointers() {
		SetupOne<0>(0);
	}

private:
	template <usize n>
	inline static void SetupOne(usize offset) {
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
		usize currentIndex_ = 0;

	public:
		using Arr = AttribAt<n>;
		using Elm = ElmAt<n>;
		static constexpr usize len = lenAt<n>;

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

	template <usize n>
	Attribute<n> Attr() {
		return Attribute<n>(this);
	}

	template <usize n, usize i>
	ElmAt<n>* Get() {
		auto offset = ElementOffset<n, i>;
		return reinterpret_cast<ElmAt<n>*>(data + offset);
	}

	template <usize n, usize i>
	void Set(ElmAt<n> value) {
		auto offset = ElementOffset<n, i>;
		std::memcpy(data + offset, &value, sizeof(Attribute<n>::Elm));
	}
};

using GLGenBuf = void(*)(GLsizei, GLuint*);
using GLDelBuf = void(*)(GLsizei, GLuint*);

template <usize count, GLGenBuf gen, GLDelBuf del>
class GLObjects {
private:
	static_assert(count > 0, "GLObjects must contain at least one object. 0 is provided.");

	std::array<GLuint, count> handles;

public:
	GLObjects() noexcept { gen(count, handles.data()); }
	~GLObjects() noexcept { del(count, handles.data()); }
	GLObjects(const GLObjects&) = delete;
	GLObjects& operator=(const GLObjects&) = delete;
	GLObjects(GLObjects&& that)
		: handles{ std::move(that.handles) } {
		std::fill(that.handles.begin(), that.handles.end(), 0);
	}
	GLObjects& operator=(GLObjects&& that) {
		glDeleteBuffers(count, this->handles.data());
		this->handles = std::move(that.handles);
		std::fill(that.handles.begin(), that.handles.end(), 0);
		return *this;
	}

	GLuint handle(usize index) const { return handles[index]; }
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
template <usize count>
using StateObjects = GLObjects<count, GenStateObjects_Internal_, DelStateObjects_Internal_>;
/// A `StateObjects` alias with `count` defaulted to 1
using StateObject = StateObjects<1>;

/// Very primitive wrapper around a "vertex buffer object". This does not
/// manage OpenGL buffer usages, nor does it handle binding.
template <usize count>
using BufferObjects = GLObjects<count, GenBufferObjects_Internal_, DelBufferObjects_Internal_>;
/// A `BufferObjects` alias with `count` defaulted to 1
using BufferObject = BufferObjects<1>;

/// Wrapper around an OpenGL shader object handle.
class Shader {
private:
	GLuint handle;

private:
	Shader(GLuint handle) noexcept;

public:
	static std::optional<Shader> New(GLenum type, const std::string& source);
	~Shader() noexcept;
	Shader(const Shader&) = delete;
	Shader& operator=(const Shader&) = delete;
	Shader(Shader&& source) noexcept;
	Shader& operator=(Shader&& source) noexcept;
	
	operator GLuint() const { return handle; }
	GLuint id() const { return handle; }
};

/// Wrapper around an OpenGL shader program handle.
class ShaderProgram {
private:
	GLuint handle;

private:
	ShaderProgram(GLuint handle) noexcept;

public:
	static std::optional<ShaderProgram> FromSource(const std::string& vshSource, const std::string& fshSource);
	static std::optional<ShaderProgram> New(const Shader& vsh, const Shader& fsh);
	~ShaderProgram() noexcept;
	ShaderProgram(const ShaderProgram&) = delete;
	ShaderProgram& operator=(const ShaderProgram&) = delete;
	ShaderProgram(ShaderProgram&& source) noexcept;
	ShaderProgram& operator=(ShaderProgram&& source) noexcept;

	operator GLuint() const { return handle; }
	GLuint id() const { return handle; }
};

} // namespace HOEngine
