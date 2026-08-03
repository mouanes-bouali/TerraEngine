#include "ui/InspectorPanel.h"
#include <cstring>
#include <iostream>

void InspectorPanel::render(Engine& engine)
{
    ImGui::Begin("Inspector");

    if (m_selected == INVALID_ENTITY) {
        ImGui::Text("No entity selected");
        ImGui::Text("Click an entity in the viewport to select it");
        ImGui::End();
        return;
    }

    auto& scene = engine.scene();
    if (!scene.exists(m_selected)) {
        ImGui::Text("Entity no longer exists");
        m_selected = INVALID_ENTITY;
        ImGui::End();
        return;
    }

    // ── Entity Name ──
    if (scene.has<CTag>(m_selected)) {
        auto& tag = scene.get<CTag>(m_selected);
        char buf[64];
        std::strncpy(buf, tag.name.c_str(), sizeof(buf) - 1);
        buf[sizeof(buf) - 1] = '\0';
        if (ImGui::InputText("Name", buf, sizeof(buf))) {
            tag.name = buf;
        }
        ImGui::Text("Type: %s", tag.type.c_str());
    } else {
        ImGui::Text("Entity ID: %u (no tag)", m_selected);
    }

    ImGui::Separator();

    // ── Transform ──
    if (scene.has<CTransform>(m_selected)) {
        auto& t = scene.get<CTransform>(m_selected);
        if (ImGui::CollapsingHeader("Transform", ImGuiTreeNodeFlags_DefaultOpen)) {
            ImGui::DragFloat3("Position", &t.x, 0.1f);
            ImGui::DragFloat("Rotation", &t.rotation, 0.1f);
            ImGui::DragFloat3("Scale", &t.scaleX, 0.1f);
        }
    }

    // ── Renderable ──
    if (scene.has<CRenderable>(m_selected)) {
        auto& r = scene.get<CRenderable>(m_selected);
        if (ImGui::CollapsingHeader("Renderable", ImGuiTreeNodeFlags_DefaultOpen)) {
            ImGui::Text("Mesh Handle: %u", r.meshHandle);
            ImGui::Text("Texture ID: %d", r.textureId);
            float col[4] = {r.color.r, r.color.g, r.color.b, r.color.a};
            if (ImGui::ColorEdit4("Color", col)) {
                r.color = {col[0], col[1], col[2], col[3]};
            }
        }
    }

    // ── Health ──
    if (scene.has<CHealth>(m_selected)) {
        auto& h = scene.get<CHealth>(m_selected);
        if (ImGui::CollapsingHeader("Health")) {
            ImGui::DragInt("Current", &h.current, 1, 0, h.max);
            ImGui::DragInt("Max", &h.max, 1, 1, 9999);
            if (ImGui::Button("Remove Health")) {
                scene.remove<CHealth>(m_selected);
            }
        }
    }

    // ── Player ──
    if (scene.has<CPlayer>(m_selected)) {
        auto& p = scene.get<CPlayer>(m_selected);
        if (ImGui::CollapsingHeader("Player")) {
            ImGui::DragFloat("Jump Speed", &p.jumpSpeed, 0.1f);
            ImGui::DragFloat("Move Speed", &p.moveSpeed, 0.1f);
        }
    }

    // ── Gravity ──
    if (scene.has<CGravity>(m_selected)) {
        auto& g = scene.get<CGravity>(m_selected);
        if (ImGui::CollapsingHeader("Gravity")) {
            ImGui::DragFloat("Strength", &g.strength, 0.1f);
            ImGui::DragFloat("Weight", &g.weight, 0.1f);
            if (ImGui::Button("Remove Gravity")) {
                scene.remove<CGravity>(m_selected);
            }
        }
    }

    // ── Collider ──
    if (scene.has<CCollider>(m_selected)) {
        auto& c = scene.get<CCollider>(m_selected);
        if (ImGui::CollapsingHeader("Collider")) {
            ImGui::DragFloat("Radius", &c.radius, 0.1f);
            ImGui::DragFloat("Offset Y", &c.offsetY, 0.1f);
            if (ImGui::Button("Remove Collider")) {
                scene.remove<CCollider>(m_selected);
            }
        }
    }

    // ── Add Component ──
    ImGui::Separator();
    if (ImGui::BeginCombo("Add Component", "Select...")) {
        if (!scene.has<CHealth>(m_selected) && ImGui::Selectable("Health")) {
            scene.internal().addComponent<CHealth>(m_selected, CHealth{100, 100});
        }
        if (!scene.has<CGravity>(m_selected) && ImGui::Selectable("Gravity")) {
            scene.internal().addComponent<CGravity>(m_selected, CGravity{9.8f, 1.0f});
        }
        if (!scene.has<CCollider>(m_selected) && ImGui::Selectable("Collider")) {
            scene.internal().addComponent<CCollider>(m_selected, CCollider{1.0f, 0.0f});
        }
        if (!scene.has<CPlayer>(m_selected) && ImGui::Selectable("Player")) {
            scene.internal().addComponent<CPlayer>(m_selected, CPlayer{8.0f, 5.0f});
        }
        if (!scene.has<CEnemy>(m_selected) && ImGui::Selectable("Enemy")) {
            scene.internal().addComponent<CEnemy>(m_selected);
        }
        ImGui::EndCombo();
    }

    // ── Delete Entity ──
    ImGui::Separator();
    if (ImGui::Button("Delete Entity", ImVec2(-1, 0))) {
        scene.destroy(m_selected);
        std::cout << "Deleted entity ID: " << m_selected << "\n";
        m_selected = INVALID_ENTITY;
    }

    ImGui::End();
}