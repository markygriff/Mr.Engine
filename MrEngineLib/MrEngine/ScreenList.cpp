//
//  ScreenList.cpp
//  MisterEngineLib
//
//  Created by Mark Griffith on 2016-08-07.
//  Copyright © 2016 Mark Griffith. All rights reserved.
//

#include "ScreenList.hpp"
#include "IGameScreen.hpp"
#include "IMainGame.hpp"


namespace MrEngine
{
    
    
    ScreenList::ScreenList(IMainGame* game) :
    m_game(game)
    {
        //empty
    }
    
    ScreenList::~ScreenList()
    {
        destroy();
    }
    
    IGameScreen* ScreenList::moveNext()
    {
        IGameScreen* currentScreen = getCurrentScreen();
        
        if (currentScreen->getNextScreenIndex() != SCREEN_INDEX_NONE)
        {
            m_currentScreenIndex = currentScreen->getNextScreenIndex();
        }
        return getCurrentScreen();
    }
    
    IGameScreen* ScreenList::movePrev()
    {
        IGameScreen* currentScreen = getCurrentScreen();
        
        if (currentScreen->getPrevScreenIndex() != SCREEN_INDEX_NONE)
        {
            m_currentScreenIndex = currentScreen->getPrevScreenIndex();
        }
        return getCurrentScreen();
    }
    
    void ScreenList::setScreen(int nextScreenIndex)
    {
        m_currentScreenIndex = nextScreenIndex;
    }
    
    void ScreenList::addScreen(IGameScreen* newScreen)
    {
        newScreen->m_screenIndex = (int)m_screens.size();

        //add new screen to vector of screens
        m_screens.push_back(newScreen);
        
        //set up the new screen
        newScreen->build();
        newScreen->setParentGame(m_game);
    }
    
    void ScreenList::destroy()
    {
        //destroy the entire screenList
        for (size_t i = 0; i < m_screens.size(); i++)
        {
            m_screens[i]->destroy();
        }
        
        //no screens left
        m_screens.resize(0);
        m_currentScreenIndex = SCREEN_INDEX_NONE;
    }
    
    IGameScreen* ScreenList::getCurrentScreen()
    {
        //error check
        if(m_currentScreenIndex == SCREEN_INDEX_NONE) return nullptr;
        
        return m_screens[m_currentScreenIndex];
    }
    
    
}