#include "assets/MeshLibrary.h"
#include <iostream>
#include <filesystem>
#include <fstream>
#include <sstream>
#include <algorithm>
#include <stdexcept>
#include <glm/glm.hpp>

namespace fs = std::filesystem;

void MeshLibrary::scanFolder(const std::string& folderPath) {
    if (!fs::exists(folderPath)) {
        std::cerr << "MeshLibrary: folder not found: " << folderPath << "\n";
        return;
    }

    int loaded = 0;
    for (const auto& entry : fs::directory_iterator(folderPath)) {
        if (entry.path().extension() == ".obj") {
            if (loadFile(entry.path().string())) {
                loaded++;
            }
        }
    }
    std::cout << "MeshLibrary: loaded " << loaded << " meshes from " << folderPath << "\n";
}

bool MeshLibrary::loadFile(const std::string& filepath) {
    std::string name = fs::path(filepath).stem().string();  // "player.obj" → "player"

    // Avoid double-loading the same logical mesh.
    if (m_nameToIndex.count(name)) {
        std::cout << "MeshLibrary: '" << name << "' already loaded, skipping\n";
        return false;
    }

    MeshData mesh;
    mesh.name = name;

    if (!parseObj(filepath, mesh)) {
        std::cerr << "MeshLibrary: failed to parse " << filepath
                  << " — falling back to a cube for '" << name << "'\n";
        mesh = makeCube(name);
    }

    int index = static_cast<int>(m_meshes.size());
    m_meshes.push_back(std::move(mesh));
    m_nameToIndex[name] = index;

    std::cout << "MeshLibrary: '" << name << "' → handle " << index
              << " (" << m_meshes[index].vertexCount << " vertices)\n";
    return true;
}

// ─────────────────────────────────────────────────────────────────────
// .obj parser
// Handles: v (position), vt (texcoord), vn (normal), f (triangles)
// Output layout per vertex (8 floats): x,y,z, r,g,b, u,v
// When a normal is present it is converted into a pseudo-color so the
// geometry is visible without any lighting code.
// ─────────────────────────────────────────────────────────────────────
bool MeshLibrary::parseObj(const std::string& filepath, MeshData& out) {
    std::ifstream file(filepath);
    if (!file.is_open()) {
        std::cerr << "MeshLibrary: cannot open " << filepath << "\n";
        return false;
    }

    std::vector<glm::vec3> positions;
    std::vector<glm::vec2> texcoords;
    std::vector<glm::vec3> normals;

    std::string line;
    while (std::getline(file, line)) {
        std::istringstream iss(line);
        std::string prefix;
        iss >> prefix;

        if (prefix == "v") {
            glm::vec3 p;
            iss >> p.x >> p.y >> p.z;
            positions.push_back(p);
        }
        else if (prefix == "vt") {
            glm::vec2 uv;
            iss >> uv.x >> uv.y;
            texcoords.push_back(uv);
        }
        else if (prefix == "vn") {
            glm::vec3 n;
            iss >> n.x >> n.y >> n.z;
            normals.push_back(n);
        }
        else if (prefix == "f") {
            // Face: f v1/vt1/vn1 v2/vt2/vn2 v3/vt3/vn3
            for (int i = 0; i < 3; i++) {
                std::string vert;
                iss >> vert;
                std::replace(vert.begin(), vert.end(), '/', ' ');

                std::istringstream viss(vert);
                int vIdx = 0, vtIdx = 0, vnIdx = 0;
                viss >> vIdx >> vtIdx >> vnIdx;

                // .obj indices are 1-based; clamp to valid ranges.
                vIdx  = (vIdx  > 0 && vIdx  <= (int)positions.size()) ? vIdx  - 1 : 0;
                vtIdx = (vtIdx > 0 && vtIdx <= (int)texcoords.size()) ? vtIdx - 1 : 0;
                vnIdx = (vnIdx > 0 && vnIdx <= (int)normals.size())  ? vnIdx - 1 : 0;

                out.vertices.push_back(positions[vIdx].x);
                out.vertices.push_back(positions[vIdx].y);
                out.vertices.push_back(positions[vIdx].z);

                // Pseudo-color from normal (gives shape visibility without lighting).
                if (vnIdx < (int)normals.size()) {
                    out.vertices.push_back(normals[vnIdx].x * 0.5f + 0.5f);
                    out.vertices.push_back(normals[vnIdx].y * 0.5f + 0.5f);
                    out.vertices.push_back(normals[vnIdx].z * 0.5f + 0.5f);
                } else {
                    out.vertices.push_back(1.0f);
                    out.vertices.push_back(1.0f);
                    out.vertices.push_back(1.0f);
                }

                out.vertices.push_back(texcoords[vtIdx].x);
                out.vertices.push_back(texcoords[vtIdx].y);
            }
        }
    }

    out.vertexCount = static_cast<uint32_t>(out.vertices.size() / 8);
    return out.vertexCount > 0;
}

// ─────────────────────────────────────────────────────────────────────
// Procedural fallback cube
//   -1..+1 on each axis, per-face color coding (matches old renderer)
// ─────────────────────────────────────────────────────────────────────
MeshData MeshLibrary::makeCube(const std::string& name) {
    MeshData cube;
    cube.name = name;

    // 6 faces × 2 triangles × 3 vertices = 36 vertices
    static const float verts[] = {
        // positions            // colors             // uv
        -0.5f, -0.5f, -0.5f,    1.0f, 0.0f, 0.0f,    0.0f, 0.0f,
         0.5f, -0.5f, -0.5f,    1.0f, 0.0f, 0.0f,    1.0f, 0.0f,
         0.5f,  0.5f, -0.5f,    1.0f, 0.0f, 0.0f,    1.0f, 1.0f,
         0.5f,  0.5f, -0.5f,    1.0f, 0.0f, 0.0f,    1.0f, 1.0f,
        -0.5f,  0.5f, -0.5f,    1.0f, 0.0f, 0.0f,    0.0f, 1.0f,
        -0.5f, -0.5f, -0.5f,    1.0f, 0.0f, 0.0f,    0.0f, 0.0f,

        -0.5f, -0.5f,  0.5f,    0.0f, 1.0f, 0.0f,    0.0f, 0.0f,
         0.5f, -0.5f,  0.5f,    0.0f, 1.0f, 0.0f,    1.0f, 0.0f,
         0.5f,  0.5f,  0.5f,    0.0f, 1.0f, 0.0f,    1.0f, 1.0f,
         0.5f,  0.5f,  0.5f,    0.0f, 1.0f, 0.0f,    1.0f, 1.0f,
        -0.5f,  0.5f,  0.5f,    0.0f, 1.0f, 0.0f,    0.0f, 1.0f,
        -0.5f, -0.5f,  0.5f,    0.0f, 1.0f, 0.0f,    0.0f, 0.0f,

        -0.5f,  0.5f,  0.5f,    0.0f, 0.0f, 1.0f,    1.0f, 0.0f,
        -0.5f,  0.5f, -0.5f,    0.0f, 0.0f, 1.0f,    1.0f, 1.0f,
        -0.5f, -0.5f, -0.5f,    0.0f, 0.0f, 1.0f,    0.0f, 1.0f,
        -0.5f, -0.5f, -0.5f,    0.0f, 0.0f, 1.0f,    0.0f, 1.0f,
        -0.5f, -0.5f,  0.5f,    0.0f, 0.0f, 1.0f,    0.0f, 0.0f,
        -0.5f,  0.5f,  0.5f,    0.0f, 0.0f, 1.0f,    1.0f, 0.0f,

         0.5f,  0.5f,  0.5f,    1.0f, 1.0f, 0.0f,    1.0f, 0.0f,
         0.5f,  0.5f, -0.5f,    1.0f, 1.0f, 0.0f,    1.0f, 1.0f,
         0.5f, -0.5f, -0.5f,    1.0f, 1.0f, 0.0f,    0.0f, 1.0f,
         0.5f, -0.5f, -0.5f,    1.0f, 1.0f, 0.0f,    0.0f, 1.0f,
         0.5f, -0.5f,  0.5f,    1.0f, 1.0f, 0.0f,    0.0f, 0.0f,
         0.5f,  0.5f,  0.5f,    1.0f, 1.0f, 0.0f,    1.0f, 0.0f,

        -0.5f, -0.5f, -0.5f,    0.0f, 1.0f, 1.0f,    0.0f, 1.0f,
         0.5f, -0.5f, -0.5f,    0.0f, 1.0f, 1.0f,    1.0f, 1.0f,
         0.5f, -0.5f,  0.5f,    0.0f, 1.0f, 1.0f,    1.0f, 0.0f,
         0.5f, -0.5f,  0.5f,    0.0f, 1.0f, 1.0f,    1.0f, 0.0f,
        -0.5f, -0.5f,  0.5f,    0.0f, 1.0f, 1.0f,    0.0f, 0.0f,
        -0.5f, -0.5f, -0.5f,    0.0f, 1.0f, 1.0f,    0.0f, 1.0f,

        -0.5f,  0.5f, -0.5f,    1.0f, 0.0f, 1.0f,    0.0f, 1.0f,
         0.5f,  0.5f, -0.5f,    1.0f, 0.0f, 1.0f,    1.0f, 1.0f,
         0.5f,  0.5f,  0.5f,    1.0f, 0.0f, 1.0f,    1.0f, 0.0f,
         0.5f,  0.5f,  0.5f,    1.0f, 0.0f, 1.0f,    1.0f, 0.0f,
        -0.5f,  0.5f,  0.5f,    1.0f, 0.0f, 1.0f,    0.0f, 0.0f,
        -0.5f,  0.5f, -0.5f,    1.0f, 0.0f, 1.0f,    0.0f, 1.0f,
    };

    cube.vertices.assign(verts, verts + sizeof(verts) / sizeof(float));
    cube.vertexCount = static_cast<uint32_t>(cube.vertices.size() / 8);
    return cube;
}

// ─────────────────────────────────────────────────────────────────────
// Clean lookup API
// ─────────────────────────────────────────────────────────────────────
int MeshLibrary::getHandle(const std::string& name) const {
    auto it = m_nameToIndex.find(name);
    return (it != m_nameToIndex.end()) ? it->second : -1;
}

bool MeshLibrary::has(const std::string& name) const {
    return m_nameToIndex.count(name) > 0;
}

const MeshData& MeshLibrary::getByName(const std::string& name) const {
    auto it = m_nameToIndex.find(name);
    if (it == m_nameToIndex.end()) {
        throw std::runtime_error("MeshLibrary: no mesh named '" + name + "'");
    }
    return m_meshes[it->second];
}

const MeshData& MeshLibrary::get(int handle) const {
    if (handle < 0 || handle >= static_cast<int>(m_meshes.size())) {
        throw std::runtime_error("MeshLibrary: invalid handle " + std::to_string(handle));
    }
    return m_meshes[handle];
}