//
//  DebugRenderer.cpp
//  MisterEngineLib
//
//  Created by Mark Griffith on 2016-08-08.
//  Copyright © 2016 Mark Griffith. All rights reserved.
//

#include "DebugRenderer.hpp"

const float PI = 3.14159265359f;


namespace
{
    ///color vert
    const char* VERT_SRC = R"(#version 410
    //The vertex shader operates on each vertex

    //input data from the VBO. Each vertex is 2 floats
    in vec2 vertexPosition;
    in vec4 vertexColor;

    out vec2 fragmentPosition;
    out vec4 fragmentColor;

    uniform mat4 P;

    void main() {
        //Set the x,y position on the screen
        gl_Position.xy = (P * vec4(vertexPosition, 0.0, 1.0)).xy;
        //the z position is zero since we are in 2D
        gl_Position.z = 0.0;
        
        //Indicate that the coordinates are normalized
        gl_Position.w = 1.0;
        
        fragmentPosition = vertexPosition;
        
        fragmentColor = vertexColor;
    })";

    ///color frag
    const char* FRAG_SRC = R"(#version 410
    //The fragment shader operates on each pixel in a given polygon

    in vec2 fragmentPosition;
    in vec4 fragmentColor;

    //This is the 3 component float vector that gets outputted to the screen
    //for each pixel.
    out vec4 color;

    void main() {
        
        color = fragmentColor;
    })";
}


namespace MrEngine {

    
    DebugRenderer::DebugRenderer()
    {
        //empty
    }
    
    DebugRenderer::~DebugRenderer()
    {
        dispose();
    }
    
    void DebugRenderer::init()
    {
        //shader initialization
        m_program.compileShadersFromSource(VERT_SRC, FRAG_SRC);
        m_program.addAttribute("vertexPosition");
        m_program.addAttribute("vertexColor");
        m_program.linkShaders();
        
        //set up buffers
        glGenVertexArrays(1, &m_vao);
        glGenBuffers(1, &m_vbo);
        glGenBuffers(1, &m_ibo);
        
        glBindVertexArray(m_vao);
        glBindBuffer(GL_ARRAY_BUFFER, m_vbo);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_ibo); ///< difference for index drawing!
        
        glEnableVertexAttribArray(0);
        glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, sizeof(DebugVertex), (void*)offsetof(DebugVertex, position));
        glEnableVertexAttribArray(1);
        glVertexAttribPointer(1, 4, GL_UNSIGNED_BYTE, GL_TRUE, sizeof(DebugVertex), (void*)offsetof(DebugVertex, color));
        
        glBindVertexArray(0);
    }
    
    void DebugRenderer::end()
    {
        glBindBuffer(GL_ARRAY_BUFFER, m_vbo);
        //orphan buffer (speed optimization)
        glBufferData(GL_ARRAY_BUFFER, m_verts.size() * sizeof(DebugVertex), nullptr, GL_DYNAMIC_DRAW);
        //upload the data
        glBufferSubData(GL_ARRAY_BUFFER, 0, m_verts.size() * sizeof(DebugVertex), m_verts.data());
        glBindBuffer(GL_ARRAY_BUFFER, 0);

        
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_ibo);
        //orphan buffer (speed optimization)
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, m_indices.size() * sizeof(GLuint), nullptr, GL_DYNAMIC_DRAW);
        //upload the data
        glBufferSubData(GL_ELEMENT_ARRAY_BUFFER, 0, m_indices.size() * sizeof(GLuint), m_indices.data());
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
        
        m_numElements = (int)m_indices.size();
        m_indices.clear();
        m_verts.clear();

    }
    
    glm::vec2 rotatePoint(glm::vec2 point, float angle)
    {
        glm::vec2 newVec;
        
        newVec.x = point.x * cos(angle) - point.y* sin(angle);
        newVec.y = point.x * sin(angle) + point.y * cos(angle);
        
        return newVec;
    }
    
    void DebugRenderer::drawLine(const glm::vec2& x, const glm::vec2& y, const ColorRGBA8& color)
    {
        int i = m_verts.size();
        m_verts.resize(m_verts.size() + 2);
        
        m_verts[i].position = x;
        m_verts[i].color = color;
        
        m_verts[i + 1].position = y;
        m_verts[i + 1].color = color;
        
        m_indices.push_back(i);
        m_indices.push_back(i + 1);
    }
    
    void DebugRenderer::drawBox(glm::vec4& destRect, const ColorRGBA8& color, float angle)
    {
        int i = (int)m_verts.size();
        m_verts.resize(m_verts.size() + 4);
        
        glm::vec2 halfDimensions(destRect.z / 2.0f, destRect.w / 2.0f);
        
        //get points of glyph centered at origin
        glm::vec2 tl(-halfDimensions.x, halfDimensions.y);
        glm::vec2 bl(-halfDimensions.x, -halfDimensions.y);
        glm::vec2 br(halfDimensions.x, -halfDimensions.y);
        glm::vec2 tr(halfDimensions.x, halfDimensions.y);
        
        glm::vec2 positionOffset(destRect.x, destRect.y);
        
        //rotate centered points and correct their positioning
        //bc we want (0,0) at the bottom left
        m_verts[i].position = rotatePoint(tl, angle) + halfDimensions + positionOffset;
        m_verts[i+1].position = rotatePoint(bl, angle) + halfDimensions + positionOffset;
        m_verts[i+2].position = rotatePoint(br, angle) + halfDimensions + positionOffset;
        m_verts[i+3].position = rotatePoint(tr, angle) + halfDimensions + positionOffset;
        
        for (int j = i; j < i + 4; j++)
        {
            m_verts[j].color = color;
        }
        
        m_indices.reserve(m_indices.size() + 8);
        
        m_indices.push_back(i);
        m_indices.push_back(i + 1);
    
        m_indices.push_back(i + 1);
        m_indices.push_back(i + 2);
        
        m_indices.push_back(i + 2);
        m_indices.push_back(i + 3);
        
        m_indices.push_back(i + 3);
        m_indices.push_back(i);
    }

    void DebugRenderer::drawCircle(const glm::vec2& centerPoint, const ColorRGBA8 color, float radius)
    {
        //number of vertices in our circle
        static const int NUM_VERTS = 100;
        
        //the starting index of our circle vertices
        int start = (int)m_verts.size();
        
        //resize our vertex buffer
        m_verts.resize(m_verts.size() + NUM_VERTS);
        
        for ( int i = 0; i < NUM_VERTS; i++)
        {
            //get radians
            float angle = (float)i /NUM_VERTS * PI * 2.0f;
            
            m_verts[start + i].position.x = cos(angle) * radius + centerPoint.x;
            m_verts[start + i].position.y = sin(angle) * radius + centerPoint.y;
            m_verts[start + i].color = color;
        }
        
        //remember that every line requires TWO indices
        //so reserve twice the number of vertices
        m_indices.reserve(m_indices.size() + NUM_VERTS * 2);
        
        //set indices for indexed drawing
        for (int i = 0; i < NUM_VERTS - 1; i++)
        {
            m_indices.push_back(start + i);
            m_indices.push_back(start + i + 1);
        }
        m_indices.push_back(start + NUM_VERTS - 1);
        m_indices.push_back(start);
        
    }
    
    void DebugRenderer::render(const glm::mat4& projectionMatrix, float lineWidth)
    {
        m_program.use();
        
        //
        GLint pUniform = m_program.getUniformLocation("P");
        glUniformMatrix4fv(pUniform, 1, GL_FALSE, &projectionMatrix[0][0]);
        
        glLineWidth(lineWidth);
        glBindVertexArray(m_vao);
        glDrawElements(GL_LINES, m_numElements, GL_UNSIGNED_INT, 0);
        glBindVertexArray(0);
        
        m_program.unuse();
    }
    
    void DebugRenderer::dispose()
    {
        if (m_vao)
        {
            glDeleteVertexArrays(1, &m_vao);
        }
        if (m_vbo)
        {
            glDeleteBuffers(1, &m_vbo);
        }
        if (m_ibo)
        {
            glDeleteBuffers(1, &m_ibo);
        }
        m_program.dispose();
    }
}