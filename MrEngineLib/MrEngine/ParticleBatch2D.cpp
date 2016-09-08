#include "ParticleBatch2D.hpp"

#include <iostream>


namespace MrEngine
{
    
    
    ParticleBatch2D::ParticleBatch2D()
    {
        
    }
    
    ParticleBatch2D::~ParticleBatch2D()
    {
        delete[] m_particles;
    }
    
    
    void ParticleBatch2D::init(int maxParticles,
                               float decayRate,
                               GLTexture texture,
                               std::function<void(Particle2D&, float)> updateFunc)
    {
        m_maxParticles = maxParticles;
        m_particles = new Particle2D[maxParticles];
        
        m_decayRate = decayRate;
        
        mm_texture = texture;
        
        m_updateFunc = updateFunc;
    }
    
    void ParticleBatch2D::addParticle(const glm::vec2& position,
                                      const ColorRGBA8& color,
                                      glm::vec2& velocity,
                                      float width)
    {
        int particleIdx = findFreeParticle();
        
        auto& p = m_particles[particleIdx];
        
        p.life = 1.0f; // remember, this says the particle is active
        p.position = position;
        p.color = color;
        p.velocity = velocity;
        p.width = width;
    }
    
    void ParticleBatch2D::update(float deltaTime)
    {
        for (int i = 0; i < m_maxParticles; i++)
        {
            //check if particle is active
            if (m_particles[i].life > 0.0f)
            {
                m_updateFunc(m_particles[i], deltaTime);
                m_particles[i].life -= m_decayRate * deltaTime;
            }
        }
    }
    
    void ParticleBatch2D::draw(SpriteBatch* spriteBatch)
    {
        glm::vec4 uvRect(0.0f, 0.0f, 1.0f, 1.0f);
        
        for (int i = 0; i < m_maxParticles; i++)
        {
            //check if particle is active
            auto& p = m_particles[i];
            if (p.life > 0.0f)
            {
                //get dest rect of particle
                glm::vec4 destRect(p.position.x, p.position.y, p.width, p.width);
                
                //draw the particle
                spriteBatch->draw(destRect, uvRect, mm_texture.id, 0.0f, p.color);
            }
        }
    }
    
    int ParticleBatch2D::findFreeParticle()
    {
        //search for the free particle starting from the last free particle
        for (int i = m_lastFreeParticle; i < m_maxParticles; i++)
        {
            if (m_particles[i].life <= 0.0f)
            {
                m_lastFreeParticle = i;
                return i;
            }
        }
        
        //if not found, search the rest for free particle space
        for (int i = 0; i < m_lastFreeParticle; i++)
        {
            if (m_particles[i].life <= 0.0f)
            {
                m_lastFreeParticle = i;
                return i;
            }
        }
        
        //no particles are free, just override 0th particles
        return 0;
    }
}