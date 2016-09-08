//
//  IMainGame.hpp
//  MisterEngineLib
//
//  Created by Mark Griffith on 2016-08-07.
//  Copyright © 2016 Mark Griffith. All rights reserved.
//

#pragma once

#ifndef IMainGame_hpp
#define IMainGame_hpp

#include "MrEngine.hpp"
#include "Window.hpp"
#include "InputManager.hpp"

#include <stdio.h>

namespace MrEngine
{
    
    class ScreenList;
    class IGameScreen;
    
    class IMainGame
    {
    public:
        IMainGame();
        virtual ~IMainGame();
        
        /// Run and initialize
        void run();
        /// Exits game
        void exitGame();
        
        virtual void onInit() = 0;
        virtual void addScreens() = 0;
        virtual void onExit() = 0;
        
        /// Handles all sdl events
        void onSDLEvent(SDL_Event& event);
        
        //getters
        const float getFps() const { return m_fps; }
        
        InputManager inputManager; ///< main game's input manager
    
    protected:
        
        bool init();
        bool initSystems();
        virtual void update(float deltaTime);
        virtual void draw(float deltaTime);
        
        bool m_isRunning = false; ///< to check which screen is in focus
        float m_fps = 0; ///< main game's fps
        std::unique_ptr<ScreenList> m_screenList = nullptr; ///< the game's screen list
        IGameScreen* m_currentScreen = nullptr; ///< the current screen in focus
        Window m_window; ///< main game's window
    };
    
    
}

#endif /* IMainGame_hpp */
