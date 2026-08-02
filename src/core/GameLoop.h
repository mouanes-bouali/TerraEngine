// GameLoop.h
#include <SFML/Graphics.hpp>
#pragma once

#include <functional>
#include <vector>

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

typedef std::function<void(float dt)> UpdateCallback;
typedef std::function<void(float alpha)> RenderCallback;

struct GameLoop
{
    float fixedDt;
    float accumulator;
    bool running;

    // Dynamic callback lists — no fixed-size limit, no silent failure
    std::vector<UpdateCallback> updateCallbacks;
    std::vector<UpdateCallback> inputUpdateCallbacks;
    std::vector<UpdateCallback> fixedUpdateCallbacks;
    std::vector<RenderCallback> renderCallbacks;
    std::vector<UpdateCallback> imguiCallbacks;

    void init(float dt);
    void addUpdate(UpdateCallback cb);
    void addInputUpdate(UpdateCallback cb, Window &window);
    void addFixedUpdate(UpdateCallback cb);
    void addRender(RenderCallback cb);
    void addImGuiUpdate(UpdateCallback cb);
    void run(Window &window, IInput &inputSystem);
    void quit();
};