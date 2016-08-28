//
//  MrEngine.cpp
//  MisterEngineLib
//
//  Created by Mark Griffith on 2016-07-18.
//  Copyright © 2016 Mark Griffith. All rights reserved.
//

#include "MrEngine.hpp"
#include "FatalError.hpp"

#include <SDL2/SDL.h>
#include <OpenGL/gl3.h>


namespace MrEngine {
    
    
    int init()
    {
        // Initialize SDL
        if (SDL_Init(SDL_INIT_EVERYTHING) < 0)
        {
            fatalError("Could Not Initialize SDL");
        }
        
        // set up a double buffered window (minimizes flickering)
        SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
        
        return 0;
    }

    
}
