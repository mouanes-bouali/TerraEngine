#pragma once
#include "entities/EntityManager.h"
#include "entities/Components.h"
#include <vector>

// Forward declaration for fluent builder
class EntityBuilder;

// ─────────────────────────────────────────────────────────────────────
// Scene — high-level entity management API.
//
// Game code should NEVER touch EntityManager directly. Instead:
//
//   auto player = engine.scene().create()
//       .setPosition(0, 10, 0)
//       .setMesh(0)
//       .build();
//
//   engine.scene().destroy(entity);
//   engine.scene().has(entity);
//   auto& hp = engine.scene().get<CHealth>(entity);
// ─────────────────────────────────────────────────────────────────────
class Scene {
public:
    Scene() = default;

    // ── Entity lifecycle ──
    EntityBuilder create();
    void destroy(EntityID entity);
    bool exists(EntityID entity) const;

    // ── Component access ──
    template<SComponent T>
    T& get(EntityID entity) { return m_em.getComponent<T>(entity); }

    template<SComponent T>
    bool has(EntityID entity) const { return m_em.hasTag<T>(entity); }

    template<SComponent T>
    void remove(EntityID entity) { m_em.removeComponent<T>(entity); }

    template<SComponent T>
    std::vector<EntityID> findEntitiesWith() const;

    // Entity count
    uint32_t entityCount() const { return m_em.getNextID(); }

    // ── Internal (engine-only, hidden from game code) ──
    EntityManager& internal() { return m_em; }
    const EntityManager& internal() const { return m_em; }

private:
    EntityManager m_em;
};

// ── Template implementations ──
template<SComponent T>
std::vector<EntityID> Scene::findEntitiesWith() const
{
    std::vector<EntityID> result;
    const auto* pool = m_em.getPool<T>();
    if (!pool) return result;
    const auto& owners = pool->ownerList();
    result.reserve(owners.size());
    for (EntityID e : owners) {
        if (m_em.hasTag<T>(e)) result.push_back(e);
    }
    return result;
}