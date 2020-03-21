#include <memory>
#include <optional>
#include <vector>
#include <queue>
#include <unordered_map>
#include <cstdint>
#include "Engine.hpp"

namespace HOEngine {

class Component {
public:
	Component() noexcept = default;
	virtual ~Component() noexcept = default;
	Component(const Component&) = default;
	Component& operator=(const Component&) = default;
	Component(Component&&) = default;
	Component& operator=(Component&&) = default;
	virtual const UUID& GetTypeID() const = 0;
};

class Entity {
private:
	std::unordered_map<uint32_t, std::unique_ptr<Component>> components;

public:
	Entity(const Entity&) = default;
	Entity& operator=(const Entity&) = default;
	Entity(Entity&&) = default;
	Entity& operator=(Entity&&) = default;

	Component* GetComponent(const UUID& typeID);
	void AddComponent(std::unique_ptr<Component> component);
	void RemoveComponent(const UUID& typeID);
	void RemoveAllComponents();
};

struct EntityID {
	const size_t idx;
	const uint64_t gen;
};

class EntitiesStorage {
public:
	struct Entry {
		Entity value;
		uint64_t gen;
	};
	static const uint64_t INVALID_GEN = 0;

private:
	std::vector<Entry> entities;
	std::queue<size_t> tombstones;
	uint64_t nextGen = 1; // Generation 0 is reserved for static null

public:
	Entity* Get(EntityID id);
	EntityID Add(Entity entity);
	void Remove(EntityID id);

	size_t Size() const;

private:
	std::optional<uint64_t> NextAvailableSpot();
};

} // namespace HOEngine
