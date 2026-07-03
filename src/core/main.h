#pragma once
#include <SFML/Graphics.hpp>
#include <iostream>
#include "GameLoop.h"

struct  ConfigData{

    int windowWidth;
    int windowHeight;
    std::string fontType;
    int fontSize;
    sf::Color fontColor;
    std::vector<Shapes> shapes;
    int framerateLimit;
};