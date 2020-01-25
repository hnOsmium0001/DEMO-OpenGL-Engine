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

  static void SetupGL() {
    SetupOne<0>(0);
  }

private:
  template <int32_t n>
  inline static void SetupOne(size_t offset) {
    static_assert(n < elements, "Attribute index out of bounds!");

    using Arr = NthTypeOf<n, As...>;
    static_assert(std::is_array<Arr>::value, "AttributeLayout parameters must be arrays!");

    using Elm = typename std::remove_all_extents<Arr>::type;
    constexpr size_t len = std::extent<Arr>::value;

    glEnableVertexAttribArray(n);
    glVertexAttribPointer(n, len, ToGL<Elm>::value, GL_FALSE, bytes, (void*) offset);

    if constexpr (n < elements - 1) SetupOne<n + 1>(offset + sizeof(Arr));
  }
};

template <typename... As>
class Vertex : public AttributeLayout<As...> {
private:
  std::tuple<As...> data;

public:
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

      std::get<n>(vertex_->data)[currentIndex_] = value;
      ++currentIndex_;
      return *this;
    }
  };

  template <int32_t n>
  Attribute<n> Attr() {
    return Attribute<n>(this);
  }

  template <int32_t n>
  typename Attribute<n>::Arr& Get() {
    return std::get<n>(data);
  }

  template <int32_t n>
  const typename Attribute<n>::Arr& Get() const {
    return std::get<n>(data);
  }
};

} // namespace HOEngine
