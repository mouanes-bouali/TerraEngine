#pragma once
#include <vector>
#include <cstdint>
using EntityID = uint32_t;
struct CTransform {
    float x, y,z;
    float rotation;
    float scaleX, scaleY;
};
struct CHealth {
    int current;
    int max;
};
struct CGravity {
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