#ifndef Camera2D_hpp
#define Camera2D_hpp

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <stdio.h>

namespace MrEngine
{


class Camera2D
{
public:

    Camera2D();
    ~Camera2D();
    
    void init(int screenWidth, int screenHeight);
    
    void update();
    
    /// Converts screen coordinates to World corordinates
    glm::vec2 convertScreenToWorld(glm::vec2 screenCoords);
    
    /// Returns true if box is on the visible screen
    bool isBoxInView(const glm::vec2 &position, const glm::vec2 &dimensions);
    
    void offsetPosition(const glm::vec2& offset) { m_position += offset; m_needsMatrixUpdate = true; }
    void offsetScale(float offset) { m_scale += offset; if (m_scale < 0.001f) m_scale = 0.001f; m_needsMatrixUpdate = true; }
    
    //setters
    void setPosition(const glm::vec2& newPosition) { m_position = newPosition; m_needsMatrixUpdate = true; }
    void setScale(const float newScale) { m_scale = newScale; m_needsMatrixUpdate = true; }
    
    //getters
    glm::vec2 getPosition() const { return m_position; }
    float getScale() const { return m_scale; }
    glm::mat4 getCameraMatrix() const { return m_cameraMatrix; }
    float getDimRatio() const { return  (float) m_screenWidth / (float) m_screenHeight; }
    
private:
    int m_screenWidth = 500;
    int m_screenHeight = 500;
    bool m_needsMatrixUpdate = true;
    float m_scale = 1.0f;
    glm::vec2 m_position;
    glm::mat4 m_cameraMatrix; // 4x4 matrix
    glm::mat4 m_orthoMatrix;
};

    
}

#endif /* Camera2D_hpp */
