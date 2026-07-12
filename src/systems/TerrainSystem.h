#pragma once
#include "entities/EntityManager.h"
#include "entities/EntityBuilder.h"
#include "renderer/IRenderer.h"
#include <glm/glm.hpp>

struct TerrainConfig {
    float mapSize = 100.0f;      // world units
    float tileSize = 1.0f;       // size of each cube
    int gridSize = 100;          // 100x100 grid
    float heightScale = 8.0f;    // max height
    float noiseScale = 0.05f;    // frequency of terrain features
};

class TerrainSystem {
public:
    TerrainConfig config;
    
    void generate(EntityManager& em, EntityBuilder& builder, 
                  MeshHandle cubeMesh, int grassTex, int stoneTex, int sandTex);
    
    // Simple noise function (replace with Perlin/Simplex later)
    float noise(float x, float z) const;
    float getHeight(float x, float z) const;
    int getMaterial(float height) const;
    
    // Square Diamond noise (much better terrain)
    void generateSquareDiamondTerrain(EntityManager& em, EntityBuilder& builder,
                                      MeshHandle cubeMesh, int resolution,
                                      float roughness, float amplitude);
};
