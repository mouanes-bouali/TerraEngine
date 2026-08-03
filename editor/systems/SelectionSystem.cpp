#include "systems/SelectionSystem.h"
#include <glm/glm.hpp>
#include <iostream>

void SelectionSystem::update(Engine& engine)
{
    auto& input = engine.input();
    auto& scene = engine.scene();

    // ── Click to select ──
    // Simple approach: project mouse to terrain plane, find nearest entity
    if (input.isMouseButtonPressed(0) && !m_isDragging) {
        // Get mouse position
        float mx = input.getMouseX();
        float my = input.getMouseY();

        // Simple screen-to-world: use camera position + mouse delta
        // For now, use a simple approach: find entity nearest to camera target
        // offset by mouse delta
        auto& camera = engine.camera();
        float worldX = camera.target.x + (mx - 640) * 0.05f;
        float worldZ = camera.target.z + (my - 360) * 0.05f;

        // Find nearest entity with CTransform
        EntityID closest = INVALID_ENTITY;
        float closestDist = 5.0f;  // max selection radius

        for (EntityID i = 1; i < scene.entityCount(); ++i) {
            if (!scene.exists(i)) continue;
            if (!scene.has<CTransform>(i)) continue;
            // Skip terrain (entity 0)
            if (i == 0) continue;

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
            auto& tag = scene.get<CTag>(m_selected);
            std::cout << "Selected: " << tag.name << " (ID: " << m_selected << ")\n";
        } else {
            m_selected = INVALID_ENTITY;
        }
    }

    // ── Drag to move ──
    if (m_isDragging && m_selected != INVALID_ENTITY) {
        if (input.isMouseButtonPressed(0)) {
            // Update entity position based on mouse movement
            auto& camera = engine.camera();
            float mx = input.getMouseX();
            float my = input.getMouseY();
            float worldX = camera.target.x + (mx - 640) * 0.05f;
            float worldZ = camera.target.z + (my - 360) * 0.05f;

            if (scene.exists(m_selected) && scene.has<CTransform>(m_selected)) {
                auto& t = scene.get<CTransform>(m_selected);
                t.x = worldX;
                t.z = worldZ;
                // Snap Y to terrain
                t.y = engine.terrain().getHeight(worldX, worldZ);
            }
        } else {
            m_isDragging = false;  // released
        }
    }

    // ── Delete with Delete key ──
    if (m_selected != INVALID_ENTITY && input.isKeyPressed(KeyCode::Escape)) {
        scene.destroy(m_selected);
        std::cout << "Deleted entity ID: " << m_selected << "\n";
        m_selected = INVALID_ENTITY;
    }
}