#include <SFML/Graphics.hpp>
#include <iostream>
#include "renderer/IOpenGLRenderer.h"

int main() {
    // Request OpenGL 3.3 Core
    sf::ContextSettings settings;
    settings.depthBits = 24;
    settings.stencilBits = 8;
    settings.majorVersion = 3;
    settings.minorVersion = 3;
    settings.attributeFlags = sf::ContextSettings::Core;

    sf::RenderWindow window(
        sf::VideoMode(sf::Vector2u(1280, 720)),
        "OpenGL Test",
        sf::Style::Default,
        sf::State::Windowed,
        settings);

    IOpenGLRenderer renderer(window);
    if (!renderer.init()) {
        std::cerr << "Renderer init failed\n";
        return -1;
    }

    while (window.isOpen()) {
        while (auto event = window.pollEvent()) {
            if (event->is<sf::Event::Closed>())
                window.close();
        }

        renderer.beginFrame();
        renderer.drawTriangle();
        renderer.endFrame();
    }

    renderer.shutdown();
    return 0;
}