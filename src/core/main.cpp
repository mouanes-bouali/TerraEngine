#include "platform/Window.h"
#include "core/GameApp.h"
#include <iostream>

// ── Main Entry Point ──
// Just glue: create window, init GameApp, run. All engine logic
// lives in GameApp (src/core/GameApp.cpp) so the Level Editor can
// be a separate layer on top.
int main() {
    try {
        // 1. Create window
        sf::ContextSettings settings;
        settings.depthBits = 24;
        settings.majorVersion = 3;
        settings.minorVersion = 3;
        settings.attributeFlags = sf::ContextSettings::Default;
        Window window("Solum Engine", 1280, 720, settings);

        // 2. Initialize the application layer
        GameApp game(window);
        if (!game.init()) {
            std::cerr << "GameApp init failed\n";
            return -1;
        }

        // 3. Run the game (blocks until window closes)
        game.run();

        // 4. Cleanup
        game.shutdown();
        return 0;

    } catch (const std::exception& e) {
        std::cerr << "Fatal error: " << e.what() << std::endl;
        return -1;
    }
}