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
    
    glm::ivec2 s = glm::ivec2(10.0f, 2.0f);
    m_texture.init(texture, s);
}

void Player::draw(MrEngine::SpriteBatch& spriteBatch, float deltaTime)
{
    glm::vec4 destRect;
    b2Body* body = m_capsule.getBody();
    
    destRect.x = body->GetPosition().x - m_drawDims.x / 2.0f;
    destRect.y = body->GetPosition().y - m_capsule.getDimensions().y / 2.0f; ///< use collision dims
    destRect.z = m_drawDims.x;
    destRect.w = m_drawDims.y;
    
    
    //calculate animation
    int tileIndex;
    int numTiles;
    float animationSpeed = 0.04f;
    glm::vec2 velocity;
    velocity.x = body->GetLinearVelocity().x;
    velocity.y = body->GetLinearVelocity().y;
    
    if (m_onGround)
    {
        if (m_isPunching)
        {
            numTiles = 4;
            tileIndex = 1;
            
            if (m_moveState != PlayerMoveState::PUNCHING)
            {
                m_moveState = PlayerMoveState::PUNCHING;
                m_animationTime = 0.0f;
            }
            
        }
        else if ((fabs(velocity.x) > 1.0f) && ((velocity.x > 0 && m_direction > 0 )|| (velocity.x < 0 && m_direction < 0)))
        {
            //running
            numTiles = 6;
            tileIndex = 10;
            
            //update the sprite animation speed to match the velocity of the player
            animationSpeed = fabs(velocity.x) * 0.025f;
            
            if (m_moveState != PlayerMoveState::RUNNING)
            {
                m_moveState = PlayerMoveState::RUNNING;
                m_animationTime = 0.0f;
            }
        }
        else
        {
            //standing on the ground
            numTiles = 1;
            tileIndex = 0;
            m_moveState = PlayerMoveState::STANDING;
        }
    }
    else
    {
        //in the air
        
        //kicking in air
        if (m_isPunching)
        {
            numTiles = 1;
            tileIndex = 18;
            //want to last the same amount of time as the punching animation
            animationSpeed *= 0.5f;
            
            if (m_moveState != PlayerMoveState::PUNCHING)
            {
                m_moveState = PlayerMoveState::PUNCHING;
                m_animationTime = 0.0f;
            }
            
        }
        //if we are moving fast in the air...
        else if(fabs(velocity.x) > 10.0f)
        {
            //play first animation
            numTiles = 1;
            tileIndex = 10;
        }
        else if (velocity.y <= 0.0f)
        {
            //falling
            numTiles = 1;
            tileIndex = 17;
            m_moveState = PlayerMoveState::IN_AIR;
        }
        else
        {
            //rising
            numTiles = 1;
            tileIndex = 16;
            m_moveState = PlayerMoveState::IN_AIR;
        }
    }
    
    //increment animation time
    m_animationTime += animationSpeed;
    
    //check for punch end
    if (m_animationTime > numTiles)
    {
        m_isPunching = false;
    }
    
    //apply animation
    tileIndex = tileIndex + ((int)m_animationTime % numTiles);

    //direction check
    glm::vec4 uvRect = m_texture.getUVs(tileIndex);
    if (m_direction == -1)
    {
        //move forward the sprite we're looking at
        uvRect.x += 1.0f / m_texture.dims.x;
        //flipbackwards the sprite we're looking at
        uvRect.z *= -1;
    }
    
    
    //draw the batch of boxes
    //player sprite has 10x2 frames so each is 0.1 apart
    // uv(start_pixel_x, start_pixel_y, width_used_of_sheet, height_used_of_sheet)
    spriteBatch.draw(destRect, uvRect, m_texture.texture.id, 0.0f, m_color, body->GetAngle());
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
    for (b2ContactEdge* ce = body->GetContactList(); ce != nullptr; ce = ce->next)
    {
        //check if contact point is touching anything
        b2Contact* c = ce->contact;
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