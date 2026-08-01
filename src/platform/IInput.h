// src/platform/IInput.h
#pragma once

// Custom key codes (not tied to any library)
enum class KeyCode {
    Unknown = -1,
    W, A, S, D,Q,Z,
    Space, Enter, Escape,
    Up, Down, Left, Right,
    Shift, Control, Alt,
    Count  // Keep last
};

class IInput {
public:
    virtual ~IInput() = default;

    // Called by Window when an event happens
    // type: 0=mouseMove, 1=mouseDown, 2=mouseUp, 3=keyDown, 4=keyUp, 5=scroll
    // data1/data2: x/y for mouse, keyCode for keyboard, delta for scroll
    virtual void handleEvent(int type, int data1, int data2) = 0;
    
    virtual bool isKeyPressed(KeyCode key) = 0;
    virtual void update(float dt) = 0;
    
    // Mouse state
    virtual float getMouseX() const = 0;
    virtual float getMouseY() const = 0;
    virtual float getMouseDeltaX() const = 0;
    virtual float getMouseDeltaY() const = 0;
    virtual float getMouseWheelDelta() const = 0;
    virtual bool isMouseButtonPressed(int button) = 0;
};

// Event types for handleEvent()
const int EVENT_MOUSE_MOVED  = 0;
const int EVENT_MOUSE_DOWN   = 1;
const int EVENT_MOUSE_UP     = 2;
const int EVENT_KEY_DOWN     = 3;
const int EVENT_KEY_UP       = 4;
const int EVENT_SCROLL       = 5;
