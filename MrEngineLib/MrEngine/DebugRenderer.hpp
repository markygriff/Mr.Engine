//
//  DebugRenderer.hpp
//  MisterEngineLib
//
//  Created by Mark Griffith on 2016-08-08.
//  Copyright © 2016 Mark Griffith. All rights reserved.
//

#ifndef DebugRenderer_hpp
#define DebugRenderer_hpp

#include "Vertex.h"
#include "GLSLProgram.hpp"

#include <glm/glm.hpp>
#include <stdio.h>
#include <vector>

namespace MrEngine {
    
    
    class DebugRenderer
    {
    public:
        DebugRenderer();
        ~DebugRenderer();
        
        void init();
        void end();
        
        void drawLine(const glm::vec2& x, const glm::vec2& y, const ColorRGBA8& color);
        void drawBox(glm::vec4& destRect, const ColorRGBA8& color, float angle);
        void drawCircle(const glm::vec2& centerPoint, const ColorRGBA8 color, float radius);
        
        void render(const glm::mat4& projectionMatrix, float lineWidth);
        void dispose();
        
        struct DebugVertex
        {
            glm::vec2 position;
            MrEngine::ColorRGBA8 color;
        };
        
    private:
        MrEngine::GLSLProgram m_program;
        std::vector<DebugVertex> m_verts;
        std::vector<GLuint> m_indices;
        GLuint m_vbo = 0, m_vao = 0, m_ibo = 0;
        int m_numElements = 0;
    };
    
    
}

#endif /* DebugRenderer_hpp */
