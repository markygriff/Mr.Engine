//
//  EditorScreen.cpp
//  NinjaPlatformer
//
//  Created by Mark Griffith on 2016-08-24.
//  Copyright © 2016 Mark Griffith. All rights reserved.
//

#include "EditorScreen.hpp"

#include <IMainGame.hpp>

EditorScreen::EditorScreen(MrEngine::Window* window) : m_window(window)
{
    
}

EditorScreen::~EditorScreen()
{
    //empty
}

int EditorScreen::getNextScreenIndex() const
{
    return m_nextScreenIndex;
}

int EditorScreen::getPrevScreenIndex() const
{
    return SCREEN_INDEX_MAINMENU;
}

void EditorScreen::build()
{
    
}

void EditorScreen::destroy()
{
    
}

void EditorScreen::onEntry()
{
    //initialize camera
    m_camera.init(m_window->getScreenWidth(), m_window->getScreenHeight());
    //zoom out
    m_camera.setScale(m_scale); ///< pixel per meter
    
    //initialize menu
    m_menuGUI.init(m_window, true);
    m_menuGUI.loadFont("../GUI/fonts/Futhark-Adapted.ttf", 12);
    ImGui::SetNextWindowPos(ImVec2(10, 10));
    
    //initialize world physics
    b2Vec2 gravity(0.0f, -6.0f);
    m_world = std::make_unique<b2World>(gravity);
    
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
}

void EditorScreen::onExit()
{
    
}

void EditorScreen::update(float deltaTime)
{
    m_menuGUI.update(m_game->inputManager, deltaTime);
    m_camera.update();
    checkInput();
}

void EditorScreen::draw(float deltaTime)
{
    //clear the screen
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glClearColor(100, 100, 100, 255);
    
    drawUI();
    
}

void EditorScreen::drawUI()
{
    ImGuiIO& io = ImGui::GetIO();
    
    bool alwaysTrue = true;
    
    if (m_menuGUI.displayed)
    {
        ImGui::SetNextWindowSize(ImVec2(500, 500), ImGuiSetCond_Once);
        
        ImGui::Begin("Game Editor", &alwaysTrue);
        
        ImGui::Spacing();
        
        if (ImGui::CollapsingHeader("Level Creator"))
        {
            //COLOR
            ImGui::PushItemWidth(ImGui::GetWindowWidth());
            static float col2[4] = { m_red, m_blue, m_green, m_alpha };
            ImGui::ColorEdit4("color 2", col2);
            ImGui::PopItemWidth();
            ImGui::Spacing();
            
            //PHYSICS
            //dynamic button
            ImGui::PushID(0);
            ImGui::PushStyleColor(ImGuiCol_Button, ImColor::HSV(0/7.0f, 0.6f, 0.6f));
            ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImColor::HSV(0/7.0f, 0.7f, 0.7f));
            ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImColor::HSV(0/7.0f, 0.8f, 0.8f));
            if (ImGui::Button("Dynamic")) m_physicsMode = PhysicsMode::DYNAMIC;

            ImGui::PopStyleColor(3);
            ImGui::PopID();
            ImGui::SameLine();
            
            //rigid button
            ImGui::PushID(1);
            ImGui::PushStyleColor(ImGuiCol_Button, ImColor::HSV(4.0f/7.0f, 0.6f, 0.6f));
            ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImColor::HSV(4.0f/7.0f, 0.7f, 0.7f));
            ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImColor::HSV(4.0f/7.0f, 0.8f, 0.8f));
            if (ImGui::Button("Rigid")) m_physicsMode = PhysicsMode::RIGID;
            ImGui::PopStyleColor(3);
            ImGui::PopID();
            ImGui::SameLine();
            
            //display which PhysicsMode is selected
            const char* p;
            if (m_physicsMode == PhysicsMode::DYNAMIC) p = "Dynamic";
            else p = "Rigid";
            ImGui::TextColored(ImColor(1.0f, 1.0f, 1.0f), "%s", p);
            ImGui::Spacing();
            
            //OBJECTS
            //Player button
            ImGui::PushID(0);
            ImGui::PushStyleColor(ImGuiCol_Button, ImColor::HSV(2.5/7.0f, 0.6f, 0.6f));
            ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImColor::HSV(2.5/7.0f, 0.7f, 0.7f));
            ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImColor::HSV(2.5/7.0f, 0.8f, 0.8f));
            if (ImGui::Button("Player")) m_objectType = ObjectType::PLAYER;
            ImGui::PopStyleColor(3);
            ImGui::PopID();
            ImGui::SameLine();

            //Platform button
            ImGui::PushID(1);
            ImGui::PushStyleColor(ImGuiCol_Button, ImColor::HSV(3.0f/7.0f, 0.6f, 0.6f));
            ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImColor::HSV(3.0f/7.0f, 0.7f, 0.7f));
            ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImColor::HSV(3.0f/7.0f, 0.8f, 0.8f));
            if (ImGui::Button("Platform")) m_objectType = ObjectType::PLATFORM;
            ImGui::PopStyleColor(3);
            ImGui::PopID();
            ImGui::SameLine();
            
            //Light button
            ImGui::PushID(1);
            ImGui::PushStyleColor(ImGuiCol_Button, ImColor::HSV(3.5f/7.0f, 0.6f, 0.6f));
            ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImColor::HSV(3.5f/7.0f, 0.7f, 0.7f));
            ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImColor::HSV(3.5f/7.0f, 0.8f, 0.8f));
            if (ImGui::Button("Light")) m_objectType = ObjectType::LIGHT;
            ImGui::PopStyleColor(3);
            ImGui::PopID();
            ImGui::SameLine();
            
            //display which PhysicsMode is selected
            const char* o;
            if (m_objectType == ObjectType::PLAYER) o = "Player";
            else if (m_objectType == ObjectType::PLATFORM) o = "Platform";
            else o = "Light";
            ImGui::TextColored(ImColor(1.0f, 1.0f, 1.0f), "%s", o);
            ImGui::Spacing();
            
            
        }
        
        if (ImGui::CollapsingHeader("Exit"))
        {
            if (ImGui::Button("Start Game")) { endGUI(); startGameClicked(); return; }
            if (ImGui::Button("Main Menu")) { endGUI(); mainMenuClicked(); return; }
        }
        
        ImGui::End();
    }
    
    // Rendering
    glViewport(0, 0, io.DisplaySize.x, io.DisplaySize.y);
    
    ImGui::Render(); ///< calls set renderdrawlists function
}

void EditorScreen::checkInput()
{
    SDL_Event event;
    
    while(SDL_PollEvent(&event))
    {
        m_menuGUI.processEvent(&event);
        m_game->onSDLEvent(event);
        
        switch (event.type)
        {
            case SDL_MOUSEWHEEL:
                m_camera.offsetScale(m_camera.getScale() * event.wheel.y * 0.1f);
                break;
        }
        
    }
}

void EditorScreen::endGUI()
{
    ImGui::End();
    m_menuGUI.hide();
}

void EditorScreen::startGameClicked()
{
    m_nextScreenIndex = SCREEN_INDEX_GAMEPLAY;
    m_currentScreenState = MrEngine::ScreenState::CHANGE_NEXT;
}

void EditorScreen::mainMenuClicked()
{
    m_currentScreenState = MrEngine::ScreenState::CHANGE_PREVIOUS;
}

void EditorScreen::exitClicked()
{
    m_currentScreenState = MrEngine::ScreenState::EXIT_APPLICATION;
}