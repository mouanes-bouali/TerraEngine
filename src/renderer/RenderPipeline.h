#pragma once
#include "renderer/IRenderer.h"
#include "systems/RenderSystem.h"
#include "systems/CameraController.h"
#include "entities/EntityManager.h"
#include "platform/Window.h"
#include <glad/glad.h>

class RenderPipeline {
public:
    RenderPipeline(IRenderer& renderer, CameraController& camera, 
                   RenderSystem& renderSystem, EntityManager& em,
                   Window& window)
        : m_renderer(renderer)
        , m_camera(camera)
        , m_renderSystem(renderSystem)
        , m_em(em)
        , m_window(window)
    {}
    
    // Called every frame — runs ALL render steps in order
    void render(float dt) {
        // Step 1: Update camera position from yaw/pitch/distance
        m_camera.update(dt);
        
        // Step 2: Tell renderer where the camera is
        m_renderer.setCamera(m_camera.position, m_camera.target, m_camera.up);
        m_renderer.setProjection(45.0f, 1280.0f / 720.0f, 0.1f, 1000.0f);
        
        // Step 3: Read ECS components, build render list
        m_renderCtx.opaque.clear();
        m_renderSystem.collect(m_em, m_renderCtx);
        
        // Step 4: Clear screen + draw everything in one call
        m_renderer.beginFrame();
        m_renderer.drawInstanced(m_renderCtx.opaque);
        
        // Step 5: Reset OpenGL state for UI
        glDisable(GL_DEPTH_TEST);
        glUseProgram(0);
        glBindVertexArray(0);
        glBindTexture(GL_TEXTURE_2D, 0);
        glActiveTexture(GL_TEXTURE0);
    }
    
    RenderContext& getRenderContext() { return m_renderCtx; }
    
private:
    IRenderer& m_renderer;
    CameraController& m_camera;
    RenderSystem& m_renderSystem;
    EntityManager& m_em;
    Window& m_window;
    RenderContext m_renderCtx;
};