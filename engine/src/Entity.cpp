#include <utility>
#include "Entity.hpp"

using namespace HOEngine;

namespace {

	std::unordered_map<UUID, uint32_t> runtimeCompMapping;
	uint32_t nextRID = 0;

	uint32_t FindCompRID(const UUID& typeID) {
		auto it = runtimeCompMapping.find(typeID);
		if (it == runtimeCompMapping.end()) {
			runtimeCompMapping.insert({typeID, nextRID});
			return nextRID++;
		} else {
			return it->second;
		}
	}

}

Entity::Entity(const Entity& that) {
	for (const auto& [rid, compPtr] : that.components) {
		this->components.insert({rid, compPtr->Clone()});
	}
}
Entity& Entity::operator=(const Entity& that) {
	this->components.clear();
	for (const auto& [rid, compPtr] : that.components) {
		this->components.insert({rid, compPtr->Clone()});
	}
	return *this;
}

Component* Entity::GetComponent(const UUID& typeID) {
	auto rid = FindCompRID(typeID);
	auto it = components.find(rid);
	if (it != components.end()) {
		return it->second.get();
	} else {
		return nullptr;
	}
}
void Entity::AddComponent(std::unique_ptr<Component> component) {
	if (!component) return;
	auto rid = FindCompRID(component->GetTypeID());
	components[rid] = std::move(component);
}
void Entity::RemoveComponent(const UUID &typeID) {
	auto rid = FindCompRID(typeID);
	components.erase(rid);
}
void Entity::RemoveAllComponents() {
	components.clear();
}

Entity* EntitiesStorage::Get(EntityID id) {
	if (id.idx >= entities.size()) return nullptr;

	auto& candidate = entities[id.idx];
	if (candidate.gen == id.gen) {
		return &candidate.value;
	} else {
		return nullptr;
	}
}
EntityID EntitiesStorage::Add(Entity entity) {
	auto gen = nextGen++;
	auto entry = Entry{std::move(entity), gen};
	auto next = NextAvailableSpot();
	uint64_t idx;
	if (next.has_value()) {
		idx = *next;
		entities[idx] = std::move(entry);
	} else {
		idx = entities.size();
		entities.push_back(std::move(entry));
	}
	return EntityID{idx, gen};
}
void EntitiesStorage::Remove(EntityID id) {
	entities[id.idx].value.RemoveAllComponents(); // Save space for tombstone 
	entities[id.idx].gen = INVALID_GEN;
	tombstones.push(id.idx);
}

std::optional<uint64_t> EntitiesStorage::NextAvailableSpot() {
	if (tombstones.empty()) return {};
	auto result = tombstones.front();
	tombstones.pop();
	return result;
}

glm::mat4 TransformComponent::TranslationMat() const {
	glm::mat4 result;
	// Position vector -> 4th column
	result[3] = glm::vec4(position_, 1);
	return result;
};
glm::mat4 TransformComponent::RotationMat() const {
	glm::mat4 result;
	// TODO
	return result;
}
glm::mat4 TransformComponent::ScaleMat() const {
	glm::mat4 result;
	result[0][0] = scale_[0];
	result[1][1] = scale_[1];
	result[2][2] = scale_[2];
	result[3][3] = scale_[3];
	return result;
}
glm::mat4 TransformComponent::TransformMat() const {
	return ScaleMat() * RotationMat() * TranslationMat();
}

void MeshRendererComponent::Populate() {
	glBindVertexArray(vao.handle());
	glBindBuffer(GL_ARRAY_BUFFER, vbo.handle());
	this->SetupAttributes();
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo.handle());
	glBindVertexArray(0);
}

glm::mat4 CameraComponent::ViewMat() const  {
	// TODO
	return glm::mat4();
}
glm::mat4 CameraComponent::PerspectiveMat() const {
	return glm::perspective(fov_, 0.0f, nearPane_, farPane_);
}
