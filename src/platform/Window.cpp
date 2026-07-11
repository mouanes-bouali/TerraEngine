#include "platform/Window.h"
#include "platform/IInput.h"
#include "imgui/imgui.h"
#include "imgui/imgui-SFML.h"

// Window.cpp
Window::Window(const char* title, int w, int h, const sf::ContextSettings& settings)
    : handle(sf::VideoMode(sf::Vector2u(w, h), 32), title, sf::Style::Default, sf::State::Windowed, settings)
    , running(true)
{
    handle.setActive(true);
    handle.setFramerateLimit(60);
}

void Window::pollEvents(IInput& inputSystem) {
    while (auto event = handle.pollEvent()) {
        inputSystem.handleEvent(*event);
        if (event->is<sf::Event::Closed>()) {
            running = false;
            handle.close();
        }
    }
}

bool Window::isOpen() const {
    return running && handle.isOpen();
}