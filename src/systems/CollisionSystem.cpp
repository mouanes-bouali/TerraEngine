#include "CollisionSystem.h"

// ─────────────────────────────────────────────────────────────────────
// Clamp every entity with CTransform + CCollider onto the terrain.
//
// player.y = terrainHeight(player.x, player.z) + collider.offsetY
//
// Only runs when a height function is set (non-null). The terrain
// generator calls setHeightFunction() during setup.
// ─────────────────────────────────────────────────────────────────────
void CollisionSystem::update(EntityManager& em)
{
    if (!m_heightFn) return;

    static bool sigInit = false;
    static Signature SIG = 0;
    if (!sigInit) {
        SIG = (1ULL << EntityManager::template getComponentTypeID<CTransform>())
            | (1ULL << EntityManager::template getComponentTypeID<CCollider>());
        sigInit = true;
    }

    auto& colliderPool = *em.getPool<CCollider>();
    auto& colliders    = colliderPool.data();
    auto& owners       = colliderPool.ownerList();

    auto& transformPool = *em.getPool<CTransform>();
    auto& transforms    = transformPool.data();

    for (size_t i = 0; i < owners.size(); ++i) {
        EntityID e = owners[i];
        if (!em.matches(e, SIG)) continue;

        auto& c = colliders[i];

        // One sparse lookup: entity → CTransform dense index
        int32_t ti = transformPool.getSparseIndex(e);
        if (ti < 0) continue;
        auto& t = transforms[ti];

        // Ground snap: place collider center at terrain height + offset + radius.
        // The radius lifts the entity so its BOTTOM touches the ground,
        // not its center (which would make it sink halfway into the terrain).
        float groundY = m_heightFn(t.x, t.z) + c.offsetY + c.radius;
        if (t.y < groundY) {
            t.y = groundY;
            // Also zero out downward velocity so it doesn't accumulate
            auto& velPool = *em.getPool<CVelocity>();
            int32_t vi = velPool.getSparseIndex(e);
            if (vi >= 0) {
                auto& v = velPool.data()[vi];
                if (v.velocity.y < 0.0f)
                    v.velocity.y = 0.0f;
            }
        }
    }
}