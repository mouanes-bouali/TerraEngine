#include "platform/Window.h"
#include "core/GameLoop.h"
#include "core/main.h"
#include "imgui/imgui.h"
#include "imgui/imgui-SFML.h"
#include <fstream>
#include <string>
#include <iostream>

static Window* g_window = nullptr;
static ConfigData* g_config = nullptr;

// ------------------------------------------------------------
// Render callback – registered with the game loop
// ------------------------------------------------------------
void renderScene(float alpha) {
    if (!g_window || !g_config) return;

    g_window->handle.clear(sf::Color(50, 50, 50));

    // 1. Draw every shape
    for (auto& shape : g_config->shapes)
        g_window->handle.draw(*shape.drawable);

    // 2. Draw ImGui on top
    ImGui::SFML::Render(g_window->handle);

    // 3. Show the finished frame
    g_window->handle.display();
}

// ------------------------------------------------------------
// Read configuration file
// ------------------------------------------------------------
ConfigData ReadFile(const std::string& filename) {
    ConfigData config;
    config.windowWidth  = 800;
    config.windowHeight = 600;
    config.fontSize     = 18;
    config.fontColor    = sf::Color::White;
    config.framerateLimit = 60;

    std::ifstream file(filename);
    if (!file.is_open()) {
        std::cerr << "Failed to open file: " << filename << std::endl;
        return config;          // return empty config; caller checks
    }

    std::string key;
    while (file >> key) {
        if (key == "Window") {
            file >> config.windowWidth >> config.windowHeight;
        }
        else if (key == "Font") {
            file >> config.fontType >> config.fontSize;
            int r, g, b;
            file >> r >> g >> b;
            config.fontColor = sf::Color(r, g, b);
        }
        else if (key == "Circle") {
            Shapes shape;
            shape.type = "Circle";
            std::string name;
            file >> name >> shape.x >> shape.y >> shape.vx >> shape.vy;
            int r, g, b;
            file >> r >> g >> b >> shape.radius;
            shape.color = sf::Color(r, g, b);
            shape.drawable = new sf::CircleShape(shape.radius, 32);
            shape.drawable->setFillColor(shape.color);
            shape.drawable->setPosition(sf::Vector2f(shape.x, shape.y));
            config.shapes.push_back(shape);
        }
        else if (key == "Rectangle") {
            Shapes shape;
            shape.type = "Rectangle";
            std::string name;
            file >> name >> shape.x >> shape.y >> shape.vx >> shape.vy;
            int r, g, b;
            file >> r >> g >> b >> shape.width >> shape.height;
            shape.color = sf::Color(r, g, b);
            auto* rect = new sf::RectangleShape(sf::Vector2f(shape.width, shape.height));
            rect->setFillColor(shape.color);
            rect->setPosition(sf::Vector2f(shape.x, shape.y));
            shape.drawable = rect;
            config.shapes.push_back(shape);
        }
    }
    return config;
}

// ------------------------------------------------------------
// main
// ------------------------------------------------------------
int main() {
    // 1. Load config (path relative to the folder you run from)
  ConfigData config = ReadFile("C:/Users/boual/OneDrive/Desktop/Projects/Game_Engine/TerraEngine/src/core/input.txt");
    if (config.shapes.empty()) {
        std::cerr << "No shapes loaded check your input.txt file and path!" << std::endl;
        // You can still run; the window will open but be empty.
    }

    // 2. Create window
    Window window("Solum", config.windowWidth, config.windowHeight);
    g_window = &window;
    g_config = &config;

    // 3. Initialize ImGui‑SFML
    if (!ImGui::SFML::Init(window.handle)) {
        std::cerr << "Failed to initialize ImGui-SFML" << std::endl;
        return -1;
    }
    sf::Event focusEvent(sf::Event::FocusGained);

    // 4. Build game loop and register the render callback
    GameLoop loop;
    loop.init(1.0f / 60.0f);
    loop.addRender(renderScene);

    // 5. Run
    loop.run(window, config);

    // 6. Cleanup
    ImGui::SFML::Shutdown();
    return 0;
}