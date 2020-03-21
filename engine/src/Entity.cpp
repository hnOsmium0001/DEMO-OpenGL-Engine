#include <utility>
#include "Entity.hpp"

namespace HOEngine {

namespace {

	std::unordered_map<UUID, uint32_t> runtimeCompMapping;
	uint32_t nextRID = 0;

	uint32_t FindCompRID(const UUID& typeID) {
		auto it = runtimeCompMapping.find(typeID);
		if (it == runtimeCompMapping.end()) {
			runtimeCompMapping.insert(typeID, nextRID);
			return nextRID++;
		} else {
			return it->second;
		}
	}

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
	components.insert(rid, std::move(component));
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

	auto candidate = entities[id.idx];
	if (candidate.gen == id.gen) {
		return &candidate.value;
	} else {
		return nullptr;
	}
}
EntityID EntitiesStorage::Add(Entity entity) {
	auto gen = nextGen++;
	auto entry = Entry{std::move(entryity), gen};
	auto next = NextAvailableSpot();
	uint32_t idx;
	if (next.has_value()) {
		idx = *next;
		entities[idx] = std::move(entry);
	} else {
		idx = entities.size();
		entities.push_back(std::move(entry));
	}
	++size;
	return EntityID{idx, gen};
}
void EntitiesStorage::Remove(EntityID id) {
	entities[id.idx].value.RemoveAllComponents(); // Save space for tombstone 
	entities[id.idx].gen = INVALID_GEN;
	tombstones.push(id.idx);
	--size;
}

std::optional<uint32_t> EntitiesStorage::NextAvailableSpot() const {
	if (tombstones.empty()) return {};
	auto result = tombstones.top();
	tombstones.pop();
	return result;
}

} // namespace HOEngine
