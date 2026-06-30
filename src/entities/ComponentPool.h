#pragma once
#include <vector>
#include <cstdint>
#include <cassert>

using EntityID = uint32_t;

template<typename T>
class ComponentPool {
public:
    T& add(EntityID entity, const T& value);
    T& add(EntityID entity);
    void remove(EntityID entity);
    bool has(EntityID entity) const;
    T& get(EntityID entity);
    const T& get(EntityID entity) const;

    std::vector<T>& data();
    const std::vector<T>& data() const;
    std::vector<EntityID>& ownerList();
    const std::vector<EntityID>& ownerList() const;
    size_t count() const;

private:
    static constexpr int32_t INVALID_INDEX = -1;
    std::vector<T> dense;
    std::vector<EntityID> owners;
    std::vector<int32_t> sparse;
};

#include "ComponentPool.inl"