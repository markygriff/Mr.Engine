#include "Box.hpp"

#include <iostream>

Box::Box()
{
    //empty
}

Box::~Box()
{
    //empty
}

void Box::init(b2World* world,
               const glm::vec2& position,
               const glm::vec2& dims,
               MrEngine::GLTexture texture,
               MrEngine::ColorRGBA8 color,
               bool fixedRotation,
               bool isDynamic,
               float angle /* 0.0f */,
               glm::vec4 uvRect /* (0.0f, 0.0f, 1.0f, 1.0f) */ )
{
    //make the body
    b2BodyDef bodyDef;
    bodyDef.type = b2_dynamicBody;
    bodyDef.position.Set(position.x, position.y);
    bodyDef.fixedRotation = fixedRotation;
    m_fixedRotation = fixedRotation;
    bodyDef.angle = angle;

    if (isDynamic)
    {
        bodyDef.type = b2_dynamicBody;
        m_isDynamic = true;
    }
    else
    {
        bodyDef.type = b2_staticBody;
        m_isDynamic = false;
    }

    m_color = color;
    m_dimensions = dims;
    m_body = world->CreateBody(&bodyDef);
    m_texture = texture;
    m_uvRect = uvRect;

    b2PolygonShape boxShape;
    boxShape.SetAsBox(dims.x / 2.0f, dims.y / 2.0f);

    b2FixtureDef fixtureDef;
    fixtureDef.shape = &boxShape;
    fixtureDef.density = 1.0f;
    fixtureDef.friction = 0.3f;

    m_fixture = m_body->CreateFixture(&fixtureDef);
}

void Box::draw(MrEngine::SpriteBatch& spriteBatch)
{
    glm::vec4 destRect;
    destRect.x = m_body->GetPosition().x - m_dimensions.x / 2.0f;
    destRect.y = m_body->GetPosition().y - m_dimensions.y / 2.0f;
    destRect.z = m_dimensions.x;
    destRect.w = m_dimensions.y;

    //draw the batch of boxes
    spriteBatch.draw(destRect, m_uvRect, m_texture.id, 0.0f, m_color, m_body->GetAngle());
}

void Box::destroy(b2World* world)
{
    world->DestroyBody(m_body);
}
