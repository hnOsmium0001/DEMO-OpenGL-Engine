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

class Component {
public:
	Component() noexcept = default;
	virtual ~Component() noexcept {}
	Component(const Component&) = default;
	Component& operator=(const Component&) = default;
	Component(Component&&) = default;
	Component& operator=(Component&&) = default;
	std::unique_ptr<Component> Clone() const { return std::unique_ptr<Component>(this->CloneImpl()); }

	virtual const UUID& GetTypeID() const = 0;

private:
	virtual Component* CloneImpl() const = 0;
};

template <uint64_t msb, uint64_t lsb>
class ComponentUUIDMixin : virtual public Component {
public:
	virtual ~ComponentUUIDMixin() noexcept = default;
	static const UUID uuid;
	virtual const UUID& GetTypeID() const override { return uuid; }
};
template <uint64_t msb, uint64_t lsb>
const UUID ComponentUUIDMixin<msb, lsb>::uuid{msb, lsb};

class Entity {
private:
	std::unordered_map<uint32_t, std::unique_ptr<Component>> components;

public: Entity(const Entity&);
	Entity& operator=(const Entity&);
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


// =================== //
// Built-in components //
// =================== //

class TransformComponent : public ComponentUUIDMixin<0xa6b655c3a9c8d0d4, 0xa6b655c3a9c8d0d4> {
private:
	glm::vec3 position_;
	glm::quat rotation_;
	glm::vec3 scale_;

public:
	virtual ~TransformComponent() noexcept = default;
	std::unique_ptr<TransformComponent> Clone() const { return std::make_unique<TransformComponent>(*this); }
	glm::mat4 TranslationMat() const;
	glm::mat4 RotationMat() const;
	glm::mat4 ScaleMat() const;
	glm::mat4 TransformMat() const;

	glm::vec3& position() { return position_; }
	const glm::vec3& position() const { return position_; }
	glm::quat& rotation() { return rotation_; }
	const glm::quat& rotation() const { return rotation_; }
	glm::vec3& scale() { return scale_; }
	const glm::vec3& scale() const { return scale_; }

private:
	virtual TransformComponent* CloneImpl() const override { return new TransformComponent(*this); }
};

class MeshComponent : public ComponentUUIDMixin<0xf4a188a7625c4116, 0xb4d9d872756282c8> {
private:
	std::vector<SimpleVertex> vertices_;
	std::vector<GLuint> indices_;

public:
	virtual ~MeshComponent() noexcept = default;
	std::unique_ptr<MeshComponent> Clone() const { return std::make_unique<MeshComponent>(*this); }

	std::vector<SimpleVertex>& vertices() { return vertices_; }
	const std::vector<SimpleVertex>& vertices() const { return vertices_; }
	std::vector<GLuint>& indices() { return indices_; }
	const std::vector<GLuint>& indices() const { return indices_; }

private:
	virtual MeshComponent* CloneImpl() const override { return new MeshComponent(*this); }
};

// Pure virtual class, no need for UUID
class MeshRendererComponent : virtual public Component {
private:
	StateObject vao;
	BufferObject vbo;
	BufferObject ibo; 

public:
	void Populate();

private:
	/// Setup vertex attribute pointers in which ever way preferred. GL_ARRAY_BUFFER will
	/// be bound before and unbound after the function call.
	virtual void SetupAttributes() = 0;
};

class LightComponent : public ComponentUUIDMixin<0x0559640e4d14a16, 0xa9222e414f78105d> {
private:
	float strength;

public:
	virtual ~LightComponent() noexcept = default;
	std::unique_ptr<LightComponent> Clone() const { return std::make_unique<LightComponent>(*this); }

private:
	virtual LightComponent* CloneImpl() const override { return new LightComponent(*this); }
};

class CameraComponent : public ComponentUUIDMixin<0xe1d462fbad2f4a68, 0x872ecda918eac742> {
private:
	float fov_ = 90.0_deg;
	float nearPane_ = 0.1f;
	float farPane_ = 1000.0f;

public:
	virtual ~CameraComponent() noexcept = default;
	std::unique_ptr<CameraComponent> Clone() const { return std::make_unique<CameraComponent>(*this); }
	glm::mat4 ViewMat() const;
	glm::mat4 PerspectiveMat() const;

	float fov() { return fov_; }
	void fov(float newFov) { this->fov_ = newFov; }
	float nearPane() { return nearPane_; }
	void nearPane(float newNear) { this->nearPane_ = newNear; }
	float farPane() { return farPane_; }
	void farPane(float newFar) { this->farPane_ = newFar; }

private:
	virtual CameraComponent* CloneImpl() const override { return new CameraComponent(*this); }
};

} // namespace HOEngine
