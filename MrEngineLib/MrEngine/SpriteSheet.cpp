#include "SpriteSheet.hpp"


namespace MrEngine
{
    SpriteSheet::SpriteSheet()
    {
        
    }
    
    SpriteSheet::~SpriteSheet()
    {
        
    }
    
    void SpriteSheet::init()
    {
    }
    
    void SpriteSheet::begin()
    {
        m_animationTime = 0.0f;
    }
    
    void SpriteSheet::end()
    {
        m_animationTime += m_animations[m_currentState].animationSpeed;
    }
    
    void SpriteSheet::addAnimation(MrEngine::SpriteState spriteState, const GLTexture& texture, glm::ivec2& tileDims, int numTiles, int tileIndex, float animationSpeed)
    {
        Animation newAnim;
        
        newAnim.numTiles = numTiles;
        newAnim.tileIndex = tileIndex;
        newAnim.animationSpeed = animationSpeed;
        newAnim.texture = texture;
        newAnim.width = tileDims.x;
        newAnim.height = tileDims.y;
        
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
    
    bool SpriteSheet::animate(int xDir, int yDir)
    {
        //update the current tileIndex
        int currentTileIndex = m_animations[m_currentState].tileIndex + ((int)m_animationTime % m_animations[m_currentState].numTiles);
        
        //set uvRect for rendering sprite texture
        this->setUVs(currentTileIndex);
        
        //direction check
        this->flipUVs(xDir < 0 ? true : false, yDir < 0 ? true : false);
        
        return true;
    }
    
    void SpriteSheet::flipUVs(bool yPlane, bool xPlane)
    {
        //flip backwards
        if (yPlane)
        {
            //move forward the sprite we're looking at
            m_uvRect.x += 1.0f / m_animations[m_currentState].width;
            //flipbackwards the sprite we're looking at
            m_uvRect.z *= -1;
        }
        //flip upside down
        if (xPlane)
        {
            //move up the sprite we're looking at
            m_uvRect.y += 1.0f / m_animations[m_currentState].height;
            //flip upside down the sprite we're looking at
            m_uvRect.w += -1;
        }
    }
    
    void SpriteSheet::setUVs(int index)
    {
        //using int so no remainder
        int xTile = index % m_animations[m_currentState].width;
        int yTile = index / m_animations[m_currentState].width;
        
        glm::vec4 uv;
        m_uvRect.x = xTile / (float)m_animations[m_currentState].width;
        m_uvRect.y = yTile / (float)m_animations[m_currentState].height;
        m_uvRect.z = 1.0f / m_animations[m_currentState].width;
        m_uvRect.w = 1.0f / m_animations[m_currentState].height;
    }
    
    
    
}