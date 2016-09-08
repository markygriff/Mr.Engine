#ifndef InputManager_hpp
#define InputManager_hpp

#pragma once

#include <SDL2/SDL.h>
#include <glm/glm.hpp>
#include <stdio.h>
#include <unordered_map>

#include <iostream>

namespace MrEngine
{
    
    
    class InputManager
    {
    public:
        InputManager();
        ~InputManager();
        
        void update();
        
        /// Set the key map values
        void pressKey(unsigned int keyID);
        void releaseKey(unsigned int keyID);
        
        /// Helper func for updating the current event type
        void setEventType(Uint32 eventType) { m_eventType = eventType; }
        
        /// Tells the input manager the location of the cursor
        void setMouseCoords(float x, float y);
        
        /// Returns true if key is held down
        bool isKeyDown(unsigned int keyID);
        
        /// Returns true if key is pressed
        bool isKeyPressed(unsigned int keyID);
        
        /// Updates the mouse wheel
        void setMouseWheel(int m);
        int mousewheel = 0;
        
        /// Add characters into inChars
        void addInputCharacter(char c);
        /// Add characters into inChars from a string
        void addInputCharacters(const char* chars);
        
        //getters
        glm::vec2 getMouseCoords() const { return m_mouseCoords; }
        Uint32 getEventType() { return m_eventType; }
        const char* getInChars() { return m_inChars; }
        
    private:
        
        /// Identical to isKeyDown but searches the previous key map
        bool wasKeyDown(unsigned int keyID);
        
        std::unordered_map<unsigned int, bool> m_keyMap;
        std::unordered_map<unsigned int, bool> m_prevKeyMap;
        
        glm::vec2 m_mouseCoords;
        
        Uint32 m_eventType;
        
        char m_inChars[1];
    };
    

    
} /* namespace MrEngine */



#endif /* InputManager_hpp */
