#pragma once
#include <concepts>
#include <cstdint>
#include <glm/glm.hpp>

struct Component {};

template<typename T>
concept SComponent = std::derived_from<T, Component>;

using EntityID = uint32_t;
constexpr EntityID INVALID_ENTITY = UINT32_MAX;

struct CTransform : Component {
    float x = 0.0f, y = 0.0f, z = 0.0f;
    float rotation = 0.0f;
    float scaleX = 1.0f, scaleY = 1.0f, scaleZ = 1.0f;
    
    CTransform() = default;
    CTransform(float x, float y, float z = 0.0f,
               float rot = 0.0f, float sx = 1.0f, float sy = 1.0f, float sz = 1.0f)
        : x(x), y(y), z(z), rotation(rot), scaleX(sx), scaleY(sy), scaleZ(sz) {}
};

struct CHealth : Component {
    int current = 100, max = 100;
    CHealth() = default;
    CHealth(int cur, int m) : current(cur), max(m) {}
};

struct CGravity : Component {
    float strength = 9.8f, weight = 1.0f;
    CGravity() = default;
    CGravity(float s, float w) : strength(s), weight(w) {}
};

struct CLifespan : Component {
    float remaining = 0.0f, total = 0.0f;
    CLifespan() = default;
    CLifespan(float rem, float tot) : remaining(rem), total(tot) {}
};

enum MaterialType : uint8_t {
    MAT_SAND = 0,
    MAT_GRASS = 1,
    MAT_ROCK = 2,
    MAT_SNOW = 3
};

struct CEnemy    : Component {};
struct CPlayer   : Component {};
struct CTile     : Component {
    float baseHeight = 0.0f;
    float currentHeight = 0.0f;
    float deformation = 0.0f;
    bool isWalkable = true;
    MaterialType material = MAT_GRASS;
    
    CTile() = default;
    CTile(float base, float current, float deform, bool walk, MaterialType mat)
        : baseHeight(base), currentHeight(current), deformation(deform), isWalkable(walk), material(mat) {}
};
struct CProjectile : Component {};
struct CParticle : Component {};
struct CBuilding : Component {};

// Links an entity to its pre-uploaded GPU mesh/texture
struct CRenderable : Component {
    uint32_t meshHandle = 0;     // index into renderer's mesh array
    int      textureId  = -1;    // -1 = solid color, >=0 = texture
    glm::vec4 color = {1.0f, 1.0f, 1.0f, 1.0f};

    CRenderable() = default;
    CRenderable(uint32_t mesh, int tex, glm::vec4 tint = {1,1,1,1})
        : meshHandle(mesh), textureId(tex), color(tint) {}
};
