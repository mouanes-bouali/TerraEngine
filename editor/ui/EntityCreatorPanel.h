#pragma once
#include <imgui.h>

// Forward declaration — avoids circular include with Engine.h
class Engine;

// ─────────────────────────────────────────────────────────────────────
// EntityCreatorPanel — ImGui panel for creating entities visually.
//
//   EntityCreatorPanel panel;
//   panel.render(engine);  // call every frame in Edit mode
// ─────────────────────────────────────────────────────────────────────
class EntityCreatorPanel {
public:
    void render(Engine& engine);

private:
    char m_name[64] = "New Entity";
    int m_type = 0;  // 0=entity, 1=player, 2=enemy, 3=building
    int m_mesh = 0;
    const char* m_typeNames[4] = {"Entity", "Player", "Enemy", "Building"};
};