// src/platform/IInput.h
#pragma once
#include <SFML/Window/Event.hpp>

class IInput {
public:
    virtual ~IInput() = default;

    virtual void handleEvent(const sf::Event& event) = 0;
    virtual bool isKeyPressed(sf::Keyboard::Key key) = 0;
    virtual void update(float dt) = 0;
};
