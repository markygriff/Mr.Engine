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
