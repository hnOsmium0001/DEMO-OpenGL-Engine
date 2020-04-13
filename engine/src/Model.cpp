#include <utility>
#include <array>
#include <sstream>
#include <fstream>
#include <glm/gtx/string_cast.hpp>
#include "Model.hpp"

using namespace HOEngine;

void HOEngine::ReadOBJ(MeshComponent& target, std::istream& data) {
	char ctrash;
	std::vector<glm::vec3> posBuf;
	std::vector<glm::vec3> normalBuf;
	std::vector<glm::vec2> uvBuf;
	std::unordered_map<SimpleVertex, u32> knownVerts;
	u32 nextID = 0;
	auto& indices = target.indices;
	auto& vertices = target.vertices;

	std::string line;
	while (std::getline(data, line)) {
		std::istringstream iss{line};

		std::string start;
		iss >> start;
		if (start == "#") {
			continue;
		} else if (start == "v") {
			glm::vec3 pos;
			for (usize i = 0; i < 3; ++i) iss >> pos[i];
			posBuf.push_back(std::move(pos));
		} else if (start == "vt") {
			glm::vec3 normal;
			for (usize i = 0; i < 3; ++i) iss >> normal[i];
			normalBuf.push_back(std::move(normal));
		} else if (start == "vn") {
			glm::vec2 uv;
			for (usize i = 0; i < 2; ++i) iss >> uv[i];
			uvBuf.push_back(std::move(uv));
		} else if (start == "f") {
			std::array<u32, 3> tri;
			// We only support triangular faces (with 3 vertices)
			// Format: f pos/uv/normal pos/uv/normal pos/uv/normal
			for (u32 iv, it, in, i = 0; i < 3; ++i) {
				/* A vertex must have at least the position */ iss >> iv;
				iss >> ctrash;
				if (!uvBuf.empty()) iss >> it;
				iss >> ctrash;
				if (!normalBuf.empty()) iss >> in;

				// .obj file index starts at 1
				--iv;
				--it;
				--in;
				SimpleVertex candidate{
					posBuf[iv],
					normalBuf.empty() ? glm::vec3{} : normalBuf[in],
					uvBuf.empty() ? glm::vec2{} : uvBuf[it]
				};

				auto iter = knownVerts.find(candidate);
				if (iter != knownVerts.end()) {
					// The vertex already exists, reuse the known version
					tri[i] = iter->second;
				} else {
					// The vertex does not exist, create it
					knownVerts.insert({std::move(candidate), nextID});
					tri[i] = nextID;
					++nextID;
				}
			}
			indices.insert(indices.end(), tri.begin(), tri.end());
		} else if (start == "g") {
		}
	}

	vertices.reserve(knownVerts.size());
	for (const auto& [vert, idx]: knownVerts) {
		vertices.push_back(vert);
	}
}
void HOEngine::ReadOBJ(MeshComponent& target, const std::string& data) {
	std::istringstream iss(data);
	ReadOBJ(target, iss);
}
void HOEngine::ReadOBJAt(MeshComponent& target, const std::string& path) {
	std::ifstream ifs;
	ifs.open(path);
	if (!ifs) return;
	ReadOBJ(target, ifs);
}
