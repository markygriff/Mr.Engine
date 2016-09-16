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
              bool isDynamic,
              float angle = 0.0f,
              glm::vec4 uvRect = glm::vec4(0.0f, 0.0f, 1.0f, 1.0f) );

    void draw(MrEngine::SpriteBatch& spriteBatch);
    void destroy(b2World* world);
    bool testPoint(float x, float y) const { return m_fixture->TestPoint(b2Vec2(x, y)); }
    bool isDynamic() const { return m_body->GetType() == b2_dynamicBody;}

    //getters
    b2Body* getBody() const { return m_body; }
    b2Fixture* getFixture() const { return m_fixture; }
    const glm::vec2& getDimensions() const {return m_dimensions; }
    glm::vec2 getPosition() const { return glm::vec2(m_body->GetPosition().x, m_body->GetPosition().y); }
    const MrEngine::ColorRGBA8& getColor() const { return m_color; }
    glm::vec4 getUvRect() const { return m_uvRect; }
    float getAngle() const { return m_body->GetAngle(); }
    const MrEngine::GLTexture&  getTexture()       const { return m_texture; }
    const bool& getFixedRotation() const { return m_fixedRotation; }
    const bool& getIsDynamic() const { return m_isDynamic; }

private:
    b2Body* m_body = nullptr;
    b2Fixture* m_fixture = nullptr;

    bool m_fixedRotation;
    bool m_isDynamic;

    glm::vec2 m_dimensions;
    glm::vec4 m_uvRect;

    MrEngine::ColorRGBA8 m_color;
    MrEngine::GLTexture m_texture;
};

#endif /* Box_hpp */
