//
//  Capsule.hpp
//  NinjaPlatformer
//
//  Created by Mark Griffith on 2016-08-09.
//  Copyright © 2016 Mark Griffith. All rights reserved.
//

#ifndef Capsule_hpp
#define Capsule_hpp


#include <DebugRenderer.hpp>
#include <Box2D/Box2D.h>
#include <glm/glm.hpp>

#include <stdio.h>

class Capsule
{
public:
    
    void init(b2World* world,
              const glm::vec2& position,
              const glm::vec2& dims,
              float density,
              float friction,
              bool fixedRotation);
    
    void drawDebug(MrEngine::DebugRenderer& debugRenderer);
    
    void destroy(b2World *world);
    
    //getters
    b2Body* getBody() const { return m_body; }
    b2Fixture* getFixture(int index) const { return m_fixtures[index]; }
    const glm::vec2& getDimensions() const {return m_dimensions; }
    
    bool testPoint(float x, float y) const { return m_fixtures[0]->TestPoint(b2Vec2(x, y)) || m_fixtures[1]->TestPoint(b2Vec2(x, y)) || m_fixtures[2]->TestPoint(b2Vec2(x, y)); }


private:
    b2Body* m_body = nullptr;
    b2Fixture* m_fixtures[3];
    
    glm::vec2 m_dimensions;
    glm::vec4 m_uvRect;
};

#endif /* Capsule_hpp */
