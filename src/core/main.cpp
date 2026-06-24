#define SDL_MAIN_HANDLED 
#include "platform/Window.h"
#include "core/GameLoop.h"
#include <SDL2/SDL_opengl.h>

static Window* g_window = nullptr;

void renderScene(float alpha) {
    g_window->clear();
    g_window->display();
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