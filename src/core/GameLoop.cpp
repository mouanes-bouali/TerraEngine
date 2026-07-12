#include "GameLoop.h"
#include "platform/Window.h"
#include "platform/IInput.h"
#include <SFML/Graphics.hpp>
#include "imgui/imgui.h"
#include "imgui/imgui-SFML.h"

void GameLoop::init(float dt) {
    fixedDt = dt;
    accumulator = 0.0f;
    running = true;
    updateCount = 0;
    inputUpdateCount = 0;
    fixedUpdateCount = 0;
    renderCount = 0;
    imguiCount = 0;
}

void GameLoop::addUpdate(UpdateCallback cb) {
    if (updateCount < 8) updateCallbacks[updateCount++] = cb;
}
void GameLoop::addInputUpdate(UpdateCallback cb, Window& window) {
    if (inputUpdateCount < 8) inputUpdateCallbacks[inputUpdateCount++] = cb;
}

void GameLoop::addFixedUpdate(UpdateCallback cb) {
    if (fixedUpdateCount < 8) fixedUpdateCallbacks[fixedUpdateCount++] = cb;
}

void GameLoop::addRender(RenderCallback cb) {
    if (renderCount < 8) renderCallbacks[renderCount++] = cb;
}

void GameLoop::addImGuiUpdate(UpdateCallback cb) {
    if (imguiCount < 8) imguiCallbacks[imguiCount++] = cb;
}

static float getDeltaTime() {
    static sf::Clock clock;
    float dt = clock.restart().asSeconds();
    if (dt > 0.25f) dt = 0.25f;
    return dt;
}

void GameLoop::run(Window& window, IInput& inputSystem) {
    while (window.isOpen() && running) {
        window.pollEvents(inputSystem);
        float dt = getDeltaTime();

        // Variable-rate updates
        for (int i = 0; i < updateCount; ++i)
            updateCallbacks[i](dt);

        // Input updates
        for (int i = 0; i < inputUpdateCount; ++i)
            inputUpdateCallbacks[i](dt);

        // Fixed-timestep updates
        accumulator += dt;
        while (accumulator >= fixedDt) {
            for (int i = 0; i < fixedUpdateCount; ++i)
                fixedUpdateCallbacks[i](fixedDt);
            accumulator -= fixedDt;
        }

        // Render (draws to back buffer)
        float alpha = accumulator / fixedDt;
        for (int i = 0; i < renderCount; ++i)
            renderCallbacks[i](alpha);
    }
}

void GameLoop::quit() {
    running = false;
}