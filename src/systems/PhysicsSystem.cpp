#include "PhysicsSystem.h"

// ─────────────────────────────────────────────────────────────────────
// PhysicsSystem::update
//
// Two-pass, cache-friendly:
//   Pass 1 (gravity): entities with CTransform + CVelocity + CGravity
//                     get velocity.y -= gravity.strength * weight * dt
//   Pass 2 (integrate): all entities with CTransform + CVelocity
//                     get position += velocity * dt
//
// Uses dense-array iteration (DOD) instead of per-entity component lookup.
// ─────────────────────────────────────────────────────────────────────
void PhysicsSystem::update(EntityManager& em, float dt)
{
    static bool sigInit = false;
    static Signature GRAVITY_SIG = 0;
    static Signature INTEGRATE_SIG = 0;
    if (!sigInit) {
        auto tid = EntityManager::template getComponentTypeID<CTransform>;
        GRAVITY_SIG = (1ULL << tid())
                    | (1ULL << EntityManager::template getComponentTypeID<CVelocity>())
                    | (1ULL << EntityManager::template getComponentTypeID<CGravity>());
        INTEGRATE_SIG = (1ULL << tid())
                      | (1ULL << EntityManager::template getComponentTypeID<CVelocity>());
        sigInit = true;
    }

    // ── Pass 1: Apply gravity ──
    auto& gravityPool = *em.getPool<CGravity>();
    auto& gravities   = gravityPool.data();
    auto& gravOwners  = gravityPool.ownerList();
    for (size_t i = 0; i < gravOwners.size(); ++i) {
        EntityID e = gravOwners[i];
        if (!em.matches(e, GRAVITY_SIG)) continue;

        auto& g = gravities[i];
        auto& v = em.getComponent<CVelocity>(e);
        // v.y -= g.strength * g.weight * dt
        v.velocity.y -= g.strength * g.weight * dt;
    }

    // ── Pass 2: Integrate position from velocity ──
    auto& velocityPool = *em.getPool<CVelocity>();
    auto& velocities   = velocityPool.data();
    auto& velOwners    = velocityPool.ownerList();

    auto& transformPool  = *em.getPool<CTransform>();
    auto& transforms     = transformPool.data();

    for (size_t i = 0; i < velOwners.size(); ++i) {
        EntityID e = velOwners[i];
        if (!em.matches(e, INTEGRATE_SIG)) continue;

        auto& v = velocities[i];

        // One sparse lookup: entity → index in CTransform dense array.
        int32_t ti = transformPool.getSparseIndex(e);
        if (ti < 0) continue;
        auto& t = transforms[ti];

        t.x += v.velocity.x * dt;
        t.y += v.velocity.y * dt;
        t.z += v.velocity.z * dt;
    }
}