#pragma once

#include <memory>
#include <optional>
#include <vector>
#include <queue>
#include <unordered_map>
#include <cstdint>
#include <glm/glm.hpp>
#include <glm/gtc/quaternion.hpp>
#include "Engine.hpp"
#include "GLWrapper.hpp"

namespace HOEngine {

class Entity; // Fuck C++ again
class Component {
private:
	Entity* attachedEntity;

public:
	Component() noexcept = default;
	virtual ~Component() noexcept {}
	Component(const Component&) = default;
	Component& operator=(const Component&) = default;
	Component(Component&&) = default;
	Component& operator=(Component&&) = default;
	std::unique_ptr<Component> Clone() const { return std::unique_ptr<Component>(this->CloneImpl()); }

	virtual const UUID& GetTypeID() const = 0;

protected:
	virtual Component* CloneImpl() const = 0;
	Entity* Ent() const { return attachedEntity; }
	
	friend Entity;
};

struct EntityID {
	const usize idx;
	const u64 gen;
};

class Entity {
private:
	std::unordered_map<u32, std::unique_ptr<Component>> components;

public:
	/// Create a new Entity that has nothing attached.
	static Entity New();
	/// Create a new Entity with a transform and a mesh component attached
	static Entity NewObject();

	Entity() noexcept;
	Entity(const Entity&);
	Entity& operator=(const Entity&);
	Entity(Entity&&) = default;
	Entity& operator=(Entity&&) = default;

	/// Attempt to get a component with the given UUID. Might return `nullptr`
	/// if this entity does not contain the given UUID.
	Component* GetComponent(const UUID& typeID);
	/// Attempt to get a component with the given UUID. Throw an exception
	/// if this entity does not contain the given UUID.
	Component& GetComponentChecked(const UUID& typeID);
	/// Shorthand for getting the component from the class's UUID.
	template <typename Comp>
	Comp* GetComponent() { return dynamic_cast<Comp*>(GetComponent(Comp::uuid)); }

	/// Move the given component into this entity.
	void AddComponent(std::unique_ptr<Component> component);
	/// Remove a component from this entity that has the given UUID, and return
	/// the ownership to the caller.
	std::unique_ptr<Component> TakeComponent(const UUID& typeID);
	/// Destroying a component from this entity that has the given UUID.
	void RemoveComponent(const UUID& typeID);
	/// Destroy all components from this entity.
	void RemoveAllComponents();
};

class EntitiesStorage {
public:
	struct Entry {
		Entity value;
		u64 gen;
	};
	static const u64 INVALID_GEN = 0;

private:
	static EntitiesStorage New();

	~EntitiesStorage() noexcept = default;
	EntitiesStorage(const EntitiesStorage&) = default;
	EntitiesStorage& operator=(const EntitiesStorage&) = default;
	EntitiesStorage(EntitiesStorage&&) = default;
	EntitiesStorage& operator=(EntitiesStorage&&) = default;

	std::vector<Entry> entities;
	std::queue<usize> tombstones;
	u64 nextGen = 1; // Generation 0 is reserved for static null

public:
	Entity* Get(EntityID id);
	EntityID Add(Entity entity);
	void Remove(EntityID id);

	usize Size() const;

private:
	std::optional<u64> NextAvailableSpot();
};

// =================== //
// Built-in components //
// =================== //

template <u64 msb, u64 lsb>
class ComponentUUIDMixin : virtual public Component {
public:
	static const UUID uuid;
	virtual ~ComponentUUIDMixin() noexcept = default;
	virtual const UUID& GetTypeID() const override { return uuid; }
};
template <u64 msb, u64 lsb>
const UUID ComponentUUIDMixin<msb, lsb>::uuid{msb, lsb};

class TransformComponent : public ComponentUUIDMixin<0xa6b655c3a9c8d0d4, 0xa6b655c3a9c8d0d4> {
public:
	glm::vec3 pos;
	glm::quat rot;
	glm::vec3 scale;

public:
	virtual ~TransformComponent() noexcept = default;
	std::unique_ptr<TransformComponent> Clone() const { return std::make_unique<TransformComponent>(*this); }
	glm::mat4 TranslationMat() const;
	glm::mat4 RotationMat() const;
	glm::mat4 ScaleMat() const;
	glm::mat4 TransformMat() const;

protected:
	virtual TransformComponent* CloneImpl() const override { return new TransformComponent(*this); }
};

/// In-memory representation of an .obj model file.
class MeshComponent : public ComponentUUIDMixin<0xf4a188a7625c4116, 0xb4d9d872756282c8> {
public:
	std::vector<SimpleVertex> vertices;
	std::vector<GLuint> indices;

public:
	virtual ~MeshComponent() noexcept = default;
	std::unique_ptr<MeshComponent> Clone() const { return std::make_unique<MeshComponent>(*this); }

	usize VerticesSize() const;
	usize IndicesSize() const;

protected:
	virtual MeshComponent* CloneImpl() const override { return new MeshComponent(*this); }
};

/// Universal renderer component base class. Each component of this type will issue a
/// separate draw call regardless of the context.
class MeshRendererComponent : virtual public Component {
protected:
	StateObject vao;
	BufferObject vbo;
	BufferObject ibo; 

public:
	void Populate();

protected:
	/// Setup vertex attribute pointers in which ever way preferred. GL_ARRAY_BUFFER will
	/// be bound before and unbound after the function call.
	virtual void SetupAttributes() = 0;
};

class DotLightComponent : public ComponentUUIDMixin<0x0559640e4d14a16, 0xa9222e414f78105d> {
public:
	float strength;

public:
	virtual ~DotLightComponent() noexcept = default;
	std::unique_ptr<DotLightComponent> Clone() const { return std::make_unique<DotLightComponent>(*this); }

protected:
	virtual DotLightComponent* CloneImpl() const override { return new DotLightComponent(*this); }
};

class CameraComponent : public ComponentUUIDMixin<0xe1d462fbad2f4a68, 0x872ecda918eac742> {
public:
	float fov = 90.0_deg;
	float nearPane = 0.1f;
	float farPane = 1000.0f;
	glm::vec3 up{0, 1, 0};
	glm::vec3 viewRay;

public:
	virtual ~CameraComponent() noexcept = default;
	std::unique_ptr<CameraComponent> Clone() const { return std::make_unique<CameraComponent>(*this); }
	glm::mat4 ViewMat() const;
	glm::mat4 PerspectiveMat(const Window* window) const;

protected:
	virtual CameraComponent* CloneImpl() const override { return new CameraComponent(*this); }
};

} // namespace HOEngine
