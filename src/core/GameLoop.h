// GameLoop.h
#include <SFML/Graphics.hpp>
#pragma once

struct Window;     // ← forward declaration – tells the compiler "Window exists"
struct ConfigData; // ← forward declaration – tells the compiler "ConfigData exists"

struct Shapes
{
    sf::Color color;
    std::string type;
    float vx, vy;
    float x, y;
    float previousX, previousY;
    bool isVisible;

    float height;
    float width;
    float radius;
    sf::Shape *drawable;
};
typedef void (*UpdateCallback)(float dt);
typedef void (*RenderCallback)(float alpha);

struct GameLoop
{
    float fixedDt;
    float accumulator;
    bool running;

    UpdateCallback updateCallbacks[32];
    int updateCount;
    UpdateCallback fixedUpdateCallbacks[32];
    int fixedUpdateCount;
    RenderCallback renderCallbacks[32];
    int renderCount;

    void init(float dt);
    void addUpdate(UpdateCallback cb);
    void addFixedUpdate(UpdateCallback cb);
    void addRender(RenderCallback cb);
    // GameLoop.h
    void run(Window &window); // ← make sure this matches .cpp
    void quit();
};