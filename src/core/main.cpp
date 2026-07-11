#include "platform/Window.h"
#include "core/GameLoop.h"
#include "main.h"
#include "renderer/IOpenGLRenderer.h"
#include "platform/IInput.h"
#include "platform/SFMLInputSystem.h"
#include <iostream>
#include "imgui/imgui.h"
#include "imgui/imgui-SFML.h"

static IRenderer* g_renderer = nullptr;
static IInput* g_input = nullptr;

int main() {
    sf::ContextSettings settings;
    settings.depthBits = 24;
    settings.majorVersion = 3;
    settings.minorVersion = 3;
    settings.attributeFlags = sf::ContextSettings::Core;

    Window window("Engine", 1280, 720, settings);

    // ── Renderer (polymorphic via IRenderer interface) ──
    IOpenGLRenderer openGLRenderer(window.handle);
    IRenderer& renderer = openGLRenderer;
    g_renderer = &renderer;

    // ── Input (polymorphic via IInput interface) ──
    SFMLInputSystem sfmlInput;
    IInput& input = sfmlInput;
    g_input = &input;

    ConfigData emptyConfig = { 1280, 720, "Arial", 16, sf::Color::White, {}, 60 }; 

    if (!renderer.init()) {
        std::cerr << "Renderer init failed\n";
        return -1;
    }

    GameLoop loop;
    loop.init(1.0f / 60.0f);
    loop.addRender([](float alpha) {
        g_renderer->renderScene(alpha);
    });
    ImGui::SFML::Init(window.handle);

    loop.run(window, input);

    renderer.shutdown();
    ImGui::SFML::Shutdown();
    return 0;
}