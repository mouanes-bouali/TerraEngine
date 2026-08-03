#pragma once
#include "api/Engine.h"
#include "entities/Components.h"
#include <imgui.h>

// ─────────────────────────────────────────────────────────────────────
// InspectorPanel — edit the selected entity's components.
//
//   InspectorPanel inspector;
//   inspector.setSelected(selectionSystem.getSelected());
//   inspector.render(engine);  // call every frame in Edit mode
// ─────────────────────────────────────────────────────────────────────
class InspectorPanel {
public:
    void setSelected(EntityID id) { m_selected = id; }
    void render(Engine& engine);

private:
    EntityID m_selected = INVALID_ENTITY;
};