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
        // Let ImGui process the event first
        ImGui::SFML::ProcessEvent(handle, *event);
        
        // Convert SFML event to our custom event types
        if (auto* mp = event->getIf<sf::Event::MouseMoved>()) {
            inputSystem.handleEvent(EVENT_MOUSE_MOVED, mp->position.x, mp->position.y);
        }
        else if (auto* mb = event->getIf<sf::Event::MouseButtonPressed>()) {
            inputSystem.handleEvent(EVENT_MOUSE_DOWN, static_cast<int>(mb->button), 0);
        }
        else if (auto* mb = event->getIf<sf::Event::MouseButtonReleased>()) {
            inputSystem.handleEvent(EVENT_MOUSE_UP, static_cast<int>(mb->button), 0);
        }
        else if (auto* mw = event->getIf<sf::Event::MouseWheelScrolled>()) {
            inputSystem.handleEvent(EVENT_SCROLL, static_cast<int>(mw->delta), 0);
        }
        else if (auto* kp = event->getIf<sf::Event::KeyPressed>()) {
            // Convert SFML key to our KeyCode
            KeyCode code = KeyCode::Unknown;
            switch (kp->code) {
                case sf::Keyboard::Key::W: code = KeyCode::W; break;
                case sf::Keyboard::Key::A: code = KeyCode::A; break;
                case sf::Keyboard::Key::S: code = KeyCode::S; break;
                case sf::Keyboard::Key::D: code = KeyCode::D; break;
                case sf::Keyboard::Key::Space: code = KeyCode::Space; break;
                case sf::Keyboard::Key::Enter: code = KeyCode::Enter; break;
                case sf::Keyboard::Key::Escape: code = KeyCode::Escape; break;
                case sf::Keyboard::Key::Up: code = KeyCode::Up; break;
                case sf::Keyboard::Key::Down: code = KeyCode::Down; break;
                case sf::Keyboard::Key::Left: code = KeyCode::Left; break;
                case sf::Keyboard::Key::Right: code = KeyCode::Right; break;
                case sf::Keyboard::Key::F5: code = KeyCode::F5; break;
                default: break;
            }
            inputSystem.handleEvent(EVENT_KEY_DOWN, static_cast<int>(code), 0);
        }
        else if (auto* kr = event->getIf<sf::Event::KeyReleased>()) {
            KeyCode code = KeyCode::Unknown;
            switch (kr->code) {
                case sf::Keyboard::Key::W: code = KeyCode::W; break;
                case sf::Keyboard::Key::A: code = KeyCode::A; break;
                case sf::Keyboard::Key::S: code = KeyCode::S; break;
                case sf::Keyboard::Key::D: code = KeyCode::D; break;
                case sf::Keyboard::Key::Space: code = KeyCode::Space; break;
                case sf::Keyboard::Key::Enter: code = KeyCode::Enter; break;
                case sf::Keyboard::Key::Escape: code = KeyCode::Escape; break;
                case sf::Keyboard::Key::Up: code = KeyCode::Up; break;
                case sf::Keyboard::Key::Down: code = KeyCode::Down; break;
                case sf::Keyboard::Key::Left: code = KeyCode::Left; break;
                case sf::Keyboard::Key::Right: code = KeyCode::Right; break;
                case sf::Keyboard::Key::F5: code = KeyCode::F5; break;
                default: break;
            }
            inputSystem.handleEvent(EVENT_KEY_UP, static_cast<int>(code), 0);
        }
        else if (event->is<sf::Event::Closed>()) {
            running = false;
            handle.close();
        }
    }
}

bool Window::isOpen() const {
    return running && handle.isOpen();
}
