#include "platform/Window.h"
#include "core/GameLoop.h"
#include "main.h"
#include "renderer/IOpenGLRenderer.h"
#include "platform/IInput.h"
#include "platform/SFMLInputSystem.h"
#include "systems/RenderSystem.h"
#include "systems/CameraSystem.h"
#include "systems/TerrainSystem.h"
#include "entities/EntityManager.h"
#include "entities/EntityBuilder.h"
#include <iostream>
#include "imgui/imgui.h"
#include "imgui/imgui-SFML.h"

static IRenderer *g_renderer = nullptr;
static IInput *g_input = nullptr;
static EntityManager g_em;
static RenderSystem g_renderSystem;
static RenderContext g_renderCtx;
static CameraSystem g_camera;
static TerrainSystem g_terrain;

// Terrain parameters (exposed to UI)
static int g_gridSize = 100;
static float g_roughness = 0.5f;
static float g_amplitude = 10.0f;
static float g_heightScale = 8.0f;

// FPS tracking
static int g_fps = 0;
static int g_frameCount = 0;
static float g_fpsTime = 0.0f;

int main()
{
    sf::ContextSettings settings;
    settings.depthBits = 24;
    settings.majorVersion = 3;
    settings.minorVersion = 3;
    settings.attributeFlags = sf::ContextSettings::Core;

    Window window("Engine", 1280, 720, settings);

    // ── Renderer (polymorphic via IRenderer interface) ──
    IOpenGLRenderer openGLRenderer(window.handle);
    IRenderer &renderer = openGLRenderer;
    g_renderer = &renderer;

    // ── Input (polymorphic via IInput interface) ──
    SFMLInputSystem sfmlInput;
    IInput &input = sfmlInput;
    g_input = &input;

    ConfigData emptyConfig = {1280, 720, "Arial", 16, sf::Color::White, {}, 60};

    if (!renderer.init())
    {
        std::cerr << "Renderer init failed\n";
        return -1;
    }

    // ── Setup camera (isometric-like view) ──
    g_camera.target = {0.0f, 0.0f, 0.0f};
    g_camera.distance = 35.0f;
    g_camera.yaw = -135.0f;   // diagonal view
    g_camera.pitch = -45.0f;  // 45° angle

    // ── Generate initial terrain ──
    g_terrain.config.gridSize = g_gridSize;
    g_terrain.config.heightScale = g_heightScale;
    EntityBuilder builder(g_em);
    g_terrain.generateSquareDiamondTerrain(g_em, builder, 0, g_gridSize, g_roughness, g_amplitude);
    std::cout << "Generated " << g_em.getNextID() << " initial terrain entities\n";

    GameLoop loop;
    loop.init(1.0f / 60.0f);

    // Input update (process camera input)
    loop.addInputUpdate([](float /*dt*/)
                        { g_camera.handleInput(*g_input); }, window);

    // Initialize ImGui BEFORE starting loop
    ImGui::SFML::Init(window.handle);

    // Render update
    loop.addRender([&](float /*alpha*/)
                   {
        // FPS counter
        g_frameCount++;
        g_fpsTime += 0.016f;
        if (g_fpsTime >= 1.0f) {
            g_fps = g_frameCount;
            g_frameCount = 0;
            g_fpsTime = 0.0f;
        }
        
        // 1. Update camera
        g_camera.update(g_em, 0.016f);
        
        // 2. Set camera on renderer
        g_renderer->setCamera(g_camera.position, g_camera.target, g_camera.up);
        g_renderer->setProjection(45.0f, 1280.0f / 720.0f, 0.1f, 1000.0f);
        
        // 3. Clear render context
        g_renderCtx.opaque.clear();
        
        // 4. Collect renderables from ECS
        g_renderSystem.collect(g_em, g_renderCtx);
        
        // Debug: check if we have instances
        static int lastCount = -1;
        if (g_renderCtx.opaque.size() != lastCount) {
            std::cout << "Rendering " << g_renderCtx.opaque.size() << " instances\n";
            lastCount = g_renderCtx.opaque.size();
        }
        
        // 5. Clear screen and draw terrain
        g_renderer->beginFrame();
        g_renderer->drawInstanced(g_renderCtx.opaque);
        
        // 6. ImGui: update, build UI, render
        ImGui::SFML::Update(window.handle, sf::seconds(0.016f));
        
        ImGui::Begin("Terrain Generator");
        ImGui::Text("FPS: %d", g_fps);
        ImGui::Text("Entities: %d", g_em.getNextID());
        ImGui::Text("Rendered: %d", (int)g_renderCtx.opaque.size());
        ImGui::Separator();
        ImGui::Text("Terrain Parameters");
        ImGui::SliderInt("Grid Size", &g_gridSize, 10, 200);
        ImGui::SliderFloat("Roughness", &g_roughness, 0.0f, 1.0f);
        ImGui::SliderFloat("Amplitude", &g_amplitude, 1.0f, 20.0f);
        ImGui::SliderFloat("Height Scale", &g_heightScale, 1.0f, 20.0f);
        if (ImGui::Button("Regenerate Terrain")) {
            g_renderCtx.opaque.clear();
            g_terrain.config.gridSize = g_gridSize;
            g_terrain.config.heightScale = g_heightScale;
            EntityManager newEm;
            EntityBuilder builder(newEm);
            g_terrain.generateSquareDiamondTerrain(newEm, builder, 0, g_gridSize, g_roughness, g_amplitude);
            g_em = std::move(newEm);
        }
        ImGui::Separator();
        ImGui::Text("Camera");
        ImGui::SliderFloat("Distance", &g_camera.distance, 10.0f, 100.0f);
        ImGui::SliderFloat("Yaw", &g_camera.yaw, -180.0f, 180.0f);
        ImGui::SliderFloat("Pitch", &g_camera.pitch, -89.0f, 89.0f);
        ImGui::End();
        
        ImGui::SFML::Render(window.handle);
        
        // 7. Swap buffers
        window.handle.display();
    });

    loop.run(window, input);

    renderer.shutdown();
    ImGui::SFML::Shutdown();
    return 0;
}