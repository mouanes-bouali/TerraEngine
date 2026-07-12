// src/platform/SFMLInputSystem.h
#pragma once
#include "IInput.h"
#include <unordered_map>

class SFMLInputSystem : public IInput {
private:
    std::unordered_map<sf::Keyboard::Key, bool> m_keyStates;
    float m_mouseX = 0.0f, m_mouseY = 0.0f;
    float m_mouseDeltaX = 0.0f, m_mouseDeltaY = 0.0f;
    float m_mouseWheelDelta = 0.0f;
    bool m_mouseButtons[3] = {false, false, false};

public:
    void handleEvent(const sf::Event& event) override {
        if (auto* kp = event.getIf<sf::Event::KeyPressed>())
            m_keyStates[kp->code] = true;
        else if (auto* kr = event.getIf<sf::Event::KeyReleased>())
            m_keyStates[kr->code] = false;
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

    bool isKeyPressed(sf::Keyboard::Key key) override {
        auto it = m_keyStates.find(key);
        return it != m_keyStates.end() && it->second;
    }

    void update(float dt) override {
        // Reset deltas after they've been consumed
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
