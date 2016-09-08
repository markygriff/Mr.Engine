#include "SpriteBatch.hpp"

#include <algorithm>

namespace MrEngine
{
    
    Glyph::Glyph(const glm::vec4& destRect,
                 const glm::vec4& uvRect,
                 GLuint Texture,
                 float Depth,
                 const ColorRGBA8& color) :
            texture(Texture),
            depth(Depth)
    {
        
        topLeft.color = color;
        topLeft.setPosition(destRect.x, destRect.y + destRect.w);
        topLeft.setUV(uvRect.x, uvRect.y + uvRect.w);
        
        bottomLeft.color = color;
        bottomLeft.setPosition(destRect.x, destRect.y);
        bottomLeft.setUV(uvRect.x, uvRect.y);
        
        bottomRight.color = color;
        bottomRight.setPosition(destRect.x + destRect.z, destRect.y);
        bottomRight.setUV(uvRect.x + uvRect.z, uvRect.y);
        
        topRight.color = color;
        topRight.setPosition(destRect.x + destRect.z, destRect.y + destRect.w);
        topRight.setUV(uvRect.x + uvRect.z, uvRect.y + uvRect.w);
    }
    
    Glyph::Glyph(const glm::vec4& destRect,
                 const glm::vec4& uvRect,
                 GLuint Texture,
                 float Depth,
                 const ColorRGBA8& color,
                 float angle) :
            texture(Texture),
            depth(Depth)
    {
        glm::vec2 halfDimensions(destRect.z / 2.0f, destRect.w / 2.0f);
        
        //get points of glyph centered at origin
        glm::vec2 tl(-halfDimensions.x, halfDimensions.y);
        glm::vec2 bl(-halfDimensions.x, -halfDimensions.y);
        glm::vec2 br(halfDimensions.x, -halfDimensions.y);
        glm::vec2 tr(halfDimensions.x, halfDimensions.y);
        
        //rotate centered points and correct their positioning
        //bc we want (0,0) at the bottom left
        tl = rotatePoint(tl, angle) + halfDimensions;
        bl = rotatePoint(bl, angle) + halfDimensions;
        br = rotatePoint(br, angle) + halfDimensions;
        tr = rotatePoint(tr, angle) + halfDimensions;
        
        topLeft.color = color;
        topLeft.setPosition(destRect.x + tl.x, destRect.y + tl.y);
        topLeft.setUV(uvRect.x, uvRect.y + uvRect.w);
        
        bottomLeft.color = color;
        bottomLeft.setPosition(destRect.x + bl.x, destRect.y + bl.y);
        bottomLeft.setUV(uvRect.x, uvRect.y);
        
        bottomRight.color = color;
        bottomRight.setPosition(destRect.x + br.x, destRect.y + br.y);
        bottomRight.setUV(uvRect.x + uvRect.z, uvRect.y);
        
        topRight.color = color;
        topRight.setPosition(destRect.x + tr.x, destRect.y + tr.y);
        topRight.setUV(uvRect.x + uvRect.z, uvRect.y + uvRect.w);
    }
    
    glm::vec2 Glyph::rotatePoint(glm::vec2 point, float angle)
    {
        glm::vec2 newVec;
        
        newVec.x = point.x * cos(angle) - point.y* sin(angle);
        newVec.y = point.x * sin(angle) + point.y * cos(angle);
        
        return newVec;
    }
    
    SpriteBatch::SpriteBatch() :
    m_vbo(0),
    m_vao(0)
    {
        //empty
    }

    SpriteBatch::~SpriteBatch()
    {
        //empty
    }
        
    void SpriteBatch::init()
    {
        createVertexArray();
    }
    
    void SpriteBatch::dispose()
    {
        if (m_vao != 0)
        {
            glDeleteVertexArrays(1, &m_vao);
            m_vao = 0;
        }
        if (m_vbo != 0)
        {
            glDeleteVertexArrays(1, &m_vbo);
            m_vbo = 0;
        }
    }

    void SpriteBatch::begin(GlyphSortType sortType /* GlyphSortType::TEXTURE */)
    {
        m_sortType = sortType;
        m_renderBatches.clear();
        m_glyphs.clear();
    }
    
    void SpriteBatch::end()
    {
        //assign the glyph pointers to the location of the glyphs
        m_glyphPointers.resize(m_glyphs.size());
        for (int i = 0; i < m_glyphs.size(); i++)
             {
                 m_glyphPointers[i] = &m_glyphs[i];
             }
        
        //sort the glyph pointers (faster than sorting the glyphs)
        sortGlyphs();
        
        createRenderBatches();
    }

    void SpriteBatch::draw(const glm::vec4& destRect,
                           const glm::vec4& uvRect,
                           GLuint texture, float depth,
                           const ColorRGBA8& color)
    {
        m_glyphs.emplace_back(destRect, uvRect, texture, depth, color);
    }
    
    void SpriteBatch::draw(const glm::vec4& destRect,
                           const glm::vec4& uvRect,
                           GLuint texture,
                           float depth,
                           const ColorRGBA8& color,
                           float angle)
    {
        m_glyphs.emplace_back(destRect, uvRect, texture, depth, color, angle );
    }
    
    void SpriteBatch::draw(const glm::vec4& destRect,
                           const glm::vec4& uvRect,
                           GLuint texture,
                           float depth,
                           const ColorRGBA8& color,
                           const glm::vec2& dir)
    {
        const glm::vec2  right(1.0f, 0.0f);
        
        float angle = acos(glm::dot(right,dir));
        
        if (dir.y < 0) angle = -angle;
        
        m_glyphs.emplace_back(destRect, uvRect, texture, depth, color, angle);

    }
    
    void SpriteBatch::renderBatch()
    {
        //unbind vertex array object
        glBindVertexArray(m_vao);
        
        for (int i  = 0; i < m_renderBatches.size();i++)
        {
            //bind the textures
            glBindTexture(GL_TEXTURE_2D, m_renderBatches[i].texture);
            
            //draw the arrays
            glDrawArrays(GL_TRIANGLES, m_renderBatches[i].offset, m_renderBatches[i].numVertices);
        }
        
        //unbind vertex array object
        glBindVertexArray(0);
    }
    
    void SpriteBatch::createRenderBatches()
    {
        std::vector<Vertex> vertices;
        
        //create space for 6 glyphs (size of render batch?)
        vertices.resize(m_glyphs.size() * 6);
        
        //if we have nothing to draw, return
        if (m_glyphs.empty())
        {
            return;
        }
        
        //current vertex
        int offset = 0;
        int cv = 0;
        
        //note to self: emplace_back pushes back a RenderBatch object with the constructor
        //arguments set in the brackets (offset, numVertices, texture)
        
        m_renderBatches.emplace_back(offset /* = 0 */, 6, m_glyphPointers[0]->texture);
        vertices[cv++] = m_glyphPointers[0]->topLeft;
        vertices[cv++] = m_glyphPointers[0]->bottomLeft;
        vertices[cv++] = m_glyphPointers[0]->bottomRight;
        vertices[cv++] = m_glyphPointers[0]->bottomRight;
        vertices[cv++] = m_glyphPointers[0]->topRight;
        vertices[cv++] = m_glyphPointers[0]->topLeft;
        offset += 6;
        
        //loop through the glyphs
        for (int cg = 1; cg < m_glyphs.size(); cg++)
        {
            //if we're using a different texture than before, create a new batch of glyphs
            //for the new texture
             if (m_glyphPointers[cg]->texture != m_glyphPointers[cg - 1]->texture)
             {
                m_renderBatches.emplace_back(offset, 6, m_glyphPointers[cg]->texture);
             }
            //else we're using the same texture so we don't have to create a new batch.
            //therefore we can just add to the number of vertices we have of that specific
            //batch
            else
            {
                m_renderBatches.back().numVertices += 6;
            }
            
            vertices[cv++] = m_glyphPointers[cg]->topLeft;
            vertices[cv++] = m_glyphPointers[cg]->bottomLeft;
            vertices[cv++] = m_glyphPointers[cg]->bottomRight;
            vertices[cv++] = m_glyphPointers[cg]->bottomRight;
            vertices[cv++] = m_glyphPointers[cg]->topRight;
            vertices[cv++] = m_glyphPointers[cg]->topLeft;
            offset += 6;
        }
        
        glBindBuffer(GL_ARRAY_BUFFER, m_vbo);
        
        //orphan the buffer
        glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(Vertex), nullptr, GL_DYNAMIC_DRAW);
        //upload the data
        glBufferSubData(GL_ARRAY_BUFFER, 0, vertices.size() * sizeof(Vertex), vertices.data());
        
        
        glBindBuffer(GL_ARRAY_BUFFER, 0);
    }
    
    
    void SpriteBatch::createVertexArray()
    {
        //bind our vertex array object
        if (m_vao == 0)
        {
            glGenVertexArrays(1, &m_vao);
        }
        glBindVertexArray(m_vao);

        //bind our vertex buffer object
        if (m_vbo == 0)
        {
            glGenBuffers(1, &m_vbo);
        }
        glBindBuffer(GL_ARRAY_BUFFER, m_vbo);
        
        
        //Tell opengl that we want to use the first 3
        //attribute arrays. (Pos, Col, UV)
        glEnableVertexAttribArray(0);
        glEnableVertexAttribArray(1);
        glEnableVertexAttribArray(2);
        
        // this is the position attribute pointer
        glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, position));
        
        // this is the color attribute pointer
        glVertexAttribPointer(1, 4, GL_UNSIGNED_BYTE, GL_TRUE, sizeof(Vertex), (void*)offsetof(Vertex, color));
        
        // this is the UV attribute pointer
        glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, uv));
        
        glBindVertexArray(0);
        
        
    }
    
    void SpriteBatch::sortGlyphs()
    {
        switch (m_sortType)
        {
            case GlyphSortType::BACK_TO_FRONT:
                std::stable_sort(m_glyphPointers.begin(), m_glyphPointers.end(), compareBackToFront);
                break;
            case GlyphSortType::FRONT_TO_BACK:
                std::stable_sort(m_glyphPointers.begin(), m_glyphPointers.end(), compareFrontToBack);
                break;
            case GlyphSortType::TEXTURE:
                std::stable_sort(m_glyphPointers.begin(), m_glyphPointers.end(), compareTexture);
                break;
            default:
                break;
        }
        
    }
    
    bool SpriteBatch::compareFrontToBack(Glyph* a, Glyph* b)
    {
        return (a->depth < b->depth);
    }
    bool SpriteBatch::compareBackToFront(Glyph* a, Glyph* b)
    {
        return (a->depth > b->depth);
    }
    bool SpriteBatch::compareTexture(Glyph* a, Glyph* b)
    {
        return (a->texture < b->texture);
    }
}