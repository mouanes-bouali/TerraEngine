#include "GameLoop.h"
#include "platform/Window.h"
#include <SFML/Graphics.hpp>
#include <iostream>
#include "main.h"
#include "imgui/imgui.h"
#include "imgui/imgui-SFML.h"

void GameLoop::init(float dt)
{
    fixedDt = dt;
    accumulator = 0.0f;
    running = true;
    updateCount = 0;
    fixedUpdateCount = 0;
    renderCount = 0;
}

void GameLoop::addUpdate(UpdateCallback cb)
{
    if (updateCount < 32)
        updateCallbacks[updateCount++] = cb;
}

void GameLoop::addFixedUpdate(UpdateCallback cb)
{
    if (fixedUpdateCount < 32)
        fixedUpdateCallbacks[fixedUpdateCount++] = cb;
}

void GameLoop::addRender(RenderCallback cb)
{
    if (renderCount < 32)
        renderCallbacks[renderCount++] = cb;
}

static float getDeltaTime()
{
    static sf::Clock clock;
    float dt = clock.restart().asSeconds();
    if (dt > 0.25f)
        dt = 0.25f; // clamp
    return dt;
}

void GameLoop::run(Window &window, ConfigData &config)
{
    int selectedShape = -1;
    while (window.isOpen() && running)
    {

        // 1. Process events
        window.pollEvents();

        // 2. Measure time
        float dt = getDeltaTime();
        ImGui::SFML::Update(window.handle, sf::seconds(dt));

        ImGui::SetNextWindowSize(ImVec2(300, 400), ImGuiCond_FirstUseEver);
        ImGui::Begin("ConfigUI");

        ImGui::Text("");

        for (int i = 0; i < config.shapes.size(); i++)
        {
            bool isSelected = (selectedShape == i);

            if (ImGui::Selectable(config.shapes[i].type.c_str(), isSelected))
            {
                selectedShape = i;
            }
        }
        if (selectedShape != -1)
        {
            Shapes &shape = config.shapes[selectedShape];

            ImGui::Separator();

            ImGui::Text("Selected: %s", shape.type.c_str());

            ImGui::SliderFloat("Velocity X", &shape.vx, 0.f, 1000.f);
            ImGui::SliderFloat("Velocity Y", &shape.vy, 0.f, 1000.f);
            float color[3] = {shape.color.r / 255.f, shape.color.g / 255.f, shape.color.b / 255.f};

            if (ImGui::ColorEdit3("Color", color))
            {
                shape.color.r = (int)(color[0] * 255);
                shape.color.g = (int)(color[1] * 255);
                shape.color.b = (int)(color[2] * 255);
                shape.drawable->setFillColor(shape.color);
            }
        }

        ImGui::End();

        // 3. Variable-rate updates
        for (int i = 0; i < updateCount; ++i)
            updateCallbacks[i](dt);

        // 4. Fixed-timestep updates
        accumulator += dt;
        while (accumulator >= fixedDt)
        {
            for (size_t i = 0; i < config.shapes.size(); ++i)
            {
                auto &shape = config.shapes[i];
                shape.previousX = shape.x;
                shape.previousY = shape.y;
                // Move
                shape.x += shape.vx * fixedDt;
                shape.y += shape.vy * fixedDt;

                // Get bounds
               
                sf::FloatRect bounds = shape.drawable->getGlobalBounds();

                // Bounce off walls
                if (shape.x < 0)
                {
                    shape.x = 0;
                    shape.vx = -shape.vx;
                }

                if (shape.x + bounds.size.x > config.windowWidth)
                {
                    shape.x = config.windowWidth - bounds.size.x;
                    shape.vx = -shape.vx;
                }

                if (shape.y < 0)
                {
                    shape.y = 0;
                    shape.vy = -shape.vy;
                }

                if (shape.y + bounds.size.y > config.windowHeight)
                {
                    shape.y = config.windowHeight - bounds.size.y;
                    shape.vy = -shape.vy;
                }

                // Apply corrected position
            }

            for (size_t i = 0; i < fixedUpdateCount; ++i)
                fixedUpdateCallbacks[i](fixedDt);
            accumulator -= fixedDt;
        }

        // 5. Render callbacks (they handle clear, draw, display)
        float alpha = accumulator / fixedDt;
        for (size_t i = 0; i < config.shapes.size(); ++i)
        {
            auto &shape = config.shapes[i];
            float interpolatedX = shape.previousX + (shape.x - shape.previousX) * alpha;
            float interpolatedY = shape.previousY + (shape.y - shape.previousY) * alpha;
            shape.drawable->setPosition(sf::Vector2f(interpolatedX, interpolatedY));
        }
        for (int i = 0; i < renderCount; ++i){
            renderCallbacks[i](alpha);
        }
    }
}

void GameLoop::quit()
{
    running = false;
}