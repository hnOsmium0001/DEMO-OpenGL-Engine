#pragma once

#include <string>
#include <optional>
#include <GL/gl3w.h>

namespace HOEngine {

class Shader {
private:
  GLuint handle_;

  Shader(GLuint handle) noexcept;

public:
  static std::optional<Shader> New(const std::string& source);
  Shader(const Shader&) = delete;
  Shader& operator=(const Shader&) = delete;
  Shader(Shader&& source) noexcept;
  Shader& operator=(Shader&& source) noexcept;
  ~Shader() noexcept;
  
  operator GLuint() const { return handle_; }
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
};

} // namespace HOEngine
