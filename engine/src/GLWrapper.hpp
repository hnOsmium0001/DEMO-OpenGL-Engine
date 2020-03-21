#pragma once

#include <string>
#include <optional>
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
