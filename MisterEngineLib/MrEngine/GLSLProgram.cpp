//
//  GLSLProgram.cpp
//  Markine (Game Engine)
//
//  Created by Mark Griffith on 2016-07-13.
//  Copyright © 2016 Mark Griffith. All rights reserved.
//

#include "GLSLProgram.hpp"
#include "FatalError.hpp"
#include "IOManager.hpp"

#include <fstream>
#include <vector>

namespace MrEngine {
    
    
    GLSLProgram::GLSLProgram() :
    m_numAttributes(0),
    m_programID(0),
    m_vertexShaderID(0),
    m_fragmentShaderID(0)
    {
        //empty
    }

    GLSLProgram::~GLSLProgram()
    {
        m_numAttributes = 0;
        m_programID = 0;
        m_vertexShaderID = 0;
        m_fragmentShaderID = 0;
    }

    void GLSLProgram::compileShaders(const std::string& vertexShaderFilePath, const std::string& fragmentShaderFilePath)
    {
        
        std::string vertSource;
        std::string fragSource;
        
        IOManager::readFileToBuffer(vertexShaderFilePath, vertSource);
        IOManager::readFileToBuffer(fragmentShaderFilePath, fragSource);
        
        compileShadersFromSource(vertSource.c_str(), fragSource.c_str());

    }
    
    ///TODO: create better detailed debug message
    void GLSLProgram::compileShadersFromSource(const char* vertexSource, const char* fragmentSource)
    {
        //the vertex and fragment shaders are compiled, now we link them together
        //into a program
        
        //get a program object
        m_programID = glCreateProgram();
        
        // create the vertex shader
        m_vertexShaderID = glCreateShader(GL_VERTEX_SHADER);
        if (m_vertexShaderID == 0)
        {
            fatalError("Vertex Shader failed to be created");
        }
        
        // create the fragment shader
        m_fragmentShaderID = glCreateShader(GL_FRAGMENT_SHADER);
        if (m_fragmentShaderID == 0)
        {
            fatalError("Fragment Shader failed to be created");
        }
        
        //compile the shaders
        compileShader(vertexSource, "VERTEX SHADER", m_vertexShaderID);
        compileShader(fragmentSource, "FRAGMENT SHADER", m_fragmentShaderID);
    }
    
    void GLSLProgram::linkShaders()
    {
        // attach shaders to program
        glAttachShader(m_programID, m_vertexShaderID);
        glAttachShader(m_programID, m_fragmentShaderID);

        // link program
        glLinkProgram(m_programID);

        GLint isLinked = 0;
        glGetProgramiv(m_programID, GL_LINK_STATUS, (int *) &isLinked);
        if (isLinked == GL_FALSE)
        {
            GLint maxLength = 0;
            glGetProgramiv(m_programID, GL_INFO_LOG_LENGTH, &maxLength);

            std::vector<char> errorLog(maxLength);
            glGetProgramInfoLog(m_programID, maxLength, &maxLength, &errorLog[0]);

            glDetachShader(m_programID, m_vertexShaderID);
            glDetachShader(m_programID, m_fragmentShaderID);
    //        glDeleteProgram(m_programID);
            glDeleteShader(m_vertexShaderID);
            glDeleteShader(m_fragmentShaderID);

            std::printf("%s\n", &(errorLog[0]));
            fatalError("Shaders failed to link!");
        }
    }

    void GLSLProgram::addAttribute(const std::string& attributeName)
    {
        glBindAttribLocation(m_programID, m_numAttributes, attributeName.c_str());
        m_numAttributes++;
    }

    GLint GLSLProgram::getUniformLocation(const std::string &uniformName)
    {
        GLint location = glGetUniformLocation(m_programID, uniformName.c_str());
        if (location == GL_INVALID_INDEX)
        {
            fatalError("Uniform " + uniformName + " not found in shader");
        }
        
        return location;
    }


    void GLSLProgram::use()
    {
        glUseProgram(m_programID);

        for(int i = 0; i < m_numAttributes; i++)
        {
            glEnableVertexAttribArray(i);
        }
    }

    void GLSLProgram::unuse()
    {
        glUseProgram(0);

        for(int i = 0; i < m_numAttributes; i++)
        {
            glDisableVertexAttribArray(i);
        }
    }

    void GLSLProgram::compileShader(const char* source, const std::string& name, GLuint id)
    {

        glShaderSource(id, 1, &source, nullptr);

        glCompileShader(id);

        GLint success = 0;
        glGetShaderiv(id, GL_COMPILE_STATUS, &success);

        if (success == GL_FALSE)
        {
            GLint maxLength = 0;
            glGetShaderiv(id, GL_INFO_LOG_LENGTH, &maxLength);

            // the maxLength includes the NULL character
            std::vector<char> errorLog(maxLength);
            glGetShaderInfoLog(id, maxLength, &maxLength, &errorLog[0]);

            // provide the infolog in whatever manor you deem best
            // exit with faiure
            glDeleteShader(id);

            std::printf("%s\n", &(errorLog[0]));
            fatalError("Shader " + name + " failed to compile");
        }
    }
    
    void GLSLProgram::dispose()
    {
        if (m_programID) glDeleteProgram(m_programID);
        m_numAttributes = 0;
        m_programID = 0;
        m_vertexShaderID = 0;
        m_fragmentShaderID = 0;
    }
}
