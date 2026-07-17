#pragma once
#include <glm/glm.hpp>
#include <vector>
#include <cstdint>

using MeshHandle = uint32_t;

struct RenderInstance {
    glm::mat4 modelMatrix;   // where to draw
    glm::vec4 color;         // tint color
    int textureId = -1;      // -1 = solid color, >=0 = texture index
    MeshHandle mesh = 0;     // which pre-loaded mesh to draw
};

struct RenderLight {
    glm::vec3 direction;     // sun direction for day/night
    glm::vec3 color;         // light color
    float intensity;         // 0.0 = night, 1.0 = full day
};

struct Camera {
    glm::mat4 view;
    glm::mat4 projection;
    glm::vec3 position;
};


class IRenderer {
public:
    virtual ~IRenderer() = default;

    // ----- Lifecycle -----
    virtual bool init() = 0;                    // create window, setup OpenGL
    virtual void shutdown() = 0;                // cleanup
    virtual void beginFrame() = 0;              // clear screen, setup camera
    virtual void endFrame() = 0;                // swap buffers, display

    // ----- Camera -----
    virtual void setCamera(const Camera& cam) = 0;
    virtual void renderScene(float alpha) = 0; // render all queued objects with interpolation factor alpha
    virtual void setProjection(float fov, float aspect, float nearPlane, float farPlane) = 0;
    virtual void setCamera(const glm::vec3& position, const glm::vec3& target, const glm::vec3& up) = 0;
    // ----- Lighting -----
    virtual void setLight(const RenderLight& light) = 0;

    // ----- Drawing -----
    // Main draw call: takes a list of instances, draws them all in one batch
    virtual void drawInstanced(const std::vector<RenderInstance>& instances) = 0;

    // For UI overlays, particles, or single objects
    virtual void drawSingle(const RenderInstance& instance) = 0;

    // ----- Meshes -----
    // Load a mesh from file, upload to GPU, return handle for later use
    virtual MeshHandle loadMesh(const char* filepath) = 0;

    // ----- Textures -----
    virtual int loadTexture(const char* filepath) = 0;   // returns texture ID
    virtual void unloadTexture(int textureId) = 0;
};