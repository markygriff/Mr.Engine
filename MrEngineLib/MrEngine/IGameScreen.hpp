#pragma once

#ifndef IGameScreen_hpp
#define IGameScreen_hpp

#include <stdio.h>

namespace MrEngine
{
    
    class IMainGame;
    
    
    enum class ScreenState
    {
        NONE,
        RUNNING,
        EXIT_APPLICATION,
        CHANGE_NEXT, ///< change to next screen
        CHANGE_PREVIOUS ///< change to previous screen
    };
    

    class IGameScreen
    {
        
    public:
        friend class ScreenList;
        
        IGameScreen(){
            //empty
        }
        virtual ~IGameScreen(){
            //empty
        }
        
        /// Return the index of the next or prev screen when changing screens
        virtual int getNextScreenIndex() const = 0;
        virtual int getPrevScreenIndex() const = 0;
        
        /// Called at beginning and end of application
        virtual void build() = 0; ///< will be custom for each inherited class
        virtual void destroy() = 0;
        
        /// Called when screen enters and exits focus
        virtual void onEntry() = 0;
        virtual void onExit() = 0;
        
        /// Called in game loop
        virtual void update(float deltaTime) = 0;
        virtual void draw(float deltaTime) = 0;
        
        /// Sets m_game to be the parent game
        void setParentGame(IMainGame* game) { m_game = game; }
        /// Sets current game state to running
        void setRunning()
        {
            m_currentScreenState = ScreenState::RUNNING;
        }
        
        //getters
        int getScreenIndex() { return m_screenIndex; }
        ScreenState getScreenState() { return m_currentScreenState; }
        
    protected:
        
        ScreenState m_currentScreenState = ScreenState::NONE;
        IMainGame* m_game = nullptr;
        int m_screenIndex = -1;
    };

    
}
#endif /* IGameScreen_hpp */
