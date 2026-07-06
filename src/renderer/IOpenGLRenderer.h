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
 * @brief OpenGL renderer backend with full matrix transformation support.
 * 
 * Manages the OpenGL context, shaders, textures, and drawing primitives.
 * Implements model transformations (translate, rotate, scale) via GLM.
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
    void renderScene(float alpha); // alpha for interpolation (future use)

    // Configuration (stubs for now)
    void setCamera(const Camera& cam);
    void setLight(const RenderLight& light);

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
    void drawRectangle(float time); // Draws the textured rectangle with transformations

    // OpenGL resources
    GLuint vao = 0;
    GLuint vbo = 0;
    GLuint ebo = 0;
    GLuint texture = 0;
    std::unique_ptr<Shader> m_shader;

    sf::RenderWindow& m_window;

    // Delete copy/move (non-copyable)
    IOpenGLRenderer(const IOpenGLRenderer&) = delete;
    IOpenGLRenderer& operator=(const IOpenGLRenderer&) = delete;
};