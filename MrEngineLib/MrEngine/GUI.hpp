#ifndef GUI_hpp
#define GUI_hpp

#include "InputManager.hpp"
#include "Window.hpp"

#include <SDL2/SDL.h>
#include <SDL2/SDL_syswm.h>
#include <imgui/imgui.h>
#include <glm/glm.hpp>

#include <stdio.h>

namespace MrEngine {
    
    class GUI
    {
    public:
        GUI();
        ~GUI();
        
        /// NOTE: upon coming into focus, the gui needs to update for the variables specific to that GUI object
        
        /// Sets up ImGui binding (Only needs to be called once)
        void init(MrEngine::Window* window, bool isDisplayed);
        
        void destroy();
        
        void hide();
        
        void display();
        
        /// Main rendering function specific to each gui
        //virtual void draw() = 0;
        
        /// Update the necessary data for the GUI to function
        void update(MrEngine::InputManager& inputManager, float deltaTime);
        
        /// For Processing Inputs using Input Manager
        void processInput(MrEngine::InputManager& inputManager); ///< can probably delete this bad boy
        
        /// For procesing inputs by directly using SDL_Events
        void processEvent(SDL_Event* event);
        
        void createDeviceObjects();
        
        /// Loads a specified font from file
        void loadFont(const char* fontFile, float size);
        
        /// Creates the font texture for use in all GUI's
        void createFontsTexture();
        
        /// ImGui render function
        static void renderDrawLists(ImDrawData* draw_data);
        //static void getRDL(ImDrawData* drawData) { renderDrawLists(drawData); }
        
        static const char* getClipboardText() { return SDL_GetClipboardText(); }
        static void setClipboardText(const char* text) { SDL_SetClipboardText(text); }
        static bool m_created;
        
        bool displayed = false; ///< IF YOURE ONLY USING THiS INTERNALLY CHANGE IT TO PRIV!!!!
    private:
        
        GLuint m_fontTexture = 0; ///< fonts are unique to each menu
        bool m_mousePressed[3] = { false, false, false }; ///< right,left,middle click UPDATE IN PROCESSin
        float m_mouseWheel = 0.0f;
        int m_numFonts = 0;
        
        //static member variables, same for all GUI objects
        static MrEngine::Window* m_window; ///< static window for ALL created gui object (game window)
        static int m_shaderHandle, m_vertHandle, m_fragHandle;
        static int m_attribLocationTex, m_attribLocationProjMtx;
        static int m_attribLocationPosition, m_attribLocationUV, m_attribLocationColor;
        static unsigned int m_vboHandle, m_vaoHandle, m_elementsHandle;
    };
    
    
    
} /* namespace MrEngine */

#endif /* GUI_hpp */
