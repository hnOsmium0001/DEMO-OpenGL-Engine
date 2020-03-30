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

Entity Entity::New() {
	return Entity{};
}
Entity Entity::NewObject() {
	Entity entity;
	entity.AddComponent(std::make_unique<TransformComponent>());
	entity.AddComponent(std::make_unique<MeshComponent>());
	return entity;
}

// Somehow if the contructor is default the compiler complains
Entity::Entity() noexcept {
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
Component& Entity::GetComponentChecked(const UUID& typeID) {
	auto ptr = GetComponent(typeID);
	if (!ptr) throw std::runtime_error("This entity does not contain a component that has the given UUID");
	return *ptr;
}
void Entity::AddComponent(std::unique_ptr<Component> component) {
	if (!component) return;
	auto rid = FindCompRID(component->GetTypeID());
	component->attachedEntity = this;
	components[rid] = std::move(component);
}
std::unique_ptr<Component> Entity::TakeComponent(const UUID &typeID) {
	auto rid = FindCompRID(typeID);
	auto iter = components.find(rid);
	if (iter != components.end()) {
		auto comp = std::move(iter->second);
		components.erase(rid);
		comp->attachedEntity = nullptr;
		return comp;
	}
	return nullptr;
}
void Entity::RemoveComponent(const UUID &typeID) {
	auto rid = FindCompRID(typeID);
	// No need to detach, because we are destroying the component anyways
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
	result[3] = glm::vec4(pos, 1);
	return result;
};
glm::mat4 TransformComponent::RotationMat() const {
	glm::mat4 result;
	result[0][0] = 1 - 2*rot.y*rot.y - 2*rot.z*rot.z;
	result[0][1] = 2*rot.x*rot.y + 2*rot.w*rot.z;
	result[0][2] = 2*rot.x*rot.z - 2*rot.w*rot.y;
	result[1][0] = 2*rot.x*rot.y - 2*rot.w*rot.z;
	result[1][1] = 1 - 2*rot.x*rot.x - 2*rot.z*rot.z;
	result[1][2] = 2*rot.y*rot.z + 2*rot.w*rot.x;
	result[2][0] = 2*rot.x*rot.z + 2*rot.w*rot.y;
	result[2][1] = 2*rot.w*rot.z - 2*rot.w*rot.x;
	result[2][2] = 1 - 2*rot.x*rot.x - 2*rot.y*rot.y;
	return result;
}
glm::mat4 TransformComponent::ScaleMat() const {
	glm::mat4 result;
	result[0][0] = scale[0];
	result[1][1] = scale[1];
	result[2][2] = scale[2];
	result[3][3] = scale[3];
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
	auto tr = Ent()->GetComponent<TransformComponent>();
	return glm::lookAt(tr->pos, viewRay, up);
}
glm::mat4 CameraComponent::PerspectiveMat(const Window* window) const {
	float aspect = static_cast<float>(window->width() / window->height());
	return glm::perspective(fov, aspect, nearPane, farPane);
}
