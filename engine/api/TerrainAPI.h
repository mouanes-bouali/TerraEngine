#pragma once
#include "assets/MeshData.h"
#include "systems/TerrainGenerator.h"
#include "renderer/IRenderer.h"
#include <memory>

// ─────────────────────────────────────────────────────────────────────
// TerrainAPI — high-level terrain generation and sculpting API.
//
// Game code should NEVER touch TerrainGenerator or IRenderer directly.
// Instead:
//
//   engine.terrain()
//       .setHeightScale(10.0f)
//       .generate(256, 0.5f, 10.0f)
//       .upload();
//
//   float h = engine.terrain().getHeight(x, z);
// ─────────────────────────────────────────────────────────────────────
class TerrainAPI {
public:
    TerrainAPI() = default;

    // ── Configuration (fluent) ──
    TerrainAPI& setHeightScale(float scale);
    TerrainAPI& setMapScale(float scale);

    // ── Generation ──
    TerrainAPI& generate(int resolution, float roughness, float amplitude);
    TerrainAPI& upload();  // uploads to GPU, stores handle

    // ── Sculpting (for the editor) ──
    void raise(float worldX, float worldZ, float radius, float strength);
    void lower(float worldX, float worldZ, float radius, float strength);
    void flatten(float worldX, float worldZ, float radius, float targetHeight);

    // ── Query ──
    float getHeight(float worldX, float worldZ) const;

    // ── Internal (engine-only) ──
    void bind(IRenderer* renderer);
    TerrainGenerator& internal() { return m_terrain; }
    MeshHandle handle() const { return m_handle; }

private:
    TerrainGenerator m_terrain;
    IRenderer* m_renderer = nullptr;
    MeshHandle m_handle = 0;
    float m_mapScale = 1.0f;
    float m_heightScale = 8.0f;
};