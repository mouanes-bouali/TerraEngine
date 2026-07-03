#include "platform/Window.h"
#include "core/GameLoop.h"
#include "main.h"
#include "renderer/IOpenGLRenderer.h"
#include <iostream>
#include "imgui/imgui.h"
#include "imgui/imgui-SFML.h"

static IOpenGLRenderer* g_renderer = nullptr;

int main() {
    sf::ContextSettings settings;
    settings.depthBits = 24;
    settings.majorVersion = 3;
    settings.minorVersion = 3;
    settings.attributeFlags = sf::ContextSettings::Core;

    Window window("Engine", 1280, 720, settings);
    ConfigData emptyConfig = { 1280, 720, "Arial", 16, sf::Color::White, {}, 60 }; 
    IOpenGLRenderer renderer(window .handle);
    if (!renderer.init()) {
        std::cerr << "Renderer init failed\n";
        return -1;
    }
    g_renderer = &renderer;

    GameLoop loop;
    loop.init(1.0f / 60.0f);
    loop.addRender([](float alpha) {
        g_renderer->renderScene(alpha);
    });
    ImGui::SFML::Init(window.handle);
    
    loop.run(window);
    renderer.shutdown();
    ImGui::SFML::Shutdown();
    return 0;
}