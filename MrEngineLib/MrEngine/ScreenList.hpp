#pragma once

#ifndef ScreenList_hpp
#define ScreenList_hpp

#define SCREEN_INDEX_NONE -1

#include <stdio.h>
#include <vector>

namespace MrEngine
{
    
    
    class IMainGame;
    class IGameScreen;
    
    
    class ScreenList
    {
    public:
        ScreenList(IMainGame* game);
        ~ScreenList();
        
        IGameScreen* moveNext();
        IGameScreen* movePrev();
        
        void setScreen(int nextScreenIndex);
        void addScreen(IGameScreen* newScreen);
        
        void destroy();
        
        IGameScreen* getCurrentScreen();
        
    protected:
        IMainGame* m_game = nullptr;
        std::vector<IGameScreen*> m_screens;
        int m_currentScreenIndex = -1;
    };
    
    
}

#endif /* ScreenList_hpp */
