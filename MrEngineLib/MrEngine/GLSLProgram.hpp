//
//  GLSLProgram.hpp
//  Markine (Game Engine)
//
//  Created by Mark Griffith on 2016-07-13.
//  Copyright © 2016 Mark Griffith. All rights reserved.
//

#ifndef GLSLProgram_hpp
#define GLSLProgram_hpp

#pragma once

#include <OpenGL/gl3.h>

#include <iostream>
#include <stdio.h>
#include <string>

namespace MrEngine {
    
    
    class GLSLProgram
    {
    public:
        GLSLProgram();
        ~GLSLProgram();
        
        void compileShaders(const std::string& vertexShaderFilePath, const std::string& fragmentShaderFilePath);
        
        void compileShadersFromSource(const char* vertexSource, const char* fragmentSource);
        
        void linkShaders();
        
        void addAttribute(const std::string& attributeName);
        
        GLint getUniformLocation(const std::string& uniformName);
        
        void use();
        void unuse();
        
        void dispose();
        
    private:
        
        void compileShader(const char* source, const std::string& name, GLuint id);
        
        int m_numAttributes = 0;
        
        GLuint m_programID = 0;
        GLuint m_vertexShaderID = 0;
        GLuint m_fragmentShaderID = 0;
        
    };
    
    
}

#endif /* GLSLProgram_hpp */
