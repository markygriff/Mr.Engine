#ifndef SpriteBatch_hpp
#define SpriteBatch_hpp

#pragma once

#include "Vertex.h"

#include <OpenGL/gl3.h>
#include <glm/glm.hpp>
#include <stdio.h>
#include <vector>

namespace MrEngine
{
    
    
    enum class GlyphSortType
    {
        NONE,
        FRONT_TO_BACK,
        BACK_TO_FRONT,
        TEXTURE
    };
    
    class Glyph
    {
    public:
        Glyph(){ }
        
        Glyph(const glm::vec4& destRect,
              const glm::vec4& uvRect,
              GLuint Texture,
              float Depth,
              const ColorRGBA8& color);
        
        /// Seperate initialization using angle
        Glyph(const glm::vec4& destRect,
              const glm::vec4& uvRect,
              GLuint Texture,
              float Depth,
              const ColorRGBA8& color,
              float angle);
        
        GLuint texture;
        float depth;
        
        Vertex topLeft;
        Vertex bottomLeft;
        Vertex topRight;
        Vertex bottomRight;
    
    private:
        glm::vec2 rotatePoint(glm::vec2 point, float angle);
    };
    
    
    class RenderBatch
    {
    public:
        RenderBatch(GLuint Offset, GLuint NumVertices, GLuint Texture) : offset(Offset),
            numVertices(NumVertices), texture(Texture)
        {
        }
    
        GLuint offset;
        GLuint numVertices;
        GLuint texture;
    };

        
    class SpriteBatch
    {
    public:
        SpriteBatch();
        ~SpriteBatch();
        
        void init();
        void dispose();
        
        void begin(GlyphSortType sortType = GlyphSortType::TEXTURE);
        
        /// Sorts glyphs and renders batches
        void end();
        
        /// Adds a glyph to the sprite batch
        void draw(const glm::vec4& destRect,
                  const glm::vec4& uvRect,
                  GLuint texture, 
                  float depth,
                  const ColorRGBA8& color);
        /// Using angle for rotation
        void draw(const glm::vec4& destRect,
                  const glm::vec4& uvRect,
                  GLuint texture,
                  float depth,
                  const ColorRGBA8& color,
                  float angle);
        /// Using direction vector for rotation
        void draw(const glm::vec4& destRect,
                  const glm::vec4& uvRect,
                  GLuint texture,
                  float depth,
                  const ColorRGBA8& color,
                  const glm::vec2& dir);
        
        /// Renders Sprite Batch to the screen
        void renderBatch();
        
    private:
        void createRenderBatches();
        void createVertexArray();
        
        /// Sorts the glyph pointers
        void sortGlyphs();
        
        static bool compareFrontToBack(Glyph* a, Glyph* b);
        static bool compareBackToFront(Glyph* a, Glyph* b);
        static bool compareTexture(Glyph* a, Glyph* b);
        
        GLuint m_vbo = 0;
        GLuint m_vao = 0;
        GlyphSortType m_sortType;
        
        std::vector<Glyph*> m_glyphPointers; ///< this is for sorting
        std::vector<Glyph> m_glyphs;///< the actual glyphs
        std::vector<RenderBatch> m_renderBatches;
    };
    
    
}

#endif /* SpriteBatch_hpp */
