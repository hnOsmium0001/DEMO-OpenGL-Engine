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
	virtual ~Component();
	virtual const UUID& GetTypeID() const = 0;
};

class Entity {
private:
	std::unordered_map<uint32_t, std::unique_ptr<Component>> components;

public:
	Component* GetComponent(const UUID& typeID);
	void AddComponent(std::unique_ptr<Component> component);
	void RemoveComponent(const UUID& typeID);
	void RemoveAllComponents();
};

class EntityID {
public:
	const uint32_t idx;
	const uint32_t gen;
};

class EntitiesStorage {
public:
	struct Entry {
		Entity value;
		uint32_t gen;
	};
	static const uint32_t INVALID_GEN = 0;

private:
	std::vector<Entry> entities;
	std::queue<uint32_t> tombstones;
	uint32_t size = 0;
	uint32_t nextGen = 1; // Generation 0 is reserved for static null

public:
	Entity* Get(EntityID id);
	EntityID Add(Entity entity);
	void Remove(EntityID id);

private:
	std::optional<uint32_t> NextAvailableSpot() const;
};

} // namespace HOEngine
