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
    if (updateCount < 32) updateCallbacks[updateCount++] = cb;
}

void GameLoop::addFixedUpdate(UpdateCallback cb) {
    if (fixedUpdateCount < 32) fixedUpdateCallbacks[fixedUpdateCount++] = cb;
}

void GameLoop::addRender(RenderCallback cb) {
    if (renderCount < 32) renderCallbacks[renderCount++] = cb;
}

static float getDeltaTime() {
    static sf::Clock clock;
    float dt = clock.restart().asSeconds();
    if (dt > 0.25f) dt = 0.25f;   // clamp to prevent spiral of death
    return dt;
}

void GameLoop::run(Window& window) {
    while (window.isOpen() && running) {

        // 1. Process SFML events (input, window close, resize, etc.)
        window.pollEvents();

        // 2. Measure frame time
        float dt = getDeltaTime();

        // 3. Variable-rate updates (once per frame)
        for (int i = 0; i < updateCount; ++i)
            updateCallbacks[i](dt);

        // 4. Fixed-timestep updates
        accumulator += dt;
        while (accumulator >= fixedDt) {
            for (int i = 0; i < fixedUpdateCount; ++i)
                fixedUpdateCallbacks[i](fixedDt);
            accumulator -= fixedDt;
        }

        // 5. Interpolation alpha
        float alpha = accumulator / fixedDt;

        // 6. Render callbacks – they clear, draw, and display
        for (int i = 0; i < renderCount; ++i)
            renderCallbacks[i](alpha);

        // Note: No need for SDL_Delay – SFML's display() already syncs
    }
}

void GameLoop::quit() {
    running = false;
}