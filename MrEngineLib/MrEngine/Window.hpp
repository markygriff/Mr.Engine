//
//  Window.hpp
//  MisterEngineLib
//
//  Created by Mark Griffith on 2016-07-18.
//  Copyright © 2016 Mark Griffith. All rights reserved.
//

#ifndef Window_hpp
#define Window_hpp

#include <stdio.h>
#include <SDL2/SDL.h>
#include <OpenGL/gl3.h>
#include <string>

namespace MrEngine {
    
    //defined such that we can 'bitwise or' to choose whichever combination of flags we want
    enum WindowFlags {INVISIBLE = 0x1, FULLSCREEN = 0x2, BORDERLESS = 0x4};

    class Window
    {
    public:
        Window();
        ~Window();
        
        int create(std::string windowName, int screenWidth, int screenHeight, unsigned int currentFlags);
        
        void swapBuffer();
        
        //getters
        int getScreenWidth() { return m_screenWidth; }
        int getScreenHeight() { return m_screenHeight; }
        SDL_Window* getSDLWindow() { return m_sdlWindow; }
        
    private:
        SDL_Window* m_sdlWindow;
        int m_screenWidth, m_screenHeight;
    };

}
#endif /* Window_hpp */
