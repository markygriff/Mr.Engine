//
//  GameplayScreen.cpp
//  NinjaPlatformer
//
//  Created by Mark Griffith on 2016-08-07.
//  Copyright © 2016 Mark Griffith. All rights reserved.
//

#include "GameplayScreen.hpp"
#include "Light.h"

#include <ScreenList.hpp>
#include <IMainGame.hpp>
#include <ResourceManager.hpp>

#include <SDL2/SDL.h>
#include <OpenGL/gl3.h>

#include <iostream>
#include <random>
#include <ctime>


GameplayScreen::GameplayScreen(MrEngine::Window* window) :
    m_window(window)
{
    m_screenIndex = SCREEN_INDEX_GAMEPLAY;
}

GameplayScreen::~GameplayScreen()
{
    //empty
}

int GameplayScreen::getNextScreenIndex() const
{
    return SCREEN_INDEX_NONE;
}

int GameplayScreen::getPrevScreenIndex() const
{
    return SCREEN_INDEX_MAINMENU;
}

void GameplayScreen::build() 
{
    
}

void GameplayScreen::destroy() 
{
    
}

void GameplayScreen::onEntry()
{
    m_spriteFont = std::make_unique<MrEngine::SpriteFont>("../Assets/Fonts/chintzy.ttf", 40);
    
    m_debugRenderer.init();
    
    b2Vec2 gravity(0.0f, -6.0f);
    m_world = std::make_unique<b2World>(gravity);
    
    //make the ground
    b2BodyDef groundBodyDef;
    //define size of the box
    b2PolygonShape groundBox;
    groundBox.SetAsBox(50.0f, 0.0f);
    //set the placement of the ground in space
    groundBodyDef.position.Set(0.0f, -((m_window->getScreenHeight() / 2.0f)) / m_scale  );
    b2Body* groundBody = m_world->CreateBody(&groundBodyDef);
    
    //make the ground fixture
    groundBody->CreateFixture(&groundBox, 0.0f);
    
    //load the box texture
    m_texture = MrEngine::ResourceManager::getTexture("../Assets/Textures/bricks_top.png");
    
    //make some random boxes
    std::mt19937 randGenerator;
    std::uniform_real_distribution<float> xPos(-10.0f, 10.0f);
    std::uniform_real_distribution<float> yPos(0.0f, 25.0f);
    std::uniform_real_distribution<float> size(0.5f, 2.5f);
    std::uniform_int_distribution<int> randc(0, 255);
    const int numBoxes = 300;
    
    //create new boxes
    for (int i = 0; i < numBoxes; i++)
    {
        MrEngine::ColorRGBA8 color(randc(randGenerator), randc(randGenerator), randc(randGenerator), 255);
        Box newBox;
        float s = size(randGenerator);
        newBox.init(m_world.get(), glm::vec2(xPos(randGenerator), yPos(randGenerator)), glm::vec2(s, s), m_texture, color, false, true);
        
        m_boxes.push_back(newBox);
    }

    //initialize sprite batch
    m_spriteBatch.init();
    
    // Compile our texture shader
    m_textureProgram.compileShaders("../Assets/Shaders/textureShading.vert", "../Assets/Shaders/textureShading.frag");
    m_textureProgram.addAttribute("vertexPosition");
    m_textureProgram.addAttribute("vertexColor");
    m_textureProgram.addAttribute("vertexUV");
    m_textureProgram.linkShaders();
    
    // Compile our light shader
    m_lightProgram.compileShaders("../Assets/Shaders/lightShading.vert", "../Assets/Shaders/lightShading.frag");
    m_lightProgram.addAttribute("vertexPosition");
    m_lightProgram.addAttribute("vertexColor");
    m_lightProgram.addAttribute("vertexUV");
    m_lightProgram.linkShaders();
    
    //initialize camera
    m_camera.init(m_window->getScreenWidth(), m_window->getScreenHeight());
    //zoom out
    m_camera.setScale(m_scale); ///< pixel per meter
    //initialize player
    m_player.init(m_world.get(), glm::vec2(0.0f, 35.0f), glm::vec2(2.0f, 2.0f), glm::vec2(1.0f, 1.8f), MrEngine::ColorRGBA8(255,255,255,255));
    
    //initialize gui
    m_pMenu.init(m_window, false);
    m_pMenu.loadFont("../Assets/Fonts/extrafonts/DejaVuSans.ttf", 14.0f);
}

void GameplayScreen::onExit()
{
    m_debugRenderer.dispose();
    m_textureProgram.dispose();
    m_lightProgram.dispose();
    m_world.reset();
    m_boxes.clear();
}

void GameplayScreen::update(float deltaTime)
{
    checkInput();
    
    if (m_pMenu.displayed)
    {
        m_pMenu.update(m_game->inputManager, deltaTime);
    }
    else if (m_currentScreenState == MrEngine::ScreenState::RUNNING)
    {
        m_player.update(m_game->inputManager, deltaTime);
        //update physics using time step!
        m_world->Step(deltaTime, 6, 2);
    }

    m_camera.update();
}

void GameplayScreen::draw(float deltaTime)
{
    //draw the game
    //clear the screen
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glClearColor(0, 0, 0, 255);
    
    m_textureProgram.use();
    
    //upload texture uniform
    GLint textureUniform = m_textureProgram.getUniformLocation("mySampler");
    glUniform1i(textureUniform, 0);
    glActiveTexture(GL_TEXTURE0);
    
    //camera matrix
    glm::mat4 projectionMatrix = m_camera.getCameraMatrix();
    GLint pUniform = m_textureProgram.getUniformLocation("P"); //projection matrix uniform
    glUniformMatrix4fv(pUniform, 1, GL_FALSE, &projectionMatrix[0][0]);
    
    //begin texture rendering
    m_spriteBatch.begin();
    
    //render box textures
    for (auto& box : m_boxes)
    {
        box.draw(m_spriteBatch);
    }
    
    //render the player texture
    m_player.draw(m_spriteBatch, deltaTime);
    
    drawFPS();
    
    //end of texture rendering
    m_spriteBatch.end();
    m_spriteBatch.renderBatch();
    
    m_textureProgram.unuse();
    
    //Debug Rendering
    if (m_renderDebug)
    {
        glm::vec4 destRect;
    
        //render box collision boxes
        for (auto& box : m_boxes)
        {
            destRect.x = box.getBody()->GetPosition().x - box.getDimensions().x / 2.0f;
            destRect.y = box.getBody()->GetPosition().y - box.getDimensions().y / 2.0f;
            destRect.z = box.getDimensions().x;
            destRect.w = box.getDimensions().y;
            
            m_debugRenderer.drawBox(destRect, MrEngine::ColorRGBA8(255,255,255,255), box.getBody()->GetAngle());
        }
    
        //render player debug collision box
        m_player.drawDebug(m_debugRenderer);
        m_debugRenderer.end();
        m_debugRenderer.render(projectionMatrix, 4.0f);
    }
    
    //render some test lights
    Light playerLight;
    playerLight.color = MrEngine::ColorRGBA8(255, 255, 255, 128);
    playerLight.position = m_player.getPosition();
    playerLight.size = 25.0f;
    
    Light mouseLight;
    mouseLight.color = MrEngine::ColorRGBA8(255, 0, 255, 128);
    mouseLight.position = m_camera.convertScreenToWorld(m_game->inputManager.getMouseCoords());
    mouseLight.size = 15.0f;
    
    m_lightProgram.use();
    
    pUniform = m_textureProgram.getUniformLocation("P");
    glUniformMatrix4fv(pUniform, 1, GL_FALSE, &projectionMatrix[0][0]);
    
    //additive blending
    glBlendFunc(GL_SRC_ALPHA, GL_ONE);
    
    m_spriteBatch.begin();
    
    playerLight.draw(m_spriteBatch);
    mouseLight.draw(m_spriteBatch);
    
    m_spriteBatch.end();
    m_spriteBatch.renderBatch();
    
    m_lightProgram.unuse();

    //reset to regular alpha blending
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    
    //draw the menu
    if (m_pMenu.displayed)
    {
        drawPauseMenu();
    }
}

void GameplayScreen::drawPauseMenu()
{
    ImGuiIO& io = ImGui::GetIO();
    
    static bool show_test_window = false;
    
    //display pause menu
    if (m_pMenu.displayed)
    {
        ImGui::SetNextWindowSize(ImVec2(m_window->getScreenWidth(),m_window->getScreenHeight()), ImGuiSetCond_Always);
        ImGui::SetNextWindowPos(ImVec2(0,0), ImGuiSetCond_Always);
        
        ImGui::Begin("Game Paused", &m_pMenu.displayed);
        //ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(50,0));
        if (ImGui::Button("Resume")) m_pMenu.displayed ^= 1;
        //ImGui::SameLine();
        if (ImGui::Button("Main Menu")) { endGUI(); mainMenuClicked(); return; }
        if (ImGui::Button("Show Test Window")) { show_test_window ^= 1; }
        if (ImGui::Button("Exit")) { endGUI(); exitClicked(); return; }
        
        //ImGui::PopStyleVar();
        ImGui::End();
    }
    
    if (show_test_window)
    {
        ImGui::SetNextWindowSize(ImVec2(500, 500), ImGuiSetCond_Always);
        ImGui::ShowTestWindow(&show_test_window);
    }
    
    
    // Rendering
    glViewport(0, 0, io.DisplaySize.x, io.DisplaySize.y);
    
    ImGui::Render(); ///< calls set renderdrawlists function
}

void GameplayScreen::drawFPS()
{
    //define font color
    const MrEngine::ColorRGBA8 fontColor(255, 220, 0, 255);
    // Convert float to char *
    char buffer[64];
    sprintf(buffer, "%.1f", m_game->getFps());
    
    //render text to the screen
    m_spriteFont->draw(m_spriteBatch, buffer, glm::vec2(-m_window->getScreenWidth() / 2.0f / m_scale, m_window->getScreenHeight() / 2.0f / m_scale - (m_spriteFont->getFontHeight() / m_scale) - 1.0f),
                       glm::vec2(1.0f / m_scale), 0.0f, fontColor);
}

void GameplayScreen::checkInput()
{
    SDL_Event event;
    
    while(SDL_PollEvent(&event))
    {
        m_game->onSDLEvent(event);
        
        if (m_pMenu.displayed) m_pMenu.processEvent(&event);
        
        switch (event.type)
        {
            case SDL_KEYDOWN:
                //open pause menu
                if (m_game->inputManager.isKeyPressed(SDLK_v)) m_pMenu.displayed ^= 1;
                break;
            case SDL_MOUSEWHEEL:
                m_camera.offsetScale(m_camera.getScale() * event.wheel.y * 0.1f);
                break;
            case SDL_QUIT:
                m_currentScreenState = MrEngine::ScreenState::EXIT_APPLICATION;
                break;
        }
    }
}

void GameplayScreen::endGUI()
{
    ImGui::End();
    m_pMenu.hide();
}

void GameplayScreen::mainMenuClicked()
{
    m_pMenu.hide();
    m_currentScreenState = MrEngine::ScreenState::CHANGE_PREVIOUS;
}

void GameplayScreen::exitClicked()
{
    m_pMenu.hide();
    m_currentScreenState = MrEngine::ScreenState::EXIT_APPLICATION;
}