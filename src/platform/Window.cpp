// Window.cpp
#include "Window.h"
#include <iostream>

Window::Window(const std::string& title, int w, int h) 
    : m_running(true)           // initializer list — sets members before body runs
{
    SDL_Init(SDL_INIT_VIDEO);
    
    m_window = SDL_CreateWindow(
        title.c_str(),          // .c_str() converts std::string to C char*
        SDL_WINDOWPOS_CENTERED,
        SDL_WINDOWPOS_CENTERED,
        w, h,
        SDL_WINDOW_SHOWN
    );
    
    m_renderer = SDL_CreateRenderer(m_window, -1, SDL_RENDERER_ACCELERATED);
}

Window::~Window() {             // destructor — this is your "finally" block
    SDL_DestroyRenderer(m_renderer);
    SDL_DestroyWindow(m_window);
    SDL_Quit();
}

bool Window::isOpen() const { 
    return m_running; 
}

void Window::pollEvents() {
    SDL_Event event;
    while (SDL_PollEvent(&event)) {     // & = pass address, like a reference
        if (event.type == SDL_QUIT)
            m_running = false;
        if (event.type == SDL_KEYDOWN)
            if (event.key.keysym.sym == SDLK_ESCAPE)
                m_running = false;
    }
}

void Window::clear() {
    SDL_SetRenderDrawColor(m_renderer, 20, 20, 30, 255);  // dark background
    SDL_RenderClear(m_renderer);
}

void Window::display() {
    SDL_RenderPresent(m_renderer);
}