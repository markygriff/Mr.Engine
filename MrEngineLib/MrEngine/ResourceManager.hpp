//
//  ResourceManager.hpp
//  MarkyEngine
//
//  Created by Mark Griffith on 2016-07-17.
//  Copyright © 2016 Mark Griffith. All rights reserved.
//

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
