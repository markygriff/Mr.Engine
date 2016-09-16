#include "Capsule.hpp"

void Capsule::init(b2World* world,
                   const glm::vec2& position,
                   const glm::vec2& dims,
                   float density,
                   float friction,
                   bool fixedRotation)
{
    //make the body
    b2BodyDef bodyDef;
    bodyDef.type = b2_dynamicBody;
    bodyDef.position.Set(position.x, position.y);
    bodyDef.fixedRotation = fixedRotation;
    
    m_dimensions = dims;
    m_body = world->CreateBody(&bodyDef);
    
    //create the box
    b2PolygonShape boxShape;
    boxShape.SetAsBox(dims.x / 2.0f, (dims.y - dims.x) / 2.0f);
    
    b2FixtureDef boxDef;
    boxDef.shape = &boxShape;
    boxDef.density = density;
    boxDef.friction = friction;
    m_fixtures[0] = m_body->CreateFixture(&boxDef);
    
    //create the circles (top and bottom of capsule)
    b2CircleShape circleShape;
    circleShape.m_radius = dims.x / 2.0;
    
    b2FixtureDef circleDef;
    circleDef.shape = &circleShape;
    circleDef.density = density;
    circleDef.friction = friction;
    
    //bottom circle
    circleShape.m_p.Set(0.0f, (-m_dimensions.y + dims.x) / 2.0f);
    m_fixtures[1] = m_body->CreateFixture(&circleDef);
    //top circle
    circleShape.m_p.Set(0.0f, (m_dimensions.y + -dims.x) / 2.0f);
    m_fixtures[2] = m_body->CreateFixture(&circleDef);

}

void Capsule::drawDebug(MrEngine::DebugRenderer& debugRenderer)
{
    //draw box collision
    glm::vec4 destRect;
    destRect.x = m_body->GetPosition().x - m_dimensions.x / 2.0f;
    destRect.y = m_body->GetPosition().y - (m_dimensions.y - m_dimensions.x) / 2.0f;
    destRect.z = m_dimensions.x;
    destRect.w = m_dimensions.y - m_dimensions.x;
    
    debugRenderer.drawBox(destRect, MrEngine::ColorRGBA8(255,255,255,255), m_body->GetAngle());
    
    //draw circle collision
    debugRenderer.drawCircle(glm::vec2(destRect.x + m_dimensions.x / 2.0f, destRect.y), MrEngine::ColorRGBA8(255,255,255,255), m_dimensions.x / 2.0f);
    debugRenderer.drawCircle(glm::vec2(destRect.x + m_dimensions.x / 2.0f, destRect.y + destRect.w), MrEngine::ColorRGBA8(255,255,255,255), m_dimensions.x / 2.0f);
}

void Capsule::destroy(b2World *world)
{
    if (m_body) {
            world->DestroyBody(m_body);
            m_body = nullptr;
        }
}