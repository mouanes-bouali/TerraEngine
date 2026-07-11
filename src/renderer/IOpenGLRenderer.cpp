#include "IOpenGLRenderer.h"
#include "Shaders.h"
#include <iostream>
#include <cmath>
#include <vector>
#include <glm/gtc/matrix_transform.hpp>
#include <cmath>

#include <glm/gtc/type_ptr.hpp>

// stb_image - single header image loader
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

// -------------------- Constructor / Destructor --------------------

IOpenGLRenderer::IOpenGLRenderer(sf::RenderWindow &window)
    : m_window(window), m_view(glm::mat4(1.0f)), m_projection(glm::mat4(1.0f)) {}

IOpenGLRenderer::~IOpenGLRenderer()
{
    shutdown();
}

// -------------------- Camera Configuration --------------------

void IOpenGLRenderer::setCamera(const glm::vec3 &position, const glm::vec3 &target, const glm::vec3 &up)
{
    m_view = glm::lookAt(position, target, up);
    m_viewDirty = false;
}

void IOpenGLRenderer::setCamera(const Camera& cam)
{
    m_view = cam.view;
    m_projection = cam.projection;
}

void IOpenGLRenderer::setProjection(float fov, float aspect, float nearPlane, float farPlane)
{
    m_projection = glm::perspective(glm::radians(fov), aspect, nearPlane, farPlane);
    m_projDirty = false;
}

// -------------------- Initialization --------------------

bool IOpenGLRenderer::init()
{
    // Activate OpenGL context (SFML requirement)
    m_window.setActive(true);

    // Load OpenGL function pointers via GLAD
    if (!gladLoadGL())
    {
        std::cerr << "Failed to initialize GLAD\n";
        return false;
    }

    std::cout << "OpenGL version: " << glGetString(GL_VERSION) << "\n";

    // Enable depth testing (Z-buffer)
    glEnable(GL_DEPTH_TEST);

    // -------------------------------------------------------------
    // 1. Load shaders from external files
    // -------------------------------------------------------------
    try
    {
        m_shader = std::make_unique<Shader>("assets/shaders/default.vert",
                                            "assets/shaders/default.frag");
    }
    catch (const std::exception &e)
    {
        std::cerr << "Shader creation failed: " << e.what() << "\n";
        return false;
    }

    if (m_shader->ID == 0)
    {
        std::cerr << "Shader program ID is zero – compilation/linking failed.\n";
        return false;
    }

    // -------------------------------------------------------------
    // 2. Vertex Data: A 3D CUBE (36 vertices, 6 faces)
    //    Each vertex: Position (3) + Color (3) + TexCoord (2) = 8 floats
    // -------------------------------------------------------------
    float vertices[] = {
        // positions          // colors           // texture coords
        // Back face
        -0.5f, -0.5f, -0.5f, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f,
        0.5f, -0.5f, -0.5f, 1.0f, 0.0f, 0.0f, 1.0f, 0.0f,
        0.5f, 0.5f, -0.5f, 1.0f, 0.0f, 0.0f, 1.0f, 1.0f,
        0.5f, 0.5f, -0.5f, 1.0f, 0.0f, 0.0f, 1.0f, 1.0f,
        -0.5f, 0.5f, -0.5f, 1.0f, 0.0f, 0.0f, 0.0f, 1.0f,
        -0.5f, -0.5f, -0.5f, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f,

        // Front face
        -0.5f, -0.5f, 0.5f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f,
        0.5f, -0.5f, 0.5f, 0.0f, 1.0f, 0.0f, 1.0f, 0.0f,
        0.5f, 0.5f, 0.5f, 0.0f, 1.0f, 0.0f, 1.0f, 1.0f,
        0.5f, 0.5f, 0.5f, 0.0f, 1.0f, 0.0f, 1.0f, 1.0f,
        -0.5f, 0.5f, 0.5f, 0.0f, 1.0f, 0.0f, 0.0f, 1.0f,
        -0.5f, -0.5f, 0.5f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f,

        // Left face
        -0.5f, 0.5f, 0.5f, 0.0f, 0.0f, 1.0f, 1.0f, 0.0f,
        -0.5f, 0.5f, -0.5f, 0.0f, 0.0f, 1.0f, 1.0f, 1.0f,
        -0.5f, -0.5f, -0.5f, 0.0f, 0.0f, 1.0f, 0.0f, 1.0f,
        -0.5f, -0.5f, -0.5f, 0.0f, 0.0f, 1.0f, 0.0f, 1.0f,
        -0.5f, -0.5f, 0.5f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f,
        -0.5f, 0.5f, 0.5f, 0.0f, 0.0f, 1.0f, 1.0f, 0.0f,

        // Right face
        0.5f, 0.5f, 0.5f, 1.0f, 1.0f, 0.0f, 1.0f, 0.0f,
        0.5f, 0.5f, -0.5f, 1.0f, 1.0f, 0.0f, 1.0f, 1.0f,
        0.5f, -0.5f, -0.5f, 1.0f, 1.0f, 0.0f, 0.0f, 1.0f,
        0.5f, -0.5f, -0.5f, 1.0f, 1.0f, 0.0f, 0.0f, 1.0f,
        0.5f, -0.5f, 0.5f, 1.0f, 1.0f, 0.0f, 0.0f, 0.0f,
        0.5f, 0.5f, 0.5f, 1.0f, 1.0f, 0.0f, 1.0f, 0.0f,

        // Bottom face
        -0.5f, -0.5f, -0.5f, 0.0f, 1.0f, 1.0f, 0.0f, 1.0f,
        0.5f, -0.5f, -0.5f, 0.0f, 1.0f, 1.0f, 1.0f, 1.0f,
        0.5f, -0.5f, 0.5f, 0.0f, 1.0f, 1.0f, 1.0f, 0.0f,
        0.5f, -0.5f, 0.5f, 0.0f, 1.0f, 1.0f, 1.0f, 0.0f,
        -0.5f, -0.5f, 0.5f, 0.0f, 1.0f, 1.0f, 0.0f, 0.0f,
        -0.5f, -0.5f, -0.5f, 0.0f, 1.0f, 1.0f, 0.0f, 1.0f,

        // Top face
        -0.5f, 0.5f, -0.5f, 1.0f, 0.0f, 1.0f, 0.0f, 1.0f,
        0.5f, 0.5f, -0.5f, 1.0f, 0.0f, 1.0f, 1.0f, 1.0f,
        0.5f, 0.5f, 0.5f, 1.0f, 0.0f, 1.0f, 1.0f, 0.0f,
        0.5f, 0.5f, 0.5f, 1.0f, 0.0f, 1.0f, 1.0f, 0.0f,
        -0.5f, 0.5f, 0.5f, 1.0f, 0.0f, 1.0f, 0.0f, 0.0f,
        -0.5f, 0.5f, -0.5f, 1.0f, 0.0f, 1.0f, 0.0f, 1.0f};

    // Create VAO, VBO (we don't need EBO anymore since we have 36 vertices)
    glGenVertexArrays(1, &vao);
    glGenBuffers(1, &vbo);

    glBindVertexArray(vao);

    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    // Position attribute (location = 0)
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE,
                          8 * sizeof(float), (void *)0);
    glEnableVertexAttribArray(0);

    // Color attribute (location = 1)
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE,
                          8 * sizeof(float), (void *)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);

    // Texture coordinate attribute (location = 2)
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE,
                          8 * sizeof(float), (void *)(6 * sizeof(float)));
    glEnableVertexAttribArray(2);

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);

    // -------------------------------------------------------------
    // 3. Load the Texture
    // -------------------------------------------------------------
    int textureResult = loadTexture("C:\\Users\\boual\\OneDrive\\Desktop\\Projects\\Game_Engine\\TerraEngine\\assets\\textures\\container.jpg");
    if (textureResult == -1)
    {
        std::cerr << "Warning: Failed to load texture. Rendering will continue without it.\n";
    }
    // -------------------------------------------------------------
    // 4. Set default camera and projection (will be overridden later)
    // -------------------------------------------------------------


    setCamera(glm::vec3(0.0f, 0.0f, 3.0f), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f));
    setProjection(45.0f, 800.0f / 600.0f, 0.1f, 100.0f);

    return true;
}

// -------------------- Texture Loading --------------------

int IOpenGLRenderer::loadTexture(const char *path)
{
    glGenTextures(1, &texture);
    glBindTexture(GL_TEXTURE_2D, texture);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    int width, height, nrChannels;
    stbi_set_flip_vertically_on_load(true);

    unsigned char *data = stbi_load(path, &width, &height, &nrChannels, 0);
    if (data)
    {
        GLenum format = (nrChannels == 4) ? GL_RGBA : GL_RGB;
        glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D);
        stbi_image_free(data);
        std::cout << "Successfully loaded texture: " << path << "\n";
        return 0;
    }
    else
    {
        std::cerr << "Failed to load texture: " << path << " | Error: " << stbi_failure_reason() << "\n";
        stbi_image_free(data);
        return -1;
    }
}

// -------------------- Shutdown --------------------

void IOpenGLRenderer::shutdown()
{
    if (vbo)
    {
        glDeleteBuffers(1, &vbo);
        vbo = 0;
    }
    if (vao)
    {
        glDeleteVertexArrays(1, &vao);
        vao = 0;
    }
    if (texture)
    {
        glDeleteTextures(1, &texture);
        texture = 0;
    }
    m_shader.reset();
}

// -------------------- Rendering Loop --------------------

void IOpenGLRenderer::renderScene(float /*alpha*/)
{
    beginFrame();

    // Use a real wall-clock timer for smooth animation
    static sf::Clock clock;
    float elapsed = clock.getElapsedTime().asSeconds();

    float raduis = 5.0f;
    float camX= cos(elapsed)*raduis;
    float camZ= sin(elapsed)*raduis;
    //setCamera(glm::vec3(camX, 0.0f, camZ), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f));
    drawCubes(elapsed); // Pass real elapsed time for smooth continuous rotation

    endFrame();
}

// -------------------- Frame Management --------------------

void IOpenGLRenderer::beginFrame()
{
    glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); // Clear depth buffer too!
}

void IOpenGLRenderer::endFrame()
{
    m_window.display();
}

// -------------------- Drawing 3D Cubes with MVP! --------------------

void IOpenGLRenderer::drawCubes(float time)
{
    if (!m_shader)
        return;

    m_shader->use();
    m_shader->setVec4("uOverrideColor", glm::vec4(1.0f, 1.0f, 1.0f, 1.0f));
    // ---- 1. Set View and Projection matrices (once per frame) ----
    m_shader->setMat4("view", m_view);
    m_shader->setMat4("projection", m_projection);

    // ---- 2. Bind texture ----
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, texture);
    m_shader->setInt("ourTexture", 0);

    // ---- 3. Define 10 positions for cubes ----
    std::vector<glm::vec3> cubePositions = {
        glm::vec3(0.0f, 0.0f, 0.0f),
        glm::vec3(2.0f, 5.0f, -15.0f),
        glm::vec3(-1.5f, -2.2f, -2.5f),
        glm::vec3(-3.8f, -2.0f, -12.3f),
        glm::vec3(2.4f, -0.4f, -3.5f),
        glm::vec3(-1.7f, 3.0f, -7.5f),
        glm::vec3(1.3f, -2.0f, -2.5f),
        glm::vec3(1.5f, 2.0f, -2.5f),
        glm::vec3(1.5f, 0.2f, -1.5f),
        glm::vec3(-1.3f, 1.0f, -1.5f)};

    // ---- 4. Draw each cube with its own model matrix ----
    glBindVertexArray(vao);

    for (size_t i = 0; i < cubePositions.size(); i++)
    {
        glm::mat4 model = glm::mat4(1.0f);

        // Translate
        model = glm::translate(model, cubePositions[i]);

        // Rotate: unique angle per cube
        float angle = 20.0f * static_cast<float>(i);
        // If cube index is 0, 3, 6, 9 (every 3rd), rotate over time
        if (i % 3 == 0)
        {
            angle += time * 50.0f;
        }
        model = glm::rotate(model, glm::radians(angle), glm::vec3(1.0f, 0.3f, 0.5f));

        // Pass model matrix to shader
        m_shader->setMat4("model", model);

        // Draw the cube (36 vertices = 12 triangles)
        glDrawArrays(GL_TRIANGLES, 0, 36);
    }

    glBindVertexArray(0);
}

// -------------------- Stubs --------------------

void IOpenGLRenderer::drawInstanced(const std::vector<RenderInstance> &) {}
void IOpenGLRenderer::drawSingle(const RenderInstance &) {}
void IOpenGLRenderer::unloadTexture(int) {}

void IOpenGLRenderer::setLight(const RenderLight&) {
    // stub – lighting not yet implemented
}