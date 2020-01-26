#pragma once

#include <tuple>
#include <iterator>
#include <type_traits>
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

    using Arr = NthTypeOf<n, As...>;
    static_assert(std::is_array<Arr>::value, "AttributeLayout parameters must be arrays!");

    glEnableVertexAttribArray(n);
    glVertexAttribPointer(n, lenAt<n>, ToGL<ElmAt<n>>::value, GL_FALSE, bytes, (void*) offset);

    if constexpr (n < elements - 1) SetupOne<n + 1>(offset + sizeof(Arr));
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
    using Arr = NthTypeOf<n, As...>;
    using Elm = typename std::remove_all_extents<Arr>::type;
    static constexpr size_t len = std::extent<Arr>::value;

    Attribute(Vertex<As...>* vertex)
      : vertex_{ vertex }, currentIndex_{ 0 } {
    }

    Attribute& operator<<(Elm value) {
      if (currentIndex_ >= len) {
        throw std::runtime_error("Attribute array storage index out of bounds");
      }

      auto offset = ElementOffset<n>(currentIndex_);
      uint8_t* data = vertex_->data;
      std::memcpy(data + offset, &value, sizeof(Elm));
      ++currentIndex_;
      return *this;
    }
  };

  template <size_t n>
  Attribute<n> Attr() {
    return Attribute<n>(this);
  }

  template <size_t n, size_t i>
  typename Attribute<n>::Elm* Get() {
    auto offset = ElementOffset<n, i>();
    return reinterpret_cast<typename Attribute<n>::Elm*>(data + offset);
  }
};

} // namespace HOEngine
