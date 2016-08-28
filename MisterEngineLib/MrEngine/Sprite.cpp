//
//  Sprite.cpp
//  Markine (Game Engine)
//
//  Created by Mark Griffith on 2016-07-13.
//  Copyright © 2016 Mark Griffith. All rights reserved.
//

#include "Sprite.hpp"
#include "Vertex.h"
#include "ResourceManager.hpp"

#include <cstddef>
#include <string>

namespace MrEngine {
    

    Sprite::Sprite()
    {
        m_vboID = 0;
    }

    Sprite::~Sprite()
    {
        if (m_vboID != 0)
        {
            glDeleteBuffers(1, &m_vboID);
        }
    }

    void Sprite::init(float x, float y, float w, float h, std::string texturePath)
    {
        // initialize vars
        m_x = x;
        m_y = y;
        m_w = w;
        m_h = h;
        
        m_texture = ResourceManager::getTexture(texturePath);

        if (m_vboID == 0)
        {
            glGenBuffers(1, &m_vboID);
        }

        // This array holds the vetex data
        Vertex vertexData[6];

        // FIRST TRIANGLE
        // top right corner facing left
        vertexData[0].setUV(1.0f, 1.0f);
        vertexData[0].setPosition(x+w, y+h);

        // bottom right
        vertexData[1].setUV(0.0f, 1.0f);
        vertexData[1].setPosition(x, y+h);

        // bottom left facing up
        vertexData[2].setUV(0.0f, 0.0f);
        vertexData[2].setPosition(x, y);

        // SECOND TRIANGLE
        // bottom left facing right
        vertexData[3].setUV(0.0f, 0.0f);
        vertexData[3].setPosition(x, y);

        // top left
        vertexData[4].setUV(1.0f, 0.0f);
        vertexData[4].setPosition(x+w, y);

        // top right corner facing down
        vertexData[5].setUV(1.0f, 1.0f);
        vertexData[5].setPosition(x+w, y+h);
        
        for (int i = 0; i < 6; i++)
        {
            
            vertexData[i].setColor(255, 255, 255, 255);

        }

        // tell opengl to bind our vertex buffer object
        glBindBuffer(GL_ARRAY_BUFFER, m_vboID);
        
        // upload the data to the GPU
        glBufferData(GL_ARRAY_BUFFER, sizeof(vertexData), vertexData, GL_STATIC_DRAW);

        // unbind the buffer
        glBindBuffer(GL_ARRAY_BUFFER, 0);
    }

    void Sprite::draw()
    {

        glBindTexture(GL_TEXTURE_2D, m_texture.id);
        
        glBindBuffer(GL_ARRAY_BUFFER, m_vboID);

        //Tell opengl that we want to use the first
        //attribute array. We only need one array right
        //now since we are only using position.
        glEnableVertexAttribArray(0);
        glEnableVertexAttribArray(1);
        glEnableVertexAttribArray(2);


        // this is the position attribute pointer
        glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, position));
        
        // this is the color attribute pointer
        glVertexAttribPointer(1, 4, GL_UNSIGNED_BYTE, GL_TRUE, sizeof(Vertex), (void*)offsetof(Vertex, color));
        
        // this is the UV attribute pointer
        glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, uv));
        
        //draw 6 vertices to the screen
        glDrawArrays(GL_TRIANGLES, 0, 6);

        glDisableVertexAttribArray(0);
        glDisableVertexAttribArray(1);
        glDisableVertexAttribArray(2);
        
        //unbind the texture
        glBindTexture(GL_TEXTURE_2D, 0); //????

        //unbind the buffer
        glBindBuffer(GL_ARRAY_BUFFER, 0);
    }


}