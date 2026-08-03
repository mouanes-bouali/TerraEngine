#pragma once
#include "EntityManager.h"

// Legacy entity builder — superseded by engine/api/EntityBuilder.
// Kept only for TerrainGenerator's cube-tile generation. Renamed to
// LegacyEntityBuilder to avoid ODR collision with the new fluent API.
class LegacyEntityBuilder {
private:
    EntityManager& m_em;
    EntityID       m_id = INVALID_ENTITY;

public:
    LegacyEntityBuilder(EntityManager& em) : m_em(em) {}

    LegacyEntityBuilder& operator()() {
        m_id = m_em.createEntity();
        return *this;
    }

    template<SComponent T>
    LegacyEntityBuilder& with(const T& component = T{}) {
        m_em.addComponent<T>(m_id, component);
        return *this;
    }

    EntityID build() {
        EntityID finishedID = m_id;
        m_id = INVALID_ENTITY;
        return finishedID;
    }
};
