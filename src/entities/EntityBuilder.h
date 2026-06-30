#pragma once
#include "EntityManager.h"

class EntityBuilder {
private:
    EntityManager& m_em;  // Kept alive for the whole game
    EntityID       m_id;  // The hidden current ID only the builder knows

public:
    // 1. Constructor: Connects to the manager once at launch
    EntityBuilder(EntityManager& em) : m_em(em), m_id(0) {}

    // 2. 🔥 THE MAGIC SWITCH: Overloading the () operator
    // The moment you type "builder()" in your code, this triggers automatically.
    // It creates the entity internally, sets up the state, and returns itself.
    EntityBuilder& operator()() {
        m_id = m_em.createEntity(); // Grabs the fresh ID internally!
        return *this;
    }

    // 3. The Fluent Chaining Tool: Forwards components to the hidden ID
    template<SComponent T>
    EntityBuilder& with(const T& component = T{}) {
        m_em.addComponent<T>(m_id, component);
        return *this;
    }

    // 4. The Finish Line: Delivers the ID and resets internal tracking
    EntityID build() {
        EntityID finishedID = m_id;
        m_id = 0; 
        return finishedID;
    }
};