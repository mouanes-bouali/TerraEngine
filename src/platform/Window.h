#pragma once
#include <SFML/Graphics.hpp>

class IInput; // forward declaration

struct Window {
    sf::RenderWindow handle;
    bool running;

    Window(const char* title, int w, int h, const sf::ContextSettings& settings);
    void pollEvents(IInput& inputSystem);
    bool isOpen() const;
};