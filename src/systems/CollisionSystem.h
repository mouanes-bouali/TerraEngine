#pragma once
#include "entities/EntityManager.h"
#include "entities/Components.h"
#include <functional>

// ─────────────────────────────────────────────────────────────────────
// CollisionSystem — ground collision for entities with CCollider.
//
// Signature: CTransform + CCollider
//
// For each collider entity, it clamps Y to the terrain height at (x,z)
// using the height function provided at construction. The height data is
// owned OUTSIDE this system (the terrain generator / a Map class) — the
// system receives it as a std::function for a clean dependency, keeping
// this a pure ECS system with no direct knowledge of terrain internals.
// ─────────────────────────────────────────────────────────────────────
class CollisionSystem {
public:
    // heightAt(worldX, worldZ) → terrain Y at that point
    using HeightFn = std::function<float(float, float)>;

    CollisionSystem() = default;
    CollisionSystem(HeightFn fn) : m_heightFn(std::move(fn)) {}

    void setHeightFunction(HeightFn fn) { m_heightFn = std::move(fn); }

    // Clamp each collider entity's Y onto the terrain surface.
    void update(EntityManager& em);

private:
    HeightFn m_heightFn;  // null → skip update
};