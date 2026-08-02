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

public:
    // Single source of truth: all events flow through handleEvent().
    // Window::pollEvents() converts SFML events → custom event types
    // and calls this method. No duplicate SFML handling here.
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