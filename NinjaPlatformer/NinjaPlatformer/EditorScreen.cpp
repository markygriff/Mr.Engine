//
//  EditorScreen.cpp
//  NinjaPlatformer
//
//  Created by Mark Griffith on 2016-08-24.
//  Copyright © 2016 Mark Griffith. All rights reserved.
//

#include "EditorScreen.hpp"

#include <IMainGame.hpp>
#include <ResourceManager.hpp>
#include <IOManager.hpp>
#include <LevelReaderWriter.hpp>

const b2Vec2 GRAVITY(0.0f, -6.0f);

EditorScreen::EditorScreen(MrEngine::Window* window) : m_window(window)
{
    m_screenIndex = SCREEN_INDEX_EDITOR;
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
    m_camera.setScale(32.0f); ///< pixel per meter
    
    //initialize menu
    m_menuGUI.init(m_window, true);
    m_menuGUI.loadFont("../Assets/Fonts/extrafonts/DejaVuSans.ttf", 14.0f);
    ImGui::SetNextWindowPos(ImVec2(10, 10), ImGuiSetCond_Always);
    
    m_debugRenderer.init();
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
    
    //initialize world physics
    m_world = std::make_unique<b2World>(GRAVITY);
}

void EditorScreen::onExit()
{
    
    m_saveItems.clear();
    m_loadItems.clear();
    
    m_debugRenderer.dispose();
    m_textureProgram.dispose();
    m_lightProgram.dispose();
    m_spriteBatch.dispose();
    m_world.reset();
    m_boxes.clear();
    m_lights.clear();
    
    clearLevel();
    
    //stop the world from updating
    m_active = false;
}

void EditorScreen::update(float deltaTime)
{
    checkInput();
    m_menuGUI.update(m_game->inputManager, deltaTime);
    m_camera.update();
    
    //if the game is live, update world physics
    if (m_active)
    {
        m_player.update(m_game->inputManager, deltaTime);
        //update physics using time step!
        m_world->Step(deltaTime, 6, 2);

    }
}

void EditorScreen::draw(float deltaTime)
{
    //clear the screen
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glClearColor(0.1f, 0.1f, 0.1f, 255);
    
    drawWorld();
    drawUI();
}

void EditorScreen::drawUI()
{
    ImGuiIO& io = ImGui::GetIO();
    
    //before drawing the UI, we draw the outlines of the objects we are placing
    if (m_selectMode == SelectMode::PLACE)
    {
        glm::vec2 pos = m_camera.convertScreenToWorld(glm::vec2(m_game->inputManager.getMouseCoords()));
        
        if (m_objectType == ObjectType::PLATFORM)
        {
            glm::vec4 debugBox;
            debugBox.x = pos.x - m_boxWidth / 2;
            debugBox.y = pos.y - m_boxHeight / 2;
            debugBox.z = m_boxWidth;
            debugBox.w = m_boxHeight;
            
            m_debugRenderer.drawBox(debugBox, MrEngine::ColorRGBA8(255, 255, 255, 255), m_angle);
            m_debugRenderer.end();
            m_debugRenderer.render(m_camera.getCameraMatrix(), 2.0f);
        }
        else if (m_objectType == ObjectType::LIGHT)
        {
            //create light to be placed
            Light light;
            light.color = MrEngine::ColorRGBA8(m_red, m_green, m_blue, m_alpha);
            light.position = pos;
            light.size = m_lightSize;
            
            //render light to be placed
            m_lightProgram.use();
            GLint pUniform = m_textureProgram.getUniformLocation("P");
            glUniformMatrix4fv(pUniform, 1, GL_FALSE, &m_camera.getCameraMatrix()[0][0]);
            //additive blending
            glBlendFunc(GL_SRC_ALPHA, GL_ONE);
            m_spriteBatch.begin();
            light.draw(m_spriteBatch);
            m_spriteBatch.end();
            m_spriteBatch.renderBatch();
            m_lightProgram.unuse();
            //restore blending
            glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
        }
        
    }
    //draw outline of current selected object
    else if (m_selectMode == SelectMode::SELECT)
    {
        if (m_currentLight != NONE)
        {
            //render inner radius
            m_debugRenderer.drawCircle(m_lights[m_currentLight].position, MrEngine::ColorRGBA8(1.0f, 1.0f, 1.0f, 255), LIGHT_SELECT_RADIUS);
            //render outer radius
            m_debugRenderer.drawCircle(m_lights[m_currentLight].position, m_lights[m_currentLight].color,  m_lights[m_currentLight].size);
            
            m_debugRenderer.end();
            m_debugRenderer.render(m_camera.getCameraMatrix(), 2.0f);
        }
        
        if (m_currentBox != NONE)
        {
            glm::vec4 destRect;
            destRect.x = m_boxes[m_currentBox].getBody()->GetPosition().x - m_boxes[m_currentBox].getDimensions().x / 2.0f;
            destRect.y = m_boxes[m_currentBox].getBody()->GetPosition().y - m_boxes[m_currentBox].getDimensions().y / 2.0f;
            
            destRect.z = m_boxes[m_currentBox].getDimensions().x;
            destRect.w = m_boxes[m_currentBox].getDimensions().y;
            
            //render the outline of the box
            m_debugRenderer.drawBox(destRect, MrEngine::ColorRGBA8(255, 255, 255, 255), m_boxes[m_currentBox].getBody()->GetAngle());
            m_debugRenderer.end();
            m_debugRenderer.render(m_camera.getCameraMatrix(), 2.0f);
        }
    }

    
    bool alwaysTrue = true;
    
    if (m_menuGUI.displayed)
    {
        ImGui::SetNextWindowSize(ImVec2(m_window->getScreenWidth() / 2.35f, m_window->getScreenHeight() / 2.20), ImGuiSetCond_Always);
        ImGui::SetNextWindowPos(ImVec2(0, 0), ImGuiSetCond_Always);
        
        ImGui::Begin("Game Editor", &alwaysTrue);
        
        ImGui::Spacing();
        
        if (ImGui::CollapsingHeader("Level Creator"))
        {
            ImGui::Spacing();
            
            //COLOR
            ImGui::PushItemWidth(ImGui::GetWindowWidth() - ImGui::GetWindowWidth()/4);
            float col2[4] = { m_red / 255, m_green / 255, m_blue / 255, m_alpha / 255 };
            ImGui::ColorEdit4("Object Color", col2);
            m_red = col2[0]*255;
            m_green = col2[1]*255;
            m_blue = col2[2]*255;
            m_alpha = col2[3]*255;
            ImGui::PopItemWidth();
            ImGui::Spacing();
            
            //PHYSICS
            //dynamic button
            ImGui::PushID(0);
            ImGui::PushStyleColor(ImGuiCol_Button, ImColor::HSV(0/7.0f, 0.6f, 0.6f));
            ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImColor::HSV(0/7.0f, 0.7f, 0.7f));
            ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImColor::HSV(0/7.0f, 0.9f, 0.9f));
            if (ImGui::Button("Dynamic")) m_physicsMode = PhysicsMode::DYNAMIC;

            ImGui::PopStyleColor(3);
            ImGui::PopID();
            ImGui::SameLine();
            
            //rigid button
            ImGui::PushID(1);
            ImGui::PushStyleColor(ImGuiCol_Button, ImColor::HSV(4.0f/7.0f, 0.6f, 0.6f));
            ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImColor::HSV(4.0f/7.0f, 0.7f, 0.7f));
            ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImColor::HSV(4.0f/7.0f, 0.9f, 0.9f));
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
            ImGui::PushID(2);
            ImGui::PushStyleColor(ImGuiCol_Button, ImColor::HSV(2.5/7.0f, 0.6f, 0.6f));
            ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImColor::HSV(2.5/7.0f, 0.7f, 0.7f));
            ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImColor::HSV(2.5/7.0f, 0.9f, 0.9f));
            if (ImGui::Button("Player")) m_objectType = ObjectType::PLAYER;
            ImGui::PopStyleColor(3);
            ImGui::PopID();
            ImGui::SameLine();

            //Platform button
            ImGui::PushID(1);
            ImGui::PushStyleColor(ImGuiCol_Button, ImColor::HSV(3.0f/7.0f, 0.6f, 0.6f));
            ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImColor::HSV(3.0f/7.0f, 0.7f, 0.7f));
            ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImColor::HSV(3.0f/7.0f, 0.9f, 0.9f));
            if (ImGui::Button("Platform")) m_objectType = ObjectType::PLATFORM;
            ImGui::PopStyleColor(3);
            ImGui::PopID();
            ImGui::SameLine();
            
            //Light button
            ImGui::PushID(3);
            ImGui::PushStyleColor(ImGuiCol_Button, ImColor::HSV(3.5f/7.0f, 0.6f, 0.6f));
            ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImColor::HSV(3.5f/7.0f, 0.7f, 0.7f));
            ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImColor::HSV(3.5f/7.0f, 0.9f, 0.9f));
            if (ImGui::Button("Light")) m_objectType = ObjectType::LIGHT;
            ImGui::PopStyleColor(3);
            ImGui::PopID();
            ImGui::SameLine();
            
            //display which Object Type is selected
            const char* o;
            if (m_objectType == ObjectType::PLAYER) o = "Player";
            else if (m_objectType == ObjectType::PLATFORM) o = "Platform";
            else o = "Light";
            ImGui::TextColored(ImColor(1.0f, 1.0f, 1.0f), "%s", o);
            ImGui::Spacing();
            
            if (m_objectType == ObjectType::PLAYER)
            {
                //player customization
            }
            
            //display rotations width and height sliders
            else if (m_objectType == ObjectType::PLATFORM || (m_selectMode == SelectMode::SELECT && m_currentBox != NONE))
            {
                //width slider
                ImGui::PushID(4);
                ImGui::PushStyleColor(ImGuiCol_FrameBg, ImColor::HSV(5.0/7.0f, 0.5f, 0.5f));
                ImGui::PushStyleColor(ImGuiCol_FrameBgHovered, ImColor::HSV(5.0/7.0f, 0.6f, 0.5f));
                ImGui::PushStyleColor(ImGuiCol_FrameBgActive, ImColor::HSV(5.0/7.0f, 0.7f, 0.5f));
                ImGui::PushStyleColor(ImGuiCol_SliderGrab, ImColor::HSV(5.0/7.0f, 0.9f, 0.9f));
                ImGui::SliderFloat("Width", &m_boxWidth, 0, 15.0f);
                ImGui::PopStyleColor(4);
                ImGui::PopID();
                
                //height slider
                ImGui::PushID(5);
                ImGui::PushStyleColor(ImGuiCol_FrameBg, ImColor::HSV(6.5/7.0f, 0.5f, 0.5f));
                ImGui::PushStyleColor(ImGuiCol_FrameBgHovered, ImColor::HSV(6.5/7.0f, 0.6f, 0.5f));
                ImGui::PushStyleColor(ImGuiCol_FrameBgActive, ImColor::HSV(6.5/7.0f, 0.7f, 0.5f));
                ImGui::PushStyleColor(ImGuiCol_SliderGrab, ImColor::HSV(6.5/7.0f, 0.9f, 0.9f));
                ImGui::SliderFloat("Height", &m_boxHeight, 0, 15.0f);
                ImGui::PopStyleColor(4);
                ImGui::PopID();
                
                //angle slider
                ImGui::PushID(12);
                ImGui::PushStyleColor(ImGuiCol_FrameBg, ImColor::HSV(32/255.0f, 0.5f, 0.7f));
                ImGui::PushStyleColor(ImGuiCol_FrameBgHovered, ImColor::HSV(32/255.0f, 0.6f, 0.7f));
                ImGui::PushStyleColor(ImGuiCol_FrameBgActive, ImColor::HSV(32/255.0f, 0.7f, 0.7f));
                ImGui::PushStyleColor(ImGuiCol_SliderGrab, ImColor::HSV(32/255.0f, 0.9f, 0.9f));
                ImGui::SliderAngle("Angle", &m_angle);
                ImGui::PopStyleColor(4);
                ImGui::PopID();
            }
            
            //display light size slider
            else if (m_objectType == ObjectType::LIGHT || (m_selectMode == SelectMode::SELECT && m_currentLight != NONE))
            {
                ImGui::PushID(4);
                ImGui::PushStyleColor(ImGuiCol_FrameBg, ImColor::HSV(6.0/7.0f, m_lightSize/25.0f, 0.5f));
                ImGui::PushStyleColor(ImGuiCol_FrameBgHovered, ImColor::HSV(6.0/7.0f, m_lightSize/25.0f, 0.5f));
                ImGui::PushStyleColor(ImGuiCol_FrameBgActive, ImColor::HSV(6.0/7.0f, m_lightSize/25.0f, 0.5f));
                ImGui::PushStyleColor(ImGuiCol_SliderGrab, ImColor::HSV(6.0/7.0f, m_lightSize/25.0f, 0.9f));
                ImGui::SliderFloat("Light Size", &m_lightSize, 0, 25.0f);
                ImGui::PopStyleColor(4);
                ImGui::PopID();
            }
            
            //SELECTION
            //place button
            ImGui::PushID(6);
            ImGui::PushStyleColor(ImGuiCol_Button, ImColor::HSV(0.27f, 0.6f, 0.6f));
            ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImColor::HSV(0.27f, 0.7f, 0.7f));
            ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImColor::HSV(0.27f, 0.9f, 0.9f));
            if (ImGui::Button("Place")) m_selectMode = SelectMode::PLACE;
            ImGui::PopStyleColor(3);
            ImGui::PopID();
            ImGui::SameLine();
            
            //select button
            ImGui::PushID(7);
            ImGui::PushStyleColor(ImGuiCol_Button, ImColor::HSV(0.33f, 0.6f, 0.6f));
            ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImColor::HSV(0.33f, 0.7f, 0.7f));
            ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImColor::HSV(0.33f, 0.9f, 0.9f));
            if (ImGui::Button("Select")) m_selectMode = SelectMode::SELECT;
            ImGui::PopStyleColor(3);
            ImGui::PopID();
            ImGui::SameLine();
            
            //display which SelectMode is selected
            const char* s;
            if (m_selectMode == SelectMode::PLACE) s = "Place";
            else s = "Select";
            ImGui::TextColored(ImColor(1.0f, 1.0f, 1.0f), "%s", s);
            ImGui::Spacing();
            
            //delete button
            ImGui::PushID(8);
            ImGui::PushStyleColor(ImGuiCol_Button, ImColor::HSV(0.67f, 0.6f, 0.6f));
            ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImColor::HSV(0.67f, 0.7f, 0.7f));
            ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImColor::HSV(0.67f, 0.9f, 0.9f));
            if (ImGui::Button("Delete"))
            {
                //immediately remove selected object from stack
                if (m_currentLight != NONE)
                {
                    m_lights.erase(m_lights.begin() + m_currentLight);
                }
                if (m_currentBox != NONE)
                {
                    m_boxes[m_currentBox].destroy(m_world.get());
                    m_boxes.erase(m_boxes.begin() + m_currentBox);
                }
            }
            ImGui::PopStyleColor(3);
            ImGui::PopID();
            ImGui::Spacing();ImGui::Spacing();ImGui::Spacing();
            
            //debug toggle
            ImGui::PushID(9);
            ImGui::PushStyleColor(ImGuiCol_Button, ImColor::HSV(0.75f, 0.6f, 0.6f));
            ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImColor::HSV(0.75f, 0.7f, 0.7f));
            ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImColor::HSV(0.75f, 0.9f, 0.9f));
            if (ImGui::Button("Debug Render")) m_drawDebug ^= 1;
            ImGui::PopStyleColor(3);
            ImGui::PopID();
            ImGui::SameLine();
            
            const char* d;
            if (m_drawDebug) d = "ON";
            else d = "OFF";
            ImGui::TextColored(ImColor(1.0f, 1.0f, 1.0f), "%s", d);
            ImGui::Spacing();ImGui::Spacing();ImGui::Spacing();
            
            //Play button
            ImGui::PushID(10);
            ImGui::PushStyleColor(ImGuiCol_Button, ImColor::HSV(1.0f, 0.6f, 0.6f));
            ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImColor::HSV(1.0f, 0.7f, 0.7f));
            ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImColor::HSV(1.0f, 0.9f, 0.9f));
            if (ImGui::Button("PLAY"))
            {
                //set game to active
                m_active = true;
            }
            ImGui::PopStyleColor(3);
            ImGui::PopID();
            ImGui::SameLine();

            //Pause button
            ImGui::PushID(13);
            ImGui::PushStyleColor(ImGuiCol_Button, ImColor::HSV(0.95f, 0.6f, 0.6f));
            ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImColor::HSV(0.95f, 0.7f, 0.7f));
            ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImColor::HSV(0.95f, 0.9f, 0.9f));
            if (ImGui::Button("PAUSE"))
            {
                //check if there is a player character to play with
                if (m_active) m_active = false;
            }
            ImGui::PopStyleColor(3);
            ImGui::PopID();
            ImGui::SameLine();
            
            //Reset button
            ImGui::PushID(11);
            ImGui::PushStyleColor(ImGuiCol_Button, ImColor::HSV(0.1f, 0.6f, 0.6f));
            ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImColor::HSV(0.1f, 0.7f, 0.7f));
            ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImColor::HSV(0.1f, 0.9f, 0.9f));
            if (ImGui::Button("RESET"))
            {
                
            }
            ImGui::PopStyleColor(3);
            ImGui::PopID();
            ImGui::Spacing();ImGui::Spacing();
            
            //Save button
            ImGui::PushID(13);
            ImGui::PushStyleColor(ImGuiCol_Button, ImColor(0.0f, 0.0f, 0.0f));
            ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImColor::HSV(0.95f, 0.7f, 0.7f));
            ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImColor::HSV(0.95f, 0.9f, 0.9f));
            if (ImGui::Button("SAVE")) m_saveMenu = true; saveClicked();
            ImGui::PopStyleColor(3);
            ImGui::PopID();
            ImGui::SameLine();
            
            //Load button
            ImGui::PushID(11);
            ImGui::PushStyleColor(ImGuiCol_Button, ImColor(0.0f, 0.0f, 0.0f));
            ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImColor::HSV(0.1f, 0.7f, 0.7f));
            ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImColor::HSV(0.1f, 0.9f, 0.9f));
            if (ImGui::Button("LOAD")) { m_loadMenu = true; loadClicked(); }
            ImGui::PopStyleColor(3);
            ImGui::PopID();
            ImGui::Spacing();
            
            //save pop-up menu
            if (m_saveMenu) showSaveMenu(&m_saveMenu);
            
            //load pop-up menu
            if (m_loadMenu) showLoadMenu(&m_loadMenu);
        }
        
        if (ImGui::CollapsingHeader("Help"))
        {
            ImGui::Text("User Guide:\n");
            
            ImGui::Text("Hello! Welcome to my very first editor screen.");
            ImGui::BulletText("Create an object by selecting either Player, Platform, or Light.\nLeft click to place object.");
            ImGui::BulletText("Platforms can either be Dynamic (able to move) or Rigid (immovable).");
            ImGui::BulletText("Customize each object's color with the color sliders.");
            ImGui::BulletText("Place an object by clicking 'Place'");
            ImGui::BulletText("Select a created object by clicking 'Select'");
            ImGui::BulletText("If an object is selected, press 'Delete' to destroy it.");
            ImGui::BulletText("Mouse Wheel to zoom.");
            ImGui::BulletText("Right click and drag to move around the map.");
            ImGui::BulletText("Debug Rendering:\n"
                              "- Green outline means Dynamic object\n"
                              "- Red outline means Static object"
                              );
            ImGui::BulletText("Press PLAY/PAUSE to play/pause your created level.");
            //ImGui::BulletText("Press RESET to reset your current level.");
            ImGui::BulletText("Press 'SAVE' to save the current state of your level");
            ImGui::BulletText("Press 'LOAD' to load your last save state.");
        }
        
        if (ImGui::CollapsingHeader("Exit"))
        {
            if (ImGui::Button("Start Game")) { endGUI(); startGameClicked(); return; }
            if (ImGui::Button("Main Menu")) { endGUI(); mainMenuClicked(); return; }
        }
        
        ImGui::End();
        
        //todo: change the light according to any changes in the editor
        updateSelectedBox();
        updateSelectedLight();
    }
    
    // Rendering
    glViewport(0, 0, io.DisplaySize.x, io.DisplaySize.y);
    
    ImGui::Render(); ///< calls set renderdrawlists function
}

void EditorScreen::showSaveMenu(bool* open)
{
    //Draw the menu
    ImGui::SetNextWindowSize(ImVec2(300, 220), ImGuiSetCond_Always);
    if (ImGui::Begin("Load Menu", open, ImGuiWindowFlags_MenuBar))
    {
        if (ImGui::BeginMenuBar())
        {
            if (ImGui::BeginMenu("File"))
            {
                if (ImGui::MenuItem("Close")) *open = false;
                ImGui::EndMenu();
            }
            ImGui::EndMenuBar();
        }
        
        //left
        static std::string levelPath;
        
        ImGui::TextWrapped("What would you like to name this level?");
        
        static char buf1[64] = ""; ImGui::InputText("default", buf1, 64);
        
        levelPath = buf1;
        
        ImGui::TextWrapped("LevelPath: %s", levelPath.c_str());
        ImGui::TextWrapped("Press the Save button below to save the current level");
        
        if (ImGui::Button("Save")) { saveLevel(levelPath); *open = false; }
    }
    ImGui::End();
}

void EditorScreen::showLoadMenu(bool* open)
{
    //Draw the menu
    ImGui::SetNextWindowSize(ImVec2(300, 220), ImGuiSetCond_Always);
    if (ImGui::Begin("Load Menu", open, ImGuiWindowFlags_MenuBar))
    {
        if (ImGui::BeginMenuBar())
        {
            if (ImGui::BeginMenu("File"))
            {
                if (ImGui::MenuItem("Close")) *open = false;
                ImGui::EndMenu();
            }
            ImGui::EndMenuBar();
        }
        
        // left
        static int selected = 0;
        static std::string levelPath;
        ImGui::BeginChild("left pane", ImVec2(150, 0), true);
        for (int i = 0; i < m_loadItems.size(); i++)
        {
            char label[128];
            sprintf(label, "%s", m_loadItems[i].c_str());
            if (ImGui::Selectable(label, selected == i))
            {
                selected = i;
                levelPath = m_loadItems[i];
            }
        }
        ImGui::EndChild();
        ImGui::SameLine();
        
        // right
        ImGui::BeginGroup();
        ImGui::BeginChild("item view", ImVec2(0, -ImGui::GetItemsLineHeightWithSpacing())); // Leave room for 1 line below us
        ImGui::Text("Load Level: %s?", levelPath.c_str());
        ImGui::Separator();
        ImGui::TextWrapped("Press the Load button below to load the selected level");
        ImGui::EndChild();
        
        ImGui::BeginChild("buttons");
        if (ImGui::Button("Load")) { loadLevel(levelPath); *open = false; }
        ImGui::EndChild();
        
        ImGui::EndGroup();
    }
    ImGui::End();
}

void EditorScreen::drawWorld()
{
    m_textureProgram.use();
    
    // Upload texture uniform
    GLint textureUniform = m_textureProgram.getUniformLocation("mySampler");
    glUniform1i(textureUniform, 0);
    glActiveTexture(GL_TEXTURE0);
    
    // Camera matrix
    glm::mat4 projectionMatrix = m_camera.getCameraMatrix();
    GLint pUniform = m_textureProgram.getUniformLocation("P");
    glUniformMatrix4fv(pUniform, 1, GL_FALSE, &projectionMatrix[0][0]);
    
    
    // Draw all the boxes and the player
    m_spriteBatch.begin();
    
    for (auto& box : m_boxes) {
        box.draw(m_spriteBatch);}
    
    if (m_playerCreated) m_player.draw(m_spriteBatch, 1.0f);
    
    m_spriteBatch.end();
    m_spriteBatch.renderBatch();
    m_textureProgram.unuse();

    // Draw lights
    m_lightProgram.use();
    pUniform = m_textureProgram.getUniformLocation("P");
    glUniformMatrix4fv(pUniform, 1, GL_FALSE, &projectionMatrix[0][0]);
    
    // Additive blending
    glBlendFunc(GL_SRC_ALPHA, GL_ONE);
    
    m_spriteBatch.begin();
    
    for (auto& l : m_lights)
    {l.draw(m_spriteBatch);
    }
    
    m_spriteBatch.end();
    m_spriteBatch.renderBatch();
    
    m_lightProgram.unuse();
    
    // Restore alpha blending
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    
    // Debug rendering
    if (m_drawDebug) {
        // Player collision
        if (m_playerCreated) m_player.drawDebug(m_debugRenderer);
        // Render dynamic box borders
        glm::vec4 destRect;
        for (auto& box : m_boxes) {
            destRect.x = box.getBody()->GetPosition().x - box.getDimensions().x / 2.0f;
            destRect.y = box.getBody()->GetPosition().y - box.getDimensions().y / 2.0f;
            destRect.z = box.getDimensions().x;
            destRect.w = box.getDimensions().y;
            // Dynamic is green, static is red.
            if (box.isDynamic()) {
                m_debugRenderer.drawBox(destRect, MrEngine::ColorRGBA8(0, 255, 0, 255), box.getBody()->GetAngle());
            } else {
                m_debugRenderer.drawBox(destRect, MrEngine::ColorRGBA8(255, 0, 0, 255), box.getBody()->GetAngle());
            }
        }
        // Render magenta light selection radius
        for (auto& light : m_lights) {
            m_debugRenderer.drawCircle(light.position, MrEngine::ColorRGBA8(255, 0, 255, 255), LIGHT_SELECT_RADIUS);
        }
        // Draw axis
        // +X axis
        m_debugRenderer.drawLine(glm::vec2(0.0f), glm::vec2(100000.0f, 0.0f), MrEngine::ColorRGBA8(255, 0, 0, 200));
        // -X axis
        m_debugRenderer.drawLine(glm::vec2(0.0f), glm::vec2(-100000.0f, 0.0f), MrEngine::ColorRGBA8(255, 0, 0, 100));
        // +Y axis
        m_debugRenderer.drawLine(glm::vec2(0.0f), glm::vec2(0.0f, 100000.0f), MrEngine::ColorRGBA8(0, 255, 0, 200));
        // -Y axis
        m_debugRenderer.drawLine(glm::vec2(0.0f), glm::vec2(0.0f, -100000.0f), MrEngine::ColorRGBA8(0, 255, 0, 100));
        
        m_debugRenderer.end();
        m_debugRenderer.render(m_camera.getCameraMatrix(), 2.0f);
    }
}

void EditorScreen::clearLevel() {
    m_boxes.clear();
    m_lights.clear();
    m_grabbedPlayer = false;
    m_world.reset();
    m_world = std::make_unique<b2World>(GRAVITY);
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
            case SDL_MOUSEBUTTONDOWN:
                m_isDragging = false;
                mousedownEvent(event);
                break;
                
            case SDL_KEYUP:
                m_isDragging = false;
                m_grabbedPlayer = false;
                break;
                
            case SDL_MOUSEMOTION:
                mouseMotionEvent(event);
                break;
                
            case SDL_MOUSEWHEEL:
                if (!isMouseOnMenu())
                {
                    m_camera.offsetScale(m_camera.getScale() * event.wheel.y * 0.1f);
                }
                break;
        }
    }
}

bool EditorScreen::inLightSelect(const Light& l, const glm::vec2& pos)
{
    return (glm::length(pos - l.position) <= LIGHT_SELECT_RADIUS);
}

void EditorScreen::mousedownEvent(const SDL_Event &event)
{
    static MrEngine::GLTexture tex = MrEngine::ResourceManager::getTexture("../Assets/Textures/bricks_top.png");
    
    glm::vec2 pos;
    glm::vec4 uvRect;
    
    switch (event.button.button)
    {
        case SDL_BUTTON_LEFT:
            
            //only select or place objects when mouse is not on the menu
            if (isMouseOnMenu()) break;
            
            //selection mode
            if (m_selectMode == SelectMode::SELECT)
                {
                    pos = m_camera.convertScreenToWorld(glm::vec2(event.button.x, event.button.y));
                    
                    //first we check the lights
                    if (m_currentLight != NONE && inLightSelect(m_lights[m_currentLight], pos))
                    {
                        //we selected ourself, do nothing.
                    }
                    //else look for the selected light
                    else
                    {
                        //deselect light
                        m_currentLight = NONE;
                        
                        //find the new current selected light
                        for (int i = 0; i < m_lights.size(); i++)
                        {
                            if (inLightSelect(m_lights[i], pos))
                            {
                                m_currentLight = i;
                                break;
                            }
                        }
                    }
                    
                    //if we already selected a light
                    if (m_currentLight != NONE && inLightSelect(m_lights[m_currentLight], pos) && !isMouseOnMenu())
                    {
                        // Get the offset from the center so we can drag correctly
                        m_selectOffset = pos - m_lights[m_currentLight].position;
                        m_currentBox = NONE;
                        m_isDragging = true;
                        
                        //update the display values
                        m_red = m_lights[m_currentLight].color.r;
                        m_green = m_lights[m_currentLight].color.g;
                        m_blue = m_lights[m_currentLight].color.b;
                        m_alpha = m_lights[m_currentLight].color.a;
                        m_lightSize = m_lights[m_currentLight].size;
                        break;
                    }
                    
                    //box selection checking
                    if (m_currentBox != NONE && m_boxes[m_currentBox].testPoint(pos.x, pos.y))
                    {
                        //we selected ourself, do nothing.
                    }
                    else
                    {
                        //deselect current box
                        m_currentBox = NONE;
                        //find new current box
                        for (int i = 0; i < m_boxes.size(); i++)
                        {
                            if (m_boxes[i].testPoint(pos.x, pos.y))
                            {
                                m_currentBox = i;
                                break;
                            }
                        }
                    }
                    
                    //if we selected a box
                    if (m_currentBox != NONE)
                    {
                        m_selectOffset = pos - m_boxes[m_currentBox].getPosition();
                        m_currentLight = NONE;
                        m_isDragging = true;
                        
                        //update the display values
                        m_red = m_boxes[m_currentBox].getColor().r;
                        m_green = m_boxes[m_currentBox].getColor().g;
                        m_blue = m_boxes[m_currentBox].getColor().b;
                        m_boxWidth = m_boxes[m_currentBox].getDimensions().x;
                        m_boxHeight = m_boxes[m_currentBox].getDimensions().y;
                        m_angle = m_boxes[m_currentBox].getBody()->GetAngle();
                        
                        if (m_boxes[m_currentBox].isDynamic()) m_physicsMode = PhysicsMode::DYNAMIC;
                        else m_physicsMode = PhysicsMode::RIGID;
                    }
                    
                    //if we have created the player we can pick it up
                    if (m_playerCreated)
                    {
                        if (m_player.getCapsule().testPoint(pos.x, pos.y))
                        {
                            m_grabbedPlayer = true;
                            //todo: Maybe implement a way of dragging player
                        }
                    }
                }
            
            else if (m_selectMode == SelectMode::PLACE)
            {
                switch (m_objectType)
                {
                    case ObjectType::PLATFORM:
                    {
                        //create new box platform
                        Box newBox;
                        pos = m_camera.convertScreenToWorld(glm::vec2(event.button.x, event.button.y));
                        uvRect.x = pos.x;
                        uvRect.y = pos.y;
                        uvRect.z = m_boxWidth;
                        uvRect.w = m_boxHeight;
                        newBox.init(m_world.get(), pos, glm::vec2(m_boxWidth, m_boxHeight), tex, MrEngine::ColorRGBA8(m_red, m_green, m_blue, m_alpha), false, m_physicsMode == PhysicsMode::DYNAMIC,  m_angle, uvRect);
                        
                        //pos is the upper left corner of the box
                        
                        m_boxes.push_back(newBox);
                        break;
                    }
                        
                    case ObjectType::PLAYER:
                    {
                        //destroy player
                        m_player.destroy(m_world.get());
                        
                        //re-create player in updated pos
                        pos = m_camera.convertScreenToWorld(glm::vec2(event.button.x, event.button.y));
                        
                        m_player.init(m_world.get(), pos, glm::vec2(2.0f, 2.0f), glm::vec2(1.0f, 1.8f), MrEngine::ColorRGBA8(m_red, m_green, m_blue, m_alpha));
                        
                        m_playerCreated = true;
                        break;
                    }
                        
                    case ObjectType::LIGHT:
                    {
                        //create the new light
                        Light newLight;
                        
                        newLight.position = m_camera.convertScreenToWorld(glm::vec2(event.button.x, event.button.y));
                        newLight.size = m_lightSize;
                        newLight.color = MrEngine::ColorRGBA8(m_red, m_green, m_blue, m_alpha);
                        
                        //add new light to lights
                        m_lights.push_back(newLight);
                        break;
                    }
                        
                    case ObjectType::DONE:
                    {
                        break;
                    }
                }
            }
            break;
    }
}

void EditorScreen::mouseMotionEvent(const SDL_Event &event)
{
    //basically, we need to update the positions of any selected object to match the cursor
    //be sure to take not of offset
    
    ImGuiIO& io = ImGui::GetIO();
    
    //use right click ot move camera around
    if (io.MouseDown[RIGHT])
    {
        //set offset and USE ASPECT RATIO
        m_camera.offsetPosition(glm::vec2(-event.motion.xrel / 3, event.motion.yrel * m_camera.getDimRatio() / 8));
    }
    
    //dragging an object
    if (io.MouseDown[LEFT] && (m_isDragging == true))
    {
        //light
        if (m_currentLight != NONE)
        {
            glm::vec2 pos = m_camera.convertScreenToWorld(glm::vec2(event.motion.x, event.motion.y)) - m_selectOffset;
            
            //update light to match cursor pos
            updateSelectedLight(pos);
        }
        
        //box
        if (m_currentBox != NONE)
        {
            glm::vec2 pos = m_camera.convertScreenToWorld(glm::vec2(event.motion.x, event.motion.y)) - m_selectOffset;

            //update box to match cursor pos
            updateSelectedBox(pos);
        }
        
        //player
        if (m_playerCreated)
        {
            //todo: drag player functionality
        }
    }
}

void EditorScreen::updateSelectedBox()
{
    if (m_currentBox == NONE) return;
    updateSelectedBox(m_boxes[m_currentBox].getPosition());
}

void EditorScreen::updateSelectedBox(const glm::vec2& newPosition)
{
    if (m_currentBox == NONE) return;

    static MrEngine::GLTexture tex = MrEngine::ResourceManager::getTexture("../Assets/Textures/bricks_top.png");
    
    glm::vec4 uvRect;
    uvRect.x = newPosition.x;
    uvRect.y = newPosition.y;
    uvRect.z = m_boxWidth;
    uvRect.w = m_boxHeight;
    Box newBox;
    
    newBox.init(m_world.get(), newPosition, glm::vec2(m_boxWidth, m_boxHeight), tex, MrEngine::ColorRGBA8(m_red, m_green, m_blue, m_alpha), false, m_physicsMode == PhysicsMode::DYNAMIC, m_angle, uvRect);
    
    //destroy old box and replace with new one
    m_boxes[m_currentBox].destroy(m_world.get());
    m_boxes[m_currentBox] = newBox;
}

void EditorScreen::updateSelectedLight()
{
    if (m_currentLight == NONE) return;
    updateSelectedBox(m_lights[m_currentLight].position);
}

void EditorScreen::updateSelectedLight(const glm::vec2& newPosition)
{
    if (m_currentLight == NONE) return;
    Light newLight;
    newLight.position = newPosition;
    newLight.size = m_lightSize;
    newLight.color = MrEngine::ColorRGBA8(m_red, m_green, m_blue, m_alpha);
    m_lights[m_currentLight] = newLight;
}

bool EditorScreen::isMouseOnMenu()
{
    ImGuiIO& io = ImGui::GetIO();
    
    return (io.MousePos.x < m_window->getScreenWidth() / 2.35 && io.MousePos.y < m_window->getScreenHeight() / 2.2);
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

bool EditorScreen::saveClicked()
{
    return true;
}

bool EditorScreen::saveLevel(std::string saveItem)
{
    //can only save if we've created a player
    if (!m_playerCreated)
    {
        std::cout << "Must create Player before saving!\n"; ///< make this a popup assert
        return true;
    }
    
    std::cout << "Saving...\n";
    
    //first we create the directory
    MrEngine::IOManager::makeDirectory("Levels");
    
    std::string saveFile = "Levels/" + saveItem;

    if (LevelReaderWriter::saveAsText(saveFile, m_player, m_boxes, m_lights))
    {
        puts("File successfully saved.");
    }
    else
    {
        puts("Failed to save file.");
    }
    return true;
}

bool EditorScreen::loadClicked()
{
    m_loadItems.clear();
    
    //get all directory entries
    std::vector<MrEngine::DirEntry> entries;
    MrEngine::IOManager::getDirectoryEntries("Levels", entries);
    
    // Add all files to list box
    for (auto& e : entries) {
        // Don't add directories
        if (!e.isDirectory) {
            // Remove "Levels/" substring
            e.path.erase(0, std::string("Levels/").size());
            m_loadItems.push_back(e.path);
        }
    }
    return true;
}

bool EditorScreen::loadLevel(std::string loadItem)
{
    std::cout << "Loading...\n";
    clearLevel();
    
    std::string level = "Levels/" + loadItem;
    
    if (LevelReaderWriter::loadAsText(level, m_world.get(), m_player, m_boxes, m_lights))
    {
        std::cout << "Successfully loaded game\n";
        m_playerCreated = true;
    }
    else
    {
        std::cout << "Failed to load game\n";
    }
    return true;
}

void EditorScreen::exitClicked()
{
    m_currentScreenState = MrEngine::ScreenState::EXIT_APPLICATION;
}