#include "platform/Window.h"
#include "imgui/imgui.h"
#include "imgui/imgui-SFML.h"

Window::Window(const char* title, int w, int h)
    : handle(sf::VideoMode(sf::Vector2u(w, h), 32), title), running(true)
{
    handle.setVerticalSyncEnabled(true);
    handle.setFramerateLimit(120);
}

void Window::pollEvents() {
    while (auto event = handle.pollEvent()) {
         ImGui::SFML::ProcessEvent(handle, *event); 
         
     
        if (event->is<sf::Event::Closed>()) {
            running = false;
            handle.close();
        }
    }
}

bool Window::isOpen() const {
    return running && handle.isOpen();
}
