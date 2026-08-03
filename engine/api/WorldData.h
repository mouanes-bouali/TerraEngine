#pragma once
#include <vector>
#include <string>
#include <optional>
#include <cstdint>
#include <glm/glm.hpp>

// ─────────────────────────────────────────────────────────────────────
// WorldData — serializable data model for save/load.
//
// Everything in the world (terrain + entities + components) is stored
// as plain data. This is what gets saved to world.json and loaded back.
//
// No pointers, no OpenGL handles, no ECS internals — just data.
// ─────────────────────────────────────────────────────────────────────

// ── Component Data (Serializable) ──
struct TransformData {
    float x = 0.0f, y = 0.0f, z = 0.0f;
    float rotation = 0.0f;
    float scaleX = 1.0f, scaleY = 1.0f, scaleZ = 1.0f;
};

struct MeshRefData {
    uint32_t handle = 0;
    int textureId = -1;
    float r = 1.0f, g = 1.0f, b = 1.0f, a = 1.0f;
};

struct HealthData {
    int current = 100;
    int max = 100;
};

struct PlayerData {
    float jumpSpeed = 8.0f;
    float moveSpeed = 5.0f;
};

struct GravityData {
    float strength = 9.8f;
    float weight = 1.0f;
};

struct ColliderData {
    float radius = 1.0f;
    float offsetY = 0.0f;
};

// ── Full Entity Data ──
struct EntityData {
    uint32_t id = 0;
    std::string name = "Unnamed";
    std::string type = "entity";  // "player", "enemy", "building", "entity"
    bool isActive = true;

    TransformData transform;
    std::optional<MeshRefData> mesh;
    std::optional<HealthData> health;
    std::optional<PlayerData> player;
    std::optional<GravityData> gravity;
    std::optional<ColliderData> collider;
};

// ── Terrain Data ──
struct TerrainData {
    int resolution = 256;
    float heightScale = 8.0f;
    float mapScale = 2.0f;
    float roughness = 0.5f;
    float amplitude = 10.0f;
    std::vector<float> heightmap;  // size = resolution * resolution
};

// ── Full World Data ──
struct WorldData {
    std::string version = "1.0";
    std::string name = "Untitled World";
    TerrainData terrain;
    std::vector<EntityData> entities;
    uint32_t nextEntityId = 1;
};