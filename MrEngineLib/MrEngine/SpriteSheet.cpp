//
//  SpriteSheet.cpp
//  MrEngine
//
//  Created by Mark Griffith on 2016-09-01.
//  Copyright © 2016 Mark Griffith. All rights reserved.
//

#include "SpriteSheet.hpp"


namespace MrEngine
{

    
    void SpriteSheet::init(const GLTexture& texture, glm::ivec2& tileDims)
    {
        this->m_texture = texture;
        this->m_dims = tileDims;
    }
    
    void SpriteSheet::addAnimation(MrEngine::SpriteState spriteState, int numTiles, int tileIndex, float animationSpeed)
    {
        Animation newAnim;
        
        newAnim.numTiles = numTiles;
        newAnim.tileIndex = tileIndex;
        newAnim.animationSpeed = animationSpeed;
        
        m_animations[spriteState] = newAnim;
    }
    
    bool SpriteSheet::setAnimation(SpriteState spriteState)
    {
        if (m_currentState == spriteState) return false;
        
        else
        {
            m_currentState = spriteState;
            return true;
        }
    }
    
    glm::vec4 SpriteSheet::flipUVs(glm::vec4 uvRect, bool yPlane, bool xPlane)
    {
        //flip backwards
        if (yPlane)
        {
            //move forward the sprite we're looking at
            uvRect.x += 1.0f / m_dims.x;
            //flipbackwards the sprite we're looking at
            uvRect.z *= -1;
        }
        
        //flip upside down
        if (xPlane)
        {
            //move up the sprite we're looking at
            uvRect.y += 1.0f / m_dims.y;
            //flip upside down the sprite we're looking at
            uvRect.w += -1;
        }
        
        return uvRect;
    }
    
    glm::vec4 SpriteSheet::getUVs(int index)
    {
        //using int so no remainder
        int xTile = index % m_dims.x;
        int yTile = index / m_dims.x;
        
        glm::vec4 uv;
        uv.x = xTile / (float)m_dims.x;
        uv.y = yTile / (float)m_dims.y;
        uv.z = 1.0f / m_dims.x;
        uv.w = 1.0f / m_dims.y;
        
        return uv;
    }
    
    
    
}