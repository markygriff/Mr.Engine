#include "ParticleEngine2D.hpp"
#include "ParticleBatch2D.hpp"

namespace MrEngine
{
    
    
    ParticleEngine2D::ParticleEngine2D()
    {
        
    }
    
    ParticleEngine2D::~ParticleEngine2D()
    {
        for (auto& b : m_batches)
        {
            delete b;
        }
    }
    
    void ParticleEngine2D::addParticleBatch(ParticleBatch2D* particleBatch)
    {
        m_batches.push_back(particleBatch);
    }
    
    void ParticleEngine2D::update(float deltaTime)
    {
        for (auto& b : m_batches)
        {
            b->update(deltaTime);
        }
    }
    
    void ParticleEngine2D::draw(SpriteBatch* spriteBatch)
    {
        //for (auto& b : m_batches)
        //{
            spriteBatch->begin();
        for (auto& b : m_batches)
        {
            b->draw(spriteBatch);
        }
        spriteBatch->end();
            spriteBatch->renderBatch();
        //}
    }
    
    
}