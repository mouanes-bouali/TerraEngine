#pragma once

#include <SFML/Graphics/RenderWindow.hpp>
#include <glad/glad.h>
#include <glm/glm.hpp>
#include <memory>
#include <vector>
#include <string>
#include "IRenderer.h"

// Forward declarations
class Shader;
struct Camera;
struct RenderLight;
struct RenderInstance;

// Stores GPU references for one loaded mesh
struct GPUMesh {
    GLuint vao = 0;
    GLuint vbo = 0;
    GLuint ebo = 0;
    int indexCount = 0;  // number of indices (0 = non-indexed, use vertexCount)
    int vertexCount = 0;
};

/**
 * @brief OpenGL renderer backend with full 3D MVP transformation support.
 * 
 * Manages the OpenGL context, shaders, textures, and drawing primitives.
 * Implements Model-View-Projection (MVP) matrix pipeline for 3D rendering.
 * Follows RAII: resources are freed in shutdown() or destructor.
 */
class IOpenGLRenderer : public IRenderer    {
public:
    explicit IOpenGLRenderer(sf::RenderWindow& window);
    ~IOpenGLRenderer();

    // Initialization / shutdown
    bool init() override;
    void shutdown() override;

    // Main rendering entry point
    void renderScene(float alpha)override;

    // Camera configuration
    void setCamera(const glm::vec3& position, const glm::vec3& target, const glm::vec3& up) override;
    void setProjection(float fov, float aspect, float nearPlane, float farPlane) override;

    // Drawing commands
    void drawInstanced(const std::vector<RenderInstance>& instances) override;
    void drawSingle(const RenderInstance& instance) override;

    // Texture management
    int loadTexture(const char* filePath) override;
    void unloadTexture(int textureID) override;
    void setLight(const RenderLight& light) override;
    void setCamera(const Camera& cam) override;
    void beginFrame() override;
    void endFrame() override;

    // Mesh loading
    MeshHandle loadMesh(const char* filepath) override;

private:
    // Internal helpers
    void drawCubes(float time); // Draws multiple 3D cubes with transformations
    MeshHandle addBuiltinCube(); // Creates default cube mesh, returns its handle

    // OpenGL resources
    GLuint texture = 0;
    std::unique_ptr<Shader> m_shader;

    // Camera matrices
    glm::mat4 m_view;
    glm::mat4 m_projection;
    bool m_viewDirty = true;
    bool m_projDirty = true;

    sf::RenderWindow& m_window;

    // Mesh storage — each entry is one unique mesh uploaded to GPU
    std::vector<GPUMesh> m_meshes;
    
    // Instancing buffers
    GLuint m_instanceVBO = 0;  // Stores model matrices + colors
};
