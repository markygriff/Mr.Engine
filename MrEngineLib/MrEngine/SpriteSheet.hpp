//
//  SpriteSheet.hpp
//  MrEngine
//
//  Created by Mark Griffith on 2016-09-01.
//  Copyright © 2016 Mark Griffith. All rights reserved.
//

#ifndef SpriteSheet_hpp
#define SpriteSheet_hpp

#include "GLTexture.h"

#include <glm/glm.hpp>

#include <stdio.h>
#include <vector>
#include <map>

namespace MrEngine
{


    enum class SpriteState
    {
        IDLE,
        WALKING,
        RUNNING,
        RISING,
        FALLING,
        ATTACKING,
        ATTACKING_IN_AIR,
        DEAD
    };

    struct Animation
    {
        //SpriteState spriteState;
        int numTiles;
        int tileIndex;
        float animationSpeed;
    };

    class SpriteSheet
    {
    public:
        void init(const GLTexture& texture, glm::ivec2& tileDims);
        
        void addAnimation(SpriteState spriteState, int numTiles, int tileIndex, float animationSpeed);
        bool setAnimation(SpriteState spriteState);
        
        glm::vec4 flipUVs(glm::vec4 uvRect, bool yPlane, bool xPlane);
        
        glm::vec4 getUVs(int index);
        
        Animation getAnimation() { return m_animations[m_currentState]; }
        glm::ivec2 getDims() const { return m_dims; }
        GLTexture getTexture() const { return m_texture; }
        
    private:
        int m_numTilesTotal;
        
        std::map<SpriteState, Animation> m_animations;
        
        GLTexture m_texture; ///< GLTexture to render
        glm::ivec2 m_dims; ///< dimention of each sprite (assuming all equal size for single sprite)
        
        SpriteState m_currentState;
    };

        
}
#endif /* SpriteSheet_hpp */
