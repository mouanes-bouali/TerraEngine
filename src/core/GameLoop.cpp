#include "GameLoop.h"
#include "platform/Window.h"
#include "platform/IInput.h"
#include <SFML/Graphics.hpp>
#include "imgui/imgui.h"
#include "imgui/imgui-SFML.h"

void GameLoop::init(float dt)
{
    fixedDt = dt;
    accumulator = 0.0f;
    running = true;
    updateCallbacks.clear();
    inputUpdateCallbacks.clear();
    fixedUpdateCallbacks.clear();
    renderCallbacks.clear();
    imguiCallbacks.clear();
}

void GameLoop::addUpdate(UpdateCallback cb)
{
    updateCallbacks.push_back(std::move(cb));
}

void GameLoop::addInputUpdate(UpdateCallback cb, Window &window)
{
    inputUpdateCallbacks.push_back(std::move(cb));
}

void GameLoop::addFixedUpdate(UpdateCallback cb)
{
    fixedUpdateCallbacks.push_back(std::move(cb));
}

void GameLoop::addRender(RenderCallback cb)
{
    renderCallbacks.push_back(std::move(cb));
}

void GameLoop::addImGuiUpdate(UpdateCallback cb)
{
    imguiCallbacks.push_back(std::move(cb));
}

static float getDeltaTime()
{
    static sf::Clock clock;
    float dt = clock.restart().asSeconds();
    if (dt > 0.25f)
        dt = 0.25f;
    return dt;
}

void GameLoop::run(Window &window, IInput &inputSystem)
{
    while (window.isOpen() && running)
    {
        window.pollEvents(inputSystem);
        float dt = getDeltaTime();

        // Variable-rate updates
        for (auto& cb : updateCallbacks)
            cb(dt);

        // Input updates
        for (auto& cb : inputUpdateCallbacks)
            cb(dt);

        // Fixed-timestep updates
        accumulator += dt;
        while (accumulator >= fixedDt)
        {
            for (auto& cb : fixedUpdateCallbacks)
                cb(fixedDt);
            accumulator -= fixedDt;
        }

        // Render (draws to back buffer)
        float alpha = accumulator / fixedDt;
        for (auto& cb : renderCallbacks)
            cb(alpha);
    }
}

void GameLoop::quit()
{
    running = false;
}