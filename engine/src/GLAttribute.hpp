#pragma once

#include <tuple>
#include <iterator>
#include <type_traits>
#include <stdexcept>
#include <GL/gl3w.h>
#include "Util.hpp"

namespace HOEngine {

template <typename... As>
class AttributeLayout {
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

  inline static void SetupGL() {
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
};

template <typename... As>
class Vertex : public AttributeLayout<As...> {
public:
  uint8_t data[bytes];

public:
  Vertex() : data{} {
  }

  template <int32_t n>
  class Attribute {
  private:
    Vertex<As...>* vertex_;
    size_t currentIndex_ = 0;

  public:
    using Arr = AttribAt<n>;
    using Elm = ElmAt<n>;
    static constexpr size_t len = lenAt<n>;

    Attribute(Vertex<As...>* vertex)
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
    auto offset = ElementOffset<n, i>();
    return reinterpret_cast<ElmAt<n>*>(data + offset);
  }

  template <size_t n, size_t i>
  void Set(ElmAt<n> value) {
    auto offset = ElementOffset<n, i>();
    std::memcpy(data + offset, &value, sizeof(Attribute<n>::Elm));
  }
};

} // namespace HOEngine
