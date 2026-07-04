#include "EntityManager.h"

uint8_t ComponentRegistry::counter = 1;

uint8_t ComponentRegistry::assignNextID() {
    return counter++;
}

void EntityManager::ensureSignatureSize(EntityID e) {
    if (e >= entitySignatures.size()) {
        entitySignatures.resize(e + 1, 0);
    }
}

EntityID EntityManager::createEntity() {
    EntityID e;
    if (!freeList.empty()) {
        e = freeList.back();
        freeList.pop_back();
    } else {
        e = nextID++;
        ensureSignatureSize(e);
    }
    entitySignatures[e] = 1;
    return e;
}

void EntityManager::destroyEntity(EntityID e) {
    if (e >= entitySignatures.size()) return;
    for (auto& fn : removeFunctions) fn(e);
    entitySignatures[e] = 0;
    freeList.push_back(e);
}

bool EntityManager::matches(EntityID e, const Signature& systemSignature) const {
    if (e >= entitySignatures.size()) return false;
    return (entitySignatures[e] & systemSignature) == systemSignature;
}