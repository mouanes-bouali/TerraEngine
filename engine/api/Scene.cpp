#include "api/Scene.h"
#include "api/EntityBuilder.h"

EntityBuilder Scene::create()
{
    return EntityBuilder(*this);
}

void Scene::destroy(EntityID entity)
{
    if (exists(entity))
        m_em.destroyEntity(entity);
}

bool Scene::exists(EntityID entity) const
{
    return entity < m_em.getNextID() && m_em.EntityAlive(entity);
}