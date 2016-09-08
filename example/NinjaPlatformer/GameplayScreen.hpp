//
//  GameplayScreen.hpp
//  NinjaPlatformer
//
//  Created by Mark Griffith on 2016-08-07.
//  Copyright © 2016 Mark Griffith. All rights reserved.
//

#ifndef GameplayScreen_hpp
#define GameplayScreen_hpp

#include "Box.hpp"
#include "Player.hpp"
#include "ScreenIndices.hpp"

#include <Box2D/Box2D.h>
#include <IGameScreen.hpp>
#include <SpriteBatch.hpp>
#include <GLSLProgram.hpp>
#include <Camera2D.hpp>
#include <GLTexture.h>
#include <Window.hpp>
#include <SpriteFont.h>
#include <DebugRenderer.hpp>
#include <GUI.hpp>

#include <stdio.h>
#include <vector>

class GameplayScreen : public MrEngine::IGameScreen
{
public:
    GameplayScreen(MrEngine::Window* window);
    ~GameplayScreen();
    
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
    void drawPauseMenu();
    
    
    void drawFPS();

    
private:
    void checkInput();
    void endGUI();
    void mainMenuClicked();
    void exitClicked();
    
    Player m_player;
    
    MrEngine::GLTexture m_bgTexture;
    
    ///PauseMenu m_pauseMenu;
    MrEngine::GUI m_pMenu;
    
    MrEngine::GLSLProgram m_textureProgram;
    MrEngine::GLSLProgram m_lightProgram;
    
    MrEngine::GLTexture m_texture;
    MrEngine::Camera2D m_camera;
    MrEngine::SpriteBatch m_spriteBatch;
    MrEngine::Window* m_window;
    MrEngine::DebugRenderer m_debugRenderer;
    
    std::vector<Box> m_boxes;
    Box m_ground;
    
    std::unique_ptr<b2World> m_world; ///< physics world!
    std::unique_ptr<MrEngine::SpriteFont> m_spriteFont; ///< For font rendering
    
    bool m_renderDebug = false;
    bool m_displayMenu = false;
    
    ///temporary
    float m_scale = 30.0f;
};

#endif /* GameplayScreen_hpp */
