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
#include "Player.hpp"
#include "Box.hpp"
#include "Light.h"

#include <IGameScreen.hpp>
#include <Window.hpp>
#include <Camera2D.hpp>
#include <GUI.hpp>
#include <ScreenList.hpp>
#include <GLSLProgram.hpp>
#include <SpriteBatch.hpp>

#include <Box2D/Box2D.h>
#include <stdio.h>
#include <vector>

const float LIGHT_SELECT_RADIUS = 0.5f;
const int NONE = -1;
const int LEFT = 0;
const int RIGHT = 1;

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
    DONE
};

enum class SelectMode
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
    void drawWorld();
    
    void checkInput();
    bool inLightSelect(const Light& l, const glm::vec2& pos);
    void mousedownEvent(const SDL_Event &event);
    void mouseMotionEvent(const SDL_Event &event);
    
    void updateSelectedBox();
    void updateSelectedBox(const glm::vec2& newPosition);
    void updateSelectedBox(const float col[4]);
    void updateSelectedBox(const float w, const float h, const float angle);
    
    void updateSelectedLight();
    void updateSelectedLight(const glm::vec2& newPosition);
    
    bool isMouseOnMenu();
    
    void endGUI();
    
    void startGameClicked();
    void mainMenuClicked();
    void exitClicked();
    
    int m_nextScreenIndex = SCREEN_INDEX_GAMEPLAY;
    
    PhysicsMode m_physicsMode = PhysicsMode::DYNAMIC;
    ObjectType m_objectType = ObjectType::PLAYER;
    SelectMode m_selectMode = SelectMode::PLACE;
    
    float m_red = 125.0f;
    float m_green = 125.0f;
    float m_blue = 125.0f;
    float m_alpha = 255.0f;
    float m_lightSize = 5.0f;
    float m_boxWidth = 5.0f;
    float m_boxHeight = 5.0f;
    float m_angle = 0.0f;
    glm::vec2 m_selectOffset;
    
    bool m_playerCreated = false;
    bool m_grabbedPlayer = false;
    bool m_drawDebug = false;
    bool m_isDragging = false;
    bool m_deleteSelect = false;
    bool m_active = false;
    
    Player m_player;
    
    std::vector<Box> m_boxes;
    int m_currentBox = NONE;
    
    std::vector<Light> m_lights;
    int m_currentLight = NONE;
    
    std::unique_ptr<b2World> m_world;

    MrEngine::GUI m_menuGUI; ///<< WANT TO CHANGE TO m_editorGUI
    
    MrEngine::Camera2D m_camera;
    MrEngine::Window* m_window;
    
    MrEngine::GLSLProgram m_textureProgram;
    MrEngine::GLSLProgram m_lightProgram;
    MrEngine::DebugRenderer m_debugRenderer;
    
    MrEngine::SpriteBatch m_spriteBatch;
    
    float m_scale = 20.0f;
    
};

#endif /* EditorScreen_hpp */
