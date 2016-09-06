//
//  GLTexture.h
//  MarkyEngine
//
//  Created by Mark Griffith on 2016-07-16.
//  Copyright © 2016 Mark Griffith. All rights reserved.
//

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