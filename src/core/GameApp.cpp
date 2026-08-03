#include "core/GameApp.h"
#include "core/GameLoop.h"
#include <iostream>

// ── Constructor / Destructor ──
GameApp::GameApp(Window& window)
    : m_window(window)
{
}

GameApp::~GameApp()
{
    shutdown();
}

// ── Lifecycle: init ──
// Moves all setupEngine() logic here: renderer, input, camera, terrain, player.
bool GameApp::init()
{
    EngineContext& ctx = m_ctx;

    // Create renderer
    static IOpenGLRenderer openGLRenderer(m_window.handle);
    ctx.renderer = &openGLRenderer;

    // Create input
    static SFMLInputSystem sfmlInput;
    ctx.input = &sfmlInput;

    // Init renderer
    if (!ctx.renderer->init()) {
        std::cerr << "Renderer init failed\n";
        return false;
    }

    // Setup camera
    ctx.camera.target = {0.0f, 0.0f, 0.0f};
    ctx.camera.distance = 35.0f;
    ctx.camera.yaw = -135.0f;
    ctx.camera.pitch = 54.0f;

    // Load all 3D models from assets/meshes/ into MeshLibrary (CPU side)
    ctx.meshLibrary.scanFolder("assets/meshes");

    // Upload each parsed mesh to the GPU (once at startup)
    for (const auto& mesh : ctx.meshLibrary.all()) {
        ctx.renderer->uploadMesh(mesh);
    }

    // Generate SMOOTH terrain (one indexed mesh, not 4225 cubes)
    ctx.terrain.terrainConfig.heightScale = ctx.heightScale;
    MeshData terrainMesh = ctx.terrain.generateSmoothTerrain(
        ctx.gridSize, ctx.roughness, ctx.amplitude);

    // Scale terrain vertices by mapScale (bigger map)
    for (size_t i = 0; i < terrainMesh.vertices.size(); i += 8) {
        terrainMesh.vertices[i]     *= ctx.mapScale;  // x
        // vertices[i+1] = y (height, keep as-is)
        terrainMesh.vertices[i + 2] *= ctx.mapScale;  // z
    }

    // Upload the terrain mesh to GPU
    ctx.terrainHandle = ctx.renderer->uploadMesh(terrainMesh);

    // Create ONE entity for the entire terrain
    LegacyEntityBuilder builder(ctx.em);
    builder()
        .with(CTransform{0, 0, 0, 0, 1, 1, 1})
        .with(CRenderable{ctx.terrainHandle, -1, {1, 1, 1, 1}})
        .build();

    std::cout << "Generated smooth terrain as 1 entity (GPU handle "
              << ctx.terrainHandle << ")\n";

    // ── Create the PLAYER entity ──
    float startX = 0.0f;
    float startZ = 0.0f;
    float startY = ctx.terrain.getWorldHeight(startX, startZ) + 15.0f;

    ctx.playerEntity = builder()
        .with(CTransform{startX, startY, startZ, 0, 1, 1, 1})
        .with(CRenderable{0, -1, {0.0f, 1.0f, 0.0f, 1.0f}})  // green cube
        .with(CPlayer{8.0f, 5.0f})
        .with(CGravity{9.8f, 1.0f})
        .with(CVelocity{})
        .with(CCollider{1.0f, 0.0f})
        .build();

    std::cout << "Player created at (" << startX << ", " << startY << ", "
              << startZ << ") - falling to terrain\n";

    // ── Wire collision system to the terrain heightmap ──
    ctx.collisionSystem.setHeightFunction(
        [&ctx](float wx, float wz) -> float {
            return ctx.terrain.getWorldHeight(wx / ctx.mapScale, wz / ctx.mapScale);
        });

    // ── Create RenderPipeline (owns the render step order) ──
    static RenderPipeline pipeline(*ctx.renderer, ctx.camera, ctx.renderSystem, ctx.em, m_window);
    ctx.renderPipeline = &pipeline;

    // ── Init ImGui AFTER renderer (needs GLAD loaded) ──
    m_window.handle.setActive(true);
    if (!ImGui::SFML::Init(m_window.handle)) {
        std::cerr << "ImGui init failed!\n";
        return false;
    }
    std::cout << "ImGui initialized successfully\n";

    m_initialized = true;
    return true;
}

// ── Lifecycle: run ──
// Moves the main loop here. Blocks until window closes.
void GameApp::run()
{
    if (!m_initialized) return;

    EngineContext& ctx = m_ctx;
    GameLoop loop;
    loop.init(1.0f / 60.0f);

    registerSystems(loop);

    loop.run(m_window, *ctx.input);
}

// ── Lifecycle: shutdown ──
void GameApp::shutdown()
{
    if (!m_initialized) return;
    m_ctx.renderer->shutdown();
    ImGui::SFML::Shutdown();
    m_initialized = false;
}

// ── Mode switching (Edit vs Play) ──
void GameApp::setMode(EngineMode mode)
{
    if (mode == m_mode) return;
    m_mode = mode;

    if (mode == EngineMode::Play) {
        // Hide ImGui, lock mouse, start game logic
        m_window.handle.setMouseCursorVisible(false);
        std::cout << "EngineMode: Play\n";
    } else {
        // Show ImGui, unlock mouse, freeze game logic
        m_window.handle.setMouseCursorVisible(true);
        std::cout << "EngineMode: Edit\n";
    }
}

// ── Terrain regeneration (in-place, no new handle) ──
// Uses renderer->updateMesh() so the editor can sculpt without
// recreating the GPU handle.
void GameApp::regenerateTerrain()
{
    EngineContext& ctx = m_ctx;
    ctx.terrain.terrainConfig.heightScale = ctx.heightScale;
    MeshData newTerrain = ctx.terrain.generateSmoothTerrain(
        ctx.gridSize, ctx.roughness, ctx.amplitude);

    // Scale terrain vertices by mapScale
    for (size_t i = 0; i < newTerrain.vertices.size(); i += 8) {
        newTerrain.vertices[i]     *= ctx.mapScale;
        newTerrain.vertices[i + 2] *= ctx.mapScale;
    }

    // Update the existing GPU buffer in-place (keeps the same handle)
    ctx.renderer->updateMesh(ctx.terrainHandle, newTerrain);
    std::cout << "Terrain regenerated in-place (handle " << ctx.terrainHandle << ")\n";
}

// ── UI: game-specific (FPS, player health) — shown in both modes ──
void GameApp::renderGameUI()
{
    EngineContext& ctx = m_ctx;
    ImGui::Begin("Game");
    ImGui::Text("FPS: %d", ctx.fps);
    ImGui::Text("Entities: %d", ctx.em.getNextID());
    ImGui::Text("Mode: %s", m_mode == EngineMode::Edit ? "Edit" : "Play");
    ImGui::Text("Press F5 to toggle Edit/Play");
    ImGui::End();
}

// ── UI: editor-specific (terrain controls, mesh library) — Edit mode only ──
void GameApp::renderEditorUI()
{
    EngineContext& ctx = m_ctx;

    ImGui::Begin("Terrain Generator");

    // Terrain controls
    ImGui::Text("Terrain Parameters");
    ImGui::SliderInt("Grid Size", &ctx.gridSize, 10, 256);
    ImGui::SliderFloat("Roughness", &ctx.roughness, 0.0f, 1.0f);
    ImGui::SliderFloat("Amplitude", &ctx.amplitude, 1.0f, 20.0f);
    ImGui::SliderFloat("Height Scale", &ctx.heightScale, 1.0f, 20.0f);

    if (ImGui::Button("Regenerate Terrain")) {
        regenerateTerrain();
    }

    ImGui::Separator();

    // Camera controls
    ImGui::Text("Camera");
    ImGui::SliderFloat("Distance", &ctx.camera.distance, 10.0f, 100.0f);
    ImGui::SliderFloat("Yaw", &ctx.camera.yaw, -180.0f, 180.0f);
    ImGui::SliderFloat("Pitch", &ctx.camera.pitch, -89.0f, 89.0f);

    // Mesh Library Panel
    ImGui::Separator();
    ImGui::Text("Mesh Library");
    ImGui::Text("Loaded: %zu meshes", ctx.meshLibrary.count());
    for (int i = 0; i < (int)ctx.meshLibrary.count(); i++) {
        const auto& mesh = ctx.meshLibrary.get(i);
        ImGui::Text("  [%d] %s (%u verts)", i, mesh.name.c_str(), mesh.vertexCount);
    }

    // Entity Creator
    ImGui::Separator();
    ImGui::Text("Entity Creator");

    static int selectedMesh = 0;
    static float posX = 0, posY = 0, posZ = 0;
    static float scale = 1.0f;
    static float color[3] = {1, 1, 1};

    if (ctx.meshLibrary.count() > 0) {
        std::vector<const char*> meshNames;
        for (const auto& mesh : ctx.meshLibrary.all()) {
            meshNames.push_back(mesh.name.c_str());
        }
        ImGui::Combo("Mesh", &selectedMesh, meshNames.data(), (int)meshNames.size());

        ImGui::SliderFloat3("Position", &posX, -50, 50);
        ImGui::SliderFloat("Scale", &scale, 0.1f, 5.0f);
        ImGui::ColorEdit3("Color", color);

        if (ImGui::Button("Create Entity")) {
            LegacyEntityBuilder builder(ctx.em);
            uint32_t gpuHandle = (uint32_t)selectedMesh + 1;
            builder()
                .with(CTransform{posX, posY, posZ, 0, scale, scale, scale})
                .with(CRenderable{gpuHandle, -1, {color[0], color[1], color[2], 1}})
                .build();
            std::cout << "Created entity with mesh '"
                      << ctx.meshLibrary.get(selectedMesh).name
                      << "' (GPU handle " << gpuHandle << ")\n";
        }
    } else {
        ImGui::TextDisabled("No meshes loaded - add .obj files to assets/meshes/");
    }

    ImGui::End();
}

// ── System registration ──
void GameApp::registerSystems(GameLoop& loop)
{
    EngineContext& ctx = m_ctx;

    // Input: camera reads mouse (only in Edit mode; Play mode uses player input)
    loop.addInputUpdate([this](float /*dt*/) {
        EngineContext& c = m_ctx;
        if (m_mode == EngineMode::Edit) {
            c.camera.handleInput(*c.input);
        }
    }, m_window);

    // Physics: gravity + velocity integration (fixed timestep)
    loop.addFixedUpdate([&ctx](float dt) {
        ctx.physicsSystem.update(ctx.em, dt);
    });

    // Collision: clamp entities to terrain height
    loop.addFixedUpdate([&ctx](float /*dt*/) {
        ctx.collisionSystem.update(ctx.em);
    });

    // Render: 3D scene + UI + swap
    loop.addRender([this](float /*alpha*/) {
        EngineContext& c = m_ctx;
        // FPS counter
        c.frameCount++;
        c.fpsTime += 0.016f;
        if (c.fpsTime >= 1.0f) {
            c.fps = c.frameCount;
            c.frameCount = 0;
            c.fpsTime = 0.0f;
        }

        // Render 3D scene
        c.renderPipeline->render(0.016f);

        // ImGui UI
        ImGui::SFML::Update(m_window.handle, sf::seconds(0.016f));
        renderGameUI();
        if (m_mode == EngineMode::Edit) {
            renderEditorUI();
        }
        ImGui::SFML::Render();

        // Reset framebuffer binding for next frame
        glBindFramebuffer(GL_FRAMEBUFFER, 0);

        m_window.handle.display();
    });

    // F5 toggles Edit/Play mode
    loop.addUpdate([this](float /*dt*/) {
        if (m_ctx.input->isKeyPressed(KeyCode::F5)) {
            setMode(m_mode == EngineMode::Edit ? EngineMode::Play : EngineMode::Edit);
        }
    });
}
