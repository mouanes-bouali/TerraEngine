#pragma once
#include <SFML/Graphics.hpp>

struct Window {
    sf::RenderWindow handle;
    bool running;

    Window(const char* title, int w, int h);
    void pollEvents();
    bool isOpen() const;
};