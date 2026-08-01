#pragma once
#include <string>
#include <vector>
#include <cstdint>

// ─────────────────────────────────────────────────────────────────────
// MeshData — CPU-side raw 3D model, completely independent of any renderer.
//
// DOD-friendly: ONE contiguous array of interleaved floats per mesh.
// Per-vertex layout (stride = 8 floats = 32 bytes):
//   x, y, z,   r, g, b,   u, v
//
// Loading a file → MeshData is pure CPU work.
// The renderer later turns MeshData into GPU buffers (VAO/VBO) via
// IRenderer::uploadMesh(MeshData), which is a SEPARATE step.
// ─────────────────────────────────────────────────────────────────────
struct MeshData {
    std::string name;               // e.g. "player", "enemy", "terrain"
    std::vector<float> vertices;    // interleaved position+color+uv floats
    std::vector<uint32_t> indices;  // triangle indices (empty = non-indexed)
    uint32_t vertexCount = 0;       // = vertices.size() / 8
    uint32_t indexCount = 0;        // = indices.size() (0 = use glDrawArrays)
};
