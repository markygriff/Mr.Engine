//
//  Box.hpp
//  NinjaPlatformer
//
//  Created by Mark Griffith on 2016-08-07.
//  Copyright © 2016 Mark Griffith. All rights reserved.
//

#ifndef Box_hpp
#define Box_hpp

#include <Box2D/Box2D.h>
#include <Vertex.h>
#include <SpriteBatch.hpp>
#include <GLTexture.h>

#include <glm/glm.hpp>
#include <stdio.h>

class Box
{
public:
    Box();
    ~Box();
    
    void init(b2World* world,
              const glm::vec2& position,
              const glm::vec2& dims,
              MrEngine::GLTexture texture,
              MrEngine::ColorRGBA8 color,
              bool fixedRotation,
              glm::vec4 uvRect = glm::vec4(0.0f, 0.0f, 1.0f, 1.0f) );
    
    void draw(MrEngine::SpriteBatch& spriteBatch);
    
    void destroy(b2World* world);
    
    //getters
    b2Body* getBody() const { return m_body; }
    b2Fixture* getFixture() const { return m_fixture; }
    const glm::vec2& getDimensions() const {return m_dimensions; }
    glm::vec2 getPosition() const { return glm::vec2(m_body->GetPosition().x, m_body->GetPosition().y); }
    const MrEngine::ColorRGBA8& getColor() const { return m_color; }
    
private:
    b2Body* m_body = nullptr;
    b2Fixture* m_fixture = nullptr;
    
    glm::vec2 m_dimensions;
    glm::vec4 m_uvRect;
    
    MrEngine::ColorRGBA8 m_color;
    MrEngine::GLTexture m_texture;
};

#endif /* Box_hpp */
