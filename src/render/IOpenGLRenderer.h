#pragma once
#include "IRenderer.h"
#include <SFML/Graphics.hpp>
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <vector>
#include <string>

class IOpenGLRenderer : public IRenderer {
public:
    IOpenGLRenderer(sf::RenderWindow& window);
    ~IOpenGLRenderer() override;

    bool init() override;
    void shutdown() override;
    void beginFrame() override;
    void endFrame() override;
    void setCamera(const Camera& cam) override;
    void setLight(const RenderLight& light) override;
    void drawInstanced(const std::vector<RenderInstance>& instances) override;
    void drawSingle(const RenderInstance& instance) override;
    int loadTexture(const char* filepath) override;
    void unloadTexture(int textureId) override;

private:
    sf::RenderWindow& m_window;

    unsigned int m_shaderProgram = 0;
    unsigned int m_cubeVAO = 0;
    unsigned int m_cubeVBO = 0;
    unsigned int m_cubeEBO = 0;
    unsigned int m_instanceVBO = 0;

    std::vector<unsigned int> m_textures;

    glm::mat4 m_view{1.0f};
    glm::mat4 m_projection{1.0f};

    void createCubeMesh();
    void createShader();
    unsigned int compileShader(unsigned int type, const char* source);
};