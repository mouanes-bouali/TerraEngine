// GameLoop.h
#include <SFML/Graphics.hpp>
#pragma once

struct Window;     // forward declaration – tells the compiler "Window exists"
struct ConfigData; // forward declaration – tells the compiler "ConfigData exists"
class IInput;      // forward declaration

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
#include <functional>
typedef std::function<void(float dt)> UpdateCallback;
typedef std::function<void(float alpha)> RenderCallback;

struct GameLoop
{
    float fixedDt;
    float accumulator;
    bool running;

    UpdateCallback updateCallbacks[32];
    UpdateCallback inputUpdateCallbacks[32];
    int updateCount;
    UpdateCallback fixedUpdateCallbacks[32];
    int fixedUpdateCount;
    RenderCallback renderCallbacks[32];
    int renderCount;
    int inputUpdateCount;
    UpdateCallback imguiCallbacks[32];
    int imguiCount;
    void init(float dt);
    void addUpdate(UpdateCallback cb);
    void addInputUpdate(UpdateCallback cb, Window& window);
    void addFixedUpdate(UpdateCallback cb);
    void addRender(RenderCallback cb);
    void addImGuiUpdate(UpdateCallback cb);
    void run(Window &window, IInput& inputSystem);
    void quit();
};
