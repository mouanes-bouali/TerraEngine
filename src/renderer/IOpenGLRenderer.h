#pragma once

#include <glad/glad.h>
#include <SFML/Graphics.hpp>
#include "IRenderer.h"
#include <glm/glm.hpp>
#include <vector>

class IOpenGLRenderer : public IRenderer {
public:
    explicit IOpenGLRenderer(sf::RenderWindow& window);
    ~IOpenGLRenderer() override;

    bool init() override;
    void shutdown() override;
    void beginFrame() override;
    void endFrame() override;
    void drawTriangle();

    void setCamera(const Camera&) override;
    void setLight(const RenderLight&) override;
    void drawInstanced(const std::vector<RenderInstance>&) override;
    void drawSingle(const RenderInstance&) override;
    int loadTexture(const char*) override;
    void unloadTexture(int) override;

private:
    sf::RenderWindow& m_window;
    GLuint shaderProgram = 0;
    GLuint vao = 0;
    GLuint vbo = 0;
    GLuint createShader(GLenum type, const char* source);
};