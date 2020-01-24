#include <fstream>
#include <sstream>
#include <GL/gl3w.h>
#include "Util.hpp"

namespace HOEngine {

std::ostream& operator<<(std::ostream& strm, const Dimension& dim) {
  strm << "(" << dim.width << ", " << dim.height << ")";
  return strm;
}

std::optional<std::string> ReadFileAsStr(const std::string& path) {
  std::ifstream in;
  in.open(path);
  if (!in.is_open()) return {};

  std::stringstream buf;
  buf << in.rdbuf();
  return buf.str();
}

} // namespace HOEngine
