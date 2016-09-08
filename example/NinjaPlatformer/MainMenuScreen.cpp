//
//  MainMenuScreen.cpp
//  NinjaPlatformer
//
//  Created by Mark Griffith on 2016-08-24.
//  Copyright © 2016 Mark Griffith. All rights reserved.
//

#include "MainMenuScreen.hpp"

#include <IMainGame.hpp>

MainMenuScreen::MainMenuScreen(MrEngine::Window* window) : m_window(window)
{
    m_screenIndex = SCREEN_INDEX_MAINMENU;
}

MainMenuScreen::~MainMenuScreen()
{
    //empty
}

int MainMenuScreen::getNextScreenIndex() const
{
    return m_nextScreenIndex;
}

int MainMenuScreen::getPrevScreenIndex() const
{
    return SCREEN_INDEX_NONE;
}

void MainMenuScreen::build()
{
    
}

void MainMenuScreen::destroy()
{

}

void MainMenuScreen::onEntry()
{
    //initialize camera
    m_camera.init(m_window->getScreenWidth(), m_window->getScreenHeight());
    //zoom out
    m_camera.setScale(m_scale); ///< pixel per meter
    //initialize menu
    m_menuGUI.init(m_window, true);
    m_menuGUI.loadFont("../Assets/Fonts/extrafonts/DejaVuSans.ttf", 14.0f);
}

void MainMenuScreen::onExit()
{
    m_menuGUI.hide();
}

void MainMenuScreen::update(float deltaTime)
{
    m_menuGUI.update(m_game->inputManager, deltaTime);
    m_camera.update();
    checkInput();
}

void MainMenuScreen::draw(float deltaTime)
{
    //draw the game
    //clear the screen
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glClearColor(0, 0, 0, 255);
    
    drawUI();

}

void MainMenuScreen::drawUI()
{
    ImGuiIO& io = ImGui::GetIO();
    
    if (m_menuGUI.displayed)
    {
        ImGui::SetNextWindowPos(ImVec2(0, 0));
        ImGui::SetNextWindowSize(ImVec2(m_window->getScreenWidth(), m_window->getScreenHeight()), ImGuiSetCond_Always);
        
        ImGui::Begin("Welcome to the Main Menu", &m_menuGUI.displayed);
        
        if (ImGui::Button("Start Game")) { endGUI(); startGameClicked(); return; }
        
        if (ImGui::Button("Exit Application")) { endGUI(); exitClicked(); return; }
        
        //going into new gui, end this one
        if (ImGui::Button("Game Editor")) { endGUI(); editorClicked(); return; }
        ImGui::End();
    }
    
    // Rendering
    glViewport(0, 0, io.DisplaySize.x, io.DisplaySize.y);
    
    ImGui::Render(); ///< calls set renderdrawlists function
}

void MainMenuScreen::checkInput()
{
    SDL_Event event;
    
    while(SDL_PollEvent(&event))
    {
        m_menuGUI.processEvent(&event);
        
        m_game->onSDLEvent(event);
    }
}

void MainMenuScreen::endGUI()
{
    ImGui::End();
    m_menuGUI.hide();
}

void MainMenuScreen::startGameClicked()
{
    m_nextScreenIndex = SCREEN_INDEX_GAMEPLAY;
    m_currentScreenState = MrEngine::ScreenState::CHANGE_NEXT;
}

void MainMenuScreen::editorClicked()
{
    m_nextScreenIndex = SCREEN_INDEX_EDITOR;
    m_currentScreenState = MrEngine::ScreenState::CHANGE_NEXT;

}

void MainMenuScreen::exitClicked()
{
    m_currentScreenState = MrEngine::ScreenState::EXIT_APPLICATION;
}