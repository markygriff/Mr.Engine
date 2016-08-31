//
//  ImageLoader.hpp
//  MarkyEngine
//
//  Created by Mark Griffith on 2016-07-16.
//  Copyright © 2016 Mark Griffith. All rights reserved.
//

#ifndef ImageLoader_hpp
#define ImageLoader_hpp

#pragma once

#include <stdio.h>
#include <string>

#include "GLTexture.h"
namespace MrEngine {
    
    
    //loads images into GLTextures (openGL texture)
    class ImageLoader
    {
    public:
        static GLTexture loadPNG(std::string filePath);
    };
    
    
}

#endif /* ImageLoader_hpp */
