#ifndef ParticleBatch2D_hpp
#define ParticleBatch2D_hpp

#pragma once

#include "SpriteBatch.hpp"
#include "Vertex.h"
#include "GLTexture.h"

#include <glm/glm.hpp>
#include <stdio.h>
#include <functional>


namespace MrEngine
{
 
    
    class Particle2D
    {
    public:
        glm::vec2 position = glm::vec2(0.0f);
        glm::vec2 velocity = glm::vec2(0.0f);
        ColorRGBA8 color;
        float life = 0.0f;
        float width = 0.0f;
    };
    
    //our default update function for particles!
    inline void defaultParticleUpdate(Particle2D& particle, float deltaTime)
    {
        particle.position += particle.velocity * deltaTime;
    }
    
    class ParticleBatch2D
    {
    public:
        
        ParticleBatch2D();
        ~ParticleBatch2D();
        
        void init(int maxParticles,
                  float decayRate,
                  GLTexture texture,
                  std::function<void(Particle2D&, float)> updateFunc = defaultParticleUpdate);
        
        /// Adds particle to the batch
        void addParticle(const glm::vec2& position,
                         const ColorRGBA8& color,
                         glm::vec2& velocity,
                         float width);
        
        void update(float deltaTime);
        void draw(SpriteBatch* spriteBatch);
        
    private:
        
        /// Finds free particle space index
        int findFreeParticle();
        
        /// Update Function Pointer
        std::function<void(Particle2D&, float)> m_updateFunc = defaultParticleUpdate;
        
        float m_decayRate = 0.1f; ///< how fast each particle decays
        
        Particle2D* m_particles = nullptr; ///< particle batch so caching is safe and fast:)
        
        GLTexture mm_texture; ///< texture associated with particles
        
        int m_lastFreeParticle = 0; ///< index of free particles space in batch
        int m_maxParticles = 0; ///< max number of particles in batch
    };
    
    
}


#endif /* ParticleBatch2D_hpp */
