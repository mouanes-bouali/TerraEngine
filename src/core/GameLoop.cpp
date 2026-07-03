#include "GameLoop.h"
#include "platform/Window.h"
#include <SFML/Graphics.hpp>

void GameLoop::init(float dt) {
    fixedDt = dt;
    accumulator = 0.0f;
    running = true;
    updateCount = 0;
    fixedUpdateCount = 0;
    renderCount = 0;
}

void GameLoop::addUpdate(UpdateCallback cb) {
    if (updateCount < 8) updateCallbacks[updateCount++] = cb;
}

void GameLoop::addFixedUpdate(UpdateCallback cb) {
    if (fixedUpdateCount < 8) fixedUpdateCallbacks[fixedUpdateCount++] = cb;
}

void GameLoop::addRender(RenderCallback cb) {
    if (renderCount < 8) renderCallbacks[renderCount++] = cb;
}

static float getDeltaTime() {
    static sf::Clock clock;
    float dt = clock.restart().asSeconds();
    if (dt > 0.25f) dt = 0.25f;
    return dt;
}

void GameLoop::run(Window& window) {
    while (window.isOpen() && running) {
        window.pollEvents();
        float dt = getDeltaTime();

        // Variable-rate updates
        for (int i = 0; i < updateCount; ++i)
            updateCallbacks[i](dt);

        // Fixed-timestep updates
        accumulator += dt;
        while (accumulator >= fixedDt) {
            for (int i = 0; i < fixedUpdateCount; ++i)
                fixedUpdateCallbacks[i](fixedDt);
            accumulator -= fixedDt;
        }

        // Render
        float alpha = accumulator / fixedDt;
        for (int i = 0; i < renderCount; ++i)
            renderCallbacks[i](alpha);
    }
}

void GameLoop::quit() {
    running = false;
}