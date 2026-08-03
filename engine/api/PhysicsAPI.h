#pragma once
#include "systems/PhysicsSystem.h"
#include "systems/CollisionSystem.h"
#include "entities/EntityManager.h"

// ─────────────────────────────────────────────────────────────────────
// PhysicsAPI — high-level physics API.
//
// Game code should NEVER touch PhysicsSystem or CollisionSystem directly.
// Instead:
//
//   engine.physics().setGravity(9.8f);
//   engine.physics().setTerrainHeightFn([](float x, float z) { return ...; });
//   engine.physics().step(dt);
// ─────────────────────────────────────────────────────────────────────
class PhysicsAPI {
public:
    PhysicsAPI() = default;

    // ── Configuration ──
    void setTerrainHeightFn(CollisionSystem::HeightFn fn);

    // ── Simulation ──
    void step(EntityManager& em, float dt);

    // ── Internal (engine-only) ──
    PhysicsSystem& physics() { return m_physics; }
    CollisionSystem& collision() { return m_collision; }

private:
    PhysicsSystem m_physics;
    CollisionSystem m_collision;
};