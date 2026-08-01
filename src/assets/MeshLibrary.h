#pragma once
#include "assets/MeshData.h"
#include <string>
#include <vector>
#include <unordered_map>

// ─────────────────────────────────────────────────────────────────────
// MeshLibrary — owns ALL loaded mesh data on the CPU side.
//
// 1. Constructor scans `assets/meshes/` for supported files (.obj)
// 2. Each file is parsed into a MeshData (positions, colors, uvs)
// 3. MeshData are stored in ONE contiguous vector (DOD-friendly)
// 4. A name→index map gives a CLEAN API:
//        MeshHandle h = lib.getHandle("player");
//        const MeshData& md = lib.get(h);
//
// The library knows NOTHING about OpenGL/GPU. Uploading to the GPU is
// a separate step done by Renderer::uploadMesh(meshData).
// ─────────────────────────────────────────────────────────────────────
class MeshLibrary {
public:
    // Load every supported model from `assets/meshes/`
    void scanFolder(const std::string& folderPath);

    // Load ONE model file explicitly
    bool loadFile(const std::string& filepath);

    // ---- Clean API ----
    // "player" → index into m_meshes. Returns -1 if not found.
    int getHandle(const std::string& name) const;
    // "player" → const reference. Throws if missing.
    const MeshData& getByName(const std::string& name) const;
    // index → const reference. Bounds-checked.
    const MeshData& get(int handle) const;

    bool     has(const std::string& name) const;
    size_t   count() const { return m_meshes.size(); }
    const std::vector<MeshData>& all() const { return m_meshes; }

private:
    // Parses one .obj file into vertices (position + pseudo-color + uv).
    // Returns false if the file is missing or has no faces.
    bool parseObj(const std::string& filepath, MeshData& out);
    // Generates a cube mesh procedurally (used as a fallback).
    MeshData makeCube(const std::string& name);

    // Single contiguous array of all meshes (DOD-friendly, cache-friendly).
    std::vector<MeshData> m_meshes;
    // name → index into m_meshes
    std::unordered_map<std::string, int> m_nameToIndex;
};