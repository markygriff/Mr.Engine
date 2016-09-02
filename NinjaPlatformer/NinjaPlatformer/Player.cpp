//
//  Player.cpp
//  NinjaPlatformer
//
//  Created by Mark Griffith on 2016-08-08.
//  Copyright © 2016 Mark Griffith. All rights reserved.
//

#include "Player.hpp"

#include <ResourceManager.hpp>
#include <SDL2/SDL.h>
#include <iostream>

Player::Player()
{
    
}

Player::~Player()
{
    
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
    m_texture.init(texture, spriteDims);
    
    m_texture.addAnimation(MrEngine::SpriteState::IDLE, 1, 0, 0.04f);
    m_texture.addAnimation(MrEngine::SpriteState::RUNNING, 6, 10, 0.04);
    m_texture.addAnimation(MrEngine::SpriteState::ATTACKING, 5, 1, 0.04f);
    m_texture.addAnimation(MrEngine::SpriteState::ATTACKING_IN_AIR, 1, 18, 0.02f);
    m_texture.addAnimation(MrEngine::SpriteState::RISING, 1, 16, 0.04f);
    m_texture.addAnimation(MrEngine::SpriteState::FALLING, 1, 17, 0.04f);
}

void Player::draw(MrEngine::SpriteBatch& spriteBatch, float deltaTime)
{
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
            if (m_texture.setAnimation(MrEngine::SpriteState::ATTACKING)) m_animationTime = 0.0f;
        }
        else if ((fabs(velocity.x) > 1.0f) && ((velocity.x > 0 && m_direction > 0 )|| (velocity.x < 0 && m_direction < 0)))
        {
            //running
            if (m_texture.setAnimation(MrEngine::SpriteState::RUNNING)) m_animationTime = 0.0f;
        }
        else m_texture.setAnimation(MrEngine::SpriteState::IDLE);
    }
    else
    {
        //in the air
        if (m_isPunching)
        {
            //kicking in air
            if (m_texture.setAnimation(MrEngine::SpriteState::ATTACKING_IN_AIR)) m_animationTime = 0.0f;
        }
        //if we are moving fast in the air...
        else if(fabs(velocity.x) > 10.0f)
        {
            //play first animation
        }
        
        //falling
        else if (velocity.y <= 0.0f) m_texture.setAnimation(MrEngine::SpriteState::FALLING);
        //rising
        else m_texture.setAnimation(MrEngine::SpriteState::RISING);
    }
    
    int tileIndex = m_texture.getAnimation().tileIndex;
    int numTiles = m_texture.getAnimation().numTiles;
    float animationSpeed = m_texture.getAnimation().animationSpeed;
    
    //increment animation time
    m_animationTime += animationSpeed;
    
    //check for punch end
    if (m_animationTime > numTiles) m_isPunching = false;
    
    //apply animation
    tileIndex = tileIndex + ((int)m_animationTime % numTiles);

    //uvRect for rendering sprite
    glm::vec4 uvRect = m_texture.getUVs(tileIndex);
    
    //direction check
    if (m_direction == -1) uvRect = m_texture.flipUVs(uvRect, true, false);
    
    //draw the batch of boxes
    //player sprite has 10x2 frames so each is 0.1 apart
    // uv(start_pixel_x, start_pixel_y, width_used_of_sheet, height_used_of_sheet)
    spriteBatch.draw(destRect, uvRect, m_texture.getTexture().id, 0.0f, m_color, body->GetAngle());
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
            bool right = false;
            for (int i = 0; i < b2_maxManifoldPoints; i++)
            {
                if (manifold.points[i].y < (body->GetPosition().y - m_capsule.getDimensions().y / 2.0f + m_capsule.getDimensions().x / 2.0f - 0.1f))
                {
                    below = true;
                    break;
                }
                if (manifold.points[i].x > (body->GetPosition().x))
                {
                    right = true;
                    break;
                }
            }
            
            if (below)
            {
                //jump!
                if (inputManager.isKeyPressed(SDLK_w) || inputManager.isKeyPressed(SDLK_UP))
                {
                    body->ApplyLinearImpulse(b2Vec2(0.0f, 10.0f), b2Vec2(0.0f, 0.0f), true);
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