#include "platform/Window.h"
#include "core/GameLoop.h"
#include "main.h"
#include "renderer/IOpenGLRenderer.h"
#include "renderer/RenderPipeline.h"
#include "platform/IInput.h"
#include "platform/SFMLInputSystem.h"
#include "systems/RenderSystem.h"
#include "systems/CameraController.h"
#include "systems/TerrainGenerator.h"
#include "systems/PhysicsSystem.h"
#include "systems/CollisionSystem.h"
#include "entities/EntityManager.h"
#include "entities/EntityBuilder.h"
#include "assets/MeshLibrary.h"
#include <iostream>
#include <glad/glad.h>
#include "imgui/imgui.h"
#include "imgui/imgui-SFML.h"

// ── Engine Context (replaces globals) ──
struct EngineContext {
    IRenderer* renderer = nullptr;
    IInput* input = nullptr;
    EntityManager em;
    RenderSystem renderSystem;
    PhysicsSystem physicsSystem;
    CollisionSystem collisionSystem;
    CameraController camera;
    TerrainGenerator terrain;
    MeshLibrary meshLibrary;
    RenderPipeline* renderPipeline = nullptr;
    EntityID playerEntity = INVALID_ENTITY;
    
    // Terrain parameters
    int gridSize = 256;           // higher resolution = more vertices (less blurry)
    float roughness = 0.5f;
    float amplitude = 10.0f;
    float heightScale = 8.0f;
    float mapScale = 2.0f;        // scale terrain vertices (2x bigger map)
    
    // FPS tracking
    int fps = 0;
    int frameCount = 0;
    float fpsTime = 0.0f;
};

// ── Step 1: Create Window ──
Window createWindow() {
    sf::ContextSettings settings;
    settings.depthBits = 24;
    settings.majorVersion = 3;
    settings.minorVersion = 3;
    settings.attributeFlags = sf::ContextSettings::Default;
    return Window("Solum Engine", 1280, 720, settings);
}

// ── Step 2: Setup Engine (renderer, input, camera, terrain) ──
bool setupEngine(EngineContext& ctx, Window& window) {
    // Create renderer
    static IOpenGLRenderer openGLRenderer(window.handle);
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
    MeshHandle terrainHandle = ctx.renderer->uploadMesh(terrainMesh);
    
    // Create ONE entity for the entire terrain
    EntityBuilder builder(ctx.em);
    builder()
        .with(CTransform{0, 0, 0, 0, 1, 1, 1})
        .with(CRenderable{terrainHandle, -1, {1, 1, 1, 1}})
        .build();
    
    std::cout << "Generated smooth terrain as 1 entity (GPU handle " 
              << terrainHandle << ")\n";
    
    // ── Create the PLAYER entity ──
    // Starts high above the terrain, falls via gravity, lands via collision.
    float startX = 0.0f;
    float startZ = 0.0f;
    float startY = ctx.terrain.getWorldHeight(startX, startZ) + 15.0f;  // 15 units up
    
    ctx.playerEntity = builder()
        .with(CTransform{startX, startY, startZ, 0, 1, 1, 1})
        .with(CRenderable{0, -1, {0.0f, 1.0f, 0.0f, 1.0f}})  // green cube
        .with(CPlayer{8.0f, 5.0f})   // jump speed, move speed
        .with(CGravity{9.8f, 1.0f})  // gravity pulls down
        .with(CVelocity{})           // velocity starts at zero
        .with(CCollider{1.0f, 0.0f}) // sphere collider radius 0.5
        .build();
    
    std::cout << "Player created at (" << startX << ", " << startY << ", " 
              << startZ << ") - falling to terrain\n";
    
    // ── Wire collision system to the terrain heightmap ──
    // Note: terrain vertices are scaled by mapScale, so world coords
    // must be divided by mapScale before querying the heightmap.
    ctx.collisionSystem.setHeightFunction(
        [&ctx](float wx, float wz) -> float {
            return ctx.terrain.getWorldHeight(wx / ctx.mapScale, wz / ctx.mapScale);
        });
    
    return true;
}

// ── Step 3: Render UI (ImGui) ──
void renderUI(EngineContext& ctx, Window& window) {
    ImGui::SFML::Update(window.handle, sf::seconds(0.016f));
    
    ImGui::Begin("Terrain Generator");
    
    // Stats
    ImGui::Text("FPS: %d", ctx.fps);
    ImGui::Text("Entities: %d", ctx.em.getNextID());
    ImGui::Text("Rendered: %d", (int)ctx.renderPipeline->getRenderContext().opaque.size());
    
    ImGui::Separator();
    
    // Terrain controls
    ImGui::Text("Terrain Parameters");
    ImGui::SliderInt("Grid Size", &ctx.gridSize, 10, 100);
    ImGui::SliderFloat("Roughness", &ctx.roughness, 0.0f, 1.0f);
    ImGui::SliderFloat("Amplitude", &ctx.amplitude, 1.0f, 20.0f);
    ImGui::SliderFloat("Height Scale", &ctx.heightScale, 1.0f, 20.0f);
    
    if (ImGui::Button("Regenerate Terrain")) {
        // Generate new smooth terrain mesh
        ctx.terrain.terrainConfig.heightScale = ctx.heightScale;
        MeshData newTerrain = ctx.terrain.generateSmoothTerrain(
            ctx.gridSize, ctx.roughness, ctx.amplitude);
        
        // Upload to GPU (gets a new handle each time)
        MeshHandle newHandle = ctx.renderer->uploadMesh(newTerrain);
        
        // Replace the terrain entity with the new mesh
        EntityManager newEm;
        EntityBuilder builder(newEm);
        builder()
            .with(CTransform{0, 0, 0, 0, 1, 1, 1})
            .with(CRenderable{newHandle, -1, {1, 1, 1, 1}})
            .build();
        ctx.em = std::move(newEm);
    }
    
    ImGui::Separator();
    
    // Camera controls
    ImGui::Text("Camera");
    ImGui::SliderFloat("Distance", &ctx.camera.distance, 10.0f, 100.0f);
    ImGui::SliderFloat("Yaw", &ctx.camera.yaw, -180.0f, 180.0f);
    ImGui::SliderFloat("Pitch", &ctx.camera.pitch, -89.0f, 89.0f);
    
    // ── NEW: Mesh Library Panel ──
    ImGui::Separator();
    ImGui::Text("Mesh Library");
    ImGui::Text("Loaded: %zu meshes", ctx.meshLibrary.count());
    for (int i = 0; i < (int)ctx.meshLibrary.count(); i++) {
        const auto& mesh = ctx.meshLibrary.get(i);
        ImGui::Text("  [%d] %s (%u verts)", i, mesh.name.c_str(), mesh.vertexCount);
    }
    
    // ── NEW: Entity Creator ──
    ImGui::Separator();
    ImGui::Text("Entity Creator");
    
    static int selectedMesh = 0;
    static float posX = 0, posY = 0, posZ = 0;
    static float scale = 1.0f;
    static float color[3] = {1, 1, 1};
    
    if (ctx.meshLibrary.count() > 0) {
        // Build dropdown items from loaded mesh names
        std::vector<const char*> meshNames;
        for (const auto& mesh : ctx.meshLibrary.all()) {
            meshNames.push_back(mesh.name.c_str());
        }
        ImGui::Combo("Mesh", &selectedMesh, meshNames.data(), (int)meshNames.size());
        
        ImGui::SliderFloat3("Position", &posX, -50, 50);
        ImGui::SliderFloat("Scale", &scale, 0.1f, 5.0f);
        ImGui::ColorEdit3("Color", color);
        
        if (ImGui::Button("Create Entity")) {
            EntityBuilder builder(ctx.em);
            // GPU mesh handle = library index + 1, because index 0 on the
            // renderer is the built-in cube. uploadMesh() pushes after it.
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
    
    ImGui::SFML::Render();
    
    // Reset framebuffer binding for next frame
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

// ── Step 4: Render Scene (3D) via RenderPipeline ──
void renderScene(EngineContext& ctx) {
    ctx.renderPipeline->render(0.016f);
}

// ── Step 5: Update FPS Counter ──
void updateFPS(EngineContext& ctx) {
    ctx.frameCount++;
    ctx.fpsTime += 0.016f;
    if (ctx.fpsTime >= 1.0f) {
        ctx.fps = ctx.frameCount;
        ctx.frameCount = 0;
        ctx.fpsTime = 0.0f;
    }
}

// ── Step 6: Register Systems with GameLoop ──
void registerSystems(EngineContext& ctx, GameLoop& loop, Window& window) {
    // Input: camera reads mouse
    loop.addInputUpdate([&ctx](float /*dt*/) {
        ctx.camera.handleInput(*ctx.input);
    }, window);
    
    // Physics: gravity + velocity integration (fixed timestep for stability)
    loop.addFixedUpdate([&ctx](float dt) {
        ctx.physicsSystem.update(ctx.em, dt);
    });
    
    // Collision: clamp entities to terrain height (after physics moves them)
    loop.addFixedUpdate([&ctx](float /*dt*/) {
        ctx.collisionSystem.update(ctx.em);
    });
    
    // Render: 3D scene + UI + swap
    loop.addRender([&ctx, &window](float /*alpha*/) {
        updateFPS(ctx);
        renderScene(ctx);
        renderUI(ctx, window);
        window.handle.display();
    });
}

// ── Main Entry Point ──
int main() {
    try {
    // 1. Create window
    Window window = createWindow();
    
    // 2. Setup engine (renderer, input, camera, terrain)
    EngineContext ctx;
    if (!setupEngine(ctx, window)) {
        return -1;
    }
    
    // 3. Create RenderPipeline (owns the render step order)
    static RenderPipeline pipeline(*ctx.renderer, ctx.camera, ctx.renderSystem, ctx.em, window);
    ctx.renderPipeline = &pipeline;
    
    // 4. Init ImGui AFTER renderer (needs GLAD loaded)
    window.handle.setActive(true);
    if (!ImGui::SFML::Init(window.handle)) {
        std::cerr << "ImGui init failed!\n";
        return -1;
    }
    std::cout << "ImGui initialized successfully\n";
    
    // 4. Create game loop
    GameLoop loop;
    loop.init(1.0f / 60.0f);
    
    // 5. Register systems
    registerSystems(ctx, loop, window);
    
    // 6. Run the game (blocks until window closes)
    loop.run(window, *ctx.input);
    
    // 7. Cleanup
    ctx.renderer->shutdown();
    ImGui::SFML::Shutdown();
    return 0;
    
} catch (const std::exception& e) {
    std::cerr << "Fatal error: " << e.what() << std::endl;
    return -1;
}
}
