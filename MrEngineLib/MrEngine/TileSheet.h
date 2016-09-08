#ifndef TileSheet_h
#define TileSheet_h

#include "GLTexture.h"

#include <glm/glm.hpp>

namespace MrEngine
{
    


class TileSheet
{
public:
    
    void init(const GLTexture& texture, glm::ivec2& tileDims)
    {
        this->texture = texture;
        this->dims = tileDims;
    }
    
    glm::vec4 getUVs(int index)
    {
        //using int so no remainder
        int xTile = index % dims.x;
        int yTile = index / dims.x;
        
        glm::vec4 uv;
        uv.x = xTile / (float)dims.x;
        uv.y = yTile / (float)dims.y;
        uv.z = 1.0f / dims.x;
        uv.w = 1.0f / dims.y;
        
        return uv;
    }
    
    GLTexture texture;
    glm::ivec2 dims;
    
};
    
    
}


#endif /* TileSheet_h */
