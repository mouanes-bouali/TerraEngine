#include "IOpenGLRenderer.h"
#include "Shaders.h"
#include <iostream>
#include <cmath>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

// stb_image - single header image loader
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

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
    // 1. Load shaders from external files
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
    // 2. Vertex Data: positions + colors + texture coordinates
    //    Stride = 8 floats per vertex (3 pos + 3 color + 2 texCoord)
    // -------------------------------------------------------------
    float vertices[] = {
        // positions          // colors           // texture coords
         0.5f,  0.5f, 0.0f,   1.0f, 0.0f, 0.0f,   1.0f, 1.0f, // top right
         0.5f, -0.5f, 0.0f,   0.0f, 1.0f, 0.0f,   1.0f, 0.0f, // bottom right
        -0.5f, -0.5f, 0.0f,   0.0f, 0.0f, 1.0f,   0.0f, 0.0f, // bottom left
        -0.5f,  0.5f, 0.0f,   1.0f, 1.0f, 0.0f,   0.0f, 1.0f  // top left
    };

    // Indices for two triangles forming a rectangle
    unsigned int indices[] = {
        0, 1, 3,   // first triangle
        1, 2, 3    // second triangle
    };

    // Create VAO, VBO, and EBO
    glGenVertexArrays(1, &vao);
    glGenBuffers(1, &vbo);
    glGenBuffers(1, &ebo);

    glBindVertexArray(vao);

    // ---- Upload VBO (Vertex Data) ----
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    // ---- Upload EBO (Index Data) ----
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

    // ---- Set Vertex Attribute Pointers ----
    // Position attribute (location = 0)
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE,
                          8 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    // Color attribute (location = 1)
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE,
                          8 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);

    // Texture coordinate attribute (location = 2)
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE,
                          8 * sizeof(float), (void*)(6 * sizeof(float)));
    glEnableVertexAttribArray(2);

    // Unbind for safety
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
    // Note: EBO is UNBOUND when we unbind the VAO, but the VAO remembers it.

    // -------------------------------------------------------------
    // 3. Load the Texture
    // -------------------------------------------------------------
    if (!loadTexture("assets/textures/container.jpg")) {
        std::cerr << "Warning: Failed to load texture. Rendering will continue without it.\n";
    }

    return true;
}

// -------------------- Texture Loading --------------------

int IOpenGLRenderer::loadTexture(const char* path) {
    // Generate a texture object
    glGenTextures(1, &texture);
    glBindTexture(GL_TEXTURE_2D, texture);

    // Set texture wrapping/filtering parameters
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    // Load the image data using stb_image
    int width, height, nrChannels;
    stbi_set_flip_vertically_on_load(true);

    unsigned char* data = stbi_load(path, &width, &height, &nrChannels, 0);
    if (data) {
        GLenum format = (nrChannels == 4) ? GL_RGBA : GL_RGB;
        glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D);
        stbi_image_free(data);
        std::cout << "Successfully loaded texture: " << path << "\n";
        return 0; // Success
    } else {
        std::cerr << "Failed to load texture: " << path << " | Error: " << stbi_failure_reason() << "\n";
        stbi_image_free(data);
        return -1; // Failure
    }
}

// -------------------- Shutdown --------------------

void IOpenGLRenderer::shutdown() {
    if (vbo) {
        glDeleteBuffers(1, &vbo);
        vbo = 0;
    }
    if (ebo) {
        glDeleteBuffers(1, &ebo);
        ebo = 0;
    }
    if (vao) {
        glDeleteVertexArrays(1, &vao);
        vao = 0;
    }
    if (texture) {
        glDeleteTextures(1, &texture);
        texture = 0;
    }
    // Shader is managed by unique_ptr – automatically deleted
    m_shader.reset();
}

// -------------------- Rendering Loop --------------------

void IOpenGLRenderer::renderScene(float alpha) {
    beginFrame();

    static float startTime = static_cast<float>(std::clock()) / CLOCKS_PER_SEC;
    float currentTime = static_cast<float>(std::clock()) / CLOCKS_PER_SEC - startTime;
    drawRectangle(currentTime);

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

// -------------------- Drawing with Transformations! --------------------

void IOpenGLRenderer::drawRectangle(float time) {
    if (!m_shader) return;

    m_shader->use();

    // =============================================================
    // 1. BUILD THE TRANSFORMATION MATRIX
    //    Order: Scale → Rotate → Translate (applied in reverse)
    // =============================================================
    glm::mat4 transform = glm::mat4(1.0f); // Start with identity matrix

    // Translate: move to bottom-right of the screen
    transform = glm::translate(transform, glm::vec3(0.5f, -0.5f, 0.0f));

    // Rotate: spin continuously around Z-axis over time
    transform = glm::rotate(transform, time * glm::radians(50.0f), glm::vec3(0.0f, 0.0f, 1.0f));

    // Scale: make it 50% of its original size
    transform = glm::scale(transform, glm::vec3(0.5f, 0.5f, 0.5f));

    // ---- Pass the transform to the shader ----
    m_shader->setMat4("transform", transform);

    // =============================================================
    // 2. OPTIONAL: Time-varying color override (kept from before)
    // =============================================================
    float greenValue = (std::sin(time) / 2.0f) + 0.5f;
    m_shader->setVec4("uOverrideColor", glm::vec4(0.0f, greenValue, 0.0f, 1.0f));
    m_shader->setFloat("uOffset", 0.0f); // Set to 0 to stay centered

    // =============================================================
    // 3. BIND TEXTURE AND DRAW
    // =============================================================
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, texture);
    m_shader->setInt("ourTexture", 0);

    glBindVertexArray(vao);
    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
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

void IOpenGLRenderer::unloadTexture(int) {
    // TODO: texture cleanup
}