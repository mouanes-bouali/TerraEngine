#include "api/PhysicsAPI.h"

void PhysicsAPI::setTerrainHeightFn(CollisionSystem::HeightFn fn)
{
    m_collision.setHeightFunction(std::move(fn));
}

void PhysicsAPI::step(EntityManager& em, float dt)
{
    m_physics.update(em, dt);
    m_collision.update(em);
}