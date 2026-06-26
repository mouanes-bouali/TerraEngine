#include "platform/Window.h"
#include "core/GameLoop.h"
#include "core/main.h"
#include <fstream>
#include <string>
#include <iostream>

static Window* g_window = nullptr;
static ConfigData* g_config = nullptr;

void renderScene(float alpha) {
    if (g_window && g_config) {
        g_window->handle.clear(sf::Color(50, 50, 50));
        for (auto &shape : g_config->shapes) {
            g_window->handle.draw(*shape.drawable);
        }
        g_window->handle.display();
    }
}

ConfigData ReadFile(const std::string& filename) {
    ConfigData config;
    std::ifstream file(filename);
    std::string key;

    if (!file.is_open()) {
        std::cerr << "Failed to open file: " << filename << std::endl;
        return config;
    }

    while (file >> key) {
        if (key == "Window") {
            file >> config.windowWidth >> config.windowHeight;
        } else if (key == "Font") {
            file >> config.fontType >> config.fontSize;
            int r, g, b;
            file >> r >> g >> b;
            config.fontColor = sf::Color(r, g, b);
        } else if (key == "Circle") {
            Shapes shape;
            shape.type = "Circle";
            std::string name;
            file >> name >> shape.x >> shape.y >> shape.vx >> shape.vy;
            int r, g, b;
            file >> r >> g >> b >> shape.radius;
            shape.color = sf::Color(r, g, b);
            auto circle = new sf::CircleShape(shape.radius, 32);
            circle->setFillColor(shape.color);
            circle->setPosition(sf::Vector2f(shape.x, shape.y));
            shape.drawable = circle;
            config.shapes.push_back(shape);
            
        } else if (key == "Rectangle") {
            Shapes shape;
            shape.type = "Rectangle";
            std::string name;
            file >> name >> shape.x >> shape.y >> shape.vx >> shape.vy;
            int r, g, b;
            file >> r >> g >> b >> shape.width >> shape.height;
            shape.color = sf::Color(r, g, b);
            auto rect = new sf::RectangleShape();
            sf::Vector2f size(shape.width, shape.height);
            rect->setSize(size);
            rect->setFillColor(shape.color);
            rect->setPosition(sf::Vector2f(shape.x, shape.y));
            shape.drawable = rect;
            config.shapes.push_back(shape);
        }
    }

    return config;
}

int main() {
    ConfigData config = ReadFile("src/core/input.txt");
    Window window("Solum", config.windowWidth, config.windowHeight);
    g_window = &window;
    g_config = &config;

    GameLoop loop;
    loop.init(1.0f / 60.0f);
    loop.addRender(renderScene);
    loop.run(window, config);
    return 0;
}
