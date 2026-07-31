#include "TerrainGenerator.h"
#include "entities/Components.h"
#include "systems/HeightMap.h"
#include <cmath>
#include <algorithm>
#include <iostream>
#include <glm/glm.hpp>

// Simple pseudo-random noise (replace with Perlin/Simplex later)
float TerrainGenerator::noise(float x, float z) const {
    return sin(x * 1.5f) * cos(z * 1.5f) * 0.5f
         + sin(x * 3.7f + 1.3f) * cos(z * 3.2f + 2.1f) * 0.25f
         + sin(x * 7.1f + 3.7f) * cos(z * 6.9f + 1.1f) * 0.125f;
}

float TerrainGenerator::getHeight(float x, float z) const {
    return noise(x * terrainConfig.noiseScale, z * terrainConfig.noiseScale) * terrainConfig.heightScale;
}

int TerrainGenerator::getMaterial(float height) const {
    if (height < 0.0f) return MAT_SAND;
    if (height < 2.0f) return MAT_SAND;
    if (height < 5.0f) return MAT_GRASS;
    if (height < 7.0f) return MAT_ROCK;
    return MAT_SNOW;
}

void TerrainGenerator::generate(EntityManager& em, EntityBuilder& builder,
                              MeshHandle cubeMesh, int grassTex, int stoneTex, int sandTex) {
    std::cout << "Terrain config: gridSize=" << terrainConfig.gridSize 
              << ", tileSize=" << terrainConfig.tileSize 
              << ", mapSize=" << terrainConfig.mapSize << "\n";
              
    float halfSize = terrainConfig.mapSize / 2.0f;
    int count = 0;
    int expected = terrainConfig.gridSize * terrainConfig.gridSize;
    
    std::cout << "Starting terrain generation...\n";
    
    // Count materials for debug
    int sandCount = 0, grassCount = 0, rockCount = 0, snowCount = 0;
    float minHeight = 999.0f, maxHeight = -999.0f;
    
    for (int x = 0; x < terrainConfig.gridSize; ++x) {
        for (int z = 0; z < terrainConfig.gridSize; ++z) {
            float worldX = x * terrainConfig.tileSize - halfSize;
            float worldZ = z * terrainConfig.tileSize - halfSize;
            float height = getHeight(worldX, worldZ);
            int material = getMaterial(height);
            
            // Track min/max height
            if (height < minHeight) minHeight = height;
            if (height > maxHeight) maxHeight = height;
            
            // Count materials
            if (material == MAT_SAND) sandCount++;
            else if (material == MAT_GRASS) grassCount++;
            else if (material == MAT_ROCK) rockCount++;
            else if (material == MAT_SNOW) snowCount++;
            
            // Pick texture based on material (use -1 for no texture)
            int tex = -1;  // No texture - use vertex colors
            if (grassTex >= 0 && material == MAT_GRASS) tex = grassTex;
            else if (sandTex >= 0 && material == MAT_SAND) tex = sandTex;
            else if (stoneTex >= 0 && material >= MAT_ROCK) tex = stoneTex;
            
            // VIBRANT colors with variation
            glm::vec4 color = {1, 1, 1, 1};
            if (material == MAT_SAND) {
                // Bright yellow/tan sand
                color = {1.0f, 0.9f, 0.4f, 1.0f};
            } else if (material == MAT_GRASS) {
                // Bright green grass
                color = {0.2f, 0.8f, 0.2f, 1.0f};
            } else if (material == MAT_ROCK) {
                // Gray rock
                color = {0.6f, 0.6f, 0.6f, 1.0f};
            } else if (material == MAT_SNOW) {
                // White snow
                color = {1.0f, 1.0f, 1.0f, 1.0f};
            }
            
            // Add height-based brightness (higher = brighter)
            float brightness = 0.6f + (height / terrainConfig.heightScale) * 0.4f;
            color *= brightness;
            
            // Add position-based variation so adjacent tiles look different
            float variation = sin(x * 0.5f) * cos(z * 0.5f) * 0.15f;
            color.r += variation;
            color.g += variation;
            color.b += variation;
            
            builder()
                .with(CTransform{worldX, height, worldZ, 0, 1, 1, 1})
                .with(CRenderable{cubeMesh, tex, color})
                .with(CTile{height, height, 0.0f, true, (MaterialType)material})
                .build();
            count++;
        }
    }
    std::cout << "Created " << count << " terrain tiles (expected " << expected << ")\n";
    std::cout << "Material distribution:\n";
    std::cout << "  Sand: " << sandCount << "\n";
    std::cout << "  Grass: " << grassCount << "\n";
    std::cout << "  Rock: " << rockCount << "\n";
    std::cout << "  Snow: " << snowCount << "\n";
    std::cout << "Height range: " << minHeight << " to " << maxHeight << "\n";
}

void TerrainGenerator::generateSquareDiamondTerrain(EntityManager& em, EntityBuilder& builder,
                                                   MeshHandle cubeMesh, int resolution,
                                                   float roughness, float amplitude) {
    std::cout << "Generating Square Diamond terrain: " << resolution << "x" << resolution << "\n";
    
    // Generate heightmap using Square Diamond algorithm
    HeightMap heightMap(resolution);
    heightMap.generateSquareDiamond(resolution, roughness, amplitude);
    
    // Normalize to 0-1 range
    float minH = *std::min_element(heightMap.heights.begin(), heightMap.heights.end());
    float maxH = *std::max_element(heightMap.heights.begin(), heightMap.heights.end());
    heightMap.normalize(minH, maxH);
    
    std::cout << "Height range after normalization: " << minH << " to " << maxH << "\n";
    std::cout << "Actual resolution used: " << heightMap.resolution << "\n";
    
    // Use the actual resolution from the heightmap (may differ due to 2^n+1 requirement)
    int actualRes = heightMap.resolution;
    float halfSize = actualRes / 2.0f;
    int count = 0;
    
    for (int x = 0; x < actualRes; ++x) {
        for (int z = 0; z < actualRes; ++z) {
            float worldX = x - halfSize;
            float worldZ = z - halfSize;
            float height = heightMap.getHeight(x, z) * terrainConfig.heightScale;
            
            // Determine material based on normalized height (0-1)
            int material;
            if (height < 0.2f * terrainConfig.heightScale) material = MAT_SAND;
            else if (height < 0.5f * terrainConfig.heightScale) material = MAT_GRASS;
            else if (height < 0.75f * terrainConfig.heightScale) material = MAT_ROCK;
            else material = MAT_SNOW;
            
            // Color based on material - VERY DISTINCT
            glm::vec4 color = {1, 1, 1, 1};
            if (material == MAT_SAND) color = {1.0f, 1.0f, 0.0f, 1.0f};  // YELLOW
            else if (material == MAT_GRASS) color = {0.0f, 1.0f, 0.0f, 1.0f};  // GREEN
            else if (material == MAT_ROCK) color = {0.5f, 0.5f, 0.5f, 1.0f};  // GRAY
            else if (material == MAT_SNOW) color = {1.0f, 1.0f, 1.0f, 1.0f};  // WHITE
            
            builder()
                .with(CTransform{worldX, height, worldZ, 0, 1, 1, 1})
                .with(CRenderable{cubeMesh, -1, color})
                .with(CTile{height, height, 0.0f, true, (MaterialType)material})
                .build();
            count++;
        }
    }
    std::cout << "Created " << count << " terrain tiles\n";
}

// ─────────────────────────────────────────────────────────────────────
// Smooth terrain: generates ONE indexed mesh from a heightmap.
//
// Instead of 4225 cube entities, this creates a single MeshData with:
//   - res × res vertices (each at a height from the heightmap)
//   - (res-1)² × 2 triangle indices connecting adjacent vertices
//
// The result is a smooth, continuous surface — no blocky cubes.
// ─────────────────────────────────────────────────────────────────────
MeshData TerrainGenerator::generateSmoothTerrain(int resolution, float roughness, float amplitude) {
    std::cout << "Generating smooth terrain: " << resolution << "x" << resolution << "\n";

    // 1. Generate heightmap using Square Diamond algorithm
    HeightMap heightMap(resolution);
    heightMap.generateSquareDiamond(resolution, roughness, amplitude);

    // Normalize to 0-1 range
    float minH = *std::min_element(heightMap.heights.begin(), heightMap.heights.end());
    float maxH = *std::max_element(heightMap.heights.begin(), heightMap.heights.end());
    heightMap.normalize(minH, maxH);

    int res = heightMap.resolution;  // actual resolution (may differ from requested)
    float halfSize = res / 2.0f;
    float heightScale = terrainConfig.heightScale;

    MeshData terrain;
    terrain.name = "smooth_terrain";

    // 2. Generate vertices: one per grid point
    for (int z = 0; z < res; z++) {
        for (int x = 0; x < res; x++) {
            float worldX = (x - halfSize);
            float worldZ = (z - halfSize);
            float h = heightMap.getHeight(x, z) * heightScale;

            // Position (x, height, z)
            terrain.vertices.push_back(worldX);
            terrain.vertices.push_back(h);
            terrain.vertices.push_back(worldZ);

            // Color based on height (sand → grass → rock → snow)
            float normalizedHeight = h / heightScale;  // 0.0 to 1.0
            glm::vec3 color;
            if (normalizedHeight < 0.2f) {
                color = {0.9f, 0.8f, 0.4f};  // sand
            } else if (normalizedHeight < 0.5f) {
                // Blend sand → grass
                float t = (normalizedHeight - 0.2f) / 0.3f;
                color = glm::mix(glm::vec3(0.9f, 0.8f, 0.4f), glm::vec3(0.2f, 0.7f, 0.2f), t);
            } else if (normalizedHeight < 0.75f) {
                // Blend grass → rock
                float t = (normalizedHeight - 0.5f) / 0.25f;
                color = glm::mix(glm::vec3(0.2f, 0.7f, 0.2f), glm::vec3(0.5f, 0.5f, 0.5f), t);
            } else {
                // Blend rock → snow
                float t = (normalizedHeight - 0.75f) / 0.25f;
                color = glm::mix(glm::vec3(0.5f, 0.5f, 0.5f), glm::vec3(1.0f, 1.0f, 1.0f), t);
            }
            terrain.vertices.push_back(color.r);
            terrain.vertices.push_back(color.g);
            terrain.vertices.push_back(color.b);

            // UV (for texturing later)
            terrain.vertices.push_back(static_cast<float>(x) / res);
            terrain.vertices.push_back(static_cast<float>(z) / res);
        }
    }
    terrain.vertexCount = static_cast<uint32_t>(res * res);

    // 3. Generate indices: two triangles per grid cell
    for (int z = 0; z < res - 1; z++) {
        for (int x = 0; x < res - 1; x++) {
            uint32_t topLeft     = z * res + x;
            uint32_t topRight    = z * res + (x + 1);
            uint32_t bottomLeft  = (z + 1) * res + x;
            uint32_t bottomRight = (z + 1) * res + (x + 1);

            // Triangle 1: top-left, bottom-left, top-right (CCW winding)
            terrain.indices.push_back(topLeft);
            terrain.indices.push_back(bottomLeft);
            terrain.indices.push_back(topRight);

            // Triangle 2: top-right, bottom-left, bottom-right
            terrain.indices.push_back(topRight);
            terrain.indices.push_back(bottomLeft);
            terrain.indices.push_back(bottomRight);
        }
    }
    terrain.indexCount = static_cast<uint32_t>(terrain.indices.size());

    std::cout << "Smooth terrain: " << terrain.vertexCount << " vertices, "
              << terrain.indexCount << " indices ("
              << terrain.indexCount / 3 << " triangles)\n";

    return terrain;
}
