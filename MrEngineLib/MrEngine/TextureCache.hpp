//
//  TextureCache.hpp
//  MarkyEngine
//
//  Created by Mark Griffith on 2016-07-17.
//  Copyright © 2016 Mark Griffith. All rights reserved.
//

#ifndef TextureCache_hpp
#define TextureCache_hpp

#pragma once

#include <stdio.h>
#include <map>
#include "GLTexture.h"

namespace MrEngine {
    
    
    class TextureCache
    {
    public:
        TextureCache();
        ~TextureCache();
        
        /// Loads texture and returns it
        GLTexture getTexture(std::string texturePath);
        
    private:
        std::map<std::string, GLTexture> mm_textureMap;
        
    };
    
    
}

#endif /* TextureCache_hpp */
