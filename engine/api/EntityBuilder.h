#pragma once
#include "entities/EntityManager.h"
#include "entities/Components.h"
#include <glm/glm.hpp>

// Forward declaration to avoid circular deps
class Scene;

// ─────────────────────────────────────────────────────────────────────
// EntityBuilder — fluent API for creating entities.
//
// Game code should NEVER touch EntityManager or Components directly.
// Instead:
//
//   auto player = engine.scene().create()
//       .setPosition(0, 10, 0)
//       .setMesh(0)
//       .setColor(0, 1, 0, 1)
//       .makePlayer(8.0f, 5.0f)
//       .build();
// ─────────────────────────────────────────────────────────────────────
class EntityBuilder {
public:
    explicit EntityBuilder(Scene& scene);

    // ── Transform ──
    EntityBuilder& setPosition(float x, float y, float z);
    EntityBuilder& setRotation(float angle);
    EntityBuilder& setScale(float x, float y, float z);

    // ── Mesh / Renderable ──
    EntityBuilder& setMesh(uint32_t meshHandle);
    EntityBuilder& setTexture(int texId);
    EntityBuilder& setColor(float r, float g, float b, float a = 1.0f);

    // ── Convenience component factories ──
    EntityBuilder& makePlayer(float jumpSpeed = 8.0f, float moveSpeed = 5.0f);
    EntityBuilder& makeEnemy();
    EntityBuilder& makeBuilding();
    EntityBuilder& setTag(const std::string& name, const std::string& type = "entity");
    EntityBuilder& addGravity(float strength = 9.8f, float weight = 1.0f);
    EntityBuilder& addCollider(float radius = 0.5f, float offsetY = 0.0f);
    EntityBuilder& tagHealth(int current = 100, int max = 100);

    // ── Generic component (for custom components) ──
    template<SComponent T, typename... Args>
    EntityBuilder& with(Args&&... args);

    // ── Finalize ──
    EntityID build();

private:
    Scene& m_scene;
    EntityID m_id = INVALID_ENTITY;
    EntityManager& internal();
};

// Template implementation
template<SComponent T, typename... Args>
EntityBuilder& EntityBuilder::with(Args&&... args)
{
    internal().addComponent<T>(m_id, T{std::forward<Args>(args)...});
    return *this;
}
