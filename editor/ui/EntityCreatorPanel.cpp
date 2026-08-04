#include "ui/EntityCreatorPanel.h"
#include "api/EntityBuilder.h"
#include "api/WorldSerializer.h"
#include <iostream>

void EntityCreatorPanel::render(Engine& engine)
{
    ImGui::Begin("Entity Creator");

    // ── Name ──
    ImGui::InputText("Name", m_name, sizeof(m_name));

    // ── Type ──
    ImGui::Combo("Type", &m_type, m_typeNames, 4);

    // ── Mesh selection ──
    auto& meshLib = engine.meshLibrary();
    if (meshLib.count() > 0) {
        std::vector<const char*> meshNames;
        for (const auto& mesh : meshLib.all()) {
            meshNames.push_back(mesh.name.c_str());
        }
        ImGui::Combo("Mesh", &m_mesh, meshNames.data(), (int)meshNames.size());
    } else {
        ImGui::TextDisabled("No meshes loaded");
        m_mesh = 0;
    }

    // ── Color ──
    static float color[3] = {1, 1, 1};
    ImGui::ColorEdit3("Color", color);

    // ── Position ──
    static float pos[3] = {0, 5, 0};
    ImGui::DragFloat3("Position", pos, 0.1f);

    // ── Create Button ──
    if (ImGui::Button("Create Entity")) {
        uint32_t meshHandle = (meshLib.count() > 0) ? static_cast<uint32_t>(m_mesh + 1) : 0;

        auto builder = engine.scene().create()
            .setPosition(pos[0], pos[1], pos[2])
            .setMesh(meshHandle)
            .setColor(color[0], color[1], color[2], 1.0f)
            .setTag(m_name, m_typeNames[m_type]);

        if (m_type == 1) {  // Player
            builder.makePlayer(8.0f, 5.0f)
                   .addGravity(9.8f, 1.0f)
                   .addCollider(1.0f, 0.0f);
        } else if (m_type == 2) {  // Enemy
            builder.makeEnemy()
                   .tagHealth(100, 100);
        } else if (m_type == 3) {  // Building
            builder.makeBuilding();
        }

        auto id = builder.build();
        std::cout << "Created entity: " << m_name << " (ID: " << id << ")\n";
    }

    ImGui::Separator();

    // ── Save/Load ──
    if (ImGui::Button("Save World")) {
        WorldSerializer::save("assets/world.json", engine);
    }
    ImGui::SameLine();
    if (ImGui::Button("Load World")) {
        WorldSerializer::load("assets/world.json", engine);
    }

    ImGui::End();
}