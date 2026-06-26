#include "platform/Window.h"

Window::Window(const char* title, int w, int h)
    : handle(sf::VideoMode(sf::Vector2u(w, h), 32), title), running(true)
{
    handle.setFramerateLimit(60);
}

void Window::pollEvents() {
    while (auto event = handle.pollEvent()) {
        if (event->is<sf::Event::Closed>()) {
            running = false;
            handle.close();
        }
    }
}

bool Window::isOpen() const {
    return running && handle.isOpen();
}
