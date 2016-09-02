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
        int numTiles;
        int tileIndex;
        float animationSpeed;
    };

    class SpriteSheet
    {
    public:
        void init(const GLTexture& texture, glm::ivec2& tileDims);
        void begin(); ///< reset animation time to 0
        void end(); ///< update animation time
        
        bool isAnimationFinished() { return m_animationTime > m_animations[m_currentState].numTiles; }
        
        void addAnimation(SpriteState spriteState, int numTiles, int tileIndex, float animationSpeed);
        bool setAnimation(SpriteState spriteState);
        bool animate(int xDir, int yDir);
        
        void flipUVs(bool yPlane, bool xPlane);
        void setUVs(int index);
        
        Animation getAnimation() { return m_animations[m_currentState]; }
        glm::ivec2 getDims() const { return m_dims; }
        glm::vec4 getUVs() { return m_uvRect; }
        GLTexture getTexture() const { return m_texture; }
        
    private:
        int m_numTilesTotal;
        float m_animationTime;
        
        std::map<SpriteState, Animation> m_animations;
        
        GLTexture m_texture; ///< GLTexture to render
        glm::ivec2 m_dims; ///< dimention of each sprite (assuming all equal size for single sprite)
        glm::vec4 m_uvRect;
        
        SpriteState m_currentState;
    };

        
}
#endif /* SpriteSheet_hpp */
