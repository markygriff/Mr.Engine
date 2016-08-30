//
//  Player.hpp
//  NinjaPlatformer
//
//  Created by Mark Griffith on 2016-08-08.
//  Copyright © 2016 Mark Griffith. All rights reserved.
//

#ifndef Player_hpp
#define Player_hpp

#include "Capsule.hpp"

#include <SpriteBatch.hpp>
#include <GLTexture.h>
#include <stdio.h>
#include <InputManager.hpp>
#include <TileSheet.h>


enum class PlayerMoveState {STANDING, RUNNING, PUNCHING, IN_AIR};

class Player
{
public:
    Player();
    ~Player();
    
    void init(b2World* world,
              const glm::vec2& position,
              const glm::vec2& drawDims,
              const glm::vec2& collisionDims,
              MrEngine::ColorRGBA8 color);
    
    void draw(MrEngine::SpriteBatch& spriteBatch, float deltaTime);
    void drawDebug(MrEngine::DebugRenderer& debugrenderer);
    
    void update(MrEngine::InputManager& inputManager, float deltaTime);
    
    void destroy(b2World* world) { m_capsule.destroy(world); }
    
    //getters
    const Capsule& getCapsule() const { return m_capsule; }
    
    glm::vec2 getPosition() const { return glm::vec2(m_capsule.getBody()->GetPosition().x, m_capsule.getBody()->GetPosition().y); }

private:
    Capsule m_capsule;
    
    //MrEngine::GLTexture m_texture;
    MrEngine::ColorRGBA8 m_color;
    MrEngine::TileSheet m_texture;
    
    glm::vec2 m_drawDims;
    
    PlayerMoveState m_moveState = PlayerMoveState::STANDING;
    
    int m_direction = 1;
    float m_animationTime = 0.0f;
    
    bool m_onGround = false;
    bool m_isPunching = false;
    
    //MrEngine::GLTexture m_texture;
};

#endif /* Player_hpp */
