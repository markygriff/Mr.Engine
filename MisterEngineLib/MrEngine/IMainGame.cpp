//
//  IMainGame.cpp
//  MisterEngineLib
//
//  Created by Mark Griffith on 2016-08-07.
//  Copyright © 2016 Mark Griffith. All rights reserved.
//

#include "IMainGame.hpp"
#include "Timing.hpp"
#include "ScreenList.hpp"
#include "IGameScreen.hpp"

#include <iostream>

namespace MrEngine
{
    
    IMainGame::IMainGame()
    {
        //initialize m_screenList
        m_screenList = std::make_unique<ScreenList>(this);
    }
    
    IMainGame::~IMainGame()
    {
        //empty
    }
    
    void IMainGame::run()
    {
        if ( !init() ) return;
        
        //the fps we want
        const float DESIRED_FPS = 60.0f;
        //maximum steps, used to avoid the 'spiral of death'
        const int MAX_PHYSICS_STEPS = 6;
        //set max delta time to avoid speedup
        const float MAX_DELTA_TIME = 1.0f;
        //milliseconds per second
        const float MS_PER_SECOND = 1000;
        //the fps we want in ms / frame
        const float DESIRED_FRAMETIME = MS_PER_SECOND / DESIRED_FPS;
        
        //set max fps to 60
        FpsLimiter limiter;
        limiter.setMaxFPS(DESIRED_FPS);
        
        //starting time
        float prevTicks = SDL_GetTicks();
        
        //game loop!
        m_isRunning = true;
        while (m_isRunning)
        {
            limiter.begin();
            
            //time at the start of the frame
            Uint32 newTicks = SDL_GetTicks();
            //total time the frame took
            Uint32 frameTime = SDL_GetTicks() - prevTicks;
            //update previous frame time
            prevTicks = newTicks;
            
            //how much extra time we have in the frame
            float totalDeltaTime = frameTime / DESIRED_FRAMETIME;
            
            //update input manager
            inputManager.update();
            
            //SEMI FIXED TIME STEP
            int updateCount = 0;
            while (totalDeltaTime > 0.0f && updateCount < MAX_PHYSICS_STEPS && m_isRunning)
            {
                //limit deltatime to 1.0 so no speedup (1.0 being one frame and .2 being a fifth of a frame)
                float deltaTime = std::min(totalDeltaTime, MAX_DELTA_TIME);
                
                deltaTime = deltaTime / DESIRED_FPS;
                
                //std::cout <<"deltaTime: " << deltaTime << std::endl;
                update(deltaTime);
            
                draw(deltaTime);
//                std::cout << deltaTime << std::endl;
//                std::cout <<  " T: " << totalDeltaTime << std::endl;
                totalDeltaTime -= deltaTime;
                
                //if we have reached the maximum physics steps, just continue on with the frame
                updateCount++;
            }
            
            m_fps = limiter.end();
            
            //swap window buffer for less flickering
            m_window.swapBuffer();
        }
    }
    
    void IMainGame::exitGame()
    {
        m_currentScreen->onExit();
        
        if (m_screenList)
        {
            m_screenList->destroy();
            m_screenList.reset();
        }
        
        m_isRunning = false;
    }
    
    bool IMainGame::init()
    {
        //initialize game engine
        MrEngine::init();
        
        SDL_GL_SetAttribute(SDL_GL_ACCELERATED_VISUAL, 1);
        
        if (!initSystems()) return false;
        
        //initialize the current game
        onInit();
        
        //initialize game screens and add them to the screenList
        addScreens();
        
        //set the MainGame's current game screen
        m_currentScreen = m_screenList->getCurrentScreen();
        
        //initialize game screen elements
        m_currentScreen->onEntry();
        
        //set the initial game screen to ScreenState::RUNNING
        m_currentScreen->setRunning();
        
        return true;
    }
    
    bool IMainGame::initSystems()
    {
        m_window.create("DEFAULT_NAME", 1340, 820, 0);
        
        return true;
    }
    
    void IMainGame::onSDLEvent(SDL_Event& event)
    {
        //set the event type
        ///inputManager.setEventType(event.type);
        
        switch (event.type) {
            case SDL_QUIT:
                exitGame();
                break;
            case SDL_MOUSEMOTION:
                inputManager.setMouseCoords((float)event.motion.x, (float)event.motion.y);
                break;
            case SDL_KEYDOWN:
                inputManager.pressKey(event.key.keysym.sym);
                break;
            case SDL_KEYUP:
                inputManager.releaseKey(event.key.keysym.sym);
                break;
            case SDL_MOUSEBUTTONDOWN:
                inputManager.pressKey(event.button.button);
                break;
            case SDL_MOUSEBUTTONUP:
                inputManager.releaseKey(event.button.button);
                break;
            case SDL_TEXTINPUT:
                inputManager.addInputCharacters(event.text.text);
                break;
            case SDL_MOUSEWHEEL:
                if (event.wheel.y > 0)
                    inputManager.setMouseWheel(1);
                if (event.wheel.y < 0)
                    inputManager.setMouseWheel(-1);
                break;
            case (SDLK_ESCAPE):
            {
                //empty
            }
            default:
                break;
        }
    }
    
    void IMainGame::update(float deltaTime)
    {
        if (m_currentScreen)
        {
            switch (m_currentScreen->getScreenState())
            {
                //update the current running screen
                case ScreenState::RUNNING:
                    m_currentScreen->update(deltaTime);
                    break;
                    
                //change to next screen
                case ScreenState::CHANGE_NEXT:
                    //clean up running screen
                    m_currentScreen->onExit();
                    m_currentScreen = m_screenList->moveNext();
                    if (m_currentScreen)
                    {
                        //initialize new running screen
                        m_currentScreen->setRunning();
                        m_currentScreen->onEntry();
                    }
                    break;
                    
                //change to previous screen
                case ScreenState::CHANGE_PREVIOUS:
                    //clean up running screen
                    m_currentScreen->onExit();
                    m_currentScreen = m_screenList->movePrev();
                    if (m_currentScreen)
                    {
                        //initialize new running screen
                        m_currentScreen->setRunning();
                        m_currentScreen->onEntry();
                    }
                    break;
                
                //exit game
                case ScreenState::EXIT_APPLICATION:
                    exitGame();
                    break;
                    
                // if NONE, do nothing
                default:
                    break;
                
            }
        }
        else { exitGame(); }
    }
    
    void IMainGame::draw(float deltaTime)
    {
        if (m_currentScreen && m_currentScreen->getScreenState() == ScreenState::RUNNING)
        {
            m_currentScreen->draw(deltaTime);
        }
    }
}
