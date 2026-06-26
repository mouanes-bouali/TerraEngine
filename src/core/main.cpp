#include "platform/Window.h"
#include "core/GameLoop.h"

static Window* g_window = nullptr;

void renderScene(float alpha) {
    if (g_window) {
        g_window->handle.clear(sf::Color(50, 50, 50));
        // Render your scene here
        g_window->handle.display();
    }
}

int main() {
    Window window("Solum", 1280, 720);
    g_window = &window;

    GameLoop loop;
    loop.init(1.0f / 60.0f);
    loop.addRender(renderScene);
    loop.run(window);
    return 0;
}