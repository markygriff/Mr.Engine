#ifndef GLTexture_h
#define GLTexture_h

#pragma once

#include <OpenGL/gl3.h>

#include <string>

namespace MrEngine {
    
    /// data only struct
    struct GLTexture
    {
        GLuint id;
        int width;
        int height;
        std::string filePath;
    };
    
        
}


#endif /* GLTexture_h */
