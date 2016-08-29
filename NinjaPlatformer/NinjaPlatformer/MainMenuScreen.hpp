//
//  MainMenuScreen.hpp
//  NinjaPlatformer
//
//  Created by Mark Griffith on 2016-08-24.
//  Copyright © 2016 Mark Griffith. All rights reserved.
//

#ifndef MainMenuScreen_hpp
#define MainMenuScreen_hpp

#include "ScreenIndices.hpp"

#include <IGameScreen.hpp>
#include <Window.hpp>
#include <Camera2D.hpp>
#include <GUI.hpp>
#include <ScreenList.hpp>

#include <stdio.h>

class MainMenuScreen : public MrEngine::IGameScreen
{
public:
    MainMenuScreen(MrEngine::Window*);
    ~MainMenuScreen();
    
    /// Customs
    /// Return the index of the next or prev screen when changing screens
    virtual int getNextScreenIndex() const override;
    virtual int getPrevScreenIndex() const override;
    
    /// Called at beginning and end of application
    virtual void build() override;
    virtual void destroy() override;
    
    /// Called when screen enters and exits focus
    virtual void onEntry() override;
    virtual void onExit() override;
    
    /// Called in game loop
    virtual void update(float deltaTime) override;
    virtual void draw(float deltaTime) override;
    
    void drawUI();
    
private:
    void checkInput();
    void endGUI();
    void startGameClicked();
    void editorClicked();
    void exitClicked();
    
    int m_nextScreenIndex = SCREEN_INDEX_GAMEPLAY;
    
    ///PauseMenu m_pauseMenu;
    MrEngine::GUI m_menuGUI;
    
    MrEngine::Camera2D m_camera;
    MrEngine::Window* m_window;
    
    float m_scale = 20.0f;
};

#endif /* MainMenuScreen_hpp */
