#pragma once
#include <vector>
#include <cstdint>
#include <concepts>
#include <cstdint>
#include <bitset>

// 1. The base safety anchor
struct Component {};

// 2. The safety guard (C++20 Concept)
template<typename T>
concept SComponent = std::derived_from<T, Component>;

using EntityID = uint32_t;
struct CTransform: Component {
    float x, y,z;
    float rotation;
    float scaleX, scaleY;
};
struct CHealth: Component {
    int current;
    int max;
};
struct CGravity: Component {
    float strength;
    float weight;
}; 


template<typename T>
class ComponentPool {
public:
    void add(EntityID entity, const T& value);
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

// Include implementation at the bottom (standard practice for templates)
#include "ComponentPool.inl"