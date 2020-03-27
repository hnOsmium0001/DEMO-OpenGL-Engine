#include <string>
#include <istream>
#include <glm/glm.hpp>
#include "Entity.hpp"

namespace HOEngine {

void ReadOBJ(MeshComponent& target, std::istream& data);
void ReadOBJ(MeshComponent& target, const std::string& data);
void ReadOBJAt(MeshComponent& target, const std::string& path);

} // namespace HOEngine
