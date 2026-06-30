// Include implementation at the bottom (standard practice for templates)
// No #pragma once — this file is included intentionally
#include "ComponentPool.h"
#include <cassert>
template<typename T>
void ComponentPool<T>::add(EntityID entity, const T& value) {
    if (entity >= sparse.size())
        sparse.resize(entity + 1, INVALID_INDEX);
    if (sparse[entity] != INVALID_INDEX) {
        dense[sparse[entity]] = value; // Update existing
    }

    sparse[entity] = static_cast<int32_t>(dense.size());
    dense.push_back(value);
    owners.push_back(entity);
}

template<typename T>
void ComponentPool<T>::remove(EntityID entity) {
    if (!has(entity)) return;

    int32_t idx = sparse[entity];
    int32_t last = static_cast<int32_t>(dense.size()) - 1;

    if (idx != last) {
        dense[idx] = dense[last];
        owners[idx] = owners[last];
        sparse[owners[idx]] = idx;
    }

    sparse[entity] = INVALID_INDEX;
    dense.pop_back();
    owners.pop_back();
}

template<typename T>
bool ComponentPool<T>::has(EntityID entity) const {
    return entity < sparse.size() && sparse[entity] != INVALID_INDEX;
}

template<typename T>
T& ComponentPool<T>::get(EntityID entity) {
    if (!has(entity)) {
     assert(false && "Entity does not have this component");
    }
    return dense[sparse[entity]];
}

template<typename T>
const T& ComponentPool<T>::get(EntityID entity) const {
    return dense[sparse[entity]];
}

template<typename T>
std::vector<T>& ComponentPool<T>::data() { return dense; }

template<typename T>
const std::vector<T>& ComponentPool<T>::data() const { return dense; }

template<typename T>
std::vector<EntityID>& ComponentPool<T>::ownerList() { return owners; }

template<typename T>
const std::vector<EntityID>& ComponentPool<T>::ownerList() const { return owners; }

template<typename T>
size_t ComponentPool<T>::count() const { return dense.size();
}