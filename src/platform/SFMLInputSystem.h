// src/platform/SFMLInputSystem.h
#pragma once
#include "IInput.h"
#include <unordered_map>

class SFMLInputSystem : public IInput {
private:
    std::unordered_map<sf::Keyboard::Key, bool> m_keyStates;

public:
    void handleEvent(const sf::Event& event) override {
        if (auto* kp = event.getIf<sf::Event::KeyPressed>())
            m_keyStates[kp->code] = true;
        else if (auto* kr = event.getIf<sf::Event::KeyReleased>())
            m_keyStates[kr->code] = false;
    }

    bool isKeyPressed(sf::Keyboard::Key key) override {
        auto it = m_keyStates.find(key);
        return it != m_keyStates.end() && it->second;
    }

    void update(float dt) override {
        // place for repeat delay, chord detection, etc.
    }
};
