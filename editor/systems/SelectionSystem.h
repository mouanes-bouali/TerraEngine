#pragma once
#include "entities/Components.h"

// Forward declaration — avoids circular include with Engine.h
class Engine;

// ─────────────────────────────────────────────────────────────────────
// SelectionSystem — click-to-select and drag-to-move entities.
//
//   SelectionSystem sel;
//   sel.update(engine);     // call every frame in Edit mode
//   sel.getSelected();     // returns EntityID or INVALID_ENTITY
// ─────────────────────────────────────────────────────────────────────
class SelectionSystem {
public:
    void update(Engine& engine);

    EntityID getSelected() const { return m_selected; }
    void setSelected(EntityID id) { m_selected = id; }
    void clear() { m_selected = INVALID_ENTITY; }
    bool hasSelection() const { return m_selected != INVALID_ENTITY; }

private:
    EntityID m_selected = INVALID_ENTITY;
    bool m_isDragging = false;
};