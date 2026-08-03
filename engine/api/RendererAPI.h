#pragma once
#include "renderer/IRenderer.h"
#include "assets/MeshData.h"
#include <glm/glm.hpp>

// ─────────────────────────────────────────────────────────────────────
// RendererAPI — high-level rendering API.
//
// Game code should NEVER touch IRenderer directly. Instead:
//
//   uint32_t mesh = engine.renderer().uploadMesh(data);
//   int tex = engine.renderer().loadTexture("assets/textures/grass.png");
//   engine.renderer().setCamera({0, 10, 0}, {0, 0, 0});
// ─────────────────────────────────────────────────────────────────────
class RendererAPI {
public:
    RendererAPI() = default;

    // ── Mesh Management ──
    uint32_t uploadMesh(const MeshData& data);
    void updateMesh(uint32_t handle, const MeshData& data);
    uint32_t loadMesh(const char* filepath);

    // ── Texture Management ──
    int loadTexture(const char* filepath);

    // ── Camera ──
    void setCamera(const glm::vec3& pos, const glm::vec3& target, const glm::vec3& up = {0, 1, 0});

    // ── Internal (engine-only) ──
    void bind(IRenderer* renderer);
    IRenderer* internal() { return m_renderer; }

private:
    IRenderer* m_renderer = nullptr;
};