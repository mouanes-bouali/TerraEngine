#pragma once
#include "platform/Window.h"
#include "platform/IInput.h"
#include "platform/SFMLInputSystem.h"
#include "renderer/IOpenGLRenderer.h"
#include "renderer/RenderPipeline.h"
#include "systems/RenderSystem.h"
#include "systems/CameraController.h"
#include "systems/TerrainGenerator.h"
#include "systems/PhysicsSystem.h"
#include "systems/CollisionSystem.h"
#include "entities/EntityManager.h"
#include "entities/EntityBuilder.h"
#include "assets/MeshLibrary.h"
#include <glad/glad.h>
#include "imgui/imgui.h"
#include "imgui/imgui-SFML.h"

// ── Engine Mode: Edit vs Play ──
enum class EngineMode { Edit, Play };

// ── Engine Context (owns all engine state) ──
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
    MeshHandle terrainHandle = 0;

    // Terrain parameters
    int gridSize = 256;
    float roughness = 0.5f;
    float amplitude = 10.0f;
    float heightScale = 8.0f;
    float mapScale = 2.0f;

    // FPS tracking
    int fps = 0;
    int frameCount = 0;
    float fpsTime = 0.0f;
};

// ── GameApp: the application layer that owns the engine ──
// Extracted from main.cpp so the Level Editor can be a separate
// "mode" (Edit vs Play) without living in the same file as game logic.
class GameApp {
public:
    explicit GameApp(Window& window);
    ~GameApp();

    // Lifecycle
    bool init();          // renderer, input, camera, terrain, player
    void run();           // main loop (blocks until window closes)
    void shutdown();

    // Mode switching (Edit vs Play)
    void setMode(EngineMode mode);
    EngineMode getMode() const { return m_mode; }

    // Accessors for the Editor layer
    EngineContext& context() { return m_ctx; }
    Window& window() { return m_window; }

    // Regenerate terrain in-place (uses updateMesh, no new handle)
    void regenerateTerrain();

private:
    // UI: game-specific (FPS, player health) — shown in both modes
    void renderGameUI();
    // UI: editor-specific (terrain controls, mesh library) — Edit mode only
    void renderEditorUI();

    // System registration
    void registerSystems(GameLoop& loop);

    Window& m_window;
    EngineContext m_ctx;
    EngineMode m_mode = EngineMode::Edit;
    bool m_initialized = false;
};