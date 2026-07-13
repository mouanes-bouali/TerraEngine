// src/platform/IInput.h
#pragma once
#include <SFML/Window/Event.hpp>

class IInput {
public:
    virtual ~IInput() = default;

    virtual void handleEvent(const sf::Event& event) = 0;
    virtual bool isKeyPressed(sf::Keyboard::Key key) = 0;
    virtual void update(float dt) = 0;
    
    // Mouse state
    virtual float getMouseX() const = 0;
    virtual float getMouseY() const = 0;
    virtual float getMouseDeltaX() const = 0;
    virtual float getMouseDeltaY() const = 0;
    virtual float getMouseWheelDelta() const = 0;
    virtual bool isMouseButtonPressed(int button) = 0;
};
