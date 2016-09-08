#ifndef Sprite_hpp
#define Sprite_hpp

#pragma once

#include "GLTexture.h"

#include <OpenGL/gl3.h>
#include <stdio.h>
#include <string>

namespace MrEngine {
    
    
    class Sprite
    {
    public:
        Sprite();
        ~Sprite();
        
        void init(float x, float y, float w, float h, std::string texturePath);
        void draw();

    private:
        float m_x,m_y,m_w,m_h;
        
        // vertex buffer object
        GLuint m_vboID = 0; // guarunteed to be 32 bits
        
        GLTexture m_texture;
        
        
    };
    

}
#endif /* Sprite_hpp */
