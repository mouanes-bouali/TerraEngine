#include "IOpenGLRenderer.h"
#include "Shaders.h"      // Our new Shader class
#include <iostream>
#include <cmath>         // for std::sin

// -------------------- Constructor / Destructor --------------------

IOpenGLRenderer::IOpenGLRenderer(sf::RenderWindow& window)
    : m_window(window) {}

IOpenGLRenderer::~IOpenGLRenderer() {
    shutdown();
}

// -------------------- Initialization --------------------

bool IOpenGLRenderer::init() {
    // Activate OpenGL context (SFML requirement)
    m_window.setActive(true);

    // Load OpenGL function pointers via GLAD
    if (!gladLoadGL()) {
        std::cerr << "Failed to initialize GLAD\n";
        return false;
    }

    std::cout << "OpenGL version: " << glGetString(GL_VERSION) << "\n";

    // -------------------------------------------------------------
    // 1. Load shaders from external files (using the new Shader class)
    // -------------------------------------------------------------
    try {
        m_shader = std::make_unique<Shader>("assets/shaders/default.vert",
                                            "assets/shaders/default.frag");
    } catch (const std::exception& e) {
        std::cerr << "Shader creation failed: " << e.what() << "\n";
        return false;
    }

    if (m_shader->ID == 0) {
        std::cerr << "Shader program ID is zero – compilation/linking failed.\n";
        return false;
    }

    // -------------------------------------------------------------
    // 2. Vertex data (positions + colors)
    //    - Interleaved: [x,y,z, r,g,b, ...]
    //    - Using per-vertex colors as in the "More attributes!" tutorial.
    // -------------------------------------------------------------
    float vertices[] = {
        // positions          // colors
         0.5f, -0.5f, 0.0f,   1.0f, 0.0f, 0.0f,   // bottom right -> red
        -0.5f, -0.5f, 0.0f,   0.0f, 1.0f, 0.0f,   // bottom left  -> green
         0.0f,  0.5f, 0.0f,   0.0f, 0.0f, 1.0f    // top          -> blue
    };

    // Create VAO and VBO
    glGenVertexArrays(1, &vao);
    glGenBuffers(1, &vbo);

    glBindVertexArray(vao);

    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    // Position attribute (location = 0)
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE,
                          6 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    // Color attribute (location = 1)
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE,
                          6 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);

    // Unbind for safety
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);

    return true;
}

// -------------------- Shutdown --------------------

void IOpenGLRenderer::shutdown() {
    if (vbo) {
        glDeleteBuffers(1, &vbo);
        vbo = 0;
    }
    if (vao) {
        glDeleteVertexArrays(1, &vao);
        vao = 0;
    }
    // Shader is managed by unique_ptr – automatically deleted
    m_shader.reset();
}

// -------------------- Rendering Loop --------------------

void IOpenGLRenderer::renderScene(float alpha) {
    // Future use: alpha for interpolation between ticks
    // For now, just draw the test triangle each frame.
    beginFrame();

    // Pass the current time to the draw function so we can animate uniforms.
    // We'll use the system time for demonstration.
    static float startTime = static_cast<float>(std::clock()) / CLOCKS_PER_SEC;
    float currentTime = static_cast<float>(std::clock()) / CLOCKS_PER_SEC - startTime;
    drawTriangle(currentTime);

    endFrame();
}

// -------------------- Frame Management --------------------

void IOpenGLRenderer::beginFrame() {
    glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);
}

void IOpenGLRenderer::endFrame() {
    m_window.display(); // swap buffers
}

// -------------------- Test Triangle (with time-varying uniform) --------------------

void IOpenGLRenderer::drawTriangle(float time) {
    if (!m_shader) return;

    // Use the shader program
    m_shader->use();

    // Set a uniform that changes over time (demonstration of uniforms)
    float greenValue = (std::sin(time) / 2.0f) + 0.5f;
    m_shader->setVec4("uOverrideColor", glm::vec4(0.0f, greenValue, 0.0f, 1.0f));

    // Draw the triangle
    glBindVertexArray(vao);
    glDrawArrays(GL_TRIANGLES, 0, 3);
    glBindVertexArray(0);
}

// -------------------- Stubs (to be implemented later) --------------------

void IOpenGLRenderer::setCamera(const Camera&) {
    // TODO: update view/projection matrices in shaders
}

void IOpenGLRenderer::setLight(const RenderLight&) {
    // TODO: update lighting uniforms
}

void IOpenGLRenderer::drawInstanced(const std::vector<RenderInstance>&) {
    // TODO: implement instanced rendering
}

void IOpenGLRenderer::drawSingle(const RenderInstance&) {
    // TODO: draw a single entity with its transform
}

int IOpenGLRenderer::loadTexture(const char*) {
    // TODO: texture loading
    return -1;
}

void IOpenGLRenderer::unloadTexture(int) {
    // TODO: texture cleanup
}