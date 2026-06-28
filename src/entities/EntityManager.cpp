#include "EntityManager.h"

EntityID EntityManager::createEntity(Tag tag) {
    EntityID e = nextID++;
    tags.add(e, tag);
    return e;
}

void EntityManager::destroyEntity(EntityID e) {
    tags.remove(e);
    transforms.remove(e);
    healths.remove(e);
   
    gravities.remove(e);
}

Tag EntityManager::getTag(EntityID e) {
    if (!tags.has(e)) return Tag::None;
    return tags.get(e);
}

void EntityManager::setTag(EntityID e, Tag newTag) {
    if (tags.has(e)) tags.remove(e);
    tags.add(e, newTag);
}

bool EntityManager::hasTag(EntityID e, Tag tag) {
    return tags.has(e) && tags.get(e) == tag;
}

std::vector<EntityID> EntityManager::getByTag(Tag tag) {
    std::vector<EntityID> result;
    auto& t = tags.data();
    auto& o = tags.ownerList();
    for (size_t i = 0; i < tags.count(); ++i) {
        if (t[i] == tag) result.push_back(o[i]);
    }
    return result;
}

EntityID EntityManager::createTile()       { return createEntity(Tag::Tile); }
EntityID EntityManager::createEnemy()      { return createEntity(Tag::Enemy); }
EntityID EntityManager::createProjectile() { return createEntity(Tag::Projectile); }
EntityID EntityManager::createBuilding()   { return createEntity(Tag::Building); }
EntityID EntityManager::createParticle()   { return createEntity(Tag::Particle); }
EntityID EntityManager::createPlayer()     { return createEntity(Tag::Player); }

bool EntityManager::isTile(EntityID e)       { return hasTag(e, Tag::Tile); }
bool EntityManager::isEnemy(EntityID e)      { return hasTag(e, Tag::Enemy); }
bool EntityManager::isProjectile(EntityID e) { return hasTag(e, Tag::Projectile); }
bool EntityManager::isBuilding(EntityID e)   { return hasTag(e, Tag::Building); }
bool EntityManager::isPlayer(EntityID e)     { return hasTag(e, Tag::Player); }