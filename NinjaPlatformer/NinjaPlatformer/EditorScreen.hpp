//
//  EditorScreen.hpp
//  NinjaPlatformer
//
//  Created by Mark Griffith on 2016-08-24.
//  Copyright © 2016 Mark Griffith. All rights reserved.
//

#ifndef EditorScreen_hpp
#define EditorScreen_hpp

#include "ScreenIndices.hpp"

#include <IGameScreen.hpp>
#include <Window.hpp>
#include <Camera2D.hpp>
#include <GUI.hpp>
#include <ScreenList.hpp>
#include <GLSLProgram.hpp>

#include <Box2D/Box2D.h>
#include <stdio.h>

enum class PhysicsMode
{
    RIGID,
    DYNAMIC
};

enum class ObjectType
{
    PLAYER,
    PLATFORM,
    LIGHT,
    FINISH
};

enum class SelectionMode
{
    SELECT,
    PLACE
};

class EditorScreen : public MrEngine::IGameScreen
{
public:
    EditorScreen(MrEngine::Window* window);
    ~EditorScreen();
    
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
    
private:
    void drawUI();
    void checkInput();
    void endGUI();
    
    void startGameClicked();
    void mainMenuClicked();
    void exitClicked();
    
    int m_nextScreenIndex = SCREEN_INDEX_GAMEPLAY;
    
    PhysicsMode m_physicsMode = PhysicsMode::RIGID;
    ObjectType m_objectType = ObjectType::PLAYER;
    SelectionMode m_selectMode = SelectionMode::SELECT;
    
    float m_red = 125.0f;
    float m_green = 125.0f;
    float m_blue = 125.0f;
    float m_alpha = 255.0f;
    
    bool m_drawDebug = false;
    
    std::unique_ptr<b2World> m_world;

    MrEngine::GUI m_menuGUI; ///<< WANT TO CHANGE TO m_editorGUI
    
    MrEngine::Camera2D m_camera;
    MrEngine::Window* m_window;
    
    MrEngine::GLSLProgram m_textureProgram;
    MrEngine::GLSLProgram m_lightProgram;
    
    float m_scale = 20.0f;
    
};

#endif /* EditorScreen_hpp */
