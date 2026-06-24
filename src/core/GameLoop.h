// GameLoop.h
#pragma once

struct Window;   // ← forward declaration – tells the compiler "Window exists"

typedef void (*UpdateCallback)(float dt);
typedef void (*RenderCallback)(float alpha);

struct GameLoop {
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
    void run(Window& window);
    void quit();
};