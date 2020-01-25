#pragma once

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
    using A = typename NthTypeOf<n, As...>;
    static_assert(std::is_array<A>::value, "AttributeLayout parameters must be arrays!");

    using Elm = typename std::remove_all_extents<A>::type;
    constexpr size_t len = std::extent<A>::value;

    glEnableVertexAttribArray(n);
    glVertexAttribPointer(n, len, ToGL<Elm>::value, GL_FALSE, bytes, (void*) offset);

    SetupOne<n + 1>(offset + sizeof(A));
  }

  // Base case
  template <>
  inline static void SetupOne<elements>(size_t offset) {
  }
};

} // namespace HOEngine
