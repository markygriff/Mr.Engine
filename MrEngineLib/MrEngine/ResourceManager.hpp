#ifndef ResourceManager_hpp
#define ResourceManager_hpp

#pragma once
#include "TextureCache.hpp"

#include <stdio.h>
#include <string>

namespace MrEngine {
    
    
class ResourceManager
{
public:
    static GLTexture getTexture(std::string texturePath);
    
private:
    static TextureCache m_textureCache;
    
};
    
}

#endif /* ResourceManager_hpp */
