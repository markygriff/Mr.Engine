//
//  ResourceManager.cpp
//  MarkyEngine
//
//  Created by Mark Griffith on 2016-07-17.
//  Copyright © 2016 Mark Griffith. All rights reserved.
//

#include "ResourceManager.hpp"

namespace MrEngine {
    
    
    TextureCache ResourceManager::m_textureCache;

    GLTexture ResourceManager::getTexture(std::string texturePath)
    {
        return m_textureCache.getTexture(texturePath);
    }


}