#include "api/Engine.h"
#include "api/EntityBuilder.h"
#include "platform/SFMLInputSystem.h"
#include "assets/MeshLibrary.h"
#include <iostream>
#include <glad/glad.h>
#include "imgui/imgui.h"
#include "imgui/imgui-SFML.h"

Engine& Engine::get()
{
    static Engine instance;
    return instance;
}

Engine::~Engine()
{
    shutdown();
}

bool Engine::init(Window& window)
{
    m_window = &window;

    // ── Create renderer ──
    static IOpenGLRenderer openGLRenderer(window.handle);
    if (!openGLRenderer.init()) {
        std::cerr << "Engine::init: renderer init failed\n";
        return false;
    }
    m_renderer.bind(&openGLRenderer);

    // ── Create input ──
    static SFMLInputSystem sfmlInput;
    m_input = &sfmlInput;

    // ── Setup camera ──
    m_camera.target = {0.0f, 0.0f, 0.0f};
    m_camera.distance = 35.0f;
    m_camera.yaw = -135.0f;
    m_camera.pitch = 54.0f;

    // ── Load meshes from assets/meshes/ ──
    MeshLibrary meshLibrary;
    meshLibrary.scanFolder("assets/meshes");
    for (const auto& mesh : meshLibrary.all()) {
        m_renderer.uploadMesh(mesh);
    }

    // ── Generate + upload terrain ──
    m_terrain.bind(&openGLRenderer);
    m_terrain.setHeightScale(8.0f);
    m_terrain.setMapScale(2.0f);
    m_terrain.generate(256, 0.5f, 10.0f);
    m_terrain.upload();

    // Create ONE entity for the terrain
    m_scene.create()
        .setPosition(0, 0, 0)
        .setMesh(m_terrain.handle())
        .setColor(1, 1, 1, 1)
        .build();

    // ── Create the PLAYER entity ──
    float startX = 0.0f;
    float startZ = 0.0f;
    float startY = m_terrain.getHeight(startX, startZ) + 15.0f;

    m_scene.create()
        .setPosition(startX, startY, startZ)
        .setMesh(0)
        .setColor(0, 1, 0, 1)
        .makePlayer(8.0f, 5.0f)
        .addGravity(9.8f, 1.0f)
        .addCollider(1.0f, 0.0f)
        .build();

    std::cout << "Player created at (" << startX << ", " << startY << ", "
              << startZ << ") - falling to terrain\n";

    // ── Wire physics to terrain height ──
    m_physics.setTerrainHeightFn(
        [this](float wx, float wz) -> float {
            return m_terrain.getHeight(wx, wz);
        });

    // ── Create RenderPipeline ──
    m_pipeline = std::make_unique<RenderPipeline>(
        openGLRenderer, m_camera, m_renderSystem, m_scene.internal(), window);

    // ── Init ImGui AFTER renderer (needs GLAD loaded) ──
    window.handle.setActive(true);
    if (!ImGui::SFML::Init(window.handle)) {
        std::cerr << "Engine::init: ImGui init failed\n";
        return false;
    }
    std::cout << "ImGui initialized successfully\n";

    m_initialized = true;
    return true;
}

void Engine::run()
{
    if (!m_initialized) return;

    GameLoop loop;
    loop.init(1.0f / 60.0f);

    // Input: camera reads mouse (only in Edit mode)
    loop.addInputUpdate([this](float /*dt*/) {
        if (m_mode == EngineMode::Edit) {
            m_camera.handleInput(*m_input);
        }
    }, *m_window);

    // Physics: gravity + velocity integration (fixed timestep)
    loop.addFixedUpdate([this](float dt) {
        m_physics.step(m_scene.internal(), dt);
    });

    // Render: 3D scene + UI + swap
    loop.addRender([this](float /*alpha*/) {
        // Render 3D scene
        m_pipeline->render(0.016f);

        // ImGui UI
        ImGui::SFML::Update(m_window->handle, sf::seconds(0.016f));
        ImGui::Begin("Engine");
        ImGui::Text("Mode: %s", m_mode == EngineMode::Edit ? "Edit" : "Play");
        ImGui::Text("Entities: %u", m_scene.entityCount());
        ImGui::Text("Press F5 to toggle Edit/Play");
        ImGui::End();
        ImGui::SFML::Render();

        // Reset framebuffer binding for next frame
        glBindFramebuffer(GL_FRAMEBUFFER, 0);

        m_window->handle.display();
    });

    // F5 toggles Edit/Play mode
    loop.addUpdate([this](float /*dt*/) {
        if (m_input->isKeyPressed(KeyCode::F5)) {
            setMode(m_mode == EngineMode::Edit ? EngineMode::Play : EngineMode::Edit);
        }
    });

    loop.run(*m_window, *m_input);
}

void Engine::shutdown()
{
    if (!m_initialized) return;
    ImGui::SFML::Shutdown();
    m_initialized = false;
}

void Engine::setMode(EngineMode mode)
{
    if (mode == m_mode) return;
    m_mode = mode;

    if (mode == EngineMode::Play) {
        m_window->handle.setMouseCursorVisible(false);
        std::cout << "EngineMode: Play\n";
    } else {
        m_window->handle.setMouseCursorVisible(true);
        std::cout << "EngineMode: Edit\n";
    }
}