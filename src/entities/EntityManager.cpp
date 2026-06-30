#include "EntityManager.h"

uint8_t ComponentRegistry::assignNextID() {
    return counter++;
}

EntityID EntityManager::createEntity() {
    EntityID e = nextID++;
    if (e >= entitySignatures.size()) {
        entitySignatures.resize(e + 1, 0);
    }
    entitySignatures[e] = 0;
    return e;
}

void EntityManager::destroyEntity(EntityID e) {
    if (e >= entitySignatures.size()) return;

    for (auto& fn : removeFunctions) {
        fn(e);
    }
    entitySignatures[e] = 0;
}

bool EntityManager::matches(EntityID e, const Signature& systemSignature) const {
    if (e >= entitySignatures.size()) return false;
    return (entitySignatures[e] & systemSignature) == systemSignature;
}