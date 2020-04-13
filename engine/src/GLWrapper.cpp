#include <utility>
#include <iostream>
#include "GLWrapper.hpp"
#include "MonadicUtil.hpp"

using namespace HOEngine;

Shader::Shader(GLuint handle) noexcept
	: handle{ handle } {
}
Shader::~Shader() noexcept {
	// Even though we only create a shader when if glShaderSource and glCompileShader are successful,
	// the shader (GL object) might be moved to another shader (C++ object) already
	// in this case this object don't own the shader (GL object) anymore
	if (handle != 0) {
		glDeleteShader(handle);
	}
}
std::optional<Shader> Shader::New(GLenum type, const std::string& source) {
	GLuint handle = glCreateShader(type);
	if (handle == 0) return {};
	// Create a shader (C++ object) first so that on errors the allocated GL shader object can be deleted automatically
	Shader shader(handle);

	// Give shader source code to OpenGL
	const GLchar* sourceCodes[] = { source.c_str() };
	GLint sourceLengths[] = { static_cast<GLint>(source.size()) };
	glShaderSource(handle, 1, sourceCodes, sourceLengths);

	// Try to compile the shader source code
	glCompileShader(handle);
	// Output error message when shader compilation fails
	GLint compileStatus;
	glGetShaderiv(handle, GL_COMPILE_STATUS, &compileStatus);
	if (compileStatus == GL_FALSE) {
		GLint logLen;
		glGetShaderiv(handle, GL_INFO_LOG_LENGTH, &logLen);
		std::string log;
		// Shader info log length is null-terminated, and std::string all automatically add 1 char for null termination
		// If we don't remove one char there will be two null bytes at the end
		log.resize(static_cast<std::string::usizeype>(logLen - 1));
		glGetShaderInfoLog(handle, logLen, nullptr, log.data());
		std::cerr << log << "\n";
		return {};
	}

	// Successfully constructed the shader
	return shader;
}
Shader::Shader(Shader&& source) noexcept
	: handle{ std::move(source.handle) } {
	source.handle = 0;
}
Shader& Shader::operator=(Shader&& source) noexcept {
	if (this->handle != 0) {
		glDeleteShader(this->handle);
	}
	this->handle = std::move(source.handle);
	source.handle = 0;
	return *this;
}

ShaderProgram::ShaderProgram(GLuint handle) noexcept
	: handle{ handle } {
}
ShaderProgram::~ShaderProgram() noexcept {
	// See shader objects for the reason behind this
	if (handle != 0) {
		glDeleteProgram(handle);
	}
}
std::optional<ShaderProgram> ShaderProgram::FromSource(const std::string& vshSource, const std::string& fshSource) {
	return Bind(
			ShaderProgram::New,
			Shader::New(GL_VERTEX_SHADER, vshSource),
			Shader::New(GL_FRAGMENT_SHADER, fshSource));
}
std::optional<ShaderProgram> ShaderProgram::New(const Shader& vsh, const Shader& fsh) {
	auto handle = glCreateProgram();
	if (handle == 0) return {};
	// In case something fails destructor will auto-free the program object for us
	ShaderProgram program(handle);

	glAttachShader(handle, vsh);
	glAttachShader(handle, fsh);

	glLinkProgram(handle);
	GLint linkStatus;
	glGetProgramiv(handle, GL_LINK_STATUS, &linkStatus);
	if (linkStatus == GL_FALSE) {
		GLint logLen;
		glGetProgramiv(handle, GL_INFO_LOG_LENGTH, &logLen);
		std::string log;
		log.resize(static_cast<std::string::usizeype>(logLen - 1));
		glGetProgramInfoLog(handle, logLen, nullptr, log.data());
		std::cerr << log << "\n";
		return {};
	}

	return program;
}
ShaderProgram::ShaderProgram(ShaderProgram&& source) noexcept
	: handle{ std::move(source.handle) } {
	source.handle = 0;
}
ShaderProgram& ShaderProgram::operator=(ShaderProgram&& source) noexcept {
	if (this->handle != 0) {
		glDeleteProgram(handle);
	}
	this->handle = std::move(source.handle);
	source.handle = 0;
	return *this;
}
