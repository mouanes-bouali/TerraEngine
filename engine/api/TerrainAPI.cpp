#include "api/TerrainAPI.h"
#include <cmath>
#include <iostream>

TerrainAPI& TerrainAPI::setHeightScale(float scale)
{
    m_heightScale = scale;
    m_terrain.terrainConfig.heightScale = scale;
    return *this;
}

TerrainAPI& TerrainAPI::setMapScale(float scale)
{
    m_mapScale = scale;
    return *this;
}

TerrainAPI& TerrainAPI::generate(int resolution, float roughness, float amplitude)
{
    m_terrain.terrainConfig.heightScale = m_heightScale;
    m_terrain.generateSmoothTerrain(resolution, roughness, amplitude);
    return *this;
}

TerrainAPI& TerrainAPI::upload()
{
    if (!m_renderer) {
        std::cerr << "TerrainAPI::upload: no renderer bound\n";
        return *this;
    }

    // Generate fresh mesh data from the current heightmap
    MeshData mesh = m_terrain.generateSmoothTerrain(
        m_terrain.terrainConfig.gridSize, 0.5f, 10.0f);

    // Scale vertices by mapScale
    for (size_t i = 0; i < mesh.vertices.size(); i += 8) {
        mesh.vertices[i]     *= m_mapScale;
        mesh.vertices[i + 2] *= m_mapScale;
    }

    // If we already have a handle, update in-place; otherwise upload new
    if (m_handle != 0) {
        m_renderer->updateMesh(m_handle, mesh);
    } else {
        m_handle = m_renderer->uploadMesh(mesh);
    }
    return *this;
}

void TerrainAPI::bind(IRenderer* renderer)
{
    m_renderer = renderer;
}

float TerrainAPI::getHeight(float worldX, float worldZ) const
{
    return m_terrain.getWorldHeight(worldX / m_mapScale, worldZ / m_mapScale);
}

// ── Sculpting ──
// These modify the heightmap and re-upload the mesh in-place.
// The editor calls these to sculpt terrain without recreating handles.

void TerrainAPI::raise(float worldX, float worldZ, float radius, float strength)
{
    auto& hm = m_terrain.internalHeightmap();
    int res = hm.resolution;
    float halfSize = res / 2.0f;

    // Convert world → grid coords
    int cx = static_cast<int>(std::floor(worldX / m_mapScale + halfSize));
    int cz = static_cast<int>(std::floor(worldZ / m_mapScale + halfSize));
    int r = static_cast<int>(radius / m_mapScale);

    for (int z = cz - r; z <= cz + r; ++z) {
        for (int x = cx - r; x <= cx + r; ++x) {
            if (x < 0 || x >= res || z < 0 || z >= res) continue;
            float dist = std::sqrt(float((x - cx) * (x - cx) + (z - cz) * (z - cz)));
            if (dist > r) continue;
            float falloff = 1.0f - (dist / r);  // 1 at center, 0 at edge
            float h = hm.getHeight(x, z) + strength * falloff;
            hm.setHeight(x, z, h);
        }
    }
    upload();
}

void TerrainAPI::lower(float worldX, float worldZ, float radius, float strength)
{
    raise(worldX, worldZ, radius, -strength);
}

void TerrainAPI::flatten(float worldX, float worldZ, float radius, float targetHeight)
{
    auto& hm = m_terrain.internalHeightmap();
    int res = hm.resolution;
    float halfSize = res / 2.0f;

    int cx = static_cast<int>(std::floor(worldX / m_mapScale + halfSize));
    int cz = static_cast<int>(std::floor(worldZ / m_mapScale + halfSize));
    int r = static_cast<int>(radius / m_mapScale);

    for (int z = cz - r; z <= cz + r; ++z) {
        for (int x = cx - r; x <= cx + r; ++x) {
            if (x < 0 || x >= res || z < 0 || z >= res) continue;
            float dist = std::sqrt(float((x - cx) * (x - cx) + (z - cz) * (z - cz)));
            if (dist > r) continue;
            float falloff = 1.0f - (dist / r);
            float current = hm.getHeight(x, z);
            float newH = current + (targetHeight - current) * falloff;
            hm.setHeight(x, z, newH);
        }
    }
    upload();
}