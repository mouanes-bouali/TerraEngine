#pragma once
#include <vector>
#include <cmath>
#include <algorithm>
#include <random>
#include <iostream>

class HeightMap {
public:
    std::vector<float> heights;
    int resolution;
    
    HeightMap(int res) : resolution(res) {
        heights.resize(res * res);
    }
    
    float getHeight(int x, int y) const {
        return heights[y * resolution + x];
    }
    
    void setHeight(int x, int y, float value) {
        heights[y * resolution + x] = value;
    }
    
    // Normalize heights to 0-1 range
    void normalize(float minHeight, float maxHeight) {
        float range = maxHeight - minHeight;
        if (range == 0.0f) return;
        
        for (auto& h : heights) {
            h = (h - minHeight) / range;
        }
    }
    
    // Square Diamond (Midpoint Displacement) Noise
    // Resolution must be 2^n + 1 (e.g., 65, 129, 257, 513)
    void generateSquareDiamond(int userRes, float roughness, float amplitude) {
        // Find nearest 2^n + 1 that's <= userRes
        int power = 1;
        while (power * 2 + 1 <= userRes) {
            power *= 2;
        }
        int actualRes = power + 1;  // Now actualRes = 2^n + 1
        if (actualRes < 3) actualRes = 3;
        
        // Update resolution to match actual size
        resolution = actualRes;
        heights.resize(actualRes * actualRes, 0.0f);
        
        std::cout << "Square Diamond: using " << actualRes << "x" << actualRes << " internal resolution (user requested " << userRes << ")\n";
        
        std::random_device rd;
        std::mt19937 gen(rd());
        std::uniform_real_distribution<float> dis(0.0f, 1.0f);
        
        setHeight(0, 0, dis(gen) * amplitude);
        setHeight(actualRes-1, 0, dis(gen) * amplitude);
        setHeight(0, actualRes-1, dis(gen) * amplitude);
        setHeight(actualRes-1, actualRes-1, dis(gen) * amplitude);
        
        int step = actualRes - 1;
        float currentAmplitude = amplitude;
        
        while (step > 1) {
            int half = step / 2;
            
            // Diamond step
            for (int y = half; y < actualRes; y += step) {
                for (int x = half; x < actualRes; x += step) {
                    float avg = (
                        getHeight(x - half, y - half) +
                        getHeight(x + half, y - half) +
                        getHeight(x - half, y + half) +
                        getHeight(x + half, y + half)
                    ) / 4.0f;
                    
                    float offset = (dis(gen) - 0.5f) * currentAmplitude;
                    setHeight(x, y, avg + offset);
                }
            }
            
            // Square step
            for (int y = 0; y < actualRes; y += half) {
                for (int x = (y + half) % step; x < actualRes; x += step) {
                    float avg = 0.0f;
                    int count = 0;
                    
                    if (x - half >= 0) { avg += getHeight(x - half, y); count++; }
                    if (x + half < actualRes) { avg += getHeight(x + half, y); count++; }
                    if (y - half >= 0) { avg += getHeight(x, y - half); count++; }
                    if (y + half < actualRes) { avg += getHeight(x, y + half); count++; }
                    
                    avg /= count;
                    float offset = (dis(gen) - 0.5f) * currentAmplitude;
                    setHeight(x, y, avg + offset);
                }
            }
            
            step = half;
            currentAmplitude *= roughness;
        }
    }
};