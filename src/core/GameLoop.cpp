#include "GameLoop.h"
#include "platform/Window.h"
#include <SDL2/SDL.h>

struct Window; 
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
    static Uint64 last = 0;
    Uint64 now = SDL_GetPerformanceCounter();
    float dt = (float)(now - last) / SDL_GetPerformanceFrequency();
    last = now;
    if (dt > 0.25f) dt = 0.25f;   // matches your clamp
    return dt;
}

void GameLoop::run(Window& window) {
    while (window.isOpen() && running) {
        // 1. Handle input (also updates window events)
        window.pollEvents();

        // 2. Measure time
        float dt = getDeltaTime();

        // 3. Variable-rate updates (called once per frame)
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

        // 6. Render callbacks (they do the clearing, drawing, displaying)
        for (int i = 0; i < renderCount; ++i)
            renderCallbacks[i](alpha);

        SDL_Delay(1);  // gentle on CPU
    }
}

void GameLoop::quit() {
    running = false;
}