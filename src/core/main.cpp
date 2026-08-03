#include "platform/Window.h"
#include "api/Engine.h"
#include <iostream>

// ── Main Entry Point ──
// Pure glue: create window, init the Engine facade, run.
// All engine logic lives behind Engine::get() — game code never
// touches ECS, renderer, or systems directly.
int main() {
    try {
        // 1. Create window
        sf::ContextSettings settings;
        settings.depthBits = 24;
        settings.majorVersion = 3;
        settings.minorVersion = 3;
        settings.attributeFlags = sf::ContextSettings::Default;
        Window window("Solum Engine", 1280, 720, settings);

        // 2. Initialize the engine facade
        auto& engine = Engine::get();
        if (!engine.init(window)) {
            std::cerr << "Engine init failed\n";
            return -1;
        }

        // 3. Run the game (blocks until window closes)
        engine.run();

        // 4. Cleanup
        engine.shutdown();
        return 0;

    } catch (const std::exception& e) {
        std::cerr << "Fatal error: " << e.what() << std::endl;
        return -1;
    }
}