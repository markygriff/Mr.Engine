//
//  App.cpp
//  NinjaPlatformer
//
//  Created by Mark Griffith on 2016-08-07.
//  Copyright © 2016 Mark Griffith. All rights reserved.
//

#include "App.hpp"

#include <ScreenList.hpp>

App::App()
{
    
}

App::~App()
{
    
}

void App::onInit()
{
    
}

void App::addScreens()
{
    //initialize our screens
    m_mainMenuScreen = std::make_unique<MainMenuScreen>(&m_window);
    m_gameplayScreen = std::make_unique<GameplayScreen>(&m_window);
    m_editorScreen = std::make_unique<EditorScreen>(&m_window);
    
    //add the new screen to the screen list (must be in the correct order!)
    m_screenList->addScreen(m_mainMenuScreen.get());
    m_screenList->addScreen(m_gameplayScreen.get());
    m_screenList->addScreen(m_editorScreen.get());
    
    //set the current screen in the screen list to the newly added screen
    m_screenList->setScreen(m_mainMenuScreen->getScreenIndex());
}

void App::onExit()
{
    
}

