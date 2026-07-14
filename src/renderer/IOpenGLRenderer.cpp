#include "IOpenGLRenderer.h"
#include "Shaders.h"
#include <iostream>
#include <cmath>
#include <vector>
#include <glm/gtc/matrix_transform.hpp>
#include <cmath>
#include <fstream>
#include <sstream>

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
    // 2. Create the built-in cube mesh (mesh handle 0)
    // -------------------------------------------------------------
    addBuiltinCube();

    // -------------------------------------------------------------
    // 3. Load the Texture
    // -------------------------------------------------------------
    int textureResult = loadTexture("C:\\Users\\boual\\OneDrive\\Desktop\\Projects\\Game_Engine\\TerraEngine\\assets\\textures\\container.jpg");
    if (textureResult == -1)
    {
        std::cerr << "Warning: Failed to load texture. Rendering will continue without it.\n";
    }

    // -------------------------------------------------------------
    // 4. Set default camera and projection
    // -------------------------------------------------------------
    setCamera(glm::vec3(0.0f, 0.0f, 3.0f), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f));
    setProjection(45.0f, 800.0f / 600.0f, 0.1f, 100.0f);

    return true;
}

// -------------------- Built-in Cube Mesh (Handle 0) --------------------

MeshHandle IOpenGLRenderer::addBuiltinCube()
{
    float vertices[] = {
        // positions          // colors           // texture coords
        -0.5f, -0.5f, -0.5f, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f,
         0.5f, -0.5f, -0.5f, 1.0f, 0.0f, 0.0f, 1.0f, 0.0f,
         0.5f,  0.5f, -0.5f, 1.0f, 0.0f, 0.0f, 1.0f, 1.0f,
         0.5f,  0.5f, -0.5f, 1.0f, 0.0f, 0.0f, 1.0f, 1.0f,
        -0.5f,  0.5f, -0.5f, 1.0f, 0.0f, 0.0f, 0.0f, 1.0f,
        -0.5f, -0.5f, -0.5f, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f,

        -0.5f, -0.5f,  0.5f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f,
         0.5f, -0.5f,  0.5f, 0.0f, 1.0f, 0.0f, 1.0f, 0.0f,
         0.5f,  0.5f,  0.5f, 0.0f, 1.0f, 0.0f, 1.0f, 1.0f,
         0.5f,  0.5f,  0.5f, 0.0f, 1.0f, 0.0f, 1.0f, 1.0f,
        -0.5f,  0.5f,  0.5f, 0.0f, 1.0f, 0.0f, 0.0f, 1.0f,
        -0.5f, -0.5f,  0.5f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f,

        -0.5f,  0.5f,  0.5f, 0.0f, 0.0f, 1.0f, 1.0f, 0.0f,
        -0.5f,  0.5f, -0.5f, 0.0f, 0.0f, 1.0f, 1.0f, 1.0f,
        -0.5f, -0.5f, -0.5f, 0.0f, 0.0f, 1.0f, 0.0f, 1.0f,
        -0.5f, -0.5f, -0.5f, 0.0f, 0.0f, 1.0f, 0.0f, 1.0f,
        -0.5f, -0.5f,  0.5f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f,
        -0.5f,  0.5f,  0.5f, 0.0f, 0.0f, 1.0f, 1.0f, 0.0f,

         0.5f,  0.5f,  0.5f, 1.0f, 1.0f, 0.0f, 1.0f, 0.0f,
         0.5f,  0.5f, -0.5f, 1.0f, 1.0f, 0.0f, 1.0f, 1.0f,
         0.5f, -0.5f, -0.5f, 1.0f, 1.0f, 0.0f, 0.0f, 1.0f,
         0.5f, -0.5f, -0.5f, 1.0f, 1.0f, 0.0f, 0.0f, 1.0f,
         0.5f, -0.5f,  0.5f, 1.0f, 1.0f, 0.0f, 0.0f, 0.0f,
         0.5f,  0.5f,  0.5f, 1.0f, 1.0f, 0.0f, 1.0f, 0.0f,

        -0.5f, -0.5f, -0.5f, 0.0f, 1.0f, 1.0f, 0.0f, 1.0f,
         0.5f, -0.5f, -0.5f, 0.0f, 1.0f, 1.0f, 1.0f, 1.0f,
         0.5f, -0.5f,  0.5f, 0.0f, 1.0f, 1.0f, 1.0f, 0.0f,
         0.5f, -0.5f,  0.5f, 0.0f, 1.0f, 1.0f, 1.0f, 0.0f,
        -0.5f, -0.5f,  0.5f, 0.0f, 1.0f, 1.0f, 0.0f, 0.0f,
        -0.5f, -0.5f, -0.5f, 0.0f, 1.0f, 1.0f, 0.0f, 1.0f,

        -0.5f,  0.5f, -0.5f, 1.0f, 0.0f, 1.0f, 0.0f, 1.0f,
         0.5f,  0.5f, -0.5f, 1.0f, 0.0f, 1.0f, 1.0f, 1.0f,
         0.5f,  0.5f,  0.5f, 1.0f, 0.0f, 1.0f, 1.0f, 0.0f,
         0.5f,  0.5f,  0.5f, 1.0f, 0.0f, 1.0f, 1.0f, 0.0f,
        -0.5f,  0.5f,  0.5f, 1.0f, 0.0f, 1.0f, 0.0f, 0.0f,
        -0.5f,  0.5f, -0.5f, 1.0f, 0.0f, 1.0f, 0.0f, 1.0f};

    GPUMesh mesh;
    mesh.vertexCount = 36;

    glGenVertexArrays(1, &mesh.vao);
    glGenBuffers(1, &mesh.vbo);

    glBindVertexArray(mesh.vao);
    glBindBuffer(GL_ARRAY_BUFFER, mesh.vbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    // Position attribute (location = 0)
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE,
                          8 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    // Color attribute (location = 1)
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE,
                          8 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);

    // TexCoord attribute (location = 2)
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE,
                          8 * sizeof(float), (void*)(6 * sizeof(float)));
    glEnableVertexAttribArray(2);

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);

    m_meshes.push_back(mesh);
    return 0; // Cube is always mesh handle 0
}

// -------------------- Mesh Loading (.obj) --------------------

MeshHandle IOpenGLRenderer::loadMesh(const char* filepath)
{
    std::ifstream file(filepath);
    if (!file.is_open())
    {
        std::cerr << "Failed to open mesh file: " << filepath << "\n";
        return 0; // return cube as fallback
    }

    std::vector<glm::vec3> positions;
    std::vector<glm::vec2> texcoords;
    std::vector<glm::vec3> normals;
    std::vector<float> vertexData;

    std::string line;
    while (std::getline(file, line))
    {
        std::istringstream iss(line);
        std::string prefix;
        iss >> prefix;

        if (prefix == "v")
        {
            glm::vec3 pos;
            iss >> pos.x >> pos.y >> pos.z;
            positions.push_back(pos);
        }
        else if (prefix == "vt")
        {
            glm::vec2 uv;
            iss >> uv.x >> uv.y;
            texcoords.push_back(uv);
        }
        else if (prefix == "vn")
        {
            glm::vec3 n;
            iss >> n.x >> n.y >> n.z;
            normals.push_back(n);
        }
        else if (prefix == "f")
        {
            // Parse face: f v1/vt1/vn1 v2/vt2/vn2 v3/vt3/vn3
            for (int i = 0; i < 3; i++)
            {
                std::string vert;
                iss >> vert;
                std::replace(vert.begin(), vert.end(), '/', ' ');

                std::istringstream viss(vert);
                int vIdx = 0, vtIdx = 0, vnIdx = 0;
                viss >> vIdx >> vtIdx >> vnIdx;

                // .obj indices are 1-based
                vIdx = (vIdx > 0) ? vIdx - 1 : 0;
                vtIdx = (vtIdx > 0 && vtIdx <= (int)texcoords.size()) ? vtIdx - 1 : 0;
                vnIdx = (vnIdx > 0 && vnIdx <= (int)normals.size()) ? vnIdx - 1 : 0;

                if (vIdx < (int)positions.size())
                {
                    vertexData.push_back(positions[vIdx].x);
                    vertexData.push_back(positions[vIdx].y);
                    vertexData.push_back(positions[vIdx].z);

                    // Color (use normal as pseudo-color if available, else white)
                    if (vnIdx < (int)normals.size())
                    {
                        vertexData.push_back(normals[vnIdx].x * 0.5f + 0.5f);
                        vertexData.push_back(normals[vnIdx].y * 0.5f + 0.5f);
                        vertexData.push_back(normals[vnIdx].z * 0.5f + 0.5f);
                    }
                    else
                    {
                        vertexData.push_back(1.0f);
                        vertexData.push_back(1.0f);
                        vertexData.push_back(1.0f);
                    }

                    // Texcoord
                    if (vtIdx < (int)texcoords.size())
                    {
                        vertexData.push_back(texcoords[vtIdx].x);
                        vertexData.push_back(texcoords[vtIdx].y);
                    }
                    else
                    {
                        vertexData.push_back(0.0f);
                        vertexData.push_back(0.0f);
                    }
                }
            }
        }
    }

    file.close();

    if (vertexData.empty())
    {
        std::cerr << "No vertex data loaded from: " << filepath << "\n";
        return 0;
    }

    GPUMesh mesh;
    mesh.vertexCount = (int)vertexData.size() / 8;

    glGenVertexArrays(1, &mesh.vao);
    glGenBuffers(1, &mesh.vbo);

    glBindVertexArray(mesh.vao);
    glBindBuffer(GL_ARRAY_BUFFER, mesh.vbo);
    glBufferData(GL_ARRAY_BUFFER, vertexData.size() * sizeof(float),
                 vertexData.data(), GL_STATIC_DRAW);

    // Position (3 floats)
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE,
                          8 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    // Color (3 floats)
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE,
                          8 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);
    // TexCoord (2 floats)
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE,
                          8 * sizeof(float), (void*)(6 * sizeof(float)));
    glEnableVertexAttribArray(2);

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);

    MeshHandle handle = (MeshHandle)m_meshes.size();
    m_meshes.push_back(mesh);

    std::cout << "Loaded mesh: " << filepath << " (handle " << handle
              << ", " << mesh.vertexCount << " verts)\n";
    return handle;
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
    for (auto& mesh : m_meshes)
    {
        if (mesh.vbo) glDeleteBuffers(1, &mesh.vbo);
        if (mesh.vao) glDeleteVertexArrays(1, &mesh.vao);
        if (mesh.ebo) glDeleteBuffers(1, &mesh.ebo);
    }
    m_meshes.clear();

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
    endFrame();
}

// -------------------- Frame Management --------------------

void IOpenGLRenderer::beginFrame()
{
    // Light sky blue background
    glClearColor(0.53f, 0.81f, 0.92f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

void IOpenGLRenderer::endFrame()
{
    // Do NOT swap buffers here - GameLoop does that after ImGui renders
}

// -------------------- drawInstanced / drawSingle --------------------

void IOpenGLRenderer::drawInstanced(const std::vector<RenderInstance>& instances)
{
    if (!m_shader || m_meshes.empty() || instances.empty())
        return;

    m_shader->use();
    m_shader->setMat4("view", m_view);
    m_shader->setMat4("projection", m_projection);
    
    // Bind default texture once
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, texture);
    m_shader->setInt("ourTexture", 0);
    // Use vertex colors (no texture)

    // Use the cube mesh (handle 0)
    GPUMesh& cubeMesh = m_meshes[0];
    glBindVertexArray(cubeMesh.vao);

    // Create instance data buffer (model matrix + color per instance)
    struct InstanceData {
        glm::mat4 model;
        glm::vec4 color;
    };
    
    std::vector<InstanceData> instanceData;
    instanceData.reserve(instances.size());
    
    for (auto& inst : instances) {
        instanceData.push_back({inst.modelMatrix, inst.color});
    }

    // Create/update instance VBO
    if (!m_instanceVBO) {
        glGenBuffers(1, &m_instanceVBO);
    }
    
    glBindBuffer(GL_ARRAY_BUFFER, m_instanceVBO);
    glBufferData(GL_ARRAY_BUFFER, instanceData.size() * sizeof(InstanceData), 
                 instanceData.data(), GL_DYNAMIC_DRAW);

    // Setup instanced attributes
    // mat4 instanceModel at location 3 (takes 4 slots: 3,4,5,6)
    glVertexAttribPointer(3, 4, GL_FLOAT, GL_FALSE, sizeof(InstanceData), (void*)0);
    glEnableVertexAttribArray(3);
    glVertexAttribPointer(4, 4, GL_FLOAT, GL_FALSE, sizeof(InstanceData), (void*)(sizeof(glm::vec4)));
    glEnableVertexAttribArray(4);
    glVertexAttribPointer(5, 4, GL_FLOAT, GL_FALSE, sizeof(InstanceData), (void*)(2 * sizeof(glm::vec4)));
    glEnableVertexAttribArray(5);
    glVertexAttribPointer(6, 4, GL_FLOAT, GL_FALSE, sizeof(InstanceData), (void*)(3 * sizeof(glm::vec4)));
    glEnableVertexAttribArray(6);
    
    // vec4 instanceColor at location 7
    glVertexAttribPointer(7, 4, GL_FLOAT, GL_FALSE, sizeof(InstanceData), (void*)(4 * sizeof(glm::vec4)));
    glEnableVertexAttribArray(7);

    // Mark attributes as instanced (one per instance, not per vertex)
    for (int i = 3; i <= 7; ++i) {
        glVertexAttribDivisor(i, 1);
    }

    // ONE draw call for ALL instances
    glDrawArraysInstanced(GL_TRIANGLES, 0, cubeMesh.vertexCount, (GLsizei)instances.size());

    // Cleanup
    for (int i = 3; i <= 7; ++i) {
        glVertexAttribDivisor(i, 0);
    }
    
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
}

void IOpenGLRenderer::drawSingle(const RenderInstance& instance)
{
    drawInstanced({instance});
}

void IOpenGLRenderer::unloadTexture(int) {}

void IOpenGLRenderer::setLight(const RenderLight&) {
    // stub – lighting not yet implemented
}