#pragma once
#include "ComponentPool.h"

#include <vector>
using EntityID = uint32_t;

    enum Tag{
        None,
        Player,
        Enemy,
        Projectile,
        Building,
        Tile,
        Particle
    };


struct Entity {
    EntityID id;
    Tag tag;
};
class EntityManager {
public:
    EntityID createEntity(Tag tag );
    void destroyEntity(EntityID e);

    Tag getTag(EntityID e);
    void setTag(EntityID e, Tag newTag);
    bool hasTag(EntityID e, Tag tag);
    std::vector<EntityID> getByTag(Tag tag);

    EntityID createTile();
    EntityID createEnemy();
    EntityID createProjectile();
    EntityID createBuilding();
    EntityID createParticle();
    EntityID createPlayer();

    bool isTile(EntityID e);
    bool isEnemy(EntityID e);
    bool isProjectile(EntityID e);
    bool isBuilding(EntityID e);
    bool isPlayer(EntityID e);

    ComponentPool<Tag>        tags;
    ComponentPool<CTransform> transforms;
    ComponentPool<CHealth>    healths;
    ComponentPool<CGravity>   gravities;
  

private:
    EntityID nextID = 0;
};