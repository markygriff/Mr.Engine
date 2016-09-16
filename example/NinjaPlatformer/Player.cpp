#include "Player.hpp"

#include <ResourceManager.hpp>
#include <SDL2/SDL.h>
#include <iostream>

Player::Player()
{
    //empty
}

Player::~Player()
{
    //empty
}

void Player::init(b2World* world,
                  const glm::vec2& position,
                  const glm::vec2& drawDims,
                  const glm::vec2& collisionDims,
                  MrEngine::ColorRGBA8 color)
{
    MrEngine::GLTexture texture = MrEngine::ResourceManager::getTexture("../Assets/Textures/blue_ninja.png");
    
    m_color = color;
    m_drawDims = drawDims;
    m_collisionDims = collisionDims;
    
    m_capsule.init(world, position, collisionDims, 1.0f, 0.1f, true);
    
    //initialize player sprite sheet
    glm::ivec2 spriteDims = glm::ivec2(10.0f, 2.0f);
    
    m_spriteSheet.addAnimation(MrEngine::SpriteState::IDLE, texture, spriteDims, 1, 0, 0.04f);
    m_spriteSheet.addAnimation(MrEngine::SpriteState::RUNNING, texture, spriteDims, 6, 10, 0.04f);
    m_spriteSheet.addAnimation(MrEngine::SpriteState::ATTACKING, texture, spriteDims, 5, 1, 0.04f);
    m_spriteSheet.addAnimation(MrEngine::SpriteState::ATTACKING_IN_AIR, texture, spriteDims, 1, 18, 0.02f);
    m_spriteSheet.addAnimation(MrEngine::SpriteState::RISING, texture, spriteDims, 1, 16, 0.04f);
    m_spriteSheet.addAnimation(MrEngine::SpriteState::FALLING, texture, spriteDims, 1, 17, 0.04f);
    
    m_spriteSheet.setAnimation(MrEngine::SpriteState::IDLE);
    m_spriteSheet.begin();
}

void Player::draw(MrEngine::SpriteBatch& spriteBatch, float deltaTime)
{
    //destination rectangle of where to draw the player sprite texture
    glm::vec4 destRect;
    b2Body* body = m_capsule.getBody();
    destRect.x = body->GetPosition().x - m_drawDims.x / 2.0f;
    destRect.y = body->GetPosition().y - m_capsule.getDimensions().y / 2.0f; ///< use collision dims
    destRect.z = m_drawDims.x;
    destRect.w = m_drawDims.y;
    
    glm::vec2 velocity;
    velocity.x = body->GetLinearVelocity().x;
    velocity.y = body->GetLinearVelocity().y;
    
    //ANIMATION
    
    if (m_onGround)
    {
        if (m_isPunching)
        {
            //punching
            if (m_spriteSheet.setAnimation(MrEngine::SpriteState::ATTACKING)) m_spriteSheet.begin();
        }
        else if ((fabs(velocity.x) > 1.0f) && ((velocity.x > 0 && m_direction > 0 )|| (velocity.x < 0 && m_direction < 0)))
        {
            //running
            if (m_spriteSheet.setAnimation(MrEngine::SpriteState::RUNNING)) m_spriteSheet.begin();
        }
        else m_spriteSheet.setAnimation(MrEngine::SpriteState::IDLE);
    }
    else
    {
        //in the air
        if (m_isPunching)
        {
            //kicking in air
            if (m_spriteSheet.setAnimation(MrEngine::SpriteState::ATTACKING_IN_AIR)) m_spriteSheet.begin();
        }
        //if we are moving fast in the air...
        else if(fabs(velocity.x) > 10.0f)
        {
            //play first animation
        }
        
        //falling
        else if (velocity.y <= 0.0f) m_spriteSheet.setAnimation(MrEngine::SpriteState::FALLING);
        //rising
        else m_spriteSheet.setAnimation(MrEngine::SpriteState::RISING);
    }
    
    //increment animation time
    m_spriteSheet.end();
    
    //check for punch end
    if (m_spriteSheet.isAnimationFinished()) m_isPunching = false;
    
    //animate the sprite
    m_spriteSheet.animate(m_direction, 1);
    
    //draw the player texture from the spriteSheet
    spriteBatch.draw(destRect, m_spriteSheet.getUVs(), m_spriteSheet.getTexture().id, 0.0f, m_color, body->GetAngle());
}

void Player::drawDebug(MrEngine::DebugRenderer& debugrenderer)
{
    m_capsule.drawDebug(debugrenderer);
}

void Player::update(MrEngine::InputManager& inputManager, float deltaTime)
{
    b2Body* body = m_capsule.getBody();
    
    if (inputManager.isKeyDown(SDLK_a) || inputManager.isKeyDown(SDLK_LEFT))
    {
        body->ApplyForceToCenter(b2Vec2(-100.0f, 0.0f), true);
        m_direction = -1;
    }
    else if (inputManager.isKeyDown(SDLK_d) || inputManager.isKeyDown(SDLK_RIGHT))
    {
        body->ApplyForceToCenter(b2Vec2(100.0f, 0.0f), true);
        m_direction = 1;
    }
    else
    {
        body->SetLinearVelocity(b2Vec2(body->GetLinearVelocity().x * 0.9, body->GetLinearVelocity().y));
    }
    
    float MAX_SPEED = 3.0f;
    if (body->GetLinearVelocity().x < -MAX_SPEED)
    {
        body->SetLinearVelocity(b2Vec2(-MAX_SPEED, body->GetLinearVelocity().y));
    }
    else if (body->GetLinearVelocity().x > MAX_SPEED)
    {
        body->SetLinearVelocity(b2Vec2(MAX_SPEED, body->GetLinearVelocity().y));
    }
    
    //check if the ground is below and touching, then we can JUMP!
    //look through all contact points
    m_onGround = false;
    m_touchedRight = false;
    for (b2ContactEdge* contactEdge = body->GetContactList(); contactEdge != nullptr; contactEdge = contactEdge->next)
    {
        //check if contact point is touching anything
        b2Contact* c = contactEdge->contact;
        
        if (c->IsTouching())
        {
            b2WorldManifold manifold;
            c->GetWorldManifold(&manifold);
            
            //check if touching below
            bool below = false;
            for (int i = 0; i < b2_maxManifoldPoints; i++)
            {
                if (manifold.points[i].y < (body->GetPosition().y - m_capsule.getDimensions().y / 2.0f + m_capsule.getDimensions().x / 2.0f - 0.1f))
                {
                    below = true;
                    break;
                }
                if (manifold.points[i].x > (body->GetPosition().x))
                {
                    m_touchedRight = true;
                    break;
                }
            }
            
            if (below)
            {
                //jump!
                if (inputManager.isKeyPressed(SDLK_w) || inputManager.isKeyPressed(SDLK_UP))
                {
                    body->ApplyLinearImpulse(b2Vec2(0.0f, 7.5f), b2Vec2(0.0f, 0.0f), true);
                }
                m_onGround = true;
                below = false;
                break;
            }
        }
    }
    
    //PUNCHING
    if (inputManager.isKeyPressed(SDLK_SPACE))
    {
        m_isPunching = true;
    }
}