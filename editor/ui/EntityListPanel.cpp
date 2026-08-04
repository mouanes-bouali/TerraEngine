#include "ui/EntityListPanel.h"
#include "api/Engine.h"
#include "systems/SelectionSystem.h"
#include <iostream>
#include <cstring>

void EntityListPanel::render(Engine& engine, SelectionSystem& selection)
{
    ImGui::Begin("Entity List");

    auto& scene = engine.scene();

    // ── Search bar ──
    ImGui::InputText("Search", m_searchBuf, sizeof(m_searchBuf));

    ImGui::Separator();

    // ── Entity count ──
    ImGui::Text("Entities: %u", scene.entityCount());

    ImGui::Separator();

    // ── Scrollable list ──
    ImGui::BeginChild("EntityScroll", ImVec2(0, 0), true);

    EntityID selected = selection.getSelected();

    for (EntityID i = 0; i < scene.entityCount(); ++i) {
        if (!scene.exists(i)) continue;

        // Build display name
        std::string displayName;
        if (scene.has<CTag>(i)) {
            auto& tag = scene.get<CTag>(i);
            displayName = "[" + std::to_string(i) + "] " + tag.name + " (" + tag.type + ")";
        } else {
            displayName = "[" + std::to_string(i) + "] Entity";
        }

        // Filter by search
        if (m_searchBuf[0] != '\0') {
            if (displayName.find(m_searchBuf) == std::string::npos)
                continue;
        }

        // Highlight if selected
        bool isSelected = (i == selected);
        if (isSelected) {
            ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(1.0f, 0.8f, 0.2f, 1.0f));
        }

        // Click to select
        if (ImGui::Selectable(displayName.c_str(), isSelected)) {
            selection.setSelected(i);
        }

        // Right-click for context menu
        if (ImGui::BeginPopupContextItem(("ctx" + std::to_string(i)).c_str())) {
            if (ImGui::MenuItem("Delete")) {
                scene.destroy(i);
                if (selection.getSelected() == i) {
                    selection.clear();
                }
                std::cout << "Deleted entity " << i << "\n";
            }
            ImGui::EndPopup();
        }

        if (isSelected) {
            ImGui::PopStyleColor();
        }
    }

    ImGui::EndChild();
    ImGui::End();
}