#include "TerrainSystem.h"
#include "entities/Components.h"
#include "systems/HeightMap.h"
#include <cmath>
#include <algorithm>
#include <iostream>

// Simple pseudo-random noise (replace with Perlin/Simplex later)
float TerrainSystem::noise(float x, float z) const {
    return sin(x * 1.5f) * cos(z * 1.5f) * 0.5f
         + sin(x * 3.7f + 1.3f) * cos(z * 3.2f + 2.1f) * 0.25f
         + sin(x * 7.1f + 3.7f) * cos(z * 6.9f + 1.1f) * 0.125f;
}

float TerrainSystem::getHeight(float x, float z) const {
    return noise(x * config.noiseScale, z * config.noiseScale) * config.heightScale;
}

int TerrainSystem::getMaterial(float height) const {
    if (height < 0.0f) return MAT_SAND;
    if (height < 2.0f) return MAT_SAND;
    if (height < 5.0f) return MAT_GRASS;
    if (height < 7.0f) return MAT_ROCK;
    return MAT_SNOW;
}

void TerrainSystem::generate(EntityManager& em, EntityBuilder& builder,
                              MeshHandle cubeMesh, int grassTex, int stoneTex, int sandTex) {
    std::cout << "Terrain config: gridSize=" << config.gridSize 
              << ", tileSize=" << config.tileSize 
              << ", mapSize=" << config.mapSize << "\n";
              
    float halfSize = config.mapSize / 2.0f;
    int count = 0;
    int expected = config.gridSize * config.gridSize;
    
    std::cout << "Starting terrain generation...\n";
    
    // Count materials for debug
    int sandCount = 0, grassCount = 0, rockCount = 0, snowCount = 0;
    float minHeight = 999.0f, maxHeight = -999.0f;
    
    for (int x = 0; x < config.gridSize; ++x) {
        for (int z = 0; z < config.gridSize; ++z) {
            float worldX = x * config.tileSize - halfSize;
            float worldZ = z * config.tileSize - halfSize;
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
            float brightness = 0.6f + (height / config.heightScale) * 0.4f;
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

void TerrainSystem::generateSquareDiamondTerrain(EntityManager& em, EntityBuilder& builder,
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
            float height = heightMap.getHeight(x, z) * config.heightScale;
            
            // Determine material based on normalized height (0-1)
            int material;
            if (height < 0.2f * config.heightScale) material = MAT_SAND;
            else if (height < 0.5f * config.heightScale) material = MAT_GRASS;
            else if (height < 0.75f * config.heightScale) material = MAT_ROCK;
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