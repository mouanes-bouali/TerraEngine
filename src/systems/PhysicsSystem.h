#pragma once
#include "entities/EntityManager.h"
#include "entities/Components.h"

// ─────────────────────────────────────────────────────────────────────
// PhysicsSystem — a true ECS system.
//
// Signature: CTransform + CVelocity            → integrates velocity
// Signature: CTransform + CVelocity + CGravity → applies gravity first
//
// Dense/LODOD pattern: iterates the CVelocity/CTransform dense arrays
// directly for cache-friendly, O(1) per-entity updates.
// ─────────────────────────────────────────────────────────────────────
class PhysicsSystem {
public:
    void update(EntityManager& em, float dt);
};