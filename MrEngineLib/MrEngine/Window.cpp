//
//  Window.cpp
//  MisterEngineLib
//
//  Created by Mark Griffith on 2016-07-18.
//  Copyright © 2016 Mark Griffith. All rights reserved.
//

#include "Window.hpp"
#include "FatalError.hpp"

namespace MrEngine {
    
    
    Window::Window()
    {
        //empty
    }

    Window::~Window()
    {
        //empty
    }

    int Window::create(std::string windowName, int screenWidth, int screenHeight, unsigned int currentFlags)
    {
        m_screenWidth = screenWidth;
        m_screenHeight = screenHeight;
        
        
        Uint32 flags = SDL_WINDOW_OPENGL;
        
        if (currentFlags & INVISIBLE)
        {
            flags |= SDL_WINDOW_HIDDEN;
        }
        if (currentFlags & FULLSCREEN)
        {
            flags |= SDL_WINDOW_FULLSCREEN_DESKTOP;
        }
        if (currentFlags & BORDERLESS)
        {
            flags |= SDL_WINDOW_BORDERLESS;
        }
        
        // initialize SDL window
        m_sdlWindow = SDL_CreateWindow(windowName.c_str(), SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, screenWidth, screenHeight, SDL_WINDOW_OPENGL);
        if (m_sdlWindow == nullptr)
        {
            fatalError("SDL Window could not be created :(");
        }
        
        SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE );
        
        // set up OpenGL context
        SDL_GLContext glContext = SDL_GL_CreateContext(m_sdlWindow);
        if (glContext == nullptr)
        {
            fatalError("SDL_GL context could not be created");
        }
        
        // bind a VAO
        GLuint vertexArrayID;
        glGenVertexArrays(1, &vertexArrayID);
        glBindVertexArray(vertexArrayID);
        
        //check the OpenGL version
        printf("*** OpenGL Version: %s ***", glGetString(GL_VERSION));
        
        // set background color
        glClearColor(255.0f, 255.0f, 255.0f, 1.0f);
        
        //set VSYNC
        SDL_GL_SetSwapInterval(0);
        
        //enable alpha blending
        glEnable(GL_BLEND);
        //what kind of blending we want
        //in this case, we want alpha 0 to be transparent
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
        
        return 0;
    }

    void Window::swapBuffer()
    {
        //swap buffer and draw everything to the screen
        SDL_GL_SwapWindow(m_sdlWindow);
    }

}