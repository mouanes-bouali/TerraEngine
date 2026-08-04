#pragma once
#include "entities/Components.h"
#include <imgui.h>

// Forward declaration
class Engine;
class SelectionSystem;

// ─────────────────────────────────────────────────────────────────────
// EntityListPanel — shows all entities in a scrollable list.
// Click to select, right-click to delete.
// ─────────────────────────────────────────────────────────────────────
class EntityListPanel {
public:
    void render(Engine& engine, SelectionSystem& selection);

private:
    char m_searchBuf[64] = "";
};