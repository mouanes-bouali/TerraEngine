#pragma once
#include "ComponentPool.h"
#include "Components.h"
#include <vector>
#include <cstdint>
#include <functional>
#include <memory>

constexpr size_t MAX_COMPONENTS = 64;
using Signature = uint64_t;

class ComponentRegistry {
public:
    static uint8_t assignNextID();
private:
    static uint8_t counter;
};

class EntityManager {
public:
    EntityManager() = default;
    EntityManager(const EntityManager&) = delete;
    EntityManager& operator=(const EntityManager&) = delete;
    EntityManager(EntityManager&&) = default;
    EntityManager& operator=(EntityManager&&) = default;

    EntityID createEntity();
    void destroyEntity(EntityID e);
    
    template<SComponent T>
    void addComponent(EntityID e, const T& component = T{});

    template<SComponent T>
    void removeComponent(EntityID e);

    template<SComponent T>
    T& getComponent(EntityID e);

    template<SComponent T>
    bool hasTag(EntityID e) const;
    bool EntityAlive(EntityID e) const {
        return e < entitySignatures.size() &&( entitySignatures[e]& 1 )== 1;
    }
    EntityID getNextID() const { return nextID; }
    bool matches(EntityID e, const Signature& systemSignature) const;

private:
    EntityID nextID = 0;
    std::vector<EntityID> freeList;
    std::vector<Signature> entitySignatures;
    std::vector<std::function<void(EntityID)>> removeFunctions;
    std::vector<std::shared_ptr<void>> pools;

    void ensureSignatureSize(EntityID e);

    template<SComponent T>
    struct ComponentTypeID {
        static inline const uint8_t ID = ComponentRegistry::assignNextID();
    };

public:
    template<SComponent T>
    static uint8_t getComponentTypeID() {
        return ComponentTypeID<T>::ID;
    }

    // Pool access for DOD systems
    template<SComponent T>
    ComponentPool<T>* getPool();

    template<SComponent T>
    const ComponentPool<T>* getPool() const;
};

// ============================================================
// TEMPLATE IMPLEMENTATIONS
// ============================================================

template<SComponent T>
void EntityManager::addComponent(EntityID e, const T& component) {
    ensureSignatureSize(e);
    getPool<T>()->add(e, component);
    entitySignatures[e] |= (1ULL << ComponentTypeID<T>::ID);
}

template<SComponent T>
void EntityManager::removeComponent(EntityID e) {
    if (e >= entitySignatures.size()) return;
    getPool<T>()->remove(e);
    entitySignatures[e] &= ~(1ULL << ComponentTypeID<T>::ID);
}

template<SComponent T>
T& EntityManager::getComponent(EntityID e) {
    return getPool<T>()->get(e);
}

template<SComponent T>
bool EntityManager::hasTag(EntityID e) const {
    if (e >= entitySignatures.size()) return false;
    return (entitySignatures[e] & (1ULL << ComponentTypeID<T>::ID)) != 0;
}

template<SComponent T>
ComponentPool<T>* EntityManager::getPool() {
    uint8_t compID = ComponentTypeID<T>::ID;

    if (compID >= pools.size()) {
        pools.resize(compID + 1, nullptr);
    }

    if (!pools[compID]) {
        auto newPool = std::make_shared<ComponentPool<T>>();
        pools[compID] = newPool;

        removeFunctions.push_back([this, compID](EntityID e) {
            auto* specificPool = static_cast<ComponentPool<T>*>(
                this->pools[compID].get()
            );
            if (specificPool && specificPool->has(e)) {
                specificPool->remove(e);
            }
        });
    }

    return static_cast<ComponentPool<T>*>(pools[compID].get());
}