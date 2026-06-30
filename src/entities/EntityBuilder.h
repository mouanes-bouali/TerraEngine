#pragma once
#include "EntityManager.h"

class EntityBuilder {
private:
    EntityManager& m_em;
    EntityID       m_id = INVALID_ENTITY;

public:
    EntityBuilder(EntityManager& em) : m_em(em) {}

    EntityBuilder& operator()() {
        m_id = m_em.createEntity();
        return *this;
    }

    template<SComponent T>
    EntityBuilder& with(const T& component = T{}) {
        m_em.addComponent<T>(m_id, component);
        return *this;
    }

    EntityID build() {
        EntityID finishedID = m_id;
        m_id = INVALID_ENTITY;
        return finishedID;
    }
};