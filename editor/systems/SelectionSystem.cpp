#include "systems/SelectionSystem.h"
#include "api/Engine.h"
#include <glm/glm.hpp>
#include <iostream>
#include <imgui.h>

void SelectionSystem::update(Engine& engine)
{
    auto& input = engine.input();
    auto& scene = engine.scene();

    // ── Skip if mouse is over an ImGui window ──
    // This prevents clicking on UI panels from selecting/deselecting entities
    if (ImGui::GetIO().WantCaptureMouse) {
        // Still allow dragging if we already have a selection
        if (!m_isDragging) {
            return;
        }
    }

    // ── Click to select ──
    if (input.isMouseButtonPressed(0) && !m_isDragging) {
        float mx = input.getMouseX();
        float my = input.getMouseY();

        auto& camera = engine.camera();
        float worldX = camera.target.x + (mx - 640) * 0.05f;
        float worldZ = camera.target.z + (my - 360) * 0.05f;

        EntityID closest = INVALID_ENTITY;
        float closestDist = 5.0f;

        for (EntityID i = 1; i < scene.entityCount(); ++i) {
            if (!scene.exists(i)) continue;
            if (!scene.has<CTransform>(i)) continue;
            if (i == 0) continue;  // skip terrain

            auto& t = scene.get<CTransform>(i);
            float dx = t.x - worldX;
            float dz = t.z - worldZ;
            float dist = glm::sqrt(dx * dx + dz * dz);

            if (dist < closestDist) {
                closestDist = dist;
                closest = i;
            }
        }

        if (closest != INVALID_ENTITY) {
            m_selected = closest;
            m_isDragging = true;
            // SAFE: check has<CTag> before get<CTag>
            if (scene.has<CTag>(m_selected)) {
                auto& tag = scene.get<CTag>(m_selected);
                std::cout << "Selected: " << tag.name << " (ID: " << m_selected << ")\n";
            } else {
                std::cout << "Selected entity (ID: " << m_selected << ")\n";
            }
        } else {
            m_selected = INVALID_ENTITY;
        }
    }

    // ── Drag to move ──
    if (m_isDragging && m_selected != INVALID_ENTITY) {
        if (input.isMouseButtonPressed(0)) {
            auto& camera = engine.camera();
            float mx = input.getMouseX();
            float my = input.getMouseY();
            float worldX = camera.target.x + (mx - 640) * 0.05f;
            float worldZ = camera.target.z + (my - 360) * 0.05f;

            if (scene.exists(m_selected) && scene.has<CTransform>(m_selected)) {
                auto& t = scene.get<CTransform>(m_selected);
                t.x = worldX;
                t.z = worldZ;
                t.y = engine.terrain().getHeight(worldX, worldZ);
            }
        } else {
            m_isDragging = false;
        }
    }

    // ── Delete with Escape key ──
    if (m_selected != INVALID_ENTITY && input.isKeyPressed(KeyCode::Escape)) {
        scene.destroy(m_selected);
        std::cout << "Deleted entity ID: " << m_selected << "\n";
        m_selected = INVALID_ENTITY;
    }
}