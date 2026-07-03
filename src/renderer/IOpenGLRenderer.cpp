#include "IOpenGLRenderer.h"
#include <iostream>

IOpenGLRenderer::IOpenGLRenderer(sf::RenderWindow& window)
    : m_window(window) {}

IOpenGLRenderer::~IOpenGLRenderer() {
    shutdown();
}

bool IOpenGLRenderer::init() {
    m_window.setActive(true);

    if (!gladLoadGL()) {
        std::cerr << "Failed to initialize GLAD\n";
        return false;
    }

    std::cout << "OpenGL version: " << glGetString(GL_VERSION) << "\n";

    // Triangle vertices
    float vertices[] = {
        -0.5f, -0.5f, 0.0f,
         0.5f, -0.5f, 0.0f,
         0.0f,  0.5f, 0.0f
    };

    glGenVertexArrays(1, &vao);
    glGenBuffers(1, &vbo);

    glBindVertexArray(vao);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    const char* vertexSource = R"(
        #version 330 core
        layout(location=0) in vec3 aPos;
        void main() {
            gl_Position = vec4(aPos, 1.0);
        }
    )";

    const char* fragmentSource = R"(
        #version 330 core
        out vec4 FragColor;
        void main() {
            FragColor = vec4(1.0, 0.5, 0.2, 1.0);
        }
    )";

    GLuint vs = createShader(GL_VERTEX_SHADER, vertexSource);
    GLuint fs = createShader(GL_FRAGMENT_SHADER, fragmentSource);

    shaderProgram = glCreateProgram();
    glAttachShader(shaderProgram, vs);
    glAttachShader(shaderProgram, fs);
    glLinkProgram(shaderProgram);
    glDeleteShader(vs);
    glDeleteShader(fs);

    return true;
}

GLuint IOpenGLRenderer::createShader(GLenum type, const char* source) {
    GLuint shader = glCreateShader(type);
    glShaderSource(shader, 1, &source, nullptr);
    glCompileShader(shader);

    GLint success;
    glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
    if (!success) {
        char log[512];
        glGetShaderInfoLog(shader, sizeof(log), nullptr, log);
        std::cerr << "Shader error: " << log << "\n";
    }
    return shader;
}

void IOpenGLRenderer::beginFrame() {
    glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);
}

void IOpenGLRenderer::drawTriangle() {
    glUseProgram(shaderProgram);
    glBindVertexArray(vao);
    glDrawArrays(GL_TRIANGLES, 0, 3);
}

void IOpenGLRenderer::endFrame() {
    m_window.display();
}

void IOpenGLRenderer::shutdown() {
    if (vbo) glDeleteBuffers(1, &vbo);
    if (vao) glDeleteVertexArrays(1, &vao);
    if (shaderProgram) glDeleteProgram(shaderProgram);
    shaderProgram = 0;
    vao = 0;
    vbo = 0;
}

// Stubs
void IOpenGLRenderer::setCamera(const Camera&) {}
void IOpenGLRenderer::setLight(const RenderLight&) {}
void IOpenGLRenderer::drawInstanced(const std::vector<RenderInstance>&) {}
void IOpenGLRenderer::drawSingle(const RenderInstance&) {}
int IOpenGLRenderer::loadTexture(const char*) { return -1; }
void IOpenGLRenderer::unloadTexture(int) {}