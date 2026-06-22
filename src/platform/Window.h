// Window.h
#pragma once                    // instead of include guards
#include <SDL2/SDL.h>
#include <string>               // C++ string, same concept as Java

class Window {                  // class — same as Java/C#
public:                         // public section
    Window(const std::string& title, int w, int h);  // constructor
    ~Window();                  // destructor — runs when object dies (no GC in C++)
    
    bool isOpen() const;        // const = doesn't modify the object
    void pollEvents();
    void clear();
    void display();

private:                        // private section
    SDL_Window*   m_window;     // raw pointer — you manage this memory
    SDL_Renderer* m_renderer;
    bool          m_running;
};