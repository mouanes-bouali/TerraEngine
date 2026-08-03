#include "api/EntityBuilder.h"
#include "api/Scene.h"
#include <glm/glm.hpp>

EntityBuilder::EntityBuilder(Scene& scene)
    : m_scene(scene)
{
    m_id = m_scene.internal().createEntity();
}

EntityManager& EntityBuilder::internal()
{
    return m_scene.internal();
}

// ── Transform ──
EntityBuilder& EntityBuilder::setPosition(float x, float y, float z)
{
    internal().addComponent<CTransform>(m_id);
    auto& t = internal().getComponent<CTransform>(m_id);
    t.x = x; t.y = y; t.z = z;
    return *this;
}

EntityBuilder& EntityBuilder::setRotation(float angle)
{
    auto& t = internal().getComponent<CTransform>(m_id);
    t.rotation = angle;
    return *this;
}

EntityBuilder& EntityBuilder::setScale(float x, float y, float z)
{
    auto& t = internal().getComponent<CTransform>(m_id);
    t.scaleX = x; t.scaleY = y; t.scaleZ = z;
    return *this;
}

// ── Mesh / Renderable ──
EntityBuilder& EntityBuilder::setMesh(uint32_t meshHandle)
{
    internal().addComponent<CRenderable>(m_id);
    auto& r = internal().getComponent<CRenderable>(m_id);
    r.meshHandle = meshHandle;
    return *this;
}

EntityBuilder& EntityBuilder::setTexture(int texId)
{
    auto& r = internal().getComponent<CRenderable>(m_id);
    r.textureId = texId;
    return *this;
}

EntityBuilder& EntityBuilder::setColor(float r, float g, float b, float a)
{
    internal().addComponent<CRenderable>(m_id);
    auto& cr = internal().getComponent<CRenderable>(m_id);
    cr.color = {r, g, b, a};
    return *this;
}

// ── Convenience factories ──
EntityBuilder& EntityBuilder::makePlayer(float jumpSpeed, float moveSpeed)
{
    internal().addComponent<CPlayer>(m_id, CPlayer{jumpSpeed, moveSpeed});
    return *this;
}

EntityBuilder& EntityBuilder::makeEnemy()
{
    internal().addComponent<CEnemy>(m_id);
    return *this;
}

EntityBuilder& EntityBuilder::makeBuilding()
{
    internal().addComponent<CBuilding>(m_id);
    return *this;
}

EntityBuilder& EntityBuilder::addGravity(float strength, float weight)
{
    internal().addComponent<CGravity>(m_id, CGravity{strength, weight});
    return *this;
}

EntityBuilder& EntityBuilder::addCollider(float radius, float offsetY)
{
    internal().addComponent<CCollider>(m_id, CCollider{radius, offsetY});
    return *this;
}

EntityBuilder& EntityBuilder::tagHealth(int current, int max)
{
    internal().addComponent<CHealth>(m_id, CHealth{current, max});
    return *this;
}

// ── Finalize ──
EntityID EntityBuilder::build()
{
    EntityID finished = m_id;
    m_id = INVALID_ENTITY;
    return finished;
}