#pragma once
#include "ComponentPool.h"
#include <vector>
#include <cstdint>
#include <functional>
#include <memory>

constexpr size_t MAX_COMPONENTS = 64;
using Signature = uint64_t;

// Forward declare the SComponent concept (define it in a separate Concepts file or here)
template<typename T>
concept SComponent = std::is_class_v<T>;
class ComponentRegistry {
public:
    static uint8_t assignNextID();

private:
    inline static uint8_t counter = 0;
};

class EntityManager {
public:
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

    bool matches(EntityID e, const Signature& systemSignature) const;

private:
    EntityID nextID = 0;
    std::vector<Signature> entitySignatures;
    std::vector<std::function<void(EntityID)>> removeFunctions;
    std::vector<std::shared_ptr<void>> pools;

    template<SComponent T>
    struct ComponentTypeID {
        static inline const uint8_t ID = ComponentRegistry::assignNextID();
    };

    template<SComponent T>
    ComponentPool<T>* getPool();
};

// ============================================================
// TEMPLATE IMPLEMENTATIONS (must stay in header)
// ============================================================

template<SComponent T>
void EntityManager::addComponent(EntityID e, const T& component) {
    getPool<T>()->add(e, component);
    entitySignatures[e] |= (1ULL << ComponentTypeID<T>::ID);
}

template<SComponent T>
void EntityManager::removeComponent(EntityID e) {
    getPool<T>()->remove(e);
    entitySignatures[e] &= ~(1ULL << ComponentTypeID<T>::ID);
}

template<SComponent T>
T& EntityManager::getComponent(EntityID e) {
    return getPool<T>()->get(e);
}

template<SComponent T>
bool EntityManager::hasTag(EntityID e) const {
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

        removeFunctions.push_back([this](EntityID e) {
            auto* specificPool = static_cast<ComponentPool<T>*>(this->pools[ComponentTypeID<T>::ID].get());
            if (specificPool && specificPool->has(e)) {
                specificPool->remove(e);
            }
        });
    }
    return static_cast<ComponentPool<T>*>(pools[compID].get());
}