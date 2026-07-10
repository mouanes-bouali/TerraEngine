#pragma once

#include <SFML/Graphics/RenderWindow.hpp>
#include <glad/glad.h>
#include <glm/glm.hpp>
#include <memory>
#include <vector>

// Forward declarations
class Shader;
struct Camera;
struct RenderLight;
struct RenderInstance;

/**
 * @brief OpenGL renderer backend with full 3D MVP transformation support.
 * 
 * Manages the OpenGL context, shaders, textures, and drawing primitives.
 * Implements Model-View-Projection (MVP) matrix pipeline for 3D rendering.
 * Follows RAII: resources are freed in shutdown() or destructor.
 */
class IOpenGLRenderer {
public:
    explicit IOpenGLRenderer(sf::RenderWindow& window);
    ~IOpenGLRenderer();

    // Initialization / shutdown
    bool init();
    void shutdown();

    // Main rendering entry point
    void renderScene(float alpha);

    // Camera configuration
    void setCamera(const glm::vec3& position, const glm::vec3& target, const glm::vec3& up);
    void setProjection(float fov, float aspect, float nearPlane, float farPlane);

    // Drawing commands
    void drawInstanced(const std::vector<RenderInstance>& instances);
    void drawSingle(const RenderInstance& instance);

    // Texture management
    int loadTexture(const char* filePath);
    void unloadTexture(int textureID);

private:
    // Internal helpers
    void beginFrame();
    void endFrame();
    void drawCubes(float time); // Draws multiple 3D cubes with transformations

    // OpenGL resources
    GLuint vao = 0;
    GLuint vbo = 0;
    GLuint ebo = 0;
    GLuint texture = 0;
    std::unique_ptr<Shader> m_shader;

    // Camera matrices
    glm::mat4 m_view;
    glm::mat4 m_projection;
    bool m_viewDirty = true;
    bool m_projDirty = true;

    sf::RenderWindow& m_window;

    // Delete copy/move (non-copyable)
    IOpenGLRenderer(const IOpenGLRenderer&) = delete;
    IOpenGLRenderer& operator=(const IOpenGLRenderer&) = delete;
};