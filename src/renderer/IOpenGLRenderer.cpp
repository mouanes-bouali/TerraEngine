#include "IOpenGLRenderer.h"
#include "Shaders.h"
#include <iostream>
#include <cmath>
#include <glm/gtc/type_ptr.hpp> // for glm::value_ptr (future use)

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
    glGenBuffers(1, &m_ebo);

    glBindVertexArray(vao);

    // ---- Upload VBO (Vertex Data) ----
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    // ---- Upload EBO (Index Data) ----
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_ebo);
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
        // We don't return false here so the engine still runs, just without a texture.
    }

    return true;
}

// -------------------- Texture Loading --------------------

bool IOpenGLRenderer::loadTexture(const char* path) {
    // Generate a texture object
    glGenTextures(1, &m_texture);
    glBindTexture(GL_TEXTURE_2D, m_texture);

    // Set texture wrapping/filtering parameters
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    // Load the image data using stb_image
    int width, height, nrChannels;
    stbi_set_flip_vertically_on_load(true); // OpenGL expects 0,0 at bottom-left. Images have it at top-left. Flip it.

    unsigned char* data = stbi_load(path, &width, &height, &nrChannels, 0);
    if (data) {
        // Determine the correct OpenGL format (RGB or RGBA)
        GLenum format = (nrChannels == 4) ? GL_RGBA : GL_RGB;
        
        // Upload the raw pixel data to the GPU
        glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D); // Generate mipmaps for better scaling

        stbi_image_free(data); // Free the CPU RAM copy
        std::cout << "Successfully loaded texture: " << path << "\n";
        return true;
    } else {
        std::cerr << "Failed to load texture: " << path << "\n";
        stbi_image_free(data); // Cleanup even on failure
        return false;
    }
}

// -------------------- Shutdown --------------------

void IOpenGLRenderer::shutdown() {
    if (vbo) {
        glDeleteBuffers(1, &vbo);
        vbo = 0;
    }
    if (m_ebo) {
        glDeleteBuffers(1, &m_ebo);
        m_ebo = 0;
    }
    if (vao) {
        glDeleteVertexArrays(1, &vao);
        vao = 0;
    }
    if (m_texture) {
        glDeleteTextures(1, &m_texture);
        m_texture = 0;
    }
    // Shader is managed by unique_ptr – automatically deleted
    m_shader.reset();
}

// -------------------- Rendering Loop --------------------

void IOpenGLRenderer::renderScene(float alpha) {
    beginFrame();

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

// -------------------- Drawing (Now with Textures!) --------------------

void IOpenGLRenderer::drawTriangle(float time) {
    if (!m_shader) return;

    m_shader->use();

    // 1. Set uniforms (time-varying color and offset)
    float greenValue = (std::sin(time) / 2.0f) + 0.5f;
    m_shader->setVec4("uOverrideColor", glm::vec4(0.0f, greenValue, 0.0f, 1.0f));
    m_shader->setFloat("uOffset", 0.0f); // Set to 0 to stay centered, or animate it!

    // 2. Bind the texture to Texture Unit 0
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, m_texture);
    m_shader->setInt("ourTexture", 0); // Tell the shader: "Look at Texture Unit 0"

    // 3. Draw the rectangle using the EBO (6 indices = 2 triangles)
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