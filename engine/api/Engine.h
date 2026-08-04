#pragma once
#include "api/EngineMode.h"
#include "api/Scene.h"
#include "api/TerrainAPI.h"
#include "api/RendererAPI.h"
#include "api/PhysicsAPI.h"
#include "platform/Window.h"
#include "core/GameLoop.h"
#include "systems/CameraController.h"
#include "systems/RenderSystem.h"
#include "renderer/RenderPipeline.h"
#include "ui/EntityCreatorPanel.h"
#include "ui/InspectorPanel.h"
#include "ui/EntityListPanel.h"
#include "systems/SelectionSystem.h"
#include <memory>

// ─────────────────────────────────────────────────────────────────────
// Engine — the singleton facade. The ONLY entry point for game code.
//
//   auto& engine = Engine::get();
//   engine.init(window);
//
//   auto player = engine.scene().create()
//       .setPosition(0, 10, 0)
//       .setMesh(0)
//       .setColor(0, 1, 0, 1)
//       .makePlayer(8.0f, 5.0f)
//       .build();
//
//   engine.terrain().setHeightScale(10.0f).generate(256, 0.5f, 10.0f).upload();
//   engine.run();
// ─────────────────────────────────────────────────────────────────────
class Engine {
public:
    static Engine& get();

    // ── Lifecycle ──
    bool init(Window& window);
    void run();
    void shutdown();

    // ── API Accessors ──
    Scene& scene() { return m_scene; }
    TerrainAPI& terrain() { return m_terrain; }
    RendererAPI& renderer() { return m_renderer; }
    PhysicsAPI& physics() { return m_physics; }

    // ── Mode ──
    void setMode(EngineMode mode);
    EngineMode getMode() const { return m_mode; }

    // ── Internal (engine-only) ──
    Window& window() { return *m_window; }
    CameraController& camera() { return m_camera; }
    RenderSystem& renderSystem() { return m_renderSystem; }
    RenderPipeline& pipeline() { return *m_pipeline; }
    IInput& input() { return *m_input; }

private:
    Engine() = default;
    ~Engine();
    Engine(const Engine&) = delete;
    Engine& operator=(const Engine&) = delete;

    // API layers
    Scene m_scene;
    TerrainAPI m_terrain;
    RendererAPI m_renderer;
    PhysicsAPI m_physics;

    // Engine internals
    Window* m_window = nullptr;
    IInput* m_input = nullptr;
    CameraController m_camera;
    RenderSystem m_renderSystem;
    std::unique_ptr<RenderPipeline> m_pipeline;
    EngineMode m_mode = EngineMode::Edit;
    bool m_initialized = false;

    // Editor panels (Edit mode only)
    EntityCreatorPanel m_entityCreator;
    InspectorPanel m_inspector;
    EntityListPanel m_entityList;
    SelectionSystem m_selection;
};
