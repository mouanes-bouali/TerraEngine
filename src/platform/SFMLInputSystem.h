// src/platform/SFMLInputSystem.h
#pragma once
#include "IInput.h"
#include <SFML/Window/Event.hpp>
#include <unordered_map>

class SFMLInputSystem : public IInput {
private:
    std::unordered_map<KeyCode, bool> m_keyStates;
    float m_mouseX = 0.0f, m_mouseY = 0.0f;
    float m_mouseDeltaX = 0.0f, m_mouseDeltaY = 0.0f;
    float m_mouseWheelDelta = 0.0f;
    bool m_mouseButtons[3] = {false, false, false};

    // Translate SFML key to our KeyCode
    KeyCode sfmlToKeyCode(sf::Keyboard::Key sfmlKey) const {
        switch (sfmlKey) {
            case sf::Keyboard::Key::Z: return KeyCode::Z;
            case sf::Keyboard::Key::Q: return KeyCode::Q;
            case sf::Keyboard::Key::S: return KeyCode::S;
            case sf::Keyboard::Key::D: return KeyCode::D;
            case sf::Keyboard::Key::Space: return KeyCode::Space;
            case sf::Keyboard::Key::Enter: return KeyCode::Enter;
            case sf::Keyboard::Key::Escape: return KeyCode::Escape;
            case sf::Keyboard::Key::Up: return KeyCode::Up;
            case sf::Keyboard::Key::Down: return KeyCode::Down;
            case sf::Keyboard::Key::Left: return KeyCode::Left;
            case sf::Keyboard::Key::Right: return KeyCode::Right;
            case sf::Keyboard::Key::LShift: return KeyCode::Shift;
            case sf::Keyboard::Key::LControl: return KeyCode::Control;
            case sf::Keyboard::Key::LAlt: return KeyCode::Alt;
            default: return KeyCode::Unknown;
        }
    }

public:
    // New interface: takes our event types
    void handleEvent(int type, int data1, int data2) override {
        switch (type) {
            case EVENT_MOUSE_MOVED:
                m_mouseDeltaX = data1 - m_mouseX;
                m_mouseDeltaY = data2 - m_mouseY;
                m_mouseX = data1;
                m_mouseY = data2;
                break;
            case EVENT_MOUSE_DOWN:
                if (data1 >= 0 && data1 < 3) m_mouseButtons[data1] = true;
                break;
            case EVENT_MOUSE_UP:
                if (data1 >= 0 && data1 < 3) m_mouseButtons[data1] = false;
                break;
            case EVENT_KEY_DOWN:
                m_keyStates[static_cast<KeyCode>(data1)] = true;
                break;
            case EVENT_KEY_UP:
                m_keyStates[static_cast<KeyCode>(data1)] = false;
                break;
            case EVENT_SCROLL:
                m_mouseWheelDelta += data1;
                break;
        }
    }

    // Old SFML event handler (called by Window)
    void handleSFML(const sf::Event& event) {
        if (auto* kp = event.getIf<sf::Event::KeyPressed>())
            m_keyStates[sfmlToKeyCode(kp->code)] = true;
        else if (auto* kr = event.getIf<sf::Event::KeyReleased>())
            m_keyStates[sfmlToKeyCode(kr->code)] = false;
        else if (auto* mp = event.getIf<sf::Event::MouseMoved>()) {
            m_mouseDeltaX = mp->position.x - m_mouseX;
            m_mouseDeltaY = mp->position.y - m_mouseY;
            m_mouseX = mp->position.x;
            m_mouseY = mp->position.y;
        }
        else if (auto* mb = event.getIf<sf::Event::MouseButtonPressed>()) {
            int btn = static_cast<int>(mb->button);
            if (btn >= 0 && btn < 3) m_mouseButtons[btn] = true;
        }
        else if (auto* mb = event.getIf<sf::Event::MouseButtonReleased>()) {
            int btn = static_cast<int>(mb->button);
            if (btn >= 0 && btn < 3) m_mouseButtons[btn] = false;
        }
        else if (auto* mw = event.getIf<sf::Event::MouseWheelScrolled>()) {
            m_mouseWheelDelta += mw->delta;
        }
    }

    bool isKeyPressed(KeyCode key) override {
        auto it = m_keyStates.find(key);
        return it != m_keyStates.end() && it->second;
    }

    void update(float dt) override {
        m_mouseDeltaX = 0.0f;
        m_mouseDeltaY = 0.0f;
        m_mouseWheelDelta = 0.0f;
    }

    float getMouseX() const override { return m_mouseX; }
    float getMouseY() const override { return m_mouseY; }
    float getMouseDeltaX() const override { return m_mouseDeltaX; }
    float getMouseDeltaY() const override { return m_mouseDeltaY; }
    float getMouseWheelDelta() const override { return m_mouseWheelDelta; }
    bool isMouseButtonPressed(int button) override {
        return button >= 0 && button < 3 && m_mouseButtons[button];
    }
};
