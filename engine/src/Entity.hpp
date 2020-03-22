#include <memory>
#include <optional>
#include <vector>
#include <queue>
#include <unordered_map>
#include <cstdint>
#include <glm/glm.hpp>
#include <glm/gtc/quaternion.hpp>
#include "Engine.hpp"

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

class Entity {
private:
	std::unordered_map<uint32_t, std::unique_ptr<Component>> components;

public:
	Entity(const Entity&);
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

class TransformComponent : public Component {
private:
	glm::vec3 position_;
	glm::quat rotation_;
	glm::vec3 scale_;

public:
	std::unique_ptr<TransformComponent> Clone() const { return std::make_unique<TransformComponent>(*this); }
	// TODO
	glm::mat4 TranslationMat() const;
	glm::vec3 EulerAngles() const;
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

public:
	static const UUID uuid;
	const UUID& GetTypeID() const override { return uuid; }
};
const UUID TransformComponent::uuid{0xa6b655c3a9c8d0d4, 0xa6b655c3a9c8d0d4};

template <typename Index = GLushort>
class MeshComponent : public Component {
private:
	std::vector<glm::vec3> vertices_;
	std::vector<Index> indices_;
	GLuint mode_ = GL_TRIANGLES;

public:
	std::unique_ptr<MeshComponent<Index>> Clone() const { return std::make_unique<MeshComponent<Index>>(*this); }

	std::vector<glm::vec3>& vertices() { return vertices_; }
	const std::vector<glm::vec3>& vertices() const { return vertices_; }
	std::vector<Index>& indices() { return indices_; }
	const std::vector<Index>& indices() const { return indices_; }
	GLuint mode() { return mode_; }
	void mode(GLuint newMode) { this->mode_ = newMode; }

private:
	virtual MeshComponent<Index>* CloneImpl() const override { return new MeshComponent<Index>(*this); }

public:
	static const UUID uuid;
	const UUID& GetTypeID() const override { return uuid; }
};
template <typename Index>
const UUID MeshComponent<Index>::uuid{0xf4a188a7625c4116, 0xb4d9d872756282c8};

class CameraComponent : public Component {
private:
	float fov_;
	float nearPane_;
	float farPane_;

public:
	std::unique_ptr<CameraComponent> Clone() const { return std::make_unique<CameraComponent>(*this); }

	float fov() { return fov_; }
	void fov(float newFov) { this->fov_ = newFov; }
	float nearPane() { return nearPane_; }
	void nearPane(float newNear) { this->nearPane_ = newNear; }
	float farPane() { return farPane_; }
	void farPane(float newFar) { this->farPane_ = newFar; }

private:
	virtual CameraComponent* CloneImpl() const override { return new CameraComponent(*this); }

public:
	static const UUID uuid;
	const UUID& GetTypeID() const override { return uuid; }
};
const UUID CameraComponent::uuid{0xe1d462fbad2f4a68, 0x872ecda918eac742};

} // namespace HOEngine
