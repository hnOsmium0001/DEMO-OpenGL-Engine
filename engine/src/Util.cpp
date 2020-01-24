#include "Util.hpp"

namespace HOEngine {

std::ostream& operator<<(std::ostream& strm, const Dimension& dim) {
  strm << "(" << dim.width << ", " << dim.height << ")";
  return strm;
}

} // namespace HOEngine
