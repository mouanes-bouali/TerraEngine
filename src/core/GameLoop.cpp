#include "GameLoop.h"
#include "Window.h"

int main() {
    Window window("Solum", 1280, 720);

    const float FIXED_TIMESTEP = 1.0f / 60.0f;
    float accumulator = 0.0f;
    float lastTime    = SDL_GetTicks() / 1000.0f;

    while (window.isOpen()) {
        // -- measure real time --
        float currentTime = SDL_GetTicks() / 1000.0f;
        float deltaTime   = currentTime - lastTime;
        lastTime          = currentTime;

        // -- clamp: if frame took >250ms (debugger pause etc), don't spiral --
        if (deltaTime > 0.25f) deltaTime = 0.25f;

        accumulator += deltaTime;

        // -- fixed update (physics, AI, game logic) --
        window.pollEvents();
        while (accumulator >= FIXED_TIMESTEP) {
            // PhysicsUpdate(FIXED_TIMESTEP);
            // AIUpdate(FIXED_TIMESTEP);
            accumulator -= FIXED_TIMESTEP;
        }

        // -- interpolation factor for smooth rendering --
        float alpha = accumulator / FIXED_TIMESTEP;

        // -- render --
        window.clear();
        // Render(alpha);
        window.display();

        SDL_Delay(1);
    }

    return 0;
}